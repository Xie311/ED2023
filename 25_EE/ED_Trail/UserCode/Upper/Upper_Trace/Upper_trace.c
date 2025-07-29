/*
 * @Author: X311
 * @Date: 2025-07-29 01:29:53
 * @LastEditors: X311 
 * @LastEditTime: 2025-07-29 13:44:34
 * @FilePath: \ED_Trail\UserCode\Upper\Upper_Trace\Upper_trace.c
 * @Brief: 
 * @note: 检测到黑色 高电平，灭灯
 * 
 * Copyright (c) 2025 by X311, All Rights Reserved. 
 */
#include "Upper_trace.h"


uint8_t sensorValues[5];
/**
 * @brief 循迹线程开启
 * 
 */
void Upper_Trace_TaskStart(void)
{
    const osThreadAttr_t upper_trace_Task_attributes = {
        .name       = "upper_trace_Task",
        .stack_size = 128 * 10,
        .priority   = (osPriority_t)osPriorityNormal,
    };
    osThreadNew(Upper_Trace_Task, NULL, &upper_trace_Task_attributes);
}

/**
 * @brief   循迹线程
 */
void Upper_Trace_Task(void *argument)
{
    osDelay(100);
    for (int i = 0; i < 5; i++) {
    sensorValues[i] = 0; // 初始化传感器值为0
    }

    for (;;) {
        readSensors(sensorValues);

         if (sensorValues[0] == 1) {        //最左侧压线
            // Turn llleft
        } else if (sensorValues[4] == 1) {  //最右侧压线
            // Turn rrright
        } else if (sensorValues[1] == 1) {
            // Turn left
        } else if (sensorValues[3] == 1) {
            // Turn right
        } else if (sensorValues[2] == 1) {
            // Move forward
        } else {
            // Stop or adjust
        }
    }
}


void readSensors(uint8_t *sensorValues) {
    // 读取传感器引脚的电平值  ** 黑色为高电平 **
    sensorValues[0] = HAL_GPIO_ReadPin(L2_GPIO_Port,L2_Pin); // 读取最左侧传感器
    sensorValues[1] = HAL_GPIO_ReadPin(L1_GPIO_Port,L1_Pin); // 读取左侧第二个传感器
    sensorValues[2] = HAL_GPIO_ReadPin(M_GPIO_Port,M_Pin);   // 读取中间传感器
    sensorValues[3] = HAL_GPIO_ReadPin(R1_GPIO_Port,R1_Pin);  // 读取右侧第二个传感器
    sensorValues[4] = HAL_GPIO_ReadPin(R2_GPIO_Port,R2_Pin);  // 读取最右侧传感器
}



