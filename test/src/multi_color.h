/*
 * @Description: 
 * @Author: oys111-n
 * @Date: 2025-03-11 16:21:18
 * @LastEditTime: 2025-03-13 15:56:58
 * @LastEditors: oys111-n
 */
#ifndef QIDI_MULTI_COLOR_H
#define QIDI_MULTI_COLOR_H

/*********************
 *      INCLUDES
 *********************/
#include <array>
#include <string>
#include <cstdint>
#include <map>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <set>
#include <algorithm>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include <stdexcept>
#include <regex>
#include <cctype>
#include <iomanip>
#include <any>
#include "mks_parse_multi_color_ini.h"
#include "multi_color_constants_and_structs.h"

class MultiColor
{
public:
    // 获取唯一实例的静态方法
    static MultiColor& getInstance();

    // 队列
    
    /**
     *  从队列中取出元素
     * @return 如果队列为空，返回 nullptr
     */
    QueueElement pop();

    // get 方法

    /**
     * 获取官方耗材表
     * @return   OfficalFilasList - std::vector<Filas> FilasMap，std::vector<Color> colorsMap;
     */
    OfficalFilasList getOfficalFilasList();
    /**
     * 获取料架耗材信息
     * @return Slot 
     */
    Slot getRackSlot();
    /**
     * 获取盒子耗材信息列表
     * @return  0-box_count*4, 至多0-15
     */
    std::unordered_map<int, Slot> getSlots();
    // 获取盒子数
    size_t getBoxCount();
    /**
     * 获取当前同步耗材
     * @return  eg."slot-1","slot0"
     */
    std::string getSlotSync();
    /**
     * 获取当前value_t对应料槽列表
     * @return  eg."slot0"
     */
    std::unordered_map<int, std::string> getValueSlots();

    // 线程方法

    // 开启定时更新线程
    void multiColorThreadStart();
    // 关闭定时更新线程
    void multiColorThreadStop();
    // 工具方法
    /**
     * 将数字转换为代码字符串
     * @return eg.0 -> "1A", 15 -> "4D", num 的有效范围为 0~15
     */
    static std::string numberToCodeConversion(int num);
    /**
     * 将代码字符串转换回数字
     * @param 期望输入的字符串长度为2，且第一个字符为数字，第二个字符为'A'~'D'
     * @return eg."1A" -> 0, "4D" -> 15 
     */
    static size_t convertCodeToNumber(std::string str);

private:
    // 数据变量
    OfficalFilasList officalFilasList;
    std::unordered_map<int, Slot> slots;
    Slot rackSlot;
    std::string slotSync;
    size_t boxCount;
    std::unordered_map<int, std::string> valueSlots;
    // 队列
    std::queue<QueueElement> queue_;
    std::shared_mutex queue_mtx_;
    std::condition_variable cond_var_;
    // 线程变量
    std::thread update_thread;
    std::atomic<bool> running{false}; // 线程运行标志
    std::mutex thread_mutex;
    std::condition_variable cv;

    // 向队列中添加元素
    void push(std::string key, std::any value);

    // 工具
    std::string getTextColor(std::string hex);

    template<typename T>
    void checkChange(const std::string& key, const T& oldVal, const T& newVal){
        if (oldVal != newVal) { 
            push(key, newVal);
        }
    }

    Slot checkSlotChanges(
        std::unique_ptr<mks_parse_multi_color_ini>& parser, 
        std::string name, std::string *keyName);

    // 线程方法
    void multiColorThreadRun();

    // 读取save_variables信息
    void updateSaveVariables();

    // 读取officiall_filas_list信息
    void updateOfficalFilasList();

    // 构造析构
    MultiColor(/* args */);
    ~MultiColor();
    // 删除拷贝构造函数和赋值运算符
    MultiColor(const MultiColor&) = delete;
    MultiColor& operator=(const MultiColor&) = delete;
};

#endif
