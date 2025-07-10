#ifndef __SYSTIME_TASK_H__
#define __SYSTIME_TASK_H__

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;

typedef struct {
    uint16_t year;      // 例如 2025
    uint8_t month;      // 1~12
    uint8_t day;        // 1~31
    uint8_t hour;       // 0~23
    uint8_t minute;     // 0~59
    uint8_t second;     // 0~59
} SystemTime;

extern SystemTime systemTime;
extern char g_weekStr[][4];
void SYSTIME_Task(void);
void set_system_time(uint16_t year, uint8_t month, uint8_t day,
                     uint8_t hour, uint8_t minute, uint8_t second);

#endif