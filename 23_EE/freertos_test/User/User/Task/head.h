/*
 * @Author: ZYT
 * @Date: 2025-05-12 15:18:52
 * @LastEditors: ZYT
 * @LastEditTime: 2025-05-25 03:05:58
 * @FilePath: \pantilt_freertos\User\User\Task\head.h
 * @Brief: 
 * 
 * Copyright (c) 2025 by zyt, All Rights Reserved. 
 */
#ifndef HEAD
#define HEAD
//#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "User_SMS_STS.h"
#include "SCServo.h"
#include "io_retargetToUart.h"
#include "math.h"
#include "task.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "stdlib.h"

#define RED  //红光模式还是绿光模式，主要是串口接受的不一样
extern uint16_t state;

extern __IO uint8_t Q_NO;
extern __IO uint8_t mode1;
extern __IO uint8_t mode2;
extern int16_t spe1;
extern int16_t spe2;
extern uint8_t ERROR_FLAG;
extern __IO uint8_t Reset;
#ifdef RED
extern uint8_t dot1x,dot1y,dot2x,dot2y,dot3x,dot3y,dot4x,dot4y;
extern int16_t tar_pos1,tar_pos2;
extern uint8_t dot_cal_x[400];
extern uint8_t dot_cal_y[400];
#endif // RED
extern uint8_t red_x,red_y;
extern uint8_t Rx[1];

void Servo_Start(void);
void Statemachine_Start(void);

#endif // !HEAD