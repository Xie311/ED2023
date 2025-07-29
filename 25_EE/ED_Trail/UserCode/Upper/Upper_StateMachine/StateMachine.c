/*
 * @Author: X311
 * @Date: 2024-05-13 09:00:14
 * @LastEditors: X311 
 * @LastEditTime: 2024-08-22 22:47:17
 * @FilePath: \Gantry_final\UserCode\Upper\Upper_StateMachine\StateMachine.c
 * @Brief: 
 * 
 * Copyright (c) 2024 by X311, All Rights Reserved. 
 */
#include "StateMachine.h"
#include <stdlib.h>

Chassis_MOVING_STATE Chassis_Control;

/****************线程相关函数********************/
void Chassis_StateMachine_Task(void *argument)
{
    osDelay(100);
    for (;;){
        Chassis_Control.velocity.x = 10; // 速度
        Chassis_Control.velocity.w = 4;  // 角速度

        osDelay(6);
    }
}



/**
 * @brief: 状态机线程启动
 * @return {*}
 */
void Chassis_StateMachine_TaskStart()
{
    osThreadId_t Chassis_StateMachine_TaskHandle;
    const osThreadAttr_t Chassis_StateMachine_Task_attributes = {
    .name = "Chassis_StateMachine_Task",
    .stack_size = 128 * 10,
    .priority = (osPriority_t) osPriorityHigh,
    };
    Chassis_StateMachine_TaskHandle = osThreadNew(Chassis_StateMachine_Task, NULL, &Chassis_StateMachine_Task_attributes); 
}

/**
 * @brief 初始化状态机
 * @return {*}
 */

void Chassis_StateMachine_Init()
{
    Chassis_Control.xMutex_control = xSemaphoreCreateRecursiveMutex();
    Chassis_MotorComponent.xMutex_motor = xSemaphoreCreateRecursiveMutex();
}



