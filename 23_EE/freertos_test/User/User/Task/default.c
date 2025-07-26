#include "head.h"

#define DISTANCE 1000 //云台到白板的距离 单位mm
#define PI 3.141592
#define ANGLE_MAXIMUM_UP 160  //限定舵机能往一边转的角度（4095为一圈
#define ANGLE_MAXIMUM_DOWN 300  // 限定舵机能往一边转的角度（4095为一圈
#define ANGLE_MAXIMUM_LEFT 400  //限定舵机能往一边转的角度（4095为一圈
#define ANGLE_MAXIMUM_RIGHT 400  // 限定舵机能往一边转的角度（4095为一圈

/*复位标志*/
__IO uint8_t Reset;
/*题号*/
__IO uint8_t Q_NO;

/***target from openmv***/
#ifdef GREEN
uint8_t Rxbuffer[16];
#endif
#ifdef RED
uint8_t Rxbuffer[16];
uint8_t dot1x,dot1y,dot2x,dot2y,dot3x,dot3y,dot4x,dot4y;
int16_t tar_pos1,tar_pos2;
#endif
uint8_t red_x,red_y;
/***Feedback Data***/
int16_t fdb_pos1;
int16_t fdb_pos2;

int16_t ori_pos1;
int16_t ori_pos2;

uint16_t flag;
/***Feedback Data***/

/*spe_*/
int16_t spe1;
int16_t spe2;

__IO uint8_t mode1=0; //电机模式
__IO uint8_t mode2=0; //电机模式
uint8_t ERROR_FLAG;

void StartDefaultTask(void *argument)
{/**
    #ifdef GREEN
    HAL_UART_Receive_IT(&huart3, Rxbuffer, sizeof(Rxbuffer));
    #endif 
    #ifdef RED
    HAL_UART_Receive_IT(&huart3,Rxbuffer,sizeof(Rxbuffer));
    #endif
    */
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 20); // arr=100/越小越亮
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 80); // arr=100
    WheelMode(1, 0); // 模式0：位置模式 模式1：恒速； 模式2：pwm调速；模式3：步进
    WheelMode(2,0); //模式0：位置模式 模式1：恒速； 模式2：pwm调速；模式3：步进
    WriteSpe(1,0,0);
    WriteSpe(2,0,0);
    Q_NO=1;
    HAL_UART_Receive_IT(&huart2, Rx, 1);

#ifdef RED_NO_EXTI
    if (HAL_GPIO_ReadPin(Q_NO2_GPIO_Port, Q_NO2_Pin) == 0) {
        Q_NO = 2; /*红光，模式2，不用再写mode因为已经默认是0了*/
    }
    if (HAL_GPIO_ReadPin(Q_NO3_GPIO_Port, Q_NO3_Pin) == 0) {
        Q_NO  = 3;
        mode1 = 1;
        mode2 = 1;
        WheelMode(1, mode1);
        WheelMode(2, mode2);
    }
    #endif // red
    
    #ifdef GREEN
    if (HAL_GPIO_ReadPin(Q_NO4_GPIO_Port, Q_NO4_Pin) == 0) {
        Q_NO = 4; /*绿光，拓展题，用4是为了区别于前面红光的所有模式*/
        mode1 = 1;
        mode2 = 1;
        WheelMode(1, mode1);
        WheelMode(2, mode2);
    }
    #endif
    Servo_Start();
    Statemachine_Start();
    


    for (;;) {
    fdb_pos1=ReadPos(1);
    fdb_pos2=ReadPos(2);
        //反馈数据
    if(flag==0){    //只运行一次，记录初始角度数据（一般为2048上下
        ori_pos1=fdb_pos1;
        ori_pos2=fdb_pos2;
        flag=1;       
    }
    

  /*判断角度是否在正常范围内*/  /**其实只对红光有用，绿光应该无阈值**/
  #ifdef REDa
   if(ori_pos1>=1000&&ori_pos1<=4095-1000)
    {
      if(fdb_pos1<=ori_pos1-ANGLE_MAXIMUM_LEFT||fdb_pos1>=ori_pos1+ANGLE_MAXIMUM_RIGHT)
      {
        spe1=0;
        mode1=0;
        WheelMode(1,mode1);
        spe2=0;
        mode2=0;
        WheelMode(2,mode2);
        ERROR_FLAG=1;
      }
    }

    if(ori_pos2>=1000&&ori_pos2<=4095-1000)
    {
      //if(fdb_pos2<=3000&&fdb_pos2>=2000)
      if(fdb_pos2<=ori_pos2-ANGLE_MAXIMUM_DOWN||fdb_pos2>=ori_pos2+ANGLE_MAXIMUM_UP)
      {
        spe2=0;
        mode2=0;
        WheelMode(2,mode2);
        spe1=0;
        mode1=0;
        WheelMode(1,mode1);
        ERROR_FLAG=1;

      }
    }
    /*判断角度是否在正常范围内*/ 
   #endif
    printf("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", (float)Q_NO, (float)state,(float)spe1, (float)spe2, (float)red_x, (float)red_y, (float)dot1x, (float)dot1y, (float)dot2x, (float)dot2y, (float)fdb_pos2);
   // printf("CR1:0x%04X\n",USART3->CR1);
    }
        
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == RED_NO1_RESET_Pin) // 引脚判断
    //复位 以及红光no1
    {
        Reset = 1;
        Q_NO  = 1;
        mode1 = 0;
        mode2 = 0;
        WheelMode(1, mode1);
        WheelMode(2, mode2);
    }
    #ifdef RED

    if (GPIO_Pin == Q_NO2_Pin) // 引脚判断
    {
        Q_NO = 2;
        mode1 = 0;
        mode2 = 0;
        WheelMode(1, mode1);
        WheelMode(2, mode2);
    }
    if (GPIO_Pin == Q_NO3_Pin) // 引脚判断
    {
        Q_NO = 3;
        mode1 = 1;
        mode2 = 1;
        WheelMode(1, mode1);
        WheelMode(2, mode2);
        //U_Transmit(3);
    }
    #endif
    #ifdef GREEN
    if (GPIO_Pin == Q_NO4_Pin) // 引脚判断
    {
        Q_NO = 4;
        mode1 = 1;
        mode2 = 1;
        WheelMode(1, mode1);
        WheelMode(2, mode2);
        // U_Transmit(4);
    }
    #endif
    
}