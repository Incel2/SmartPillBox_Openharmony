
#include <unistd.h>
#include "bsp_beep.h"
#include "task_define.h"

void REMIND_Task(void)
{
    while (1)
    {
        if (remind_flag)
        {
            remind_flag = 0;  // 清除标志
            printf("Remind Task is running...\n");
            beep_alarm(200, 10); // 安全执行阻塞逻辑
        }
        else if(mode == MODE9)
        {
            printf("Remind Task is running...\n");
            beep_seek(200, 10); // 安全执行阻塞逻辑
        }
        osDelay(10); // 防止死循环
    }
}