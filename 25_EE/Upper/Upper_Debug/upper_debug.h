/*
 * @Author: X311
 * @Date: 2025-07-27 00:10:26
 * @LastEditors: X311 
 * @LastEditTime: 2025-07-27 01:39:30
 * @FilePath: \ED_Trail\UserCode\Upper\Upper_Debug\Upper_Debug.h
 * @Brief: 
 * 
 * Copyright (c) 2025 by X311, All Rights Reserved. 
 */
#ifndef __UPPER_DEBUG_H
#define __UPPER_DEBUG_H

#include "Chassis_start.h"

void Upper_Debug_TaskStart(void);
void Upper_Debug_Task(void *argument);
void Upper_OLED_TaskStart(void);
void Upper_OLED_Task(void *argument);
void Upper_Reset_TaskStart(void);
void Upper_Reset_Task(void *argument);
#endif