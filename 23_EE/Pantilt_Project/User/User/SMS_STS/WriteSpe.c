#include "main.h"
#include "SCServo.h"

void FT_Write_Speed_Setup(void)
{
	setEnd(0);//SMS_STS���Ϊ��˴洢�ṹ
  WheelMode(1,1);//���ID1�л����������ģʽ
}

void FT_Write_Speed(void)
{
  //���(ID1)�Լ��ٶ�A=50*8.7deg/s^2������������ٶ�V=60*0.732=43.92rpm�������ֺ���������ת
  WriteSpe(1, 60, 50);
  HAL_Delay(5000);
  
  //���(ID1)�Լ��ٶ�A=50*8.7deg/s^2���������ٶ�0ֹͣ��ת
  WriteSpe(1, 0, 50);
  HAL_Delay(2000);
  
  //���(ID1/ID2)�Լ��ٶ�A=50*8.7deg/s^2������������ٶ�V=-60*0.732=-43.92rpm�������ֺ��ٷ�����ת
  WriteSpe(1, -60, 50);
  HAL_Delay(5000);
  
  //���(ID1/ID2)�Լ��ٶ�A=50*8.7deg/s^2���������ٶ�0ֹͣ��ת
  WriteSpe(1, 0, 50);
  HAL_Delay(2000);
}
