/*
 * @Author: X311
 * @Date: 2024-05-13 09:00:14
 * @LastEditors: X311 
 * @LastEditTime: 2024-05-23 20:14:02
 * @FilePath: \Gantry\UserCode\Upper\Upper_Servo\UpperServo.h
 * @Brief: 
 * 
 * Copyright (c) 2024 by X311, All Rights Reserved. 
 */
#ifndef __UPPERSERVO_H__
#define __UPPERSERVO_H__

#include "UpperStart.h"
void Upper_Servo_TaskStart(void);
void Upper_Motor_init();
void VelocityPlanning(float initialAngle, float maxAngularVelocity, float AngularAcceleration, float targetAngle, float currentTime, volatile float *currentAngle);
extern float KP;

#endif // __UPPERSERVO_H__