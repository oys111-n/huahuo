#include "multi_color.h"

MultiColor& MultiColor::getInstance() {
    static MultiColor instance;
    return instance;
}

MultiColor::MultiColor(/* args */)
{
    std::cout<<"构造" << std::endl;
    updateOfficalFilasList();
    updateSaveVariables();
}

MultiColor::~MultiColor()
{
    std::cout<<"析构" << std::endl;
    multiColorThreadStop();
}


void MultiColor::push(std::string key, std::any value) {
    QueueElement map;
    map.key = key;
    map.value = value;
    std::unique_lock<std::shared_mutex> lock(queue_mtx_);
    queue_.push(map);
    cond_var_.notify_one();
}

QueueElement MultiColor::pop() {
    std::shared_lock<std::shared_mutex> lock(queue_mtx_);
    if (queue_.empty()) {
        return {"", ""};
    }
    QueueElement map = queue_.front();
    queue_.pop();
    return map;
}

OfficalFilasList MultiColor::getOfficalFilasList(){
    return officalFilasList;
}

Slot MultiColor::getRackSlot(){
    return rackSlot;
}

std::unordered_map<int, Slot> MultiColor::getSlots(){
    return slots;
}
size_t MultiColor::getBoxCount(){
    return boxCount;
}

std::string MultiColor::getSlotSync(){
    return slotSync;
}
std::unordered_map<int, std::string> MultiColor::getValueSlots(){
    return valueSlots;
}

void MultiColor::updateSaveVariables(){
    auto savedVariables = std::make_unique<mks_parse_multi_color_ini>("saved_variables");

    size_t boxCountTemp = savedVariables->inifile_getint(SECTION_VARIABLES, KEY_BOX_COUNT, 0);
    std::string slotSyncTemp = savedVariables->inifile_getString(SECTION_VARIABLES, KEY_SLOT_SYNC, "");

    checkChange(KEY_BOX_COUNT, boxCount, boxCountTemp);
    checkChange(KEY_SLOT_SYNC, slotSync, slotSyncTemp);
    boxCount = boxCountTemp;
    slotSync = slotSyncTemp;

    std::string rackKeyName[] = {
        "", 
        KEY_RACK_COLOR, 
        KEY_RACK_FILAMENT, 
    };

    Slot rackSlotTemp = checkSlotChanges(savedVariables, RACK_NAME,
        rackKeyName);

    if (rackSlot.colorSlotNum != rackSlotTemp.colorSlotNum || rackSlot.filamentSlotNum != rackSlotTemp.filamentSlotNum){
        push(RACK_NAME, rackSlotTemp);
        rackSlot = rackSlotTemp;
    }
    for (int i = 0; i < boxCount * 4; ++i) {
        const std::string slotIndex = std::to_string(i);
        std::string keyName[] = {
            std::string(KEY_SLOT_PREFIX) + std::to_string(i), 
            std::string(KEY_COLOR_SLOT_PREFIX) + std::to_string(i), 
            std::string(KEY_FILAMENT_SLOT_PREFIX) + std::to_string(i), 
        };
        Slot slot = checkSlotChanges(savedVariables,
             numberToCodeConversion(i), keyName);

        if (slots[i].slotState != slot.slotState || slots[i].colorSlotNum != slot.colorSlotNum || slots[i].filamentSlotNum != slot.filamentSlotNum){
            push(std::string(KEY_SLOT_PREFIX) + std::to_string(i), slot);
            slots[i] = slot;
        }

        std::string valueSlotTemp = savedVariables->inifile_getString(
            SECTION_VARIABLES,
            std::string(KEY_VALUE_T_PREFIX) + slotIndex,
            ""
        );
        checkChange(std::string(KEY_VALUE_T_PREFIX) + slotIndex, valueSlots[i], valueSlotTemp);
        valueSlots[i] = valueSlotTemp;

    }
    savedVariables->inifile_free();
}

void MultiColor::updateOfficalFilasList(){
    auto officiallFilasList = std::make_unique<mks_parse_multi_color_ini>(
        "officiall_filas_list");

    for (size_t i = 0; i < mc_color_codes.size(); i++)
    {
        Color color;
        color.color_str = mc_color_codes[i];
        color.text_color_str = getTextColor(color.color_str);
        officalFilasList.colorsMap.emplace_back(color);
    }

    size_t sectionNums = officiallFilasList->inifile_getnsec();
    for (size_t i = 0; i < sectionNums - 1; i++)
    {
        Fila fila;
        fila.filaNum = i + 1;
        fila.filament = officiallFilasList->inifile_getString(
            SECTION_FILA_PREFIX + std::to_string(i + 1), KEY_FILAMENT, "");
        fila.vendor = officiallFilasList->inifile_getString(
            SECTION_FILA_PREFIX + std::to_string(i + 1), KEY_VENDOR, "");
        fila.maxTemp = officiallFilasList->inifile_getint(
            SECTION_FILA_PREFIX + std::to_string(i + 1), KEY_MAX_TEMP, 0);
        fila.minTemp = officiallFilasList->inifile_getint(
            SECTION_FILA_PREFIX + std::to_string(i + 1), KEY_MIN_TEMP, 0);
        officalFilasList.filasMap.emplace_back(fila);
    }
    officiallFilasList->inifile_free();
}

std::string MultiColor::getTextColor(std::string hex) {
    int r,g,b;
    if (hex[0] == '#') {
        std::stringstream ss;
        ss << std::hex << hex.substr(1);
        int hexValue;
        ss >> hexValue;
        r = (hexValue >> 16) & 0xFF;
        g = (hexValue >> 8) & 0xFF;
        b = hexValue & 0xFF;
    }

    try {
        int brightness = (r * 299 + g * 587 + b * 114) / 1000;
        return brightness > 125 ? "#000000" : "#FFFFFF";
    } catch (const std::invalid_argument&) {
        // 如果颜色格式无效，返回默认的黑色
        return "#000000";
    }
}

std::string MultiColor::numberToCodeConversion(int num) {
    if (num < 0 || num > 15) 
        return "";
    // 计算 box 数字和 slot 字符
    char box = '1' + (num / 4);    // 0~3 -> '1'、'2'、'3'、'4'
    char slot = 'A' + (num % 4);     // 0~3 -> 'A'、'B'、'C'、'D'
    return std::string({box, slot});
}

size_t MultiColor::convertCodeToNumber(std::string str) {
    if (str.size() != 2)
        return static_cast<size_t>(-1);  // 出错返回 -1
    char box = str[0];
    char slot = str[1];
    if (box < '0' || box > '9' || slot < 'A' || slot > 'D')
        return static_cast<size_t>(-1);  // 出错返回 -1
    return (box - '0') * 4 + (slot - 'A');
}

void MultiColor::multiColorThreadStart() {
    std::lock_guard<std::mutex> lock(thread_mutex);
    if (!running.load(std::memory_order_acquire)) {
        running.store(true, std::memory_order_release);
        // 使用临时线程对象避免已join线程的重复操作
        update_thread = std::thread(&MultiColor::multiColorThreadRun, this);
    }
}

Slot MultiColor::checkSlotChanges(std::unique_ptr<mks_parse_multi_color_ini>& parser, std::string name, std::string *keyName) {
    Slot slot;
    slot.idName = name;
    slot.slotState = parser->inifile_getint(
        SECTION_VARIABLES, keyName[0], 0);
    slot.colorSlotNum = parser->inifile_getint(
        SECTION_VARIABLES, keyName[1], 0);
    slot.filamentSlotNum = parser->inifile_getint(
        SECTION_VARIABLES, keyName[2], 0);

    if (slot.colorSlotNum > 0 && slot.colorSlotNum < 25){
        slot.slotColor = officalFilasList.colorsMap[slot.colorSlotNum-1].color_str;
    }
    if (slot.filamentSlotNum > 0 && slot.filamentSlotNum < 57) {
        slot.slotFilament = officalFilasList.filasMap[slot.filamentSlotNum - 1].filament;
        slot.slotMaxTemp = officalFilasList.filasMap[slot.filamentSlotNum - 1].maxTemp;
        slot.slotMinTemp = officalFilasList.filasMap[slot.filamentSlotNum - 1].minTemp;
    }

    return slot;
}

void MultiColor::multiColorThreadStop() {
    {
        std::lock_guard<std::mutex> lock(thread_mutex);
        if (running.load(std::memory_order_acquire)) {
            running.store(false, std::memory_order_release);
        }
    }
    cv.notify_all(); // 唤醒可能处于等待状态的线程

    if (update_thread.joinable()) {
        try {
            if (update_thread.joinable()) {
                update_thread.join();
            }
        } catch (const std::system_error& e) {
            std::cerr << "Thread join failed: " << e.what() << std::endl;
        }
    }
}

void MultiColor::multiColorThreadRun() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(thread_mutex);
            if (!running.load(std::memory_order_acquire)) break;

            cv.wait_for(lock, std::chrono::seconds(1), 
                [this]{ return !running.load(std::memory_order_acquire); });
            
            if (!running.load(std::memory_order_acquire)) break;
        }

        updateSaveVariables(); // 注意：需要确保此方法的线程安全性
    }
}

