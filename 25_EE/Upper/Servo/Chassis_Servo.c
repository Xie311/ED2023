/*
 * @Author: X311
 * @Date: 2025-07-27 00:10:26
 * @LastEditors: X311 
 * @LastEditTime: 2025-07-27 21:45:17
 * @FilePath: \ED_Trail\UserCode\Upper\Servo\Chassis_Servo.c
 * @Brief: 
 * 
 * Copyright (c) 2025 by X311, All Rights Reserved. 
 */
/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 13:49:36
 * @LastEditors: x311 
 * @LastEditTi 10-09 15:57:22
 * @FilePath: \Chassis_01\UserCode\Chassis\Servo\Chassis_Servo.c
 * @Brief: 底盘伺服函数
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#include "Chassis_Servo.h"
WHEEL_COMPONENT WheelComponent;

/**
 * @brief: 伺服初始化
 * @return {*}
 */
void Chassis_Servo_Init()
{
    Chassis_Servo_DjiMotorInit();
}

/**
 * @brief: 伺服线程
 * @return {*}
 */
void Chassis_Servo_Task(void const *argument)
{
    // 延时1秒，等待系统稳定（？）
    osDelay(1000);
    for (;;) {
        xSemaphoreTakeRecursive(ChassisControl.xMutex_control, portMAX_DELAY);
        // 复制底盘控制数据（目标位置 来自上位机 [StateMachine.c]）到临时变量
        CHASSIS_MOVING_STATE ChassisControl_tmp = ChassisControl;
        xSemaphoreGiveRecursive(ChassisControl.xMutex_control);

      
        double motor_velocity[4] = {0};

        CalculateTwoWheels(motor_velocity,
                           ChassisControl_tmp.velocity.x,
                           ChassisControl_tmp.velocity.w);

        // 创建一个数组存储四个电机的状态
        DJI_t hDJI_tmp[4];

        vPortEnterCritical();
        for (int i = 0; i < 4; i++) { memcpy(&(hDJI_tmp[i]), WheelComponent.hDJI[i], sizeof(DJI_t)); }
        vPortExitCritical();

        // 遍历四个电机，根据计算得到的速度调整电机状态
        for (int i = 0; i < 4; i++) { speedServo(motor_velocity[i], &(hDJI_tmp[i])); }

        // 将调整后的电机状态通过CAN总线发送
        CanTransmit_DJI_1234(&hcan_Dji,
                             hDJI_tmp[0].speedPID.output,
                             hDJI_tmp[1].speedPID.output,
                             hDJI_tmp[2].speedPID.output,
                             hDJI_tmp[3].speedPID.output);

        vPortEnterCritical();
        // 将调整后的电机状态复制回原始数组中
        for (int i = 0; i < 4; i++) { memcpy(WheelComponent.hDJI[i], &(hDJI_tmp[i]), sizeof(DJI_t)); }
        vPortExitCritical();

        osDelay(8);
    }
}

/**
 * @brief: 启动底盘伺服线程
 * @return {*}
 */
void Chassis_Servo_TaskStart()
{
    osThreadAttr_t Chassis_Servo_attributes = {
        .name       = "Chassis_Servo",
        .priority   = osPriorityHigh,
        .stack_size = 128 * 10,
    };
    osThreadNew(Chassis_Servo_Task, NULL, &Chassis_Servo_attributes);
}


/**
 * @brief 大疆电机初始化
 * @return {*}
 * @note   若选用其他电机型号，则需在这里修改motorType
 */
void Chassis_Servo_DjiMotorInit()
{
    CANFilterInit(&hcan1);
    WheelComponent.hDJI[0] = &hDJI[0];
    WheelComponent.hDJI[1] = &hDJI[1];
    WheelComponent.hDJI[2] = &hDJI[2];
    WheelComponent.hDJI[3] = &hDJI[3];
    hDJI[0].motorType      = M2006;
    hDJI[1].motorType      = M2006;
    hDJI[2].motorType      = M2006;
    hDJI[3].motorType      = M2006;
    DJI_Init();
    for (uint16_t i = 0; i < 4; i++)
    {
        hDJI[i].speedPID.KP        = 2;//5
        hDJI[i].speedPID.KI        = 0.8;//0.2
        hDJI[i].speedPID.KD        = 1.0;//0.8
        hDJI[i].speedPID.outputMax = 5000;

    }
}

/**
 * @brief T型速度规划函数
 * @param initialAngle 初始角度
 * @param maxAngularVelocity 最大角速度
 * @param AngularAcceleration 角加速度
 * @param targetAngle 目标角度
 * @param currentTime 当前时间
 * @param currentTime 当前角度
 * @todo 转换为国际单位制
 */
void VelocityPlanning(float initialAngle, float maxAngularVelocity, float AngularAcceleration, float targetAngle, float currentTime, volatile float *currentAngle)
{

    float angleDifference = targetAngle - initialAngle;     // 计算到目标位置的角度差
    float sign            = (angleDifference > 0) ? 1 : -1; // 判断角度差的正负(方向)

    float accelerationTime = maxAngularVelocity / AngularAcceleration;                                                      // 加速(减速)总时间
    float constTime        = (fabs(angleDifference) - AngularAcceleration * pow(accelerationTime, 2)) / maxAngularVelocity; // 匀速总时间
    float totalTime        = constTime + accelerationTime * 2;                                                              // 计算到达目标位置所需的总时间

    // 判断能否达到最大速度
    if (constTime > 0) {
        // 根据当前时间判断处于哪个阶段
        if (currentTime <= accelerationTime) {
            // 加速阶段
            *currentAngle = initialAngle + sign * 0.5 * AngularAcceleration * pow(currentTime, 2);
        } else if (currentTime <= accelerationTime + constTime) {
            // 匀速阶段
            *currentAngle = initialAngle + sign * maxAngularVelocity * (currentTime - accelerationTime) + 0.5 * sign * AngularAcceleration * pow(accelerationTime, 2);
        } else if (currentTime <= totalTime) {
            // 减速阶段
            float decelerationTime = currentTime - accelerationTime - constTime;
            *currentAngle          = initialAngle + sign * maxAngularVelocity * constTime + 0.5 * sign * AngularAcceleration * pow(accelerationTime, 2) + sign * (maxAngularVelocity * decelerationTime - 0.5 * AngularAcceleration * pow(decelerationTime, 2));
        } else {
            // 达到目标位置
            *currentAngle = targetAngle;
        }
    } else {
        maxAngularVelocity = sqrt(fabs(angleDifference) * AngularAcceleration);
        accelerationTime   = maxAngularVelocity / AngularAcceleration;
        totalTime          = 2 * accelerationTime;
        constTime          = 0;
        // 根据当前时间判断处于哪个阶段
        if (currentTime <= accelerationTime) {
            // 加速阶段
            *currentAngle = initialAngle + sign * 0.5 * AngularAcceleration * pow(currentTime, 2);
        } else if (currentTime <= totalTime) {
            // 减速阶段
            float decelerationTime = currentTime - accelerationTime; // 减速时间
            *currentAngle          = initialAngle + sign * 0.5 * AngularAcceleration * pow(accelerationTime, 2) + sign * (maxAngularVelocity * decelerationTime - 0.5 * AngularAcceleration * pow(decelerationTime, 2));
        } else {
            // 达到目标位置
            *currentAngle = targetAngle;
        }
    }
}