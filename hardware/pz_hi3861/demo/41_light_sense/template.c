/**
 ****************************************************************************************************
 * @file        template.c
 * @author      普中科技
 * @version     V1.0
 * @date        2024-06-05
 * @brief       光敏传感器模块实验
 * @license     Copyright (c) 2024-2034, 深圳市普中科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:普中-Hi3861
 * 在线视频:https://space.bilibili.com/2146492485
 * 公司网址:www.prechin.cn
 * 购买地址:
 *
 ****************************************************************************************************
 * 实验现象：光敏传感器模块，将ADC检测数据输出在串口助手上显示。
 *
 ****************************************************************************************************
 */

#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "bsp_led.h"
#include "bsp_adc.h"
#include "bsp_oled.h"
#include "bsp_beep.h"



osThreadId_t MODULE_Task_ID; //任务ID

void MODULE_Task(void)
{
    uint8_t i=0;
    uint16_t adc_value=0;

    led_init();//LED初始化
    
    
    adc5_init();
    int pre = 0;
    int flag=0;
    while (1) 
    {
        
        i++;
        if(i%50==0)
        {
            adc_value=get_adc5_value();
            pre = flag;
            if(adc_value>2160){
                LED(1);
                flag = 1;
            }
            else{
                LED(0);
                flag = 0;
            }
            if(pre != flag){
                oled_init();
                oled_clear();
                if(flag == 1){
                    oled_showstring(0,0,"A",12);
                    beep_init();//BEEP初始化
                    beep_alarm(10,1);
                }

                else{
                    oled_showstring(0,0,"L",12);
                }
                
                oled_showfontHZ(0,35,2,16,1);
	            oled_showfontHZ(1*16,35,3,16,1);
	            oled_showfontHZ(2*16,35,4,16,1);
	            oled_refresh_gram();
            }
            printf("adc_value=%d\r\n",adc_value);
            
        }
        usleep(10*1000);
    }
}
//任务创建
void module_task_create(void)
{
    osThreadAttr_t taskOptions;
    taskOptions.name = "ModuleTask";            // 任务的名字
    taskOptions.attr_bits = 0;               // 属性位
    taskOptions.cb_mem = NULL;               // 堆空间地址
    taskOptions.cb_size = 0;                 // 堆空间大小
    taskOptions.stack_mem = NULL;            // 栈空间地址
    taskOptions.stack_size = 1024;           // 栈空间大小 单位:字节
    taskOptions.priority = osPriorityNormal; // 任务的优先级

    MODULE_Task_ID = osThreadNew((osThreadFunc_t)MODULE_Task, NULL, &taskOptions); // 创建任务1
    if (MODULE_Task_ID != NULL)
    {
        printf("ID = %d, Create MODULE_Task_ID is OK!\n", MODULE_Task_ID);
    }
}

/**
 * @description: 初始化并创建任务
 * @param {*}
 * @return {*}
 */
static void template_demo(void)
{
    printf("普中-Hi3861开发板--光敏传感器模块实验\r\n");
    module_task_create();
}
SYS_RUN(template_demo);
