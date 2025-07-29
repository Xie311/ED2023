/*
 * @Author: Chen Yitong 3083697520@qq.com
 * @Date: 2023-09-23 11:33:41
 * @LastEditors: X311 
 * @LastEditTime: 2025-07-26 22:12:36
 * @FilePath: \ED_Trace\UserCode\Upper\StateMachine\Chassis_StateMachine.c
 * @brief 底盘状态机
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */
#include "Chassis_StateMachine.h"
CHASSIS_MOVING_STATE ChassisControl;
CHASSIS_MOVING_STATE ChassisState;
Tar_t Tar_Data_tmp; // 上位机传来的目标位置
/**
 * @brief: 状态机线程
 * @return {*}
 * @note   2024/4/12  测试代码：删去Remote_t RemoteCtl_Data_tmp = RemoteCtl_RawData，直接给定Remote_t RemoteCtl_Data_tmp结构体中RemoteCtl_Data_tmp.left，ch0、ch1、ch2的值
 */
void Chassis_StateMachine_Task(void const *argument)
{
    for (;;) {
       

        osDelay(10);
    }
}

/**
 * @brief: 状态机线程启动
 * @return {*}
 */
void Chassis_StateMachine_TaskStart()
{
    osThreadAttr_t Chassis_StateMachine_attr =
        {
            .name       = "Chassis_StateMachine",
            .priority   = osPriorityAboveNormal,
            .stack_size = 128 * 10,
        };
    osThreadNew(Chassis_StateMachine_Task, NULL, &Chassis_StateMachine_attr);
}

/**
 * @brief 初始化状态机
 * @return {*}
 */

void Chassis_StateMachine_Init()
{
    ChassisControl.xMutex_control = xSemaphoreCreateRecursiveMutex();
    ChassisState.xMutex_control   = xSemaphoreCreateRecursiveMutex();
    WheelComponent.xMutex_wheel   = xSemaphoreCreateRecursiveMutex();
    
    // const osMutexAttr_t mutex_attr =
    // {
    //     .name = "Chassis_Mutex"
    // };
    // ChassisControl.xMutex_control = osMutexNew(&mutex_attr);
    // ChassisState.xMutex_control = osMutexNew(&mutex_attr);
    // WheelComponent.xMutex_wheel = osMutexNew(&mutex_attr);
}

void Chassis_SteerinfWheelCorrect()
{
}