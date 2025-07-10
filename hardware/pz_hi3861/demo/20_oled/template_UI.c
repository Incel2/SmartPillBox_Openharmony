#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_oled.h"

// 定义常量
#define MENU_ITEMS 8 // 总菜单项数
#define MAX_DISPLAY_ITEMS 4 // 屏幕最多显示的菜单项数
#define MENU_TIME 0
#define MENU_LED 1
#define MENU_INFO 2
#define MENU_SENSOR 3
#define MENU_WIFI 4
#define MENU_BLUETOOTH 5
#define MENU_BRIGHTNESS 6
#define MENU_ABOUT 7
#define TOUCH_KEY_PIN         HI_IO_NAME_GPIO_8
#define TOUCH_KEY_GPIO_FUN    HI_IO_FUNC_GPIO_8_GPIO

// LED 任务（保持不变）
osThreadId_t LED_Task_ID;
void LED_Task(void) {
    led_init();
    while (1) {
        LED(1);
        usleep(200 * 1000);
        LED(0);
        usleep(200 * 1000);
    }
}


//传感器模块初始化
void pz_module_init(void)
{
    hi_gpio_init();                                            // GPIO初始化
    hi_io_set_pull(TOUCH_KEY_PIN, HI_IO_PULL_NONE);                   // 设置GPIO上拉
    hi_io_set_func(TOUCH_KEY_PIN, TOUCH_KEY_GPIO_FUN);                   // 设置IO为GPIO功能
    hi_gpio_set_dir(TOUCH_KEY_PIN, HI_GPIO_DIR_IN);                // 设置GPIO为输入模式
}

void led_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "LEDTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 1024;
    taskOptions.priority = osPriorityNormal;
    LED_Task_ID = osThreadNew((osThreadFunc_t)LED_Task, NULL, &taskOptions);
    if (LED_Task_ID != NULL) {
        printf("ID = %d, Create LED_Task_ID is OK!\n", LED_Task_ID);
    }
}

// OLED 任务（主菜单 UI，支持滑动）
osThreadId_t OLED_Task_ID;
void OLED_Task(void) {
    uint8_t key = 0;
    hi_gpio_value keyt=0;

    int8_t selected_item = 0; // 当前选中的菜单项
    int8_t display_start = 0; // 当前显示的第一个菜单项索引
    const uint8_t *menu_strings[MENU_ITEMS] = {
        (uint8_t *)"Time Display",
        (uint8_t *)"LED Control",
        (uint8_t *)"System Info",
        (uint8_t *)"Sensor Status",
        (uint8_t *)"WiFi Settings",
        (uint8_t *)"Bluetooth",
        (uint8_t *)"Brightness",
        (uint8_t *)"About"
    };
    const uint8_t *time_str = (uint8_t *)"2025-05-18 20:47"; // 模拟时间，匹配当前时间
    const uint8_t *prompt_str = (uint8_t *)"KEY1:Up KEY2:Down";

    oled_init();
    key_init();
    pz_module_init();
    oled_clear();
    oled_refresh_gram();

    while (1) {
        // 绘制 UI 界面
        oled_clear();

        // 显示时间（顶部，第0行）

        // 显示菜单项（第1到4行）
        for (uint8_t i = 0; i < MAX_DISPLAY_ITEMS; i++) {
            uint8_t menu_index = display_start + i;
            if (menu_index >= MENU_ITEMS) break; // 超出菜单项范围，停止绘制
            uint8_t y_pos = i * 16; // 行位置：16, 32, 48, 64
            if (menu_index == selected_item) {
                // 反白显示选中项
                oled_showstring(0, y_pos, menu_strings[menu_index], 16, 0); // mode=0，反白
            } else {
                // 正常显示未选中项
                oled_showstring(0, y_pos, menu_strings[menu_index], 16, 1); // mode=1，正常
            }
        }

        // 显示按键提示（底部，第5行）

        // 更新屏幕
        oled_refresh_gram();

        // 按键处理
        key = key_scan(1);
        hi_gpio_get_input_val(TOUCH_KEY_PIN,&keyt);

        if (key == KEY1_PRESS) {
            // 向上移动
            if(selected_item!=0)selected_item = (selected_item - 1 + MENU_ITEMS) % MENU_ITEMS;
            if (selected_item < display_start) {
                // 选中项移到屏幕外（顶部），向上滑动
                display_start = selected_item;
            }
        } else if (key == KEY2_PRESS) {
            // 向下移动
            if(selected_item!=MENU_ITEMS-1)selected_item = (selected_item + 1) % MENU_ITEMS;
            if (selected_item >= display_start + MAX_DISPLAY_ITEMS) {
                // 选中项移到屏幕外（底部），向下滑动
                display_start = selected_item - MAX_DISPLAY_ITEMS + 1;
            }
        }else if(keyt==1){
            if(selected_item!=0)selected_item = (selected_item - 1 + MENU_ITEMS) % MENU_ITEMS;
            if (selected_item < display_start) {
                // 选中项移到屏幕外（顶部），向上滑动
                display_start = selected_item;
            }
        }


        // 确保 display_start 不越界
        if (display_start < 0) display_start = 0;
        if (display_start > MENU_ITEMS - MAX_DISPLAY_ITEMS) {
            display_start = MENU_ITEMS - MAX_DISPLAY_ITEMS;
        }

        usleep(100 * 1000); // 100ms 帧间隔
    }
}

void oled_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "oledTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 8 * 2048;
    taskOptions.priority = osPriorityAboveNormal;
    OLED_Task_ID = osThreadNew((osThreadFunc_t)OLED_Task, NULL, &taskOptions);
    if (OLED_Task_ID != NULL) {
        printf("ID = %d, Task Create OK!\n", OLED_Task_ID);
    }
}

static void ui_demo(void) {
    printf("普中-Hi3861开发板--OLED 滑动菜单 UI 实验\r\n");
    led_task_create();
    oled_task_create();
}
SYS_RUN(ui_demo);