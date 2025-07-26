/*
 * @Author: ZYT
 * @Date: 2025-05-11 00:34:40
 * @LastEditors: ZYT
 * @LastEditTime: 2025-05-23 23:11:16
 * @FilePath: \pantilt_freertos\User\SCSLib\SCSerail.c
 * @Brief: 
 * 
 * Copyright (c) 2025 by zyt, All Rights Reserved. 
 */
/*
 * SCServo.c
 * ���ض��Ӳ���ӿڲ����
 * ����: 2024.12.2
 * ����: txl
 */
#include <stdint.h>
#include "SCServo.h"
#include "main.h"
#include "head.h"

#include "usart.h"
#include "gpio.h"
uint8_t wBuf[128];
uint8_t wLen = 0;
extern uint16_t flag;//����һ�����Ա���


//UART �������ݽӿ�
int readSCS(unsigned char *nDat, int nLen)
{
	return ftUart_Read(nDat, nLen);
}

//UART �������ݽӿ�
int writeSCS(unsigned char *nDat, int nLen)
{
	while(nLen--){
		if(wLen<sizeof(wBuf)){
			wBuf[wLen] = *nDat;
			wLen++;
			nDat++;
		}
	}
	return wLen;
}

//���ջ�����ˢ��
void rFlushSCS()
{
	ftBus_Delay();
}

//���ͻ�����ˢ��
void wFlushSCS()
{
	if(wLen){
		ftUart_Send(wBuf, wLen);
		wLen = 0;
	}
}
//���ڷ��ͺ���
void ftUart_Send(uint8_t *nDat , int nLen)
{
	HAL_UART_Transmit(&huart1, nDat, nLen, 100);
}
//���ڶ�ȡ����
int ftUart_Read(uint8_t *nDat, int nLen)
{
	if(HAL_OK!=HAL_UART_Receive(&huart1, nDat, nLen, 100)){
		//flag=nLen;
		return 0;
	}else{
		return nLen;
	}
}

//�Զ�����ʱ����������10us
void ftBus_Delay(void)
{
	osDelay(1);
}