#include "task_define.h"
#include <stdio.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "bsp_key.h"
#include "bsp_oled.h"

void ADJUST_Task(void){
    key_init();//按键初始化
    oled_init();

    uint8_t last_brightness = brightness;
    uint8_t key = 0;
    while(1){
        if(osSemaphoreAcquire(SEMAPHORE_BRIGHTNESS_UP,0)==osOK){
            if(brightness<0xFF){
                brightness+=127;
                oled_wr_byte(0x81,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
                oled_wr_byte(brightness,OLED_CMD);
            }
        }
        else if(osSemaphoreAcquire(SEMAPHORE_BRIGHTNESS_DOWN,0)==osOK){
            if(brightness>0x01){
                brightness-=127;
                oled_wr_byte(0x81,OLED_CMD); //对比度设置
                oled_wr_byte(brightness,OLED_CMD);
            }
        }
        else if(osSemaphoreAcquire(SEMAPHORE_VOLUME_UP,0)==osOK){
            if(volume==90){
                volume = 50;
                printf("volume = %d\n",volume);
                mode = MODE6;
            }
            else if(volume==50){
                volume =10;
                printf("volume = %d\n",volume);
                mode = MODE6;
            }
        }
        else if(osSemaphoreAcquire(SEMAPHORE_VOLUME_DOWN,0)==osOK){
            if(volume==10){
                volume = 50;
                printf("volume = %d\n",volume);
                mode = MODE6;                
            }
            else if(volume==50){
                volume =90;
                printf("volume = %d\n",volume);
                mode = MODE6;                
            }
        }
        if(last_brightness!=brightness){
            last_brightness = brightness;
            printf("brightness = %d\n",brightness);
            oled_refresh_gram();
            mode = MODE6;
        }
        usleep(100*1000);
       
    }
}