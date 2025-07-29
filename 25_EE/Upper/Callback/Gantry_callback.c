/*
 * @Author: X311
 * @Date: 2024-05-13 09:00:14
 * @LastEditors: X311 
 * @LastEditTime: 2024-07-25 01:48:05
 * @FilePath: \Gantry_final\UserCode\Upper\Callback\Gantry_callback.c
 * @note: 串口1、2、3、4分别接收x轴四组斜区距离数据；串口5接收上位机发送的砝码位置数组；串口6接收y轴距离数据
 * 
 * Copyright (c) 2024 by X311, All Rights Reserved. 
 */
#include "UpperStart.h"
#include "target.h"

int flag[6] = {0}; // 激光1\2\3\4\5;接收上位机数据
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        flag[0] = 1;
        HAL_UART_Receive_IT(&huart1, Rxbuffer[0], sizeof(Rxbuffer[0]));
    }

    if (huart->Instance == USART2) {
        //printf("2\n");
        flag[1] = 1;
        HAL_UART_Receive_IT(&huart2, Rxbuffer[1], sizeof(Rxbuffer[1]));
    }

    if (huart->Instance == USART3) {
        flag[2] = 1;
        HAL_UART_Receive_IT(&huart3, Rxbuffer[2], sizeof(Rxbuffer[2]));
    }

    if (huart->Instance == UART4) {
        flag[3] = 1;
        HAL_UART_Receive_IT(&huart4, Rxbuffer[3], sizeof(Rxbuffer[3]));
    }

    // y向雷达
    if (huart->Instance == USART6) {
        flag[4] = 1;
        HAL_UART_Receive_IT(&huart6, Rxbuffer[4], sizeof(Rxbuffer[4]));
    }


    if (huart->Instance == UART5) {
        // 接收上位机数据并解码
        flag[5] = 1;
        HAL_UART_Receive_IT(&huart5, (uint8_t *)receive_buffer, sizeof(receive_buffer));
        //osDelay(100);
    }

}
    






