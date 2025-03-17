#ifndef MKS_PARSE_MULTI_COLOR_INI_H
#define MKS_PARSE_MULTI_COLOR_INI_H

#include <map>

#include "dictionary.h"
#include "iniparser.h"
// #include "utils.h"
// #include "log.h"

#define CONFIG_PATH "/home/mks/printer_data/config/"

#define MULTI_COLOR_PATH  "C:\\Users\\51550\\Desktop\\lvgl\\test\\saved_variables.cfg"

// #define MULTI_COLOR_PATH  "/home/mks/printer_data/config/saved_variables.cfg"

#define FILAS_LIST_PATH  "C:\\Users\\51550\\Desktop\\lvgl\\test\\officiall_filas_list.cfg"

// #define FILAS_LIST_PATH  "/home/mks/printer_data/config/officiall_filas_list.cfg"

class mks_parse_multi_color_ini
{
private:
    /* data */
    dictionary *QDINI;
    std::mutex QDINI_mutex;
    static bool file_exists(const char *path);

public:
    size_t inifile_getnsec();
    size_t inifile_getsecnkeys(std::string section);
    int inifile_load_from_path(const char *path);
    int inifile_getint(std::string section, std::string key, int init_value);
    std::string inifile_getString(std::string section, std::string key, std::string def);
    std::map<std::string, std::string> inifile_getsection(std::string section);
    void inifile_free();
    mks_parse_multi_color_ini(std::string filename);
    ~mks_parse_multi_color_ini();
};

#endif