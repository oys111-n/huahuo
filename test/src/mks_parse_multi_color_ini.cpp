#include <iostream>
#include <string>
#include <mutex>

#include "mks_parse_multi_color_ini.h"

dictionary *QDINI = NULL; 

bool mks_parse_multi_color_ini::file_exists(const char *path)
{
    // struct stat buffer;
    // return (stat(path, &buffer) == 0);
    return true;
}

/**
 * @description: 从本地读取INIFile配置文件
 * @param path 文件所在路径
 * @return 0表示成功，-1表示新建文件失败，-2表示读取失败
 */
int mks_parse_multi_color_ini::inifile_load_from_path(const char *path)
{
    if (!file_exists(path)) {
        std::cout << "The config file does not exist." << std::endl;
        FILE *file = fopen(path, "w");
        if (file == NULL) {
            std::cerr << "Failed to create the config file!" << std::endl;
            return -1;
        }
        std::cout << "The config file creat success!" << std::endl;
        fclose(file);
    }
    std::lock_guard<std::mutex> lock(QDINI_mutex);
    QDINI = iniparser_load(path);

    if (QDINI == NULL) {
        std::cout << "Ini parse failure!" << std::endl;
        return -2;
    }

    return 0;
}

size_t mks_parse_multi_color_ini::inifile_getnsec(){
    if (QDINI == NULL) return 0;
    return iniparser_getnsec(QDINI);
}

size_t mks_parse_multi_color_ini::inifile_getsecnkeys(std::string section){
    if (QDINI == NULL) return 0;
    return iniparser_getsecnkeys(QDINI, section.c_str());
}
/**
 * @description: 从iniFile获取整数值
 * @param section 节名称，例如 [box_connected]
 * @param key 关键字，例如 status
 * @param init_value 初始值，当查询不到值时赋予
 * @return 返回 找到的值 or 初始值
 */
int mks_parse_multi_color_ini::inifile_getint( std::string section, std::string key, int init_value)
{
    std::string sk = section + ":" + key;
    std::lock_guard<std::mutex> lock(QDINI_mutex);
    int value = iniparser_getint(QDINI, sk.c_str(), init_value);
    return value;
}

/**
 * @description: 从iniFile获取字符串值
 * @param section 节名称，例如 [box_connected]
 * @param key 关键字，例如 status
 * @param def  字符串初值，当查找不到的时候赋予这个值
 * @return 返回获取的值
 */
std::string mks_parse_multi_color_ini::inifile_getString(std::string section, std::string key, std::string def)
{
    std::string sk = section + ":" + key;
    std::lock_guard<std::mutex> lock(QDINI_mutex);
    const char *value = iniparser_getstring(QDINI, sk.c_str(), def.c_str());
    return (std::string)value;
}

std::map<std::string, std::string> mks_parse_multi_color_ini::inifile_getsection(std::string section) 
{
    std::map<std::string, std::string> section_map;
    int keys = iniparser_getsecnkeys(QDINI, section.c_str());
    if (keys <= 0) 
    {
        return section_map;
    }

    const char **key_list = new const char *[keys];
    iniparser_getseckeys(QDINI, section.c_str(), key_list);
    for (int i = 0; i < keys; ++i) 
    {
        std::string key(key_list[i] + section.length() + 1);
        std::string value = inifile_getString(section, key, "");
        if (!value.empty()) 
            section_map[key] = value;
    }
    delete[] key_list;
    return section_map;
}

/**
 * @description: 释放iniFile占用内存，调用函数释放对应的内存
 * @param iniFile ini字典
 */
void mks_parse_multi_color_ini::inifile_free()
{
    if (QDINI != nullptr)
    {
        iniparser_freedict(QDINI);
        QDINI = nullptr;
    }
}


mks_parse_multi_color_ini::mks_parse_multi_color_ini(std::string filename)
{
    if (filename == "saved_variables"){
        inifile_load_from_path(MULTI_COLOR_PATH);
    } else if (filename == "officiall_filas_list")
    {
        inifile_load_from_path(FILAS_LIST_PATH);
    }
}

mks_parse_multi_color_ini::~mks_parse_multi_color_ini()
{
    std::lock_guard<std::mutex> lock(QDINI_mutex);
    inifile_free();
}
