
#include <unistd.h>
#include "task_define.h"
#include "bsp_oled.h"
#include <stdio.h>
#include <stdbool.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

SystemTime systemTime = {2025,1,1,12,0,0,0};

// 判断闰年
bool is_leap_year(uint16_t year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

// 每月天数
const uint8_t days_in_month[] = {
    0, 31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31
};

void SYSTIME_Task(void)
{
    oled_init();
    while (1)
    {
        // osDelay(1000);  // 延迟 1 秒
        usleep(1000000);

        // 更新时间
        systemTime.second++;
        if (systemTime.second >= 60) {
            systemTime.second = 0;
            systemTime.minute++;
        }

        if (systemTime.minute >= 60) {
            systemTime.minute = 0;
            systemTime.hour++;
        }

        uint8_t max_day = days_in_month[systemTime.month];
        if (systemTime.month == 2 && is_leap_year(systemTime.year)) {
            max_day = 29;
        }

        if (systemTime.day > max_day) {
            systemTime.day = 1;
            systemTime.month++;
        }

        if (systemTime.month > 12) {
            systemTime.month = 1;
            systemTime.year++;
        }

        printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
               systemTime.year, systemTime.month, systemTime.day,
               systemTime.hour, systemTime.minute, systemTime.second);
        // 你可以在这里调用 OLED 显示函数、打印函数等
        // oled_clear();
        // oled_showstring(0, 15, "Hello World:",8);
        // char time_str[20];
        // sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d",
        //        systemTime.year, systemTime.month, systemTime.day,
        //        systemTime.hour, systemTime.minute, systemTime.second);
        // oled_showstring(32, 48, time_str,8);
        // oled_refresh_gram();

    }
}

void set_system_time(uint16_t year, uint8_t month, uint8_t day,
                     uint8_t hour, uint8_t minute, uint8_t second)
{
    systemTime.year = year;
    systemTime.month = month;
    systemTime.day = day;
    systemTime.hour = hour;
    systemTime.minute = minute;
    systemTime.second = second;
}
