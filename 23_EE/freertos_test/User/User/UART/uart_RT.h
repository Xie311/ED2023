/*
 * @Author: ZYT
 * @Date: 2025-05-18 20:42:48
 * @LastEditors: ZYT
 * @LastEditTime: 2025-05-25 02:31:52
 * @FilePath: \pantilt_freertos\User\User\UART\uart_RT.h
 * @Brief: 
 * 
 * Copyright (c) 2025 by zyt, All Rights Reserved. 
 */
#ifndef UART_RECEIVE
#define UART_RECEIVE

#include "stm32f1xx.h"
#include "usart.h"


extern uint8_t Rxbuffer[16];

void U_Transmit(uint8_t num);

#endif // !UART_RECEIVE