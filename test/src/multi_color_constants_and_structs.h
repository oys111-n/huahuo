// multi_color_constants_and_structs.h
#ifndef MULTI_COLOR_CONSTANTS_AND_STRUCTS_H
#define MULTI_COLOR_CONSTANTS_AND_STRUCTS_H

#include <string>
#include <vector>

// 常量定义区（推荐使用constexpr）
static constexpr const char* SECTION_VARIABLES = "Variables";
static constexpr const char* KEY_BOX_COUNT = "box_count";
static constexpr const char* KEY_SLOT_SYNC = "slot_sync";
static constexpr const char* KEY_SLOT_PREFIX = "slot";
static constexpr const char* KEY_COLOR_SLOT_PREFIX = "color_slot";
static constexpr const char* KEY_FILAMENT_SLOT_PREFIX = "filament_slot";
static constexpr const char* KEY_VALUE_T_PREFIX = "value_t";
static constexpr const char* RACK_NAME = "rack";
static constexpr const char* KEY_RACK_FILAMENT = "filament";
static constexpr const char* KEY_RACK_COLOR = "color";

static constexpr const char* SECTION_FILA_PREFIX = "fila";
static constexpr const char* KEY_FILAMENT = "filament";
static constexpr const char* KEY_VENDOR = "vendor";
static constexpr const char* KEY_MAX_TEMP = "max_temp";
static constexpr const char* KEY_MIN_TEMP = "min_temp";

static const std::vector<std::string> mc_color_codes = {
    "#FAFAFA", "#060606", "#D9E3ED", "#5CF30F", "#63E492", "#2850FF",
    "#FE98FE", "#DFD628", "#228332", "#99DEFF", "#1714B0", "#CEC0FE",
    "#CADE4B", "#1353AB", "#5EA9FD", "#A878FF", "#FE717A", "#FF362D",
    "#E2DFCD", "#898F9B", "#6E3812", "#CAC59F", "#F28636", "#B87F2B"
};

// 结构体定义区
struct Slot {
    /* data */
    std::string idName;
    size_t slotState;
    size_t colorSlotNum;
    size_t filamentSlotNum;
    std::string slotFilament;
    std::string slotColor;
    std::string vendor;
    size_t slotMaxTemp;
    size_t slotMinTemp;
};

struct Fila {
    size_t filaNum;
    std::string filament;
    std::string vendor;
    size_t maxTemp;
    size_t minTemp;
};

struct Color {
    std::string color_str;
    std::string text_color_str;
};

struct OfficalFilasList {
    std::vector<Fila> filasMap;
    std::vector<Color> colorsMap;
};

struct QueueElement {
    std::string key;
    std::any value;
};

#endif // CONSTANTS_AND_STRUCTS_H
