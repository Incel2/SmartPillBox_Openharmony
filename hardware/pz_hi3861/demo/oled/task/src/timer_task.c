#include "bsp_time.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "bsp_led.h"
#include "hi_timer.h"
#include "bsp_key.h"
#include "timer_task.h"
#include "task_define.h"

MedicineReminder medicine_list[MEDICINE_COUNT]; 
Drug drug_table[MEDICINE_TABLE_COUNT]={
    {"感冒灵", "ColdRelief"},
    {"布洛芬", "Ibuprofen"},
    {"阿莫西林", "Amoxicillin"},
    {"头孢克肟", "Cefixime"},
    {"维生素C", "Vitamin C"},
    {"氯雷他定", "Loratadine"},
    {"扑热息痛", "Paracetamol"},
    {"银翘解毒丸", "Yinqiao Detox Pill"},
};
bool remind_flag = false;      
char* remind_medicine = NULL;


// 初始化提醒时间
void init_medicine_reminders()
{
    // 初始化所有触发状态
    for (int i = 0; i < MEDICINE_COUNT; i++) {
        for (int j = 0; j < MAX_REMIND_TIMES; j++) {
            medicine_list[i].triggered[j] = false;
            medicine_list[i].is_enabled = -1; // 默认启用所有药物提醒
        }
    }
    // Medicine 1: Blood pressure pill
    strcpy(medicine_list[0].name, "BP_Med");
    medicine_list[0].time_count = 3;
    medicine_list[0].remind_times[0] = (SystemTime){0, 0, 0, 12, 0, 5};
    medicine_list[0].remind_times[1] = (SystemTime){0, 0, 0, 12, 0, 30};
    medicine_list[0].remind_times[2] = (SystemTime){0, 0, 0, 12, 0, 50};
    medicine_list[0].is_enabled = 1;
    medicine_list[0].dose = 1;

    // // Medicine 2: Vitamin C
    // strcpy(medicine_list[1].name, "Vitamin_C");
    // medicine_list[1].time_count = 2;
    // medicine_list[1].remind_times[0] = (SystemTime){0, 0, 0, 12, 0, 25};
    // medicine_list[1].remind_times[1] = (SystemTime){0, 0, 0, 12, 0, 30};
    // medicine_list[1].is_enabled = 0;

    // // Medicine 3: Aspirin
    // strcpy(medicine_list[2].name, "Aspirin");
    // medicine_list[2].time_count = 2;
    // medicine_list[2].remind_times[0] = (SystemTime){0, 0, 0, 12, 0, 40};
    // medicine_list[2].remind_times[1] = (SystemTime){0, 0, 0, 12, 0, 45};
    // medicine_list[2].is_enabled = 0;

    // Medicine 4: Stomach pill
    // strcpy(medicine_list[3].name, "Stomach_Med");
    // medicine_list[3].time_count = 3;
    // medicine_list[3].remind_times[0] = (SystemTime){0, 0, 0, 12, 1, 0};
    // medicine_list[3].remind_times[1] = (SystemTime){0, 0, 0, 12, 1, 05};
    // medicine_list[3].remind_times[2] = (SystemTime){0, 0, 0, 12, 1, 10};

}

// 每秒检查所有药物的提醒时间
void check_medicine_reminders()
{
    // medicine_list[0].is_enabled = 1;
    // printf("[DEBUG] %d\n",medicine_list[3].is_enabled);

    for (int i = 0; i < MEDICINE_COUNT; i++) {
        if(medicine_list[i].is_enabled == 1){
            for (int j = 0; j < medicine_list[i].time_count; j++) {
                SystemTime *t = &medicine_list[i].remind_times[j];

                if (!medicine_list[i].triggered[j] &&
                    systemTime.hour == t->hour &&
                    systemTime.minute == t->minute
                    && systemTime.second == t->second) {

                    printf("[ALARM] Medicine: %s triggered at %02d:%02d:%02d\n",
                        medicine_list[i].name, t->hour, t->minute, t->second);

                    remind_flag = 1; // 设置提醒标志
                    remind_medicine = medicine_list[i].name; // 设置提醒药物名称
                    printf("[DEBUG] Setting remind_flag = true for %s\n", medicine_list[i].name);

                    medicine_list[i].triggered[j] = true;
                }

                // 每天凌晨 00:00:01 重置触发状态
                if (systemTime.hour == 0 && systemTime.minute == 0 && systemTime.second == 1) {
                    medicine_list[i].triggered[j] = false;
                }
            }
        }
    }
}

const char* get_english_name(const char* chinese_name) {
    for (int i = 0; i < MEDICINE_TABLE_COUNT; i++) {
        if (strcmp(drug_table[i].chinese_name, chinese_name) == 0) {
            return drug_table[i].english_name;
        }
    }
    return "Unknown";  // 未找到
}

// 主任务函数，触发提醒标志
void TIMER_Task(void)
{

    beep_init();
    led_init();
    init_medicine_reminders(); // 初始化药物提醒时间

    int last_second = -1;

    while (1) { 
        // 每秒检测一次
        if (systemTime.second != last_second) {
            last_second = systemTime.second;
            check_medicine_reminders();
        }

        usleep(100 * 1000); // 每100ms循环一次
    }
}


