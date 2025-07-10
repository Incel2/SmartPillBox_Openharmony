/**
 ****************************************************************************************************
 * @file        bsp_beep.c
 * @author      普中科技
 * @version     V1.0
 * @date        2024-06-05
 * @brief       蜂鸣器实验
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

#include "bsp_beep.h"
#include "bsp_led.h"
#include "task_define.h"

uint8_t volume = 50;

//蜂鸣器初始化
void beep_init(void)
{
    hi_gpio_init();                                            // GPIO初始化
    hi_io_set_pull(BEEP_PIN, HI_IO_PULL_UP);                   // 设置GPIO上拉
    hi_io_set_func(BEEP_PIN, BEEP_GPIO_FUN);                   // 设置IO为GPIO功能
    hi_gpio_set_dir(BEEP_PIN, HI_GPIO_DIR_OUT);                // 设置GPIO为输出模式

}

//蜂鸣器报警
void beep_alarm(uint16_t cnt,uint16_t time)
{
    uint8_t mode_temp = mode;
    mode = MODE2;
    while(cnt--)
    {
        if (osSemaphoreAcquire(SEMAPHORE_KEY3,0)==osOK)
        {
            break;
        }
        LED(1);
        BEEP(1); 
        usleep(time*(volume)); 
        BEEP(0);
        usleep(time*(100-volume)); 
    }
    LED(0);
    osSemaphoreRelease(SEMAPHORE_OUT);
    // usleep(1000);
    // mode = mode_temp;
}

void beep_seek(uint16_t cnt,uint16_t time)
{
    while(cnt--)
    {
        if (osSemaphoreAcquire(SEMAPHORE_KEY3,0)==osOK)
        {
            break;
        }
        BEEP(1); 
        usleep(time*(volume)); 
        BEEP(0);
        usleep(time*(100-volume)); 
    }
}