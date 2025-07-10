#include "task_define.h"
#include "ohos_init.h"
#include "bsp_key.h"

void BEEP_Task(void)
{
    uint16_t cnt=100,time=100;
    int volume = 50;
    key_init();//按键初始化
    beep_init();//BEEP初始化
    
    while (1) 
    {
        
        uint8_t key = key_scan(1);
        
        if(key==KEY1_PRESS){
            printf("key1 press\n");
            printf("volume = %d\n",volume);
            if(volume<100){
                volume+=10;
            }
            beep_alarm(cnt,time,volume);
        }
        if(key==KEY2_PRESS){
            printf("key2 press\n");
            printf("volume = %d\n",volume);
            if(volume>0){
                volume-=10;
            }
            beep_alarm(cnt,time,volume);
        }
        usleep(1000*1000);
    }
}