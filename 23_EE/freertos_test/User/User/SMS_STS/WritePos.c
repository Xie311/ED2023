#include "main.h"
#include "SCServo.h"

void FT_Write_Position_Setup(void)
{
	setEnd(0);//SMS_STS���Ϊ��˴洢�ṹ
}

void FT_Write_Position(void)
{
  //���(ID1)������ٶ�V=60*0.732=43.92rpm�����ٶ�A=50*8.7deg/s^2��������P1=4095λ��
  WritePosEx(1, 8190, 2400, 50);
  //HAL_Delay((4095-0)*1000/(60*50) + (60*50)*10/(50) + 50);//[(P1-P0)/(V*50)]*1000+[(V*50)/(A*100)]*1000 + 50(���)
  HAL_Delay(5000);
  //���(ID1)������ٶ�V=60*0.732=43.92rpm�����ٶ�A=50*8.7deg/s^2��������P0=0λ��
  WritePosEx(1, 4095, 2400, 50);
 // HAL_Delay((4095-0)*1000/(60*50) + (60*50)*10/(50) + 50);//[(P1-P0)/(V*50)]*1000+[(V*50)/(A*100)]*1000 + 50(���)
  HAL_Delay(5000);

}
