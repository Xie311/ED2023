/*
 * @Author: X311
 * @Date: 2025-07-26 20:50:56
 * @LastEditors: X311 
 * @LastEditTime: 2025-07-26 21:09:48
 * @FilePath: \ED_Trace\UserCode\Upper\Debug\Debug.c
 * @Brief: 
 * 
 * Copyright (c) 2025 by X311, All Rights Reserved. 
 */
#include "Debug.h"

/**
 * @brief   数据接收线程开启
 */
void Tar_Debug_TaskStart(void)
{

    osThreadId_t Tar_debug_TaskHandle;
    const osThreadAttr_t Tar_debug_Task_attributes = {
    .name       = "Tar_debug_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

    Tar_debug_TaskHandle = osThreadNew(Tar_Debug_Task, NULL, &Tar_debug_Task_attributes);
}

/**
 * @brief   数据接收线程
 */
void Tar_Debug_Task(void *argument)
{
    for (;;) {
        //printf("%d\r\n", (int)(Tar_Data.pos_x));
        osDelay(1000);
    }
}