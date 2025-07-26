/*
 * @Author: ZYT
 * @Date: 2025-05-12 23:00:46
 * @LastEditors: ZYT
 * @LastEditTime: 2025-05-25 03:26:27
 * @FilePath: \pantilt_freertos\User\User\UART\uart_RT.c
 * @Brief: 
 * 
 * Copyright (c) 2025 by zyt, All Rights Reserved. 
 */
#include "uart_RT.h"
#include "head.h"
void Decode_Greenligit(uint8_t* Receivebuffer);
void Decode_Redligit(uint8_t* Receivebuffer);

uint8_t Rx[1];
uint8_t dot_cal_x[400];
uint8_t dot_cal_y[400];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART2)
    {
        static uint8_t cnt = 0;
#ifdef GREEN
        Decode_Greenligit(Rxbuffer);
        
        #endif
        #ifdef RED
        if(cnt==0)
        {
            if(Rx[0]==0xAA)
            {
                cnt++;
                Rxbuffer[0] = 0xAA;
            }
        }else if (cnt==1)
        {
            if (Rx[0] <= 0xFF) {
                cnt++;
                Rxbuffer[1] = 0xFF;
            }
            else{
                cnt--;
                //osDelay(1);
            }
        }else if (cnt>=2&&cnt<=13)
        {
            Rxbuffer[cnt] = Rx[0];
            cnt++;
        }else if (cnt==14)
        {
            if (Rx[0] <= 0xFF) {
                cnt++;
                Rxbuffer[14] = 0xFF;
            } else {cnt=0;}
        } else if (cnt == 15) {
            if (Rx[0] == 0xAA) {
                Rxbuffer[15] = 0xAA;
                Decode_Redligit(Rxbuffer);
            } 
            cnt = 0;
        }

        #endif
        HAL_UART_Receive_IT(&huart2, Rx, 1);
        //osDelay(1);
    }
}

        //Speed = -(Speed&~(1<<15));
void U_Transmit(uint8_t num)
{
    uint8_t Txbuffer[5];
    Txbuffer[0]=0xEE;
    Txbuffer[1]=0xEE;
    Txbuffer[2]=num;
    Txbuffer[0]=0xFF;
    Txbuffer[1]=0xFF;
    HAL_UART_Transmit(&huart2,Txbuffer,sizeof(Txbuffer),0xff);
}

void Decode_Greenligit(uint8_t* Receivebuffer)
{
    if(Receivebuffer[0]==0xAA&&Receivebuffer[1]==0xFF&&Receivebuffer[14]==0xFF&&Receivebuffer[15]==0xAA)
        {
            int16_t spe1_tmp=0;
            int16_t spe2_tmp=0;
            //spe1_tmp = (Receivebuffer[2] << 8) | Receivebuffer[3];  // 16-bit 组合
           // spe2_tmp = (Receivebuffer[4] << 8) | Receivebuffer[5];      //高位在前低位在后，大端模式
           spe1_tmp = Receivebuffer[12]-Receivebuffer[10];
           spe2_tmp = Receivebuffer[13]-Receivebuffer[11];//绿-红
           //    if(spe1_tmp>128)
           //    {
           //     spe1 =-(spe1_tmp-256)*10;
           //    }else{
           //     spe1=-spe1_tmp*10;
           //    }
           //    if(spe2_tmp>128){
           //    spe2 = (spe2_tmp-256)*6;
           //    }else{
           //     spe2=spe2_tmp*6;
           //    }
           spe1 = spe1_tmp;
           spe2 = -spe2_tmp;
    }
        //HAL_UART_Receive_IT(&huart3,Receivebuffer,sizeof(Receivebuffer));
}
#ifdef RED


void Decode_Redligit(uint8_t* Receivebuffer)
{
    if(Receivebuffer[0]==0xAA&&Receivebuffer[1]==0xFF&&Receivebuffer[14]==0xFF&&Receivebuffer[15]==0xAA)
        {
            dot1x=Receivebuffer[2];
            dot1y=Receivebuffer[3];
            dot2x=Receivebuffer[4];
            dot2y=Receivebuffer[5];
            dot3x=Receivebuffer[6];
            dot3y=Receivebuffer[7];
            dot4x=Receivebuffer[8];
            dot4y=Receivebuffer[9];
            red_x=Receivebuffer[10];
            red_y=Receivebuffer[11];
            
            for(uint8_t j=0;j<100;j++)
            {
                dot_cal_x[j]=1.0*j/100*dot2x+1.0*(100-j)/100*dot1x;
                dot_cal_y[j]=1.0*j/100*dot2y+1.0*(100-j)/100*dot1y;
            }
            for(uint8_t j=0;j<100;j++)
            {
                dot_cal_x[j+100]=1.0*j/100*dot3x+1.0*(100-j)/100*dot2x;
                dot_cal_y[j+100]=1.0*j/100*dot3y+1.0*(100-j)/100*dot2y;
            }
            for(uint8_t j=0;j<100;j++)
            {
                dot_cal_x[j+200]=1.0*j/100*dot4x+1.0*(100-j)/100*dot3x;
                dot_cal_y[j+200]=1.0*j/100*dot4y+1.0*(100-j)/100*dot3y;
            }
            for(uint8_t j=0;j<100;j++)
            {
                dot_cal_x[j+300]=1.0*j/100*dot1x+1.0*(100-j)/100*dot4x;
                dot_cal_y[j+300]=1.0*j/100*dot1y+1.0*(100-j)/100*dot4y;
            }
            
        }
        
}
#endif // DEBUG