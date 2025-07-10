#include <task_define.h>
#include "hi_uart.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_io.h"
#include "systime_task.h"
#include "timer_task.h"
#include "hi_watchdog.h" // 包含看门狗相关头文件

int pre_mode;

void UART_Init(void)
{
    hi_io_set_func(UART_TX_PIN, HI_IO_FUNC_GPIO_11_UART2_TXD); // 配置IO11为UART2_TXD
    hi_io_set_func(UART_RX_PIN, HI_IO_FUNC_GPIO_12_UART2_RXD); // 配置IO12为UART2_RXD

    hi_uart_attribute uart_attr = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = HI_UART_DATA_BIT_8,
        .stop_bits = HI_UART_STOP_BIT_1,
        .parity = HI_UART_PARITY_NONE,
    };

    hi_uart_extra_attr uart_extra_attr = {
        .tx_fifo_line = HI_FIFO_LINE_HALF,
        .rx_fifo_line = HI_FIFO_LINE_HALF,
        .flow_fifo_line = HI_FIFO_LINE_HALF,
        .tx_block = HI_UART_BLOCK_STATE_BLOCK,
        .rx_block = HI_UART_BLOCK_STATE_BLOCK,
        .tx_buf_size = 1024,
        .rx_buf_size = 1024,
        .tx_use_dma = HI_UART_NONE_DMA,
        .rx_use_dma = HI_UART_NONE_DMA,
    };

    if (hi_uart_init(UART_NUM, &uart_attr, &uart_extra_attr) != HI_SUCCESS)
    {
        printf("UART init failed!\r\n");
    }
    else
    {
        printf("UART init success!\r\n");
    }
}

/**
 * @brief 解析药物提醒设置字符串
 * @param data 蓝牙接收到的原始字符串
 * @param reminder 指向要填充的MedicineReminder结构体的指针
 * @return 0 表示成功, -1 表示失败
 */
int parse_medicine_string(char* data, MedicineReminder* reminder)
{
    // 使用 strtok 需要一个可修改的字符串，蓝牙接收的 rx_data 正好适用
    char* token;
    const char* delimiter = ";";

    // 1. 验证命令头
    token = strtok(data, delimiter);
    if (token == NULL || (strcmp(token, "SETMED") != 0 && strcmp(token, "EDTMED") != 0 && strcmp(token, "ADDMED") != 0)) {
        printf("ERROR: Invalid command header.\r\n");
        return -1;
    }

    // 2. 解析药物名称
    token = strtok(NULL, delimiter);
    if (token == NULL) {
        printf("ERROR: Missing medicine name.\r\n");
        return -1;
    }
    const char* english = get_english_name(token);
    strncpy(reminder->name, english, sizeof(reminder->name) - 1);
    reminder->name[sizeof(reminder->name) - 1] = '\0'; // 确保字符串以空字符结尾

    // 3. 解析是否启用
    token = strtok(NULL, delimiter);
    if (token == NULL) {
        printf("ERROR: Missing 'is_enabled' flag.\r\n");
        return -1;
    }
    reminder->is_enabled = (atoi(token) == 1);

    // 4.解析剂量
    token = strtok(NULL, delimiter);
    if (token == NULL) {
        printf("ERROR: Missing 'dose'.\r\n");
        return -1;
    }
    reminder->dose = atoi(token);

    // 5. 解析提醒次数
    token = strtok(NULL, delimiter);
    if (token == NULL) {
        printf("ERROR: Missing 'time_count'.\r\n");
        return -1;
    }
    reminder->time_count = atoi(token);
    if (reminder->time_count > MAX_REMIND_TIMES) {
        printf("ERROR: time_count exceeds MAX_REMIND_TIMES.\r\n");
        reminder->time_count = MAX_REMIND_TIMES; // 防止数组越界
    }

    // 6. 循环解析所有提醒时间
    int temp_time_count = reminder->time_count; // 临时变量，用于记录当前已解析的提醒时间数量
    for (int i = 0; i < reminder->time_count; i++) {
        token = strtok(NULL, delimiter);
        if (token == NULL) {
            printf("ERROR: Missing time entry #%d.\r\n", i + 1);
            return -1;
        }
        if(strcmp(token,"undefined"))
        {
            temp_time_count--;
            continue;
        }
        // 解析 HH:MM 格式
        int hour, minute;
        if (sscanf(token, "%d:%d", &hour, &minute) == 2) {
            reminder->remind_times[i].hour = hour;
            reminder->remind_times[i].minute = minute;
        } else {
            printf("ERROR: Invalid time format for entry #%d.\r\n", i + 1);
            return -1;
        }
    }
    reminder->time_count = temp_time_count; // 更新实际解析的提醒时间数量
    
    // 解析成功后，可以重置触发状态
    for (int i = 0; i < reminder->time_count; i++) {
        reminder->triggered[i] = false;
    }

    printf("Medicine '%s' parsed successfully.\r\n", reminder->name);
    return 0;
}

int parse_medicine_string_del(char* data, MedicineReminder* reminder)
{
    // 使用 strtok 需要一个可修改的字符串，蓝牙接收的 rx_data 正好适用
    char* token;
    const char* delimiter = ";";

    // 1. 验证命令头
    token = strtok(data, delimiter);
    if (token == NULL || strcmp(token, "DELMED") != 0) {
        printf("ERROR: Invalid command header.\r\n");
        return -1;
    }

    // 2. 解析药物名称
    token = strtok(NULL, delimiter);
    if (token == NULL) {
        printf("ERROR: Missing medicine name.\r\n");
        return -1;
    }
    const char* english = get_english_name(token);    
    strncpy(reminder->name, english, sizeof(reminder->name) - 1);
    reminder->name[sizeof(reminder->name) - 1] = '\0'; // 确保字符串以空字符结尾

    printf("Medicine '%s' parsed successfully.\r\n", reminder->name);
    return 0;
}

int parse_seek_string(char* data, int pre_mode)
{
    // 使用 strtok 需要一个可修改的字符串，蓝牙接收的 rx_data 正好适用
    char* token;
    const char* delimiter = ";";

    // 1. 验证命令头
    token = strtok(data, delimiter);
    if (token == NULL || strcmp(token, "SEEK") != 0) {
        printf("ERROR: Invalid command header.\r\n");
        return -1;
    }

    token = strtok(NULL, delimiter);
    if (token == NULL) {
        printf("ERROR: Missing medicine name.\r\n");
        return -1;
    }
    printf("Medicine '%s' parsed successfully.\r\n", token);
    if(strcmp(token,"1") == 0){
        printf("1");
        mode = MODE9;
    }
    else if(strcmp(token,"0") == 0){
        mode = pre_mode;
        pre_mode = 0;
    }

    printf("seek string parsed successfully.\r\n");
    return 0;
}

void BluetoothTask(void)
{
    printf("enter BluetoothTask.......\r\n");

    UART_Init(); // 初始化UART

    // const char* messages[] = {
    //     "{\"count_per_time\":10, \"count_per_day\":3, \"time1\":1,\"time2\":2,\"time3\":3,\"sum\":3,\"binder\":2}\n"
    // };

    uint8_t rx_data[128] = {0}; // 接收数据
    static uint8_t tx_data[128] = {0}; // 发送数据
    int rx_len = 0;
    int set_num=0;
    static uint32_t status_report_counter = 0; // 状态报告计数器，用于控制调试信息输出频率
    static uint32_t watchdog_counter = 0; // 看门狗计数器，用于控制喂狗频率


    while (1)
    {
        rx_len = hi_uart_read_timeout(UART_NUM, rx_data, sizeof(rx_data), 200); // 超时200ms
        if (watchdog_counter++ >= 10) {
            hi_watchdog_feed();
            watchdog_counter = 0;
        }
        if (rx_len > 0)
        {
            rx_data[rx_len] = '\0'; // 添加字符串结束符
            rx_data[strcspn(rx_data, "\r\n")] = 0;
            
            printf("Recv: %s\r\n", rx_data);

            // 使用 strncmp 来安全地比较命令头
            if (strncmp(rx_data, "SETMED;", 7) == 0) 
            {
                // 这是一个设置药物的命令
                // 假设我们总是更新第一个药物槽位，或者您可以设计更复杂的逻辑来管理多个药物
                if (1) {
                    MedicineReminder new_reminder;
                    if (parse_medicine_string(rx_data, &new_reminder) == 0) {
                        // 解析成功，将其存储到我们的药物列表中
                        // 这里简化处理，总是替换第一个。实际应用中可能需要更复杂的逻辑。
                        medicine_list[set_num] = new_reminder; 
                        set_num++;
                        // medicine_count = 1; // 假设只管理一种药
                        printf("Medicine reminder updated successfully!\r\n");
                        // 你可以向APP发送一个确认回执
                        hi_uart_write(UART_NUM, (uint8_t*)"OK\n", 3);
                    } else {
                        printf("Failed to parse medicine string.\r\n");
                        hi_uart_write(UART_NUM, (uint8_t*)"ERROR: PARSE_FAILED\n", 22);
                    }
                } else {
                    printf("Medicine list is full.\r\n");
                    hi_uart_write(UART_NUM, (uint8_t*)"ERROR: LIST_FULL\n", 18);
                }

            }
            else if(strncmp(rx_data, "ADDMED;", 7) == 0)
            {
                for(int i=0;i<MEDICINE_COUNT;i++){
                    if(medicine_list[i].is_enabled==255)
                    {
                        MedicineReminder new_reminder;
                        if (parse_medicine_string(rx_data, &new_reminder) == 0) {
                            // 解析成功，将其存储到我们的药物列表中
                            // 这里简化处理，总是替换第一个。实际应用中可能需要更复杂的逻辑。
                            medicine_list[i] = new_reminder; 
                            printf("Medicine reminder add successfully!\r\n");
                        } else {
                            printf("Failed to parse medicine string.\r\n");
                        }
                    }
                }
            }
            else if(strncmp(rx_data, "EDTMED;", 7) == 0)
            {
                MedicineReminder new_reminder;
                if(parse_medicine_string(rx_data, &new_reminder) == 0){
                    for(int i=0;i<MEDICINE_COUNT;i++){
                        if(strcmp(medicine_list[i].name, new_reminder.name) == 0)
                        {
                            medicine_list[i] = new_reminder;
                            printf("Medicine reminder edit successfully!\r\n");
                        }
                    }
                }else{
                    printf("Failed to parse medicine string.\r\n");
                }
            }
            else if(strncmp(rx_data, "DELMED;", 7) == 0)
            {
                MedicineReminder new_reminder;
                if(parse_medicine_string_del(rx_data, &new_reminder) == 0){
                    for(int i=0;i<MEDICINE_COUNT;i++){
                        if(strcmp(medicine_list[i].name, new_reminder.name) == 0)
                        {
                            memset(&medicine_list[i], 0, sizeof(MedicineReminder));
                            printf("Medicine reminder edit successfully!\r\n");
                        }
                    }
                }else{
                    printf("Failed to parse medicine string.\r\n");
                }
            }
            else if(strncmp(rx_data, "SEEK;", 5) == 0)
            {
                if(pre_mode == 0){
                    pre_mode = mode;
                }
                if(parse_seek_string(rx_data, pre_mode) == 0){
                    printf("seek successfully!\r\n");
                }else{
                    printf("Failed to parse seek string.\r\n");
                }
            }
            else
            {
                uint16_t year = (rx_data[0]-'0')*1000 + (rx_data[1]-'0')*100 + (rx_data[2]-'0')*10 + (rx_data[3]-'0');
                uint8_t month = (rx_data[4]-'0')*10 + (rx_data[5]-'0');
                uint8_t day = (rx_data[6]-'0')*10 + (rx_data[7]-'0');
                uint8_t hour = (rx_data[8]-'0')*10 + (rx_data[9]-'0');
                uint8_t min = (rx_data[10]-'0')*10 + (rx_data[11]-'0');
                uint8_t sec = (rx_data[12]-'0')*10 + (rx_data[13]-'0');
                printf("%d-%02d-%02d %02d:%02d:%02d\r\n", year, month, day, hour, min, sec);
                set_system_time(year, month, day, hour, min, sec);
                // snprintf(tx_data, sizeof(tx_data), "1");
                // int tx_len = hi_uart_write(UART_NUM, (uint8_t*)tx_data, strlen(tx_data));
                // if (tx_len < 0)
                // {
                //     printf("UART write failed!\r\n");
                // }
                // else
                // {
                //     printf("Sent: %d\r\n", tx_len);
                //     printf("Sent: %s\r\n", tx_data);
                //     usleep(300000); // 100ms间隔
                // }
            }

            usleep(100); // 100ms间隔
        }
        if(open_flag)
        {
            printf("open_flag is true\r\n");
            snprintf(tx_data, sizeof(tx_data), "1");
            int tx_len = hi_uart_write(UART_NUM, (uint8_t*)tx_data, strlen(tx_data));
            if (tx_len < 0)
            {
                printf("UART write failed!\r\n");
            }
            else
            {
                printf("Sent: %d\r\n", tx_len);
                printf("Sent: %s\r\n", tx_data);
                open_flag = false;
            }
        }
        // snprintf(tx_data, sizeof(tx_data), "{\"count_per_time\":10, \"count_per_day\":3, \"time1\":1,\"time2\":2,\"time3\":3,\"sum\":3,\"binder\":2}\n");
        // int tx_len = hi_uart_write(UART_NUM, (uint8_t*)tx_data, strlen(tx_data));

        // if (tx_len < 0)
        // {
        //     printf("UART write failed!\r\n");
        // }
        // else
        // {
        //     printf("Sent: %d\r\n", tx_len);
        //     printf("Sent: %s\r\n", tx_data);
        //     usleep(300000); // 100ms间隔
        // }
    }
}

