/*
 * @Author: X311
 * @Date: 2024-05-16 22:06:32
 * @LastEditors: X311 
 * @LastEditTime: 2024-08-20 21:14:19
 * @FilePath: \Gantry_final\UserCode\Upper\Upper_Debug\upper_debug.c
 * 
 */
#include "upper_debug.h"

/**
 * @brief 调试线程开启
 * 
 */
void Upper_Debug_TaskStart(void)
{
    const osThreadAttr_t upper_debug_Task_attributes = {
        .name       = "upper_debug_Task",
        .stack_size = 128 * 10,
        .priority   = (osPriority_t)osPriorityNormal,
    };
    osThreadNew(Upper_Debug_Task, NULL, &upper_debug_Task_attributes);
}

/**
 * @brief   调试线程
 */
void Upper_Debug_Task(void *argument)
{
    osDelay(100);
    for(;;){
        printf("%f,%f\n",)

    }
    // for (;;) {
    //     printf("%f,%f,%f,%f,%f,%f,%f,%d\n",
    //            distance_aver[0], distance_aver[1], distance_aver[2], distance_aver[3],distance_aver[4],
    //            Upper[0].Motor_Y->speedPID.output, Upper[1].Motor_Y->speedPID.output, (int)stake_flag
    //     );
    //}
}

// /**
//  * @brief OLED线程开启
//  */
// void Upper_OLED_TaskStart(void)
// {
//     const osThreadAttr_t upper_oled_Task_attributes = {
//         .name       = "upper_oled_Task",
//         .stack_size = 128 * 10,
//         .priority   = (osPriority_t)osPriorityNormal,
//     };
//     osThreadNew(Upper_OLED_Task, NULL, &upper_oled_Task_attributes);
// }

// /**
//  * @brief   OLED线程
//  */
// void Upper_OLED_Task(void *argument)
// {
//     OLED_Init(); //屏幕初始化
//     OLED_Clear(); //先清屏
//     osDelay(100);
//     for (;;) {
//         OLED_ShowNum(5, 1, distance_aver[0], 10, 16);
//         //OLED_ShowNum(25, 1, distance_aver[2], 10, 16);
//         OLED_ShowNum(45, 2, distance_aver[1], 10, 16);
//         //OLED_ShowNum(65, 1, distance_aver[3], 10, 16);
//         // OLED_ShowNum(85, 1, distance_aver[4], 10, 16);

//        // OLED_ShowNum(5, 4, (int)Uart_State , 5, 16);
//         //OLED_ShowNum(80,4, (int)receive_buffer[23], 5, 16);

//         osDelay(20);
//     }
// }

