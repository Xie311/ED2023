/*
 * @Author: Chen Yitong
 * @Date: 2023-09-22 22:19:36
 * @LastEditors: x311 
 * @LastEditTime: 2024-04-29 00:03:13
 * @FilePath: \WTR_Omni_Auto_Chassis\UserCode\Chassis\Start\Chassis_Start.h
 * @brief 底盘启动文件以及所有库
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */
#ifndef __CHAEEIS_START_H__
#define __CHAEEIS_START_H__
#include "main.h"
#include "can.h"
#include "gpio.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "wtr_calculate.h"
#include "wtr_can.h"
#include "wtr_dji.h"
#include "Chassis_Callback.h"
#include "Chassis_commen.h"
#include "Chassis_UserConfig.h"
#include "Chassis_Servo.h"
#include "Chassis_StateMachine.h"
#include "Chassis_Variable.h"
#include "string.h" 
#include "io_retargetToUart.h"
#include "Debug.h"

#endif