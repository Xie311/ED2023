/*
 * @Author: X311
 * @Date: 2024-05-13 09:00:14
 * @LastEditors: X311 
 * @LastEditTime: 2025-07-28 00:54:25
 * @FilePath: \ED_Trail\UserCode\Upper\Upper_Start\UpperStart.h
 * @Brief: 
 * 
 * Copyright (c) 2024 by ChenYiTong, All Rights Reserved. 
 */
#ifndef __UPPERSTART_H__
#define __UPPERSTART_H__

#define hcan_Dji hcan1

#include "main.h"
#include "can.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wtr_calculate.h"
#include "cmsis_os.h"
#include "wtr_can.h"
#include "wtr_dji.h"
#include "ParamDef.h"
#include "UpperServo.h"
#include "StateMachine.h"
#include "string.h"
#include "math.h"
#include "USART_Init.h"
#include "target.h"
#include "upper_debug.h"

typedef __IO struct
{
    __IO struct {
        double x;
        double y;
        double z;
        double w;
    } velocity; // 速度结构体//不知道需不需要来着

    __IO struct {
        double x;  // slash
        double y;  // straight line
        double z;
    } position;
    SemaphoreHandle_t xMutex_control; // 互斥锁
} Chassis_MOVING_STATE;

typedef __IO struct {
    DJI_t *hDJI[4];                 // 只用到了三个电机，但是dji can的初始化似乎必须4个一起做，遂设4个，最后一个空挂着
    SemaphoreHandle_t xMutex_motor; // 互斥锁
} Chassis_MOTOR_COMPONENT;


extern uint16_t stepper_flag;
extern int flag[6];
extern uint8_t Uart_State;

extern Chassis_MOVING_STATE Chassis_Control;
// from Servo
extern Chassis_MOTOR_COMPONENT Chassis_MotorComponent;
#endif // __UPPERSTART_H__