/*
 * @Author: X311
 * @Date: 2024-05-13 09:00:14
 * @LastEditors: X311 
 * @LastEditTime: 2024-07-20 23:53:07
 * @FilePath: \Gantry_final\UserCode\Upper\Upper_StateMachine\StateMachine.h
 * @Brief: 
 * 
 * Copyright (c) 2024 by X311, All Rights Reserved. 
 */
#ifndef __STATEMACHINE_H__
#define __STATEMACHINE_H__

#include "UpperStart.h"

void Upper_StateMachine_TaskStart_01(void);
void Upper_StateMachine_Init_01(void);
void Upper_StateMachine_TaskStart_02(void);
void Upper_StateMachine_Init_02(void);
void Upper_StateMachine_TaskStart_03(void);
void Upper_StateMachine_Init_03(void);
void Upper_StateMachine_TaskStart_04(void);
void Upper_StateMachine_Init_04(void);

extern Upper_COMPONENT Upper[4];
extern float weight_placement[5];
// extern float initial_pos[3];
// extern float current_pos[3];
extern uint16_t stateflag[4];
extern uint16_t stake_flag;

extern float start_tick[4];
extern float cur_tick[4];
extern float pro_tick[4];

#endif 