#include <task_define.h>
#include <stdio.h>
#include "time.h"
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "bsp_key.h"
#include "bsp_led.h"


#include "beep_task.h"
#include "timer_task.h"
#include "exit_task.h"
#include "key_task.h"
#include "remind_task.h"
#include "adjust_task.h"
#include "oled_task.h"
#include "blue_task.h"
#include "systime_task.h"
#include "hall_task.h"

// LED 任务（保持不变）

void LED_Task(void) {
    led_init();
    while (1) {
        LED(1);
        usleep(200 * 1000);
        LED(0);
        usleep(200 * 1000);
    }
}



// 声明任务ID
osThreadId_t OLED_Task_ID;
osThreadId_t REFRESH_Task_ID;
osThreadId_t BEEP_Task_ID; 
osThreadId_t LED_Task_ID;
osThreadId_t TIMER_Task_ID;
osThreadId_t EXIT_Task_ID;
osThreadId_t KEY_Task_ID;
osThreadId_t REMIND_Task_ID;
osThreadId_t ADJUST_Task_ID;
osThreadId_t BluetoothTask_ID;
osThreadId_t SYSTIME_Task_ID;
osThreadId_t HALL_Task_ID;

// 定义全局变量
uint8_t brightness = 0xFF;
uint8_t mode = MODE1;
char timeStr[9];

void adjust_task_create(void){
    osThreadAttr_t taskOptions;
    taskOptions.name = "ADJUSTTask";            // 任务的名字
    taskOptions.attr_bits = 0;               // 属性位
    taskOptions.cb_mem = NULL;               // 堆空间地址
    taskOptions.cb_size = 0;                 // 堆空间大小
    taskOptions.stack_mem = NULL;            // 栈空间地址
    taskOptions.stack_size = 4*1024;           // 栈空间大小 单位:字节
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

void timer_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "TIMERTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 1024;
    taskOptions.priority = osPriorityNormal;
    TIMER_Task_ID = osThreadNew((osThreadFunc_t)TIMER_Task, NULL, &taskOptions);
    if (TIMER_Task_ID != NULL) {
        printf("ID = %d, Create TIMER_Task_ID is OK!\n", TIMER_Task_ID);
    }
}

void exit_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "EXITTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 1024;
    taskOptions.priority = osPriorityNormal;
    EXIT_Task_ID = osThreadNew((osThreadFunc_t)EXTI_Task, NULL, &taskOptions);
    if (EXIT_Task_ID != NULL) {
        printf("ID = %d, Create EXIT_Task_ID is OK!\n", EXIT_Task_ID);
    }
}

void key_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "KEYTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 1024;
    taskOptions.priority = osPriorityNormal;
    KEY_Task_ID = osThreadNew((osThreadFunc_t)KEY_Task, NULL, &taskOptions);
    if (KEY_Task_ID != NULL) {
        printf("ID = %d, Create KEY_Task_ID is OK!\n", KEY_Task_ID);
    }
}

void remind_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "REMINDTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 1024;
    taskOptions.priority = osPriorityNormal;
    REMIND_Task_ID = osThreadNew((osThreadFunc_t)REMIND_Task, NULL, &taskOptions);
    if (REMIND_Task_ID != NULL) {
        printf("ID = %d, Create REMIND_Task_ID is OK!\n", REMIND_Task_ID);
    }
}

//传感器模块初始化

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


void oled_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "oledTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 2048;
    taskOptions.priority = osPriorityNormal;
    OLED_Task_ID = osThreadNew((osThreadFunc_t)OLED_Task, NULL, &taskOptions);
    if (OLED_Task_ID != NULL) {
        printf("ID = %d, Task Create OK!\n", OLED_Task_ID);
    }
}

void refresh_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "refreshTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 2 * 2048;
    taskOptions.priority = osPriorityAboveNormal;
    REFRESH_Task_ID = osThreadNew((osThreadFunc_t)REFRESH_Task, NULL, &taskOptions);
    if (OLED_Task_ID != NULL) {
        printf("ID = %d, Task Create OK!\n", REFRESH_Task_ID);
    }
}

void systime_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "systimeTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 4*1024;
    taskOptions.priority = osPriorityNormal;
    SYSTIME_Task_ID = osThreadNew((osThreadFunc_t)SYSTIME_Task, NULL, &taskOptions);
    if (SYSTIME_Task_ID != NULL) {
        printf("ID = %d, SYSTIME_Task Create OK!\n", SYSTIME_Task_ID);
    }
}

void hall_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "hallTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 1024;
    taskOptions.priority = osPriorityNormal;
    HALL_Task_ID = osThreadNew((osThreadFunc_t)HALL_Task, NULL, &taskOptions);
    if (HALL_Task_ID != NULL) {
        printf("ID = %d, HALL_Task Create OK!\n", HALL_Task_ID);
    }
}

void blue_task_create(void)
{
    osThreadAttr_t taskOptions;

    // 创建 BluetoothTask
    taskOptions.name = "BluetoothTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 4*1024; // 栈空间大小
    taskOptions.priority = osPriorityNormal;
    BluetoothTask_ID = osThreadNew((osThreadFunc_t)BluetoothTask, NULL, &taskOptions);
    if (BluetoothTask_ID != NULL)
    {
        printf("ID = %d, Create BluetoothTask_ID is OK!\r\n", BluetoothTask_ID);
    }
}


static void main_demo(void) {
    printf("智能药盒\r\n");
    //led_task_create();
    timer_task_create();
    oled_task_create();
    refresh_task_create();
    exit_task_create();
    // key_task_create();
    remind_task_create();
    //beep_task_create();
    adjust_task_create();
    blue_task_create();
    systime_task_create();
    hall_task_create();
}

SYS_RUN(main_demo);