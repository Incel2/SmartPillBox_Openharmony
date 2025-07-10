#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_oled.h"

#define BALL_COUNT 3
// 定义常量
const uint8_t OLED_WIDTH = 128;
const uint8_t OLED_HEIGHT = 64;
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
// void OLED_Task(void) {
//     uint8_t key = 0;
//     int16_t x = 0, y = 0, last_y = -1;
//     int16_t step = 1;
//     const uint8_t *scrollText = (uint8_t *)" Helloworld ";

//     oled_init();
//     key_init();
//     // oled_clear_buffer(current_buffer);
//     // oled_showstring_buffer(current_buffer, x, y, scrollText, 16);
//     // oled_swap_buffers();
//     // oled_refresh_gram_buffer();

//     oled_clear();
//     oled_showstring(0, y, scrollText, 16);
//     oled_refresh_gram();
//     last_y = y;

//     while (1) {
//         key = key_scan(1);
//         if (key == KEY1_PRESS && y > 0) {
//             y -= step;
//         } else if (key == KEY2_PRESS && y < Y_WIDTH - 16) {
//             y += step;
//         }

//         if (y != last_y) {
//             // oled_fill_buffer(current_buffer, 0, last_y, X_WIDTH - 1, last_y + 16, 0);
//             // oled_showstring_buffer(current_buffer, x, y, scrollText, 16);
//             // oled_swap_buffers();
//             // oled_refresh_gram_buffer();
//             oled_clear();
//             oled_showstring(0, y, scrollText, 16);
//             oled_refresh_gram();
//             last_y = y;
//         }
//         //usleep(1000); // 10ms 帧间隔
//     }
// }

typedef struct {
    int16_t x, y;
    int8_t vx, vy;
}Ball;



void OLED_Task(void *argument)
{
    oled_init();

    Ball balls[BALL_COUNT] = {
        {10, 10, 4, 2},
        {30, 20, -1, 2},
        {60, 40, 1, -2},
    };

    const uint16_t frame_time_us = 25000; // 每帧50ms = 20FPS
    uint64_t frame_start = 0, frame_end = 0;
    uint32_t fps = 0;
    char fps_str[16];

    int16_t last_x[BALL_COUNT], last_y[BALL_COUNT];

    while (1)
    {
        frame_start = hi_get_us();

        // 擦除所有小球旧位置
        // for (int i = 0; i < BALL_COUNT; i++) {
        //     oled_draw_bigpoint(last_x[i], last_y[i], 0);
        // }
        oled_clear();
        // 更新每个小球位置并画新点
        for (int i = 0; i < BALL_COUNT; i++) {
            // 更新位置
            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;

            // 边缘反弹
            if (balls[i].x <= 1 || balls[i].x >= OLED_WIDTH - 2) {
                balls[i].vx = -balls[i].vx;
                balls[i].x = (balls[i].x <= 1) ? 1 : (OLED_WIDTH - 2);
            }
            if (balls[i].y <= 1 || balls[i].y >= OLED_HEIGHT - 2) {
                balls[i].vy = -balls[i].vy;
                balls[i].y = (balls[i].y <= 1) ? 1 : (OLED_HEIGHT - 2);
            }

            // 记录当前位置用于下次擦除
            last_x[i] = balls[i].x;
            last_y[i] = balls[i].y;

            // 画出新位置
            oled_draw_bigpoint(balls[i].x, balls[i].y, 1);
        }

        // 显示FPS
        snprintf(fps_str, sizeof(fps_str), "FPS:%2lu", fps);
        oled_showstring(80, 0, (uint8_t *)fps_str, 12);

        // 刷新屏幕
        oled_refresh_gram();

        usleep(frame_time_us); // 控制帧率

        // 更新帧率信息
        frame_end = hi_get_us();
        uint32_t delta = frame_end - frame_start;
        fps = (delta > 0) ? (1000000 / delta) : 0;
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