#ifndef _BLUE_TASK_H_
#define _BLUE_TASK_H_

void UART_Init(void);

void SendDataViaBluetooth(const char* data);

void BluetoothTask(void);

#endif

