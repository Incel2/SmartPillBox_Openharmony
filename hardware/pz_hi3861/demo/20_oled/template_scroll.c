#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_oled.h"

// 定义常量

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

// OLED 任务（双缓冲）
osThreadId_t OLED_Task_ID;
void OLED_Task(void) {
    uint8_t key = 0;
    int16_t x = 0, y = 0, last_y = -1;
    int16_t step = 1;
    const uint8_t *scrollText = (uint8_t *)" Helloworld ";

    oled_init();
    key_init();
    // oled_clear_buffer(current_buffer);
    // oled_showstring_buffer(current_buffer, x, y, scrollText, 16);
    // oled_swap_buffers();
    // oled_refresh_gram_buffer();

    oled_clear();
    oled_showstring(0, y, scrollText, 16,1);
    oled_refresh_gram();
    last_y = y;

    while (1) {
        key = key_scan(1);
        if (key == KEY1_PRESS && y > 0) {
            y -= step;
        } else if (key == KEY2_PRESS && y < Y_WIDTH - 16) {
            y += step;
        }

        if (y != last_y) {
            // oled_fill_buffer(current_buffer, 0, last_y, X_WIDTH - 1, last_y + 16, 0);
            // oled_showstring_buffer(current_buffer, x, y, scrollText, 16);
            // oled_swap_buffers();
            // oled_refresh_gram_buffer();
            oled_clear();
            oled_showstring(0, y, scrollText, 16,1);
            oled_refresh_gram();
            last_y = y;
        }
        //usleep(1000); // 10ms 帧间隔
    }
}




void oled_task_create(void) {
    osThreadAttr_t taskOptions;
    taskOptions.name = "oledTask";
    taskOptions.attr_bits = 0;
    taskOptions.cb_mem = NULL;
    taskOptions.cb_size = 0;
    taskOptions.stack_mem = NULL;
    taskOptions.stack_size = 4*2048;
    taskOptions.priority = osPriorityAboveNormal;
    OLED_Task_ID = osThreadNew((osThreadFunc_t)OLED_Task, NULL, &taskOptions);
    if (OLED_Task_ID != NULL) {
        printf("ID = %d, Task Create OK!\n", OLED_Task_ID);
    }
}

static void template_demo(void) {
    printf("普中-Hi3861开发板--OLED液晶显示实验\r\n");
    led_task_create();
    oled_task_create();
}
SYS_RUN(template_demo);