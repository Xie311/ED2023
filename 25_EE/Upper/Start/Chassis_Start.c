/*
 * @Author Chen Yitong
 * @Date 2023-09-22 22:19:27
 * @LastEditors: X311 
 * @LastEditTime: 2025-07-26 21:13:08
 * @FilePath: \ED_Trace\UserCode\Upper\Start\Chassis_Start.c
 * @brief 底盘启动文件
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */
#include "Chassis_Start.h"

/**
 * @brief 默认任务，启动其他线程
 * @return {*}
 */
void StartDefaultTask(void const *argument)
{
    //================初始化====================
    Chassis_StateMachine_Init(); // 状态机初始化
    Chassis_Servo_Init();        // 底盘电机初始化

    //================启动线程==================
    //OPS_Decode_TaskStart();
    Chassis_StateMachine_TaskStart(); // 状态机进程启动
    Chassis_Servo_TaskStart();        // 底盘伺服进程启动
    Tar_Debug_TaskStart();            // 调试线程

    for (;;) {
        //HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        osDelay(800);
    }
}