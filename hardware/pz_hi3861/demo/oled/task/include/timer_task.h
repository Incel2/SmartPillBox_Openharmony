#ifndef __TIMER_TASK_H__
#define __TIMER_TASK_H__

#include "task_define.h"
#include "bsp_time.h"
#include <stdbool.h>

#define MAX_REMIND_TIMES 5  // 每种药物的最大提醒次数
#define MEDICINE_COUNT 4    // 药物总数
#define MEDICINE_TABLE_COUNT 10

// 药物提醒
typedef struct {
    char name[20];                             // 药物名称
    int time_count;                            // 每天提醒次数
    SystemTime remind_times[MAX_REMIND_TIMES]; // 提醒时间（时分秒）
    bool triggered[MAX_REMIND_TIMES];          // 每个时间点是否已触发
    uint8_t is_enabled;                           // 是否启用
    uint8_t dose;
} MedicineReminder;

typedef struct {
    const char* chinese_name;
    const char* english_name;
} Drug;

// 中英文药品表
extern Drug drug_table[MEDICINE_TABLE_COUNT];


// 药物提醒列表
extern MedicineReminder medicine_list[MEDICINE_COUNT];


// extern bool remind_flag;

const char* get_english_name(const char* chinese_name);
void TIMER_Task(void);

#endif
