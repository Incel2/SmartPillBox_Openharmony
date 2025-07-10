#include "task_define.h"
#include "bsp_key.h"
#include "bsp_oled.h"
#include "cmsis_os2.h"
#include "ohos_init.h"
#include "time.h"
#include "menu.h"

extern osThreadId_t LED_Task_ID;
extern osThreadId_t ADJUST_Task_ID;
extern uint8_t brightness;

// const uint8_t *menu_strings[MENU_ITEMS_NUM] = {
//       (uint8_t *)"Time Display",
//       (uint8_t *)"LED Control",
//       (uint8_t *)"System Info",
//       (uint8_t *)"Beep Control",
//       (uint8_t *)"WiFi Settings",
//       (uint8_t *)"Bluetooth",
//       (uint8_t *)"Brightness",
//       (uint8_t *)"About"
//     };
    
// 菜单文字
const uint8_t *menu_strings[MENU_ITEMS_NUM] = {
    (uint8_t *)"Time Display",
    (uint8_t *)"Brightness"
};

const uint8_t *submenu_titles[MENU_ITEMS_NUM] = {
    (uint8_t *)"LED Control Page",
    (uint8_t *)"Brightness Page"
};

const uint8_t *submenu_contents[MENU_ITEMS_NUM] = {
    (uint8_t *)"LED: Blinking",
    (uint8_t *)"Set Brightness"
};

const uint8_t *submenu_prompt = (uint8_t *)"Press KEY3 to return";
struct tm timeinfo;



// TIME 子菜单界面
void draw_time_submenu() {
    oled_clear();  // 清空显示屏

    // 获取当前系统时间
    timeinfo.tm_year = 2025 - 1900;  // 年份从1900年开始
    timeinfo.tm_mon = 5 - 1;         // 月份从0开始（0表示1月）
    timeinfo.tm_mday = 18;           // 日
    timeinfo.tm_hour = 22;           // 小时
    timeinfo.tm_min = 50;            // 分钟
    timeinfo.tm_sec = 0;             // 秒
    timeinfo.tm_isdst = -1;          // 自动判断夏令时

    // 将 struct tm 转换为 time_t
    time_t now = mktime(&timeinfo); // 转换为本地时间

    // 显示小时、分钟和秒
    uint8_t x = 10, y = 16;  // 初始位置

    // 显示小时
    oled_shownum(x, y, timeinfo.tm_hour, 2, 24);  // 显示小时，2位数
    x += 24;  // 右移显示位置，给冒号腾出空间

    // 显示冒号 ":"
    oled_showchar(x, y + 3, ':', 24, 1);
    x += 24 / 2;  // 冒号稍微缩小，调整位置
 
    // 显示分钟
    oled_shownum(x, y, timeinfo.tm_min, 2, 24);  // 显示分钟，2位数
    x += 24;  // 右移显示位置，给冒号腾出空间

    // 显示冒号 ":"
    oled_showchar(x, y + 3, ':', 24, 1);
    x += 24 / 2;  // 冒号稍微缩小，调整位置

    // 显示秒钟
    oled_shownum(x, y, timeinfo.tm_sec, 2, 24);  // 显示秒钟，2位数

    oled_refresh_gram();  // 更新显示
}

// 亮度调节子菜单界面
void draw_brightness_submenu() {
    oled_clear();
    oled_showstring(0, 0, submenu_titles[MENU_BRIGHTNESS], 16, 1);
    oled_showstring(0, 16, submenu_contents[MENU_BRIGHTNESS], 16, 1);
    oled_shownum(86, 16, (brightness * 100) / 255, 2, 16);
    oled_showstring(0, 32, submenu_prompt, 16, 1);
}

// 子菜单绘制调度器
void draw_submenu(int submenu_id) {
    switch (submenu_id) {
        case MENU_TIME_ITEM:
            draw_time_submenu();
            break;
        case MENU_BRIGHTNESS_ITEM:
            draw_brightness_submenu();
            break;
    }
}

// 子菜单按键处理器
void handle_submenu_keys(int submenu_id, int key) {
    if (submenu_id == MENU_BRIGHTNESS) {
        if (key == KEY1_PRESS && brightness < 255) {
            brightness += 15;
        } else if (key == KEY2_PRESS && brightness > 0) {
            brightness -= 15;
        }
    }
}

void OLED_Task(void) {
    uint8_t key = 0;
    int8_t selected_item = 0;
    int8_t display_start = 0;
    uint8_t in_submenu = 0;
    int8_t current_submenu = -1;

    oled_init();
    key_init();
    oled_clear();
    oled_refresh_gram();

    osThreadSuspend(LED_Task_ID);
    osThreadSuspend(ADJUST_Task_ID);

    while (1) {
        // 确保每次刷新主菜单
        if (!in_submenu) {
            // printf("Main menu selected item: %d\n", selected_item);
            draw_main_menu(selected_item, display_start);
            oled_refresh_gram();  // 确保刷新屏幕
        } else {
            draw_submenu(current_submenu);
            oled_refresh_gram();
        }

        // 获取按键输入
        key = key_scan(1); // 此函数需要确保在每次循环时返回最新的按键状态

        // 按键处理：主菜单和子菜单的不同逻辑
        if (!in_submenu) {
            // printf("Key pressed: %d\n", key);
            if (key == KEY1_PRESS) {
                selected_item = (selected_item - 1 + MENU_ITEMS_NUM) % MENU_ITEMS_NUM;
                if (selected_item < display_start) {
                    display_start = selected_item;
                }
            } else if (key == KEY2_PRESS) {
                selected_item = (selected_item + 1) % MENU_ITEMS_NUM;
                if (selected_item >= display_start + MAX_DISPLAY_ITEMS) {
                    display_start = selected_item - MAX_DISPLAY_ITEMS + 1;
                }
            } else if (key == KEY3_PRESS) {
                // 进入子菜单
                in_submenu = 1;
                current_submenu = selected_item;

                if (current_submenu == MENU_TIME_ITEM ) {
                    osThreadResume(LED_Task_ID);
                } else if (current_submenu == MENU_BRIGHTNESS_ITEM) {
                    osThreadResume(ADJUST_Task_ID);
                }
            }
        } else {
            // 子菜单按键处理
            // printf("Submenu key pressed: %d\n", key);
            handle_submenu_keys(current_submenu, key);

            if (key == KEY3_PRESS) {
                // 返回主菜单
                in_submenu = 0;
                if (current_submenu == MENU_TIME_ITEM) {
                    osThreadSuspend(LED_Task_ID);
                } else if (current_submenu == MENU_BRIGHTNESS_ITEM) {
                    osThreadSuspend(ADJUST_Task_ID);
                }
                current_submenu = -1;            }
        }

        // 防止菜单过多导致界面显示不完整
        if (MENU_ITEMS_NUM <= MAX_DISPLAY_ITEMS) {
            // 如果所有项目都能在屏幕上显示，display_start 应始终为 0
            display_start = 0;
        } else {
            // 如果项目不能完全显示（需要滚动），则应用滚动逻辑和边界检查
            // 确保 display_start 不会向上滚动太多（变为负数）
            if (display_start < 0) {
                display_start = 0;
            }
            // 确保 display_start 不会向下滚动太多
            if (display_start > MENU_ITEMS_NUM - MAX_DISPLAY_ITEMS) {
                display_start = MENU_ITEMS_NUM - MAX_DISPLAY_ITEMS;
            }
        }

        // usleep(100 * 1000); // 等待100ms，防止过于频繁刷新
    }
    }

