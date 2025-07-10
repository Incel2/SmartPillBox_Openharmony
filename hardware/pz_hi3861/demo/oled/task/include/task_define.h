#ifndef __TASK_DEFINE_H__
#define __TASK_DEFINE_H__

#include "cmsis_os2.h"
#include "launcher.h"
#include "systime_task.h"
#include "timer_task.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "hi_types_base.h"

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;

// // 定义菜单常量
// #define MENU_ITEMS 8 // 总菜单项数
// #define MAX_DISPLAY_ITEMS 4 // 屏幕最多显示的菜单项数
// #define MENU_TIME 0
// #define MENU_LED 1
// #define MENU_INFO 2
// #define MENU_BEEP 3
// #define MENU_WIFI 4
// #define MENU_BLUETOOTH 5
// #define MENU_BRIGHTNESS 6
// #define MENU_ABOUT 7

// 定义蓝牙常量
#define HI_SUCCESS 0
#define UART_NUM HI_UART_IDX_2
#define UART_TX_PIN HI_IO_NAME_GPIO_11 // IO11 作为 UART TX
#define UART_RX_PIN HI_IO_NAME_GPIO_12 // IO12 作为 UART RX
#define UART_BAUDRATE 9600

// 定义按键常量
#define KEY1_PIN         HI_IO_NAME_GPIO_5
#define KEY1_GPIO_FUN    HI_IO_FUNC_GPIO_5_GPIO
#define KEY2_PIN         HI_IO_NAME_GPIO_6
#define KEY2_GPIO_FUN    HI_IO_FUNC_GPIO_6_GPIO
#define KEY3_PIN         HI_IO_NAME_GPIO_8
#define KEY3_GPIO_FUN    HI_IO_FUNC_GPIO_8_GPIO
// 霍尔传感器DO引脚连接的GPIO
#define HALL_DO_GPIO     HI_GPIO_IDX_13// 假设DO接GPIO2（根据实际接线修改）

#define KEY1_PRESS  1
#define KEY2_PRESS  2
#define KEY3_PRESS  3
#define KEY_UNPRESS 0 

#define MODE1 1 //主模式
#define MODE2 2 //报警
#define  MODE3 3 //亮度调整
#define MODE4 4 //时间
#define MODE5 5 //音量调整
#define MODE6 6 //成功提示
#define MODE7 7 //药物
#define MODE8 8 //关于
#define MODE9 9 //寻找

#define MAX_ITEMS 10

//信号量定义
extern osSemaphoreId_t SEMAPHORE_KEY1; //主菜单key1按下
extern osSemaphoreId_t SEMAPHORE_KEY2; //主材带key2按下
extern osSemaphoreId_t SEMAPHORE_KEY3; //报警模块key1或key2按下
extern osSemaphoreId_t SEMAPHORE_IN; //报警模块key1或key2按下
extern osSemaphoreId_t SEMAPHORE_OUT; //报警模块key1或key2按下
extern osSemaphoreId_t SEMAPHORE_BRIGHTNESS_UP; //亮度调整模块key1或key2按下
extern osSemaphoreId_t SEMAPHORE_BRIGHTNESS_DOWN; //亮度调整模块key1或key2按下
extern osSemaphoreId_t SEMAPHORE_VOLUME_UP; //音量调整模块key1或key2按下
extern osSemaphoreId_t SEMAPHORE_VOLUME_DOWN; //音量调整模块key1或key2按下

extern hi_u64 press_start_time; //按键按下时间
extern hi_bool key_pressed_flag_key1; //按键是否按下标志
extern hi_bool key_pressed_flag_key2; 


extern uint8_t brightness;
extern uint8_t mode;
extern uint8_t volume;
extern bool remind_flag;
extern bool open_flag;
extern char* remind_medicine;
extern Launcher launcher;
extern SystemTime systemTime;
extern char timeStr[9];
//任务ID
extern osThreadId_t OLED_Task_ID;
extern osThreadId_t BEEP_Task_ID; 
extern osThreadId_t LED_Task_ID;
extern osThreadId_t ADJUST_Task_ID;
extern osThreadId_t BluetoothTask_ID;

#endif