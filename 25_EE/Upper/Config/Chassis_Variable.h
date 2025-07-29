/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 17:18:52
 * @LastEditors: x311 
 * @LastEditTime: 2024-04-17 13:17:08
 * @FilePath: \WTR_Omni_Auto_Chassis\UserCode\Chassis\Config\Chassis_Variable.h
 * @Brief: 结构体、变量的声明
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */
#ifndef __CHASSIS_VARIABLE_H__
#define __CHASSIS_VARIABLE_H__

#include "Chassis_Start.h"
#include "Chassis_commen.h"
extern CHASSIS_MOVING_STATE ChassisState;   // 底盘运动状态
extern CHASSIS_MOVING_STATE ChassisControl; // 底盘运动状态控制
extern WHEEL_COMPONENT WheelComponent;      // 轮子控制

#endif // __CHASSIS_VARIABLE_H__