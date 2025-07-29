/*
 * @Author: X311
 * @Date: 2024-05-13 09:00:14
 * @LastEditors: X311 
 * @LastEditTime: 2024-08-22 16:15:48
 * @FilePath: \Gantry_final\UserCode\Upper\Upper_Start\UpperStart.c
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
    /*** 接收雷达数据 ***/
    Gantry_usart_init();
    Upper_Decode_TaskStart();  //在最前面开启decode线程，尽量避免雷达传零导致疯跑

    /*** 接受树莓派数据 ***/
    Upper_Target_Init();
    Target_Decode_TaskStart();    // 接收线程开启

    /**** 调试代码 ****/
    // weight_placement[0]=1;
    // weight_placement[1]=1;
    // weight_placement[2]=1;
    // weight_placement[3]=1;
    // weight_placement[4]=1;


    /*初始化函数*/
    Upper_StateMachine_Init_01();
    Upper_StateMachine_Init_02();
    Upper_StateMachine_Init_03();
    Upper_StateMachine_Init_04();

    osDelay(200);

    // HAL_GPIO_WritePin(Reset_01_GPIO_Port, Reset_01_Pin, 0);
    // HAL_GPIO_WritePin(Reset_02_GPIO_Port, Reset_02_Pin, 0);
    // HAL_GPIO_WritePin(Reset_03_GPIO_Port, Reset_03_Pin, 0);
    // HAL_GPIO_WritePin(Reset_04_GPIO_Port, Reset_04_Pin, 0);

    HAL_GPIO_WritePin(cylinder_01_GPIO_Port, cylinder_01_Pin, GPIO_PIN_SET);       // SET气缸抬升
    HAL_GPIO_WritePin(cylinder_02_GPIO_Port, cylinder_02_Pin, GPIO_PIN_SET);       // SET气缸抬升
    HAL_GPIO_WritePin(cylinder_03_GPIO_Port, cylinder_03_Pin, GPIO_PIN_SET);       // SET气缸抬升
    HAL_GPIO_WritePin(cylinder_04_GPIO_Port, cylinder_04_Pin, GPIO_PIN_SET);       // SET气缸抬升
    HAL_GPIO_WritePin(cylinder_05_GPIO_Port, cylinder_05_Pin, GPIO_PIN_SET);       // SET气缸抬升


    HAL_GPIO_WritePin(electromagnet_01_GPIO_Port, electromagnet_01_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(electromagnet_02_GPIO_Port, electromagnet_02_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(electromagnet_03_GPIO_Port, electromagnet_03_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(electromagnet_04_GPIO_Port, electromagnet_04_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(electromagnet_05_GPIO_Port, electromagnet_05_Pin, GPIO_PIN_SET);

    osDelay(2500);
    Upper_Motor_init();
    

    /*开启线程*/
    Upper_StateMachine_TaskStart_01();
    Upper_StateMachine_TaskStart_02();
    Upper_StateMachine_TaskStart_03();
    Upper_StateMachine_TaskStart_04();
    Upper_Servo_TaskStart();

    /*** 如果要用OLED调试，先注释servo线程的cantransmit，然后开启此线程，在Upper_debug文件中的oled显示函数里加上想要显示的数据 ***/
    //Upper_OLED_TaskStart();

    //Upper_Debug_TaskStart();

    Upper_Reset_TaskStart();

    for (;;) {
        osDelay(800);
    }
}