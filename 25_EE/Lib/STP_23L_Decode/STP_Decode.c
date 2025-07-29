/*
 * @Author: X311
 * @Date: 2024-05-13 09:00:14
 * @LastEditors: X311 
 * @LastEditTime: 2024-07-25 01:41:27
 * @FilePath: \Gantry_final\UserCode\Lib\STP_23L_Decode\STP_Decode.c
 * @Brief: 
 * 
 * Copyright (c) 2024 by X311, All Rights Reserved. 
 */

#include "STP_Decode.h"

uint8_t Rxbuffer[5][195];  //接收数据数组，共195位，含12组数据
LidarPointTypedef LidarData[5][12];
float distance_aver[5];
void STP_23L_Decode(uint16_t num)       //num:指明是第几个雷达，本代码框架中范围为0-3
{
    //printf("1\n");
    if ((Rxbuffer[num][0] == Rxbuffer[num][1]) && (Rxbuffer[num][1] == Rxbuffer[num][2]) && (Rxbuffer[num][2] == Rxbuffer[num][3]) && (Rxbuffer[num][3] == 0xAA)) // 检测帧头
    {
        if (Rxbuffer[num][5] == PACK_GET_DISTANCE)                                                                       //检测命令码
        {   
            uint32_t CS_sum = 0;
            for (uint16_t i = 4; i < 194; i++) CS_sum += Rxbuffer[num][i];
            if (Rxbuffer[num][194] == CS_sum%256)                                                                        //检测校验码
            {
           
                for (uint16_t i = 0; i < 12; i++)
                {
                    // OLED_ShowNum(90, 3, Rxbuffer[5], 2, 8);
                    LidarData[num][i].distance   = Rxbuffer[num][10 + 15 * i] + (Rxbuffer[num][11 + 15 * i] * 256); // 只有distance是有用的数据 注意前面是低位后面是高位
                    LidarData[num][i].noise      = (Rxbuffer[num][13 + 15 * i] << 8) + Rxbuffer[num][12 + 15 * i];
                    LidarData[num][i].peak       = (Rxbuffer[num][17 + 15 * i] << 24) + (Rxbuffer[num][15 + 15 * i] << 16) + (Rxbuffer[num][16 + 15 * i] << 8) + Rxbuffer[num][15 + 15 * i];
                    LidarData[num][i].confidence = Rxbuffer[num][18 + 15 * i];
                    LidarData[num][i].intg       = (Rxbuffer[num][22 + 15 * i] << 24) + (Rxbuffer[num][21 + 15 * i] << 16) + (Rxbuffer[num][20 + 15 * i] << 8) + Rxbuffer[num][19 + 15 * i];
                    LidarData[num][i].reftof     = (Rxbuffer[num][24 + 15 * i] << 8) + Rxbuffer[num][23 + 15 * i];
                }
                float distance_sum = 0;
                for (uint16_t i = 0; i < 12; i++) {
                    distance_sum += LidarData[num][i].distance;
                }
                distance_aver[num] = distance_sum / 12;
            }
         }
    }
}

/**
 * @brief 启动激光解码线程
 *
 */
void Upper_Decode_TaskStart(void)
{
    osThreadAttr_t upper_decode_Task_attributes = {
        .name       = "upper_decode_Task",
        .stack_size = 128 * 10,
        .priority   = (osPriority_t)osPriorityHigh,
    };
    osThreadNew(Upper_Decode_Task, NULL, &upper_decode_Task_attributes);
}

/**
 * @brief   激光解码线程
 */
void Upper_Decode_Task(void)
{
    osDelay(20);
    for (;;) {
        for(int i=0;i<5;i++){
            if(flag[i]){
                STP_23L_Decode(i);
                flag[i] = 0;
            }
        }
        osDelay(3);
    }
}