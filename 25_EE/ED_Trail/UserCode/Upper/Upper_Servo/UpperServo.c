/*
 * @Author: X311
 * @Date: 2024-05-13 09:00:14
 * @FilePath: \ED_Trail\UserCode\Upper\Upper_Servo\UpperServo.c
 * @Brief:
 *
 * Copyright (c) 2024 by X311, All Rights Reserved.
 */
#include "UpperServo.h"
// 底盘电机结构体
Chassis_MOTOR_COMPONENT Chassis_MotorComponent;
float KP = 8;  //30
/********线程相关部分*************/
/**
 * @brief 伺服函数
 * @note 
*/
void Upper_Servo_Task(void *argument)
{
    osDelay(100);
    for (;;) {
        xSemaphoreTakeRecursive(Chassis_Control.xMutex_control, portMAX_DELAY);
        // 复制底盘控制数据（目标位置 来自上位机 [StateMachine.c]）到临时变量
        Chassis_MOVING_STATE Chassis_Control_tmp = Chassis_Control;
        xSemaphoreGiveRecursive(Chassis_Control.xMutex_control);

        // 创建一个数组存储三个电机的速度
        double motor_velocity[4] = {0};

        CalculateTwoWheels(motor_velocity,
                           Chassis_Control_tmp.velocity.x,
                           Chassis_Control_tmp.velocity.w);


        // 创建一个数组存储四个电机的状态
        DJI_t hDJI_tmp[4];

        vPortEnterCritical();
        for (int i = 0; i < 4; i++) { memcpy(&(hDJI_tmp[i]), Chassis_MotorComponent.hDJI[i], sizeof(DJI_t)); }
        vPortExitCritical();

        // 遍历四个电机，根据计算得到的速度调整电机状态
        // for (int i = 0; i < 4; i++) { speedServo(motor_velocity[i], &(hDJI_tmp[i])); }
        for (int i = 0; i < 4; i++) { speedServo(2, &(hDJI_tmp[i])); }

        //将调整后的电机状态通过CAN总线发送
        // CanTransmit_DJI_1234(&hcan_Dji,
        //                      hDJI_tmp[0].speedPID.output,
        //                      hDJI_tmp[1].speedPID.output,
        //                      hDJI_tmp[2].speedPID.output,
        //                      hDJI_tmp[3].speedPID.output);

        //print("%f\n",hDJI_tmp[0].speedPID.output); 


        vPortEnterCritical();
        // 将调整后的电机状态复制回原始数组中
        for (int i = 0; i < 4; i++) { memcpy(Chassis_MotorComponent.hDJI[i], &(hDJI_tmp[i]), sizeof(DJI_t)); }
        vPortExitCritical();

        osDelay(4);
    }
}

void Upper_Servo_TaskStart(void)
{
    const osThreadAttr_t Upper_Servo_attributes = {
        .name       = "Upper_Servo",
        .stack_size = 128 * 10,
        .priority   = (osPriority_t)osPriorityNormal,
    };
     osThreadNew(Upper_Servo_Task, NULL, &Upper_Servo_attributes);
}

/*******封装函数部分********/
void Upper_Motor_init() // 电机初始化
{
    CANFilterInit(&hcan1);
    // 初始化电机
    Chassis_MotorComponent.hDJI[0] = &hDJI[0];
    Chassis_MotorComponent.hDJI[1] = &hDJI[1];

    hDJI[0].motorType  = M3508;
    hDJI[1].motorType  = M3508;
    
    DJI_Init();

    hDJI[0].speedPID.KP        = 4.0; // 5
    hDJI[0].speedPID.KI        = 0.4; // 0.2
    hDJI[0].speedPID.KD        = 0.8; // 0.8
    hDJI[0].speedPID.outputMax = 6000;

    hDJI[1].speedPID.KP        = 4.0; // 5
    hDJI[1].speedPID.KI        = 0.4; // 0.2
    hDJI[1].speedPID.KD        = 0.8; // 0.8
    hDJI[1].speedPID.outputMax = 6000;

}

/**
 * @brief T型速度规划函数
 * @param initialAngle 初始角度
 * @param maxAngularVelocity 最大角速度
 * @param AngularAcceleration 角加速度
 * @param targetAngle 目标角度
 * @param currentTime 当前时间
 * @param currentAngle 当前角度
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