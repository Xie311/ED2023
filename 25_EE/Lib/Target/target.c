/*
 * @Author: X311
 * @Date: 2024-05-14 00:50:30
 * @LastEditors: X311 
 * @LastEditTime: 2024-07-27 23:56:04
 * @FilePath: \Gantry_final\UserCode\Lib\Target\target.c
 * @Brief: 
 * 
 * Copyright (c) 2024 by X311, All Rights Reserved. 
 */
/**
 * @file wtr_target.c
 * @author X311
 * @brief 接受上位机数据
 * @version 0.0
 * @date 2024-05-12
 * @copyright Copyright (c) 2024
 *
 */
#include "target.h"

uint8_t receive_buffer[24];
float  weight_placement[5] = {0};
uint8_t Uart_State=0;
/**
 * @brief 上位机数据接收
 *
 */
void Upper_Target_Init()
{
    __HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);
    HAL_UART_Receive_IT(&huart5, receive_buffer, sizeof(receive_buffer));
}

//单次接收
void Upper_Target_Decode()
{
    static union {
        uint8_t data[20];
        float weight_state[5];
    } state;
   osDelay(13);

    /*******接收砝码数据********/
    if (receive_buffer[0] == 0xFF){
        if(receive_buffer[1]==0xFE){
            if (receive_buffer[22] == 0xFE) {
                if (receive_buffer[23] == 0xFF){
                    for (int i = 0; i < 20; i++) {
                        state.data[i] = receive_buffer[i + 2];
                    }

                for (int t = 0; t < 5; t++) {
                    weight_placement[t] = state.weight_state[t];
                }
                }
            }
        }
    }
}

void Target_Decode(void)
{
    float weight_placement_tmp[5] = {0};
    int tar_count = 0; // 计数连续相同数组的次数
    Uart_State = 0;

    for (; Uart_State == 0;) {
        if (flag[5] == 1) {
            Upper_Target_Decode();
            flag[5] = 0;
        }
            if (weight_placement[0] == 1) weight_placement_tmp[0] = 1;
            if (weight_placement[1] == 1) weight_placement_tmp[1] = 1;
            if (weight_placement[2] == 1) weight_placement_tmp[2] = 1;
            if (weight_placement[3] == 1) weight_placement_tmp[3] = 1;
            if (weight_placement[4] == 1) weight_placement_tmp[4] = 1;

            tar_count++; // 首次接收时，计数器初始化为1

            if (tar_count >= 10) {
                if (weight_placement_tmp[0] == 1) weight_placement[0] = 1;
                if (weight_placement_tmp[1] == 1) weight_placement[1] = 1;
                if (weight_placement_tmp[2] == 1) weight_placement[2] = 1;
                if (weight_placement_tmp[3] == 1) weight_placement[3] = 1;
                if (weight_placement_tmp[4] == 1) weight_placement[4] = 1;

                Uart_State = 1;
            }
    }
}


/**
 * @brief 启动上位机数据解码线程
 *
 */
void Target_Decode_TaskStart(void)
{
    osThreadAttr_t target_decode_Task_attributes = {
        .name       = "target_decode_Task",
        .stack_size = 128 * 10,
        .priority   = (osPriority_t)osPriorityNormal,
    };
    osThreadNew(Target_Decode_Task, NULL, &target_decode_Task_attributes);
}

/**
 * @brief   上位机数据解码线程
 */
void Target_Decode_Task(void)
{
    float weight_placement_tmp[5] = {0};
    // int switch_flag = 0;         // 判断每次接收到的数组与基准数组是否相等
    int tar_count = 0;           // 计数连续相同数组的次数

    // TaskHandle_t xHandle = NULL; // 任务句柄
    // // 获取当前任务的句柄
    // xHandle = xTaskGetCurrentTaskHandle();
    for (;;) {
        if (Uart_State == 0)  
        {
            if (flag[5] == 1) {
                Upper_Target_Decode();
                flag[5] = 0;

                if (weight_placement[0] == 1) weight_placement_tmp[0] = 1;
                if (weight_placement[1] == 1) weight_placement_tmp[1] = 1;
                if (weight_placement[2] == 1) weight_placement_tmp[2] = 1;
                if (weight_placement[3] == 1) weight_placement_tmp[3] = 1;
                if (weight_placement[4] == 1) weight_placement_tmp[4] = 1;
        
                tar_count++; // 首次接收时，计数器初始化为1

                if (tar_count >= 10)
                {
                    if (weight_placement_tmp[0] == 1) weight_placement[0] = 1;
                    if (weight_placement_tmp[1] == 1) weight_placement[1] = 1;
                    if (weight_placement_tmp[2] == 1) weight_placement[2] = 1;
                    if (weight_placement_tmp[3] == 1) weight_placement[3] = 1;
                    if (weight_placement_tmp[4] == 1) weight_placement[4] = 1;

                    Uart_State  = 1;           
                }
            }
        }
        // if (Uart_State == 0) {
        //     if (flag[5] == 1) {
        //         Upper_Target_Decode();
        //         for (int i = 0; i < 5; i++) {
        //             weight_placement_tmp[i] = weight_placement[i];
        //         }
        //         flag[5] = 0;
        //         tar_count   = 1; // 首次接收时，计数器初始化为1
        //         Uart_State  = 1;
        //     }
        // } 
        
        // else if (Uart_State == 1) {
        //     if (flag[5] == 1) {
        //         Upper_Target_Decode();
        //         flag[5] = 0;
        //         switch_flag = 0;

        //         for (int i = 0; i < 5; i++) {
        //             if (weight_placement_tmp[i] != weight_placement[i]) {
        //                 switch_flag = 1;
        //                 break;
        //             }
        //         }

        //         // 收到的数组与基准数组相等
        //         if (switch_flag == 0) {
        //             tar_count++;
        //         }
        //         // 收到的数组与基准数组不相等
        //         else {
        //             tar_count = 1; // 重新计数
        //             for (int i = 0; i < 5; i++) {
        //                 weight_placement_tmp[i] = weight_placement[i];
        //             }
        //         }

        //         // 如果连续(x次)接收到同样的数组，则把这个数组设置为最终值
        //         if (tar_count >= 15) {
        //             Uart_State = 2;
        //             vTaskSuspend(xHandle);
        //         }
        //     }

            osDelay(6);
        }
}