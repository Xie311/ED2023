
#ifndef __PARAMDEF_H__
#define __PARAMDEF_H__

#include "FreeRTOS.h"
#include "semphr.h"
#include "UpperStart.h"
#define BELT_LENGTH_PER_ROUND 95.5044167 // 输出轴转一圈同步带走的长度，单位:mm

typedef __IO struct
{
    __IO struct {
        double x;
        double y;
        double z;
    } velocity; // 速度结构体//不知道需不需要来着

    __IO struct {
        double x;  // slash
        double y;  // straight line
        double z;
    } position;
    SemaphoreHandle_t xMutex_control; // 互斥锁
} UpperState;

typedef __IO struct {
    DJI_t *Motor_X;
    DJI_t *Motor_Y;
    _Bool cylinder;
    _Bool electromagnet;
    UpperState gantry_t;
} Upper_COMPONENT;

extern Upper_COMPONENT Core_xy[2]; // 全局的Upper状态参数
// extern UpperState TargetState[2];              //目标状态

// extern float initial_angle[3];
// extern float current_angle[3];

#endif // __PARAMDEF_H__