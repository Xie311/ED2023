/*
 * @Author: X311
 * @Date: 2024-04-16
 * @LastEditors: x311 
 * @LastEditTime: 2024-04-21 23:33:12
 * @FilePath: \WTR_Omni_Auto_Chassis\UserCode\Chassis\Callback\Chassis_Callback.c
 * @Brief: 底盘用中断回调函数
 */

#include "Chassis_Callback.h"

/**
 * @brief: 串口接收中断回调函数
 * @param {UART_HandleTypeDef} *huart 中断串口
 * @return {*}
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // if (huart->Instance == OPS_UART) 
    // {
    //     // 检查起始和结束字符以确保数据完整性
    //     if ((ops_buffer[0] == 0x0D) && (ops_buffer[1] == 0x0A) && (ops_buffer[26] == 0x0A) && (ops_buffer[27] == 0x0D))
    //     {
    //         OPS_Decode(); // 当数据接收完成时调用OPS码盘解码函数
    //         HAL_UART_Receive_IT(&huart8, ops_buffer, sizeof(ops_buffer));
    //     }
    // }


}


