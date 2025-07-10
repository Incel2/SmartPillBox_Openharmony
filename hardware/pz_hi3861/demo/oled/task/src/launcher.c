// Launcher.c
#include "Launcher.h"
#include "task_define.h"

Launcher launcher;

int64_t speed[20] = {1,0,1,1,1,2,3,3,4,4,4,4,3,3,2,1,1,1,0,1};
int64_t speed2[32] = {2,3,3,4,5,6,7,7,8,9,10,11,12,13,
                      13,12,11,10,9,8,7,7,6,5,4,3,3,2};
void Launcher_init(Launcher* launcher) {
    Camera_init(&launcher->camera, 128, 64);  // 假设 OLED 屏幕为 128x64
    Menu_init(&launcher->menu,NULL);
    Selector_init(&launcher->selector, &launcher->menu);
}

void Launcher_render(Launcher* launcher) {
    // 渲染当前页面
    // printf("Rendering menu with camera at position (%d, %d)\n", launcher->camera.x_position, launcher->camera.y_position);
    Menu_renderWithCamera(&launcher->menu, &launcher->camera);

}

void Launcher_moveMenu(Launcher* launcher, uint8_t currentIndex, uint8_t previousIndex) {
    // 根据用户输入改变菜单的选择项
    printf("Moving menu from index %d to index %d\n", previousIndex, currentIndex);
    uint8_t current_x = launcher->menu.items[currentIndex]->x_position;
    uint8_t previous_x = launcher->menu.items[previousIndex]->x_position;
    uint8_t i = 0;
    uint8_t j = 0;

    // uint8_t num = 6;
    // int64_t target = 0;
    // target = current_x - 48;
    // int64_t start = launcher->camera.x_position;
    // int64_t delta = target - start;
    // int64_t step = delta / num;
    // int64_t remainder = delta % num;
    // for(i=0;i<num;i++)
    // {
    //     int64_t move;

    //     if (i == num-1) {
    //         // 最后一步，加上余数，确保最终位置等于 target
    //         move = target - launcher->camera.x_position;
    //     } else {
    //         move = step;
    //     }

    //     Camera_move(&launcher->camera, move, 0);
    //     Launcher_render(launcher);
    //     //usleep(20000);
    // }
    while(launcher->camera.x_position + 48 != current_x)
    {   
        // printf("current_x : %d \n", current_x);

        // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
        // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);

        if (abs(current_x - previous_x) < 100){
            // 更新相机位置
            if (launcher->camera.x_position + 48 < current_x) {
                launcher->camera.x_position = speed[i] + launcher->camera.x_position;
                // printf("x_position : %ld \n", launcher->camera.x_position);
                // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
                // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
                //Camera_move(&launcher->camera, speed[i], 0);  // 向左滚动
            } else if (launcher->camera.x_position + 48 >= current_x) {
                launcher->camera.x_position = launcher->camera.x_position - speed[i];
                // printf("x_position : %ld \n", launcher->camera.x_position);
                // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
                // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
                //Camera_move(&launcher->camera, -speed[i], 0);   // 向右滚动
            }
            // printf("speed : %d %d \n", speed[i], i);
            i = i + 1;
            if (i >= 20) {
                i = 0;  // 重置速度索引
            }
        }
        else if (abs(current_x - previous_x) >=100){
            // 更新相机位置
            if (launcher->camera.x_position + 48 < current_x) {
                launcher->camera.x_position = speed2[j] + launcher->camera.x_position;
                // printf("x_position : %ld \n", launcher->camera.x_position);
                // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
                // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
                //Camera_move(&launcher->camera, speed2[j], 0);  // 向左滚动
            } else if (launcher->camera.x_position + 48 >= current_x) {
                launcher->camera.x_position = launcher->camera.x_position - speed2[j];
                // printf("x_position : %ld \n", launcher->camera.x_position);
                // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
                // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
                //Camera_move(&launcher->camera, -speed2[j], 0);   // 向右滚动
            }
            // printf("speed : %d %d \n", speed2[i], i);
            j++;
            if (j >= 32) {
                j = 0;  // 重置速度索引
            }
        }

        Launcher_render(launcher);
        // uint8_t end = hi_get_us();
        // printf("Time taken for move: %d us\n", 1000000/(end - start));
        usleep(10000);  // 延时，避免滚动过快
    }
}
