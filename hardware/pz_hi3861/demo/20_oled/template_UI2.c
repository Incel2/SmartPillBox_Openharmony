#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_oled.h"
#include "bsp_beep.h"

// 定义常量
#define MENU_ITEMS 8 // 总菜单项数
#define MAX_DISPLAY_ITEMS 4 // 屏幕最多显示的菜单项数
#define MENU_TIME 0
#define MENU_LED 1
#define MENU_INFO 2
#define MENU_BEEP 3
#define MENU_WIFI 4
#define MENU_BLUETOOTH 5
#define MENU_BRIGHTNESS 6
#define MENU_ABOUT 7
#define TOUCH_KEY_PIN         HI_IO_NAME_GPIO_8
#define TOUCH_KEY_GPIO_FUN    HI_IO_FUNC_GPIO_8_GPIO

uint8_t brightness = 0xFF;
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

osThreadId_t BEEP_Task_ID; //任务ID

void BEEP_Task(void)
{
    uint16_t cnt=100,time=100;
    key_init();//按键初始化
    beep_init();//BEEP初始化
    
    while (1) 
    {
        
        uint8_t key = key_scan(1);
        
        if(key==KEY1_PRESS){
            printf("key1 press\n");
            if(time<2000){
                time+=100;
                cnt-=100;
            }
            beep_alarm(cnt,time);
        }
        if(key==KEY2_PRESS){
            printf("key2 press\n");
            if(time>100){
                time-=100;
                cnt+=100;
            }
            beep_alarm(cnt,time);
        }
        //usleep(1000*1000);
    }
}
//蜂鸣器任务创建

osThreadId_t ADJUST_Task_ID;

void ADJUST_Task(void){
    key_init();//按键初始化
    oled_init();
    uint8_t last_brightness = brightness;
    uint8_t key = 0;
    while(1){
        key = key_scan(1);
        if(key==KEY1_PRESS){
            if(brightness<0xFF){
                brightness+=8;
                oled_wr_byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
                oled_wr_byte(brightness,OLED_CMD);

            }
        }
        else if(key==KEY2_PRESS){
            if(brightness>0x00){
                brightness-=8;
                oled_wr_byte(0x81,OLED_CMD); //对比度设置
                oled_wr_byte(brightness,OLED_CMD);
            }
        }
        if(last_brightness!=brightness){
            last_brightness = brightness;
            printf("brightness = %d\n",brightness);
            
            //oled_shownum(96,16,brightness/256,3,16);
            oled_refresh_gram();
        }
        usleep(100*1000);
    }
}

void adjust_task_create(void){
    osThreadAttr_t taskOptions;
    taskOptions.name = "ADJUSTTask";            // 任务的名字
    taskOptions.attr_bits = 0;               // 属性位
    taskOptions.cb_mem = NULL;               // 堆空间地址
    taskOptions.cb_size = 0;                 // 堆空间大小
    taskOptions.stack_mem = NULL;            // 栈空间地址
    taskOptions.stack_size = 1024;           // 栈空间大小 单位:字节
    taskOptions.priority = osPriorityNormal; // 任务的优先级
    ADJUST_Task_ID = osThreadNew((osThreadFunc_t)ADJUST_Task, NULL, &taskOptions); // 创建任务1
    if (ADJUST_Task_ID!= NULL){
        printf("ID = %d, Create ADJUST_Task_ID is OK!\n", ADJUST_Task_ID);
    }
}
void beep_task_create(void)
{
    osThreadAttr_t taskOptions;
    taskOptions.name = "BEEPTask";            // 任务的名字
    taskOptions.attr_bits = 0;               // 属性位
    taskOptions.cb_mem = NULL;               // 堆空间地址
    taskOptions.cb_size = 0;                 // 堆空间大小
    taskOptions.stack_mem = NULL;            // 栈空间地址
    taskOptions.stack_size = 1024;           // 栈空间大小 单位:字节
    taskOptions.priority = osPriorityNormal; // 任务的优先级

    BEEP_Task_ID = osThreadNew((osThreadFunc_t)BEEP_Task, NULL, &taskOptions); // 创建任务1
    if (BEEP_Task_ID != NULL)
    {
        printf("ID = %d, Create BEEP_Task_ID is OK!\n", BEEP_Task_ID);
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
    hi_gpio_value keyt = 0;
    int8_t selected_item = 0; // 当前选中的菜单项
    int8_t display_start = 0; // 当前显示的第一个菜单项索引
    uint8_t in_submenu = 0; // 是否在子界面（0: 主菜单, 1: 子界面）
    int8_t current_submenu = -1; // 当前子界面索引
    const uint8_t *menu_strings[MENU_ITEMS] = {
        (uint8_t *)"Time Display",
        (uint8_t *)"LED Control",
        (uint8_t *)"System Info",
        (uint8_t *)"Beep Control",
        (uint8_t *)"WiFi Settings",
        (uint8_t *)"Bluetooth",
        (uint8_t *)"Brightness",
        (uint8_t *)"About"
    };
    const uint8_t *submenu_titles[MENU_ITEMS] = {
        (uint8_t *)"Time Display Page",
        (uint8_t *)"LED Control Page",
        (uint8_t *)"System Info Page",
        (uint8_t *)"Beep Control Page",
        (uint8_t *)"WiFi Settings Page",
        (uint8_t *)"Bluetooth Page",
        (uint8_t *)"Brightness Page",
        (uint8_t *)"About Page"
    };
    const uint8_t *submenu_contents[MENU_ITEMS] = {
        (uint8_t *)"Time: 2025-05-18 22:50",
        (uint8_t *)"LED: Blinking",
        (uint8_t *)"Device: Hi3861",
        (uint8_t *)"Sensors: N/A",
        (uint8_t *)"WiFi: Disconnected",
        (uint8_t *)"Bluetooth: Off",
        (uint8_t *)"Brightness: ",
        (uint8_t *)"Version: 1.0"
    };
    const uint8_t *submenu_prompt = (uint8_t *)"Touch:Back";

    oled_init();
    key_init();
    pz_module_init();
    oled_clear();
    oled_refresh_gram();

    osThreadSuspend(LED_Task_ID);
    osThreadSuspend(BEEP_Task_ID);
    osThreadSuspend(ADJUST_Task_ID);

    while (1) {
        // 绘制 UI 界面
        oled_clear();

        if (!in_submenu) {
            // 主菜单界面
            // 显示菜单项（第0到3行）
            for (uint8_t i = 0; i < MAX_DISPLAY_ITEMS; i++) {
                uint8_t menu_index = display_start + i;
                if (menu_index >= MENU_ITEMS) break; // 超出菜单项范围，停止绘制
                uint8_t y_pos = i * 16; // 行位置：0, 16, 32, 48
                if (menu_index == selected_item) {
                    // 反白显示选中项
                    oled_showstring(0, y_pos, menu_strings[menu_index], 16, 0); // mode=0，反白
                } else {
                    // 正常显示未选中项
                    oled_showstring(0, y_pos, menu_strings[menu_index], 16, 1); // mode=1，正常
                }
            }
        } else {
            // 子界面
            // 显示标题（第0行）
            oled_showstring(0, 0, submenu_titles[current_submenu], 16, 1);
            // 显示内容（第1行）
            oled_showstring(0, 16, submenu_contents[current_submenu], 16, 1);
            if(current_submenu==MENU_BRIGHTNESS){
                oled_shownum(80,16,brightness/256*100,2,16);
                //printf("brightness = %d\n",brightness);
            }
            // 显示返回提示（第2行）
            oled_showstring(0, 32, submenu_prompt, 16, 1);
            
        }

        // 更新屏幕
        oled_refresh_gram();

        // 按键处理
        key = key_scan(1);
        hi_gpio_get_input_val(TOUCH_KEY_PIN, &keyt);

        if (!in_submenu) {
            // 主菜单按键处理
            if (key == KEY1_PRESS) {
                // 向上移动
                selected_item = (selected_item - 1 + MENU_ITEMS) % MENU_ITEMS;
                if (selected_item < display_start) {
                    // 选中项移到屏幕外（顶部），向上滑动
                    display_start = selected_item;
                }
            } else if (key == KEY2_PRESS) {
                // 向下移动
                selected_item = (selected_item + 1) % MENU_ITEMS;
                if (selected_item >= display_start + MAX_DISPLAY_ITEMS) {
                    // 选中项移到屏幕外（底部），向下滑动
                    display_start = selected_item - MAX_DISPLAY_ITEMS + 1;
                }
            } else if (keyt == 1) {
                // 进入子界面
                in_submenu = 1;
                current_submenu = selected_item;
                if(current_submenu==MENU_LED){
                    osThreadResume(LED_Task_ID);
                }else if(current_submenu==MENU_BEEP){
                    osThreadResume(BEEP_Task_ID);
                }
                else if(current_submenu==MENU_BRIGHTNESS){
                    osThreadResume(ADJUST_Task_ID);
                }
                //osThreadSuspend(OLED_Task_ID);
            }
        } else {
            // 子界面按键处理
            if (keyt == 1) {
                // 返回主菜单
                in_submenu = 0;
                if(current_submenu==MENU_LED){
                    osThreadSuspend(LED_Task_ID);
                }else if(current_submenu==MENU_BEEP){
                    osThreadSuspend(BEEP_Task_ID);
                }else if(current_submenu==MENU_BRIGHTNESS){
                    osThreadSuspend(ADJUST_Task_ID);
                }
                current_submenu = -1;
                //osThreadResume(OLED_Task_ID);
            }
            if(key==KEY1_PRESS){
                
            }else if(key==KEY2_PRESS){
                
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
    beep_task_create();
    adjust_task_create();
}
SYS_RUN(ui_demo);