
#include <unistd.h>
#include "bsp_led.h"
#include "hi_timer.h"
#include "bsp_exti.h"
#include "task_define.h"

void EXTI_Task(void)
{
    //led_init();//LED初始化
    exti_init();//外部中断初始化
    
    while (1) 
    {
        static hi_bool key1 = HI_FALSE;
        static hi_bool key2 = HI_FALSE;
        static hi_bool long_press_sent = HI_FALSE;
        static hi_bool waiting_for_double_click = HI_FALSE;
        static hi_u64 last_click_time = 0;
        static hi_u64 double_click_timeout = 300; // 双击最大间隔，单位 ms

        if (key_pressed_flag_key1 || key_pressed_flag_key2)
        {
            if(key_pressed_flag_key1)
            {
                key1 = HI_TRUE;
            }
            else if(key_pressed_flag_key2)
            {
                key2 = HI_TRUE;
            }
            uint8_t key_1 = 1;
            uint8_t key_2 = 1;
            hi_gpio_get_input_val(KEY1_PIN, &key_1);
            hi_gpio_get_input_val(KEY2_PIN, &key_2);
            if (key_1 == 0 || key_2 == 0)
            {
                // 持续按下中，检测长按
                hi_u64 now = hi_get_milli_seconds();
                if (!long_press_sent && (now - press_start_time > 1000))
                {
                    long_press_sent = HI_TRUE;
                    printf("Long press\n");
                }
            }
            else
            {
                // 按键已抬起
                hi_u64 now = hi_get_milli_seconds();
                hi_u64 press_time = now - press_start_time;
                key_pressed_flag_key1 = HI_FALSE;
                key_pressed_flag_key2 = HI_FALSE;

                if (press_time < 1000 && !long_press_sent)
                {
                    // 是短按，进一步判断是否是双击
                    if (!waiting_for_double_click)
                    {
                        // 第一次点击，开始等待第二次
                        waiting_for_double_click = HI_TRUE;
                        last_click_time = now;
                    }
                    else
                    {
                        if (now - last_click_time <= double_click_timeout)
                        {
                            // 在规定时间内再次点击，判定为双击
                            waiting_for_double_click = HI_FALSE;
                            printf("Double click\n");
                            if(mode==MODE1)
                            {
                                key1 = HI_FALSE;
                                osSemaphoreRelease(SEMAPHORE_IN);
                            }
                            else if(mode==MODE3 || mode==MODE4 || mode==MODE5 || mode==MODE7 || mode==MODE8)
                            {
                                key1 = HI_FALSE;
                                osSemaphoreRelease(SEMAPHORE_OUT);
                            }
                        }
                    }
                }

                long_press_sent = HI_FALSE;
            }
        }
            // 检查等待双击的超时情况（必须放在函数末尾）
        if (waiting_for_double_click)
        {
            hi_u64 now = hi_get_milli_seconds();
            if (now - last_click_time > double_click_timeout)
            {
                waiting_for_double_click = HI_FALSE;
                printf("Single click\n");
                if(mode==MODE1)
                {
                    if(key1)
                    {
                        osSemaphoreRelease(SEMAPHORE_KEY1);
                        key1 = HI_FALSE;
                    }
                    else if(key2)
                    {
                        osSemaphoreRelease(SEMAPHORE_KEY2);
                    }
                }
                else if(mode==MODE2)
                {
                    osSemaphoreRelease(SEMAPHORE_KEY3);
                }
                else if(mode==MODE3)
                {
                    if(key1)
                    {
                        printf("key1\n");
                        osSemaphoreRelease(SEMAPHORE_BRIGHTNESS_UP);
                        key1 = HI_FALSE;
                    }
                    else if(key2)
                    {
                        printf("key2\n");
                        osSemaphoreRelease(SEMAPHORE_BRIGHTNESS_DOWN);
                    }
                }
                else if(mode==MODE5)
                {
                    if(key1)
                    {
                        osSemaphoreRelease(SEMAPHORE_VOLUME_UP);
                        key1 = HI_FALSE;
                    }
                    else if(key2)
                    {
                        osSemaphoreRelease(SEMAPHORE_VOLUME_DOWN);
                    }
                }
            }
        }
        usleep(1000);
    }
}