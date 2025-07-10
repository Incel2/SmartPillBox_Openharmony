#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_gpio.h"
#include "hi_io.h"
#include "hi_types.h"
#include "task_define.h"

bool open_flag = false;


// 霍尔传感器任务 - 使用轮询方式替代中断方式
void HALL_Task(void)
{
    // 初始化GPIO
    hi_gpio_init();
    
    // 配置GPIO为输入模式，上拉
    hi_io_set_func(HALL_DO_GPIO, HI_IO_FUNC_GPIO_13_GPIO);
    hi_gpio_set_dir(HALL_DO_GPIO, HI_GPIO_DIR_IN);
    hi_io_set_pull(HALL_DO_GPIO, HI_IO_PULL_UP); // 启用上拉电阻
    
    hi_gpio_value lastVal = HI_GPIO_VALUE0;
    hi_gpio_value currentVal = HI_GPIO_VALUE0;
    
    // 初始化读取一次当前状态
    hi_gpio_get_input_val(HALL_DO_GPIO, &lastVal);
    printf("霍尔传感器初始化状态: %d\n", lastVal);
    
    // 初始状态判断
    if (lastVal == HI_GPIO_VALUE0) {
        printf("[初始状态] 药盒已关闭(磁铁靠近)\n");
    } else {
        printf("[初始状态] 药盒已打开(磁铁远离)！\n");
    }
    
    while(1) {
        hi_gpio_get_input_val(HALL_DO_GPIO, &currentVal);
        
        // 只有当状态发生变化时才输出信息
        if (currentVal != lastVal) {
            if (currentVal == HI_GPIO_VALUE0) {
                // 当值为1时，表示磁铁靠近，药盒关闭
                printf("[检测] 药盒已关闭(磁铁靠近)\n");
            } else {
                open_flag = 1;
                // 当值为0时，表示磁铁远离，药盒打开
                printf("[警报] 药盒被打开(磁铁远离)！\n");
            }
            lastVal = currentVal;
        }
        
        // 降低轮询频率，减少CPU占用
        osDelay(100);
    }
}





