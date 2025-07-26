#include "main.h"
#include "SCServo.h"
#include <stdio.h>

void FT_Get_FT_ID_Setup(void)
{
}

void FT_Get_FT_ID(void)
{
  int ID = Ping(1);
  if(!getLastError()){
    printf("Servo ID:%d\n", ID);
    HAL_Delay(100);
  }else{
    printf("Ping servo ID error!\n");
    HAL_Delay(2000);
  }
}
