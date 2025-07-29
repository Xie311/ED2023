/*
 * @Author: X311
 * @Date: 2024-05-13 09:00:14
 * @LastEditors: X311 
 * @LastEditTime: 2025-07-29 17:54:16
 * @FilePath: \ED_Trail\UserCode\Upper\Upper_Start\UpperStart.c
 * @Brief: 
 * 
 * Copyright (c) 2024 by X311, All Rights Reserved. 
 */
#include "UpperStart.h"

/**
 * @brief 默认任务，启动其他线程
 *
 * @param argument
 */
void StartDefaultTask(void *argument)
{
    /*** 接收串口初始化 ***/
    //Gantry_usart_init(); 

    /*** 接受上位机数据 ***/
    // Upper_Target_Init();
    //TIM_HandleTypeDef htim2; // Declare htim2

    // Target_Decode_TaskStart();    // 接收线程开启

    /**** 调试代码 ****/
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 999);                                                                                                                                              

    /*** 初始化函数 ***/
    Upper_Motor_init();
    Chassis_StateMachine_Init();  // 状态机初始化

    // CanTransmit_DJI_1234(&hcan1,
    //                          1000, 1000,300,300);

    /*** 启动线程 ***/
    //Upper_Servo_TaskStart();
    Chassis_StateMachine_TaskStart();

    /*** 调试线程 ***/
    //Upper_OLED_TaskStart();
    //Upper_Debug_TaskStart();


    for (;;) {
        osDelay(800);
        
    }
}