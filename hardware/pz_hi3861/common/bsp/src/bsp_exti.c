/**
 ****************************************************************************************************
 * @file        bsp_exti.c
 * @author      普中科技
 * @version     V1.0
 * @date        2024-06-05
 * @brief       外部中断实验
 * @license     Copyright (c) 2024-2034, 深圳市普中科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:普中-Hi3861
 * 在线视频:https://space.bilibili.com/2146492485
 * 公司网址:www.prechin.cn
 * 购买地址:
 *
 */
#include "bsp_exti.h"
#include <unistd.h>
#include "bsp_led.h"
#include "task_define.h"
#include "menu.h"
#include <stdio.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

osSemaphoreId_t SEMAPHORE_KEY1;
osSemaphoreId_t SEMAPHORE_KEY2;
osSemaphoreId_t SEMAPHORE_KEY3;
osSemaphoreId_t SEMAPHORE_IN;
osSemaphoreId_t SEMAPHORE_OUT;
osSemaphoreId_t SEMAPHORE_BRIGHTNESS_DOWN;
osSemaphoreId_t SEMAPHORE_BRIGHTNESS_UP;
osSemaphoreId_t SEMAPHORE_VOLUME_DOWN;
osSemaphoreId_t SEMAPHORE_VOLUME_UP;

volatile hi_u64 last_press_time = 0;
volatile hi_u8  click_count = 0;
hi_bool key_pressed_flag_key1 = HI_FALSE;
hi_bool key_pressed_flag_key2 = HI_FALSE;
hi_u64 press_start_time = 0;
#define LONG_PRESS_TIME     2000 // 长按 2000ms
#define DOUBLE_CLICK_TIME    500 // 双击间隔 500ms


// key1按键回调函数
hi_void key1_exti_callback(void)
{
    static uint8_t key1=0;
    //usleep(10*1000);//消抖
    hi_gpio_get_input_val(KEY1_PIN,&key1);
    if(key1==0)
    {
        press_start_time = hi_get_milli_seconds();
        key_pressed_flag_key1 = HI_TRUE;
    }
    // if(mode==MODE1)
    // {
    //     if(key1==0)
    //     {
    //         printf("down");
    //         osSemaphoreRelease(SEMAPHORE_KEY1);
    //     }
    // }
    // else if(mode==MODE2)
    // {
    //     if(key1==0)
    //     {
    //         osSemaphoreRelease(SEMAPHORE_KEY3);
    //     }
    // }
}


//key2按键回调函数
hi_void key2_exti_callback(void)
{
    static uint8_t key2=0;
    //usleep(10*1000);//消抖
    hi_gpio_get_input_val(KEY2_PIN,&key2);
    if(key2==0)
    {
        press_start_time = hi_get_milli_seconds();
        key_pressed_flag_key2 = HI_TRUE;
    }
    // if(mode==MODE1)
    // {
    //     if(key2==0)
    //     {
    //         printf("2");
    //         osSemaphoreRelease(SEMAPHORE_KEY2);
    //     }   
    // }
    // else if(mode==MODE2)
    // {
    //     if(key2==0)
    //     {
    //         osSemaphoreRelease(SEMAPHORE_KEY3);
    //     }
    // }
}

//外部中断初始化
void exti_init(void)
{
    SEMAPHORE_KEY1 = osSemaphoreNew(1, 0, NULL);
    SEMAPHORE_KEY2 = osSemaphoreNew(1, 0, NULL);
    SEMAPHORE_KEY3 = osSemaphoreNew(1, 0, NULL);
    SEMAPHORE_IN = osSemaphoreNew(1, 0, NULL);
    SEMAPHORE_OUT = osSemaphoreNew(1, 0, NULL);
    SEMAPHORE_BRIGHTNESS_DOWN = osSemaphoreNew(1, 0, NULL);
    SEMAPHORE_BRIGHTNESS_UP = osSemaphoreNew(1, 0, NULL);
    SEMAPHORE_VOLUME_UP = osSemaphoreNew(1, 0, NULL);
    SEMAPHORE_VOLUME_DOWN = osSemaphoreNew(1, 0, NULL);

    hi_gpio_init();                                            // GPIO初始化

    hi_io_set_pull(KEY1_PIN, HI_IO_PULL_UP);                   // 设置GPIO上拉
    hi_io_set_func(KEY1_PIN, KEY1_GPIO_FUN);                   // 设置IO为GPIO功能
    hi_gpio_set_dir(KEY1_PIN, HI_GPIO_DIR_IN);                // 设置GPIO为输入模式
    hi_gpio_register_isr_function(KEY1_PIN,                         // 按键引脚
                                  HI_INT_TYPE_EDGE,            // 下降沿检测
                                  HI_GPIO_EDGE_FALL_LEVEL_LOW, // 低电平时触发
                                  &key1_exti_callback,         // 触发后调用的回调函数
                                  NULL);                       // 回调函数的传参值

    hi_io_set_pull(KEY2_PIN, HI_IO_PULL_UP);                   // 设置GPIO上拉
    hi_io_set_func(KEY2_PIN, KEY2_GPIO_FUN);                   // 设置IO为GPIO功能
    hi_gpio_set_dir(KEY2_PIN, HI_GPIO_DIR_IN);                // 设置GPIO为输入模式
    hi_gpio_register_isr_function(KEY2_PIN,                         // 按键引脚
                                  HI_INT_TYPE_EDGE,            // 下降沿检测
                                  HI_GPIO_EDGE_FALL_LEVEL_LOW, // 低电平时触发
                                  &key2_exti_callback,         // 触发后调用的回调函数
                                  NULL);                       // 回调函数的传参值
}

