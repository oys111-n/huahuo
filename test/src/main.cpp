#include "multi_color.h"
#include <windows.h>
#include <map>
#include <any>
#include <string>
#include <iostream>
// #include <stdio.h>
int main() {
    // 在栈上创建对象，并传递参数
    // 获取单例实例并调用方法

    MultiColor& s = MultiColor::getInstance();

    QueueElement queueElement = s.pop();
    s.multiColorThreadStart();
    while(true){
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        if (queueElement.key != "")
        {
            try{
                /* code */
                if (queueElement.key == KEY_BOX_COUNT) {
                    std::cout << KEY_BOX_COUNT<< std::any_cast<size_t>(queueElement.value) << std::endl;
                }
                if (queueElement.key == KEY_SLOT_SYNC) {
                    std::cout << KEY_SLOT_SYNC << std::any_cast<std::string>(queueElement.value) << std::endl;
                }
                if (queueElement.key == RACK_NAME) {
                    Slot slot = std::any_cast<Slot>(queueElement.value);
                        std::cout <<"----------------"<< "\n"
                        << "idName: " << slot.idName << "\n"
                        << "slotState: " << slot.slotState << "\n"
                        << "colorSlotNum: " << slot.colorSlotNum << "\n"
                        << "filamentSlotNum: " << slot.filamentSlotNum << "\n"
                        << "slotFliament: " << slot.slotFilament << "\n"
                        << "slotColor: " << slot.slotColor << "\n"
                        << "vendor: " << slot.vendor << "\n"
                        << "slotMaxTemp: " << slot.slotMaxTemp << "\n"
                        << "slotMinTemp: " << slot.slotMinTemp << "\n"
                        <<"----------------"<< std::endl;
                }
                for (size_t i = 0; i<16; i++){
                    if (queueElement.key == (std::string(KEY_VALUE_T_PREFIX) + std::to_string(i))) {
                        std::cout << "valutslot:" << std::any_cast<std::string>(queueElement.value) << std::endl; 
                    }else if (queueElement.key == (std::string(KEY_SLOT_PREFIX) + std::to_string(i))) {
                        Slot slot = std::any_cast<Slot>(queueElement.value);
                        std::cout <<"----------------"<< "\n"
                        << "idName: " << slot.idName << "\n"
                        << "slotState: " << slot.slotState << "\n"
                        << "colorSlotNum: " << slot.colorSlotNum << "\n"
                        << "filamentSlotNum: " << slot.filamentSlotNum << "\n"
                        << "slotFliament: " << slot.slotFilament << "\n"
                        << "slotColor: " << slot.slotColor << "\n"
                        << "vendor: " << slot.vendor << "\n"
                        << "slotMaxTemp: " << slot.slotMaxTemp << "\n"
                        << "slotMinTemp: " << slot.slotMinTemp << "\n"
                        <<"----------------"<< std::endl;
                    }
                }
            } catch (const std::bad_any_cast& e) {
                std::cerr << "Bad any cast: " << e.what() << std::endl;
            }
        }
        queueElement = s.pop();

    }
    s.multiColorThreadStop();

    return 0;
}