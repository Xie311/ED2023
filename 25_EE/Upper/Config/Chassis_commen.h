/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 12:54:05
 * @LastEditors: Chen Yitong 
 * @LastEditTime: 2023-10-21 21:56:42
 * @FilePath: \WTR_Chassis\麦克纳姆轮\UserCode\Chassis\Config\Chassis_commen.h
 * @Brief: 底盘结构体定义
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#ifndef __CHASSIS_COMMEN_H__
#define __CHASSIS_COMMEN_H__

#include "Chassis_Start.h"
#include "FreeRTOS.h"
#include "semphr.h"

typedef enum {
    Stop       = 1, // 停止
    Run        = 3, // 启动
    Correcting = 2, // 底盘校准
} CHASSIS_STATE;    // 底盘状态


typedef __IO struct Tar_chassis_t
{
    double pos_x; // 目标位置x
    double pos_y; // 目标位置y
    double vw;    // 目标角速度
    CHASSIS_STATE state; // 底盘状态
} Tar_t; // 上位机传来的目标位置


typedef __IO struct
{
    __IO struct {
        double x;
        double y;
        double w;
    } velocity; // 速度结构体

    __IO struct {
        double x;
        double y;
        double w;
    } position;
    SemaphoreHandle_t  xMutex_control; // 互斥锁
} CHASSIS_MOVING_STATE; // 底盘运动状态

typedef __IO struct {
    DJI_t *hDJI[4];
    SemaphoreHandle_t xMutex_wheel; // 互斥锁
} WHEEL_COMPONENT; //轮子控制 [在Chssis_Variable中声明，在Servo.c中被调用]


#endif // __CHASSIS_COMMEN_H__ 