#include "main.h"
#include "SCServo.h"

void FT_Write_Position_Setup(void)
{
	setEnd(0);//SMS_STS舵机为大端存储结构
}

void FT_Write_Position(void)
{
  //舵机(ID1)以最高速度V=60*0.732=43.92rpm，加速度A=50*8.7deg/s^2，运行至P1=4095位置
  WritePosEx(1, 8190, 2400, 50);
  //HAL_Delay((4095-0)*1000/(60*50) + (60*50)*10/(50) + 50);//[(P1-P0)/(V*50)]*1000+[(V*50)/(A*100)]*1000 + 50(误差)
  HAL_Delay(5000);
  //舵机(ID1)以最高速度V=60*0.732=43.92rpm，加速度A=50*8.7deg/s^2，运行至P0=0位置
  WritePosEx(1, 4095, 2400, 50);
 // HAL_Delay((4095-0)*1000/(60*50) + (60*50)*10/(50) + 50);//[(P1-P0)/(V*50)]*1000+[(V*50)/(A*100)]*1000 + 50(误差)
  HAL_Delay(5000);

}
