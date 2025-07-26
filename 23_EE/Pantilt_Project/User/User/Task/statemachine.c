#include "head.h"

uint16_t state;

void Statemachine_Task(void *argument)
{
    osDelay(100);
    for (;;) {
    #ifdef RED
        if(Q_NO==1)
        {
            /*past，和reset一套了*/
        }
        else if (Q_NO==2)
        {
            /* 仍然使用位置模式 */
            uint16_t square_size=139;        //待确认各个点是哪个位置
            tar_pos1=2350-square_size;
            tar_pos2=2200+150;
            osDelay(3000);
            tar_pos1=2350+square_size;
            tar_pos2=2200+150;
            osDelay(3000);
            tar_pos1=2350+square_size;
            tar_pos2=2200-28;
            osDelay(3000);
            tar_pos1=2350-square_size;
            tar_pos2=2200-28;
            osDelay(3000);
            tar_pos1=2350-square_size;
            tar_pos2=2200+150;
            osDelay(3000);
        }
        else if (Q_NO>=3)       //实际上3和4一起算了
        {
            
            //static uint8_t state=0;
            /**
            spe1=-3*(dot_cal_x[state]-red_x);
            spe2=5*(dot_cal_y[state]-red_y);
            //if(abs(dot_cal_x[state]-red_x)<10&&abs(dot_cal_y[state]-red_y)<10) 
            //{
                state++;
                if(state%100==0) osDelay(100);
                if(state==400) state=0;
                osDelay(20);
            //}
            **/
/**/
            if(state==0){
                spe1=-6*(dot1x-red_x);       //正负待定，乘系数待定
                spe2=5*(dot1y-red_y);
                if(abs(dot1x-red_x)<10&&abs(dot1y-red_y)<10) 
                {
                   state=1;
                   spe1 = 0;
                   spe2 = 0;
                   osDelay(500);
                }
            }else if (state==1)
            {
                spe1=-6*(dot2x-red_x);       //正负待定，乘系数待定
                spe2=5*(dot2y-red_y);
                if(abs(dot2x-red_x)<10&&abs(dot2y-red_y)<10)
                {
                    state=2;
                    spe1 = 0;
                    spe2 = 0;
                    osDelay(500);
                } 
            }else if (state==2)
            {
                spe1=-6*(dot3x-red_x);       //正负待定，乘系数待定
                spe2=5*(dot3y-red_y);
                if(abs(dot3x-red_x)<10&&abs(dot3y-red_y)<10) 
                {
                    state=3;
                    spe1 = 0;
                    spe2 = 0;

                    osDelay(500);
                }
            }else if (state==3)
            {
                spe1=-6*(dot4x-red_x);       //正负待定，乘系数待定
                spe2=5*(dot4y-red_y);
                if(abs(dot4x-red_x)<10&&abs(dot4y-red_y)<10) 
                {
                    state=0;
                    spe1 = 0;
                    spe2 = 0;
                    osDelay(500);
                }
            }
            
           /**/
        }
        
    #endif
    #ifdef GREEN
    if(Q_NO>=4)
    {
        // spe1=red_x-80;  //不一定是80和60，待测, 正负待定，乘系数待定
        // spe2=red_y-60;
        //spe1和spe2由openmv直接发过来,否则要在绿光部分解码red坐标
    }
    
    #endif // DEBUG 
    osDelay(1);
    }
        
    
}

void Statemachine_Start(void)
{
    osThreadId_t StatemachineHandle;
    const osThreadAttr_t Statemachine_attributes = {
        .name       = "Statemachine",
        .stack_size = 128 *3,
        .priority   = (osPriority_t)osPriorityNormal,
    };
    StatemachineHandle = osThreadNew(Statemachine_Task, NULL, &Statemachine_attributes);
}
