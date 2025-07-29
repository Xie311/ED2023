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


/************************变量定义 ***************************/

extern uint8_t Uart_flag;
#endif