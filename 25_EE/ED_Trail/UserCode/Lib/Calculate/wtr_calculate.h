/*
 * @Author: szf
 * @Date: 2023-02-22 12:06:17
 * @LastEditors: X311 
 * @LastEditTime: 2025-07-26 22:49:25
 * @FilePath: \ED_Trace\UserCode\Lib\Calculate\wtr_calculate.h
 * @brief大疆电机PID以及底盘逆解
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#pragma once

#include "chassis_start.h"

#include "wtr_dji.h"

#define r_underpan_3 0.1934
#define r_underpan_4 0.25
#define r_wheel      0.076

// 定义数组，分别存放四个轮子对应电机的速度
extern double moter_speed[4];

// 声明运动学逆解函数
void CalculateFourMecanumWheels(double *moter_speed, double vx, double vy, double vw);

// 位置伺服
void positionServo(float ref, DJI_t *motor);

// 速度伺服
void speedServo(float ref, DJI_t *motor);

// 圆周死区控制
void DeadBand(double x, double y, double *new_x, double *new_y, double threshould);

// 单维度死区控制
void DeadBandOneDimensional(double x, double *new_x, double threshould);

