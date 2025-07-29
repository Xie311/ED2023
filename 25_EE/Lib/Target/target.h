/**
 * @file wtr_target.h
 * @author X311
 * @brief 
 * @version 0.0
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __WTR_TARGET_H
#define __WTR_TARGET_H

#include "UpperStart.h"
void Upper_Target_Init();
void Upper_Target_Decode();
void Target_Decode_TaskStart();
void Target_Decode_Task();

void Target_Decode();
/************************ 用户定义 ***************************/
#define Tar_UART_HANDLE huart5 // 串口选项
#define Tar_UART        UART5  // 串口选项

/************************变量定义 ***************************/
extern float weight_placement[5];
extern uint8_t receive_buffer[24];
extern uint8_t Uart_flag;
#endif