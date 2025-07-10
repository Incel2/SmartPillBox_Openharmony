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
    oled_init();
    
    oled_clear();
    int num = (90.0/255)*100;
    printf("num = %d\n",num);
	oled_shownum(0,0,num,2, 16);

	oled_refresh_gram();
    
    while (1) 
    {
        usleep(10*1000); //10ms
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