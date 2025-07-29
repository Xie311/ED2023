#include "wtr_uart.h"
#include "wtr_dji.h"
#include "string.h"
#include "math.h"
#include "usart.h"

Remote_t RemoteCtl_RawData;
uint8_t JoyStickReceiveData[18];

double angMax = 360;
double posRef; // 工程中也可调用需要参数来传值

/**
 * @brief 解码大疆电机
 * @return {*}
 */
void DjiRemoteCtl_Decode()
{

    RemoteCtl_RawData.ch0 = ((int16_t)JoyStickReceiveData[0] | ((int16_t)JoyStickReceiveData[1] << 8)) & 0x07FF;
    RemoteCtl_RawData.ch1 = (((int16_t)JoyStickReceiveData[1] >> 3) | ((int16_t)JoyStickReceiveData[2] << 5)) & 0x07FF;
    RemoteCtl_RawData.ch2 = (((int16_t)JoyStickReceiveData[2] >> 6) | ((int16_t)JoyStickReceiveData[3] << 2) |
                             ((int16_t)JoyStickReceiveData[4] << 10)) &
                            0x07FF;
    RemoteCtl_RawData.ch3   = (((int16_t)JoyStickReceiveData[4] >> 1) | ((int16_t)JoyStickReceiveData[5] << 7)) & 0x07FF;
    RemoteCtl_RawData.left  = ((JoyStickReceiveData[5] >> 4) & 0x000C) >> 2;
    RemoteCtl_RawData.right = ((JoyStickReceiveData[5] >> 4) & 0x0003);
    RemoteCtl_RawData.wheel = ((int16_t)JoyStickReceiveData[16]) | ((int16_t)JoyStickReceiveData[17] << 8);
    /* UART1 callback decode function  */
}

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

//
//}
