/*
 * @Author: szf
 * @Date: 2023-02-22 12:04:21
 * @LastEditTime: 2025-07-28 23:40:42
 * @LastEditors: X311 
 * @brief 运动学逆解算及PID计算函数
 * @FilePath: \ED_Trail\UserCode\Lib\Calculate\wtr_calculate.c
 */

#include "wtr_calculate.h"
#include <math.h>
#define M_PI 3.14159265358979323846
//#include "Chassis_UserConfig.h"

double moter_speed[4];

/*********************两轮差速底盘********************/
// 两轮差速底盘参数
#define Width_2         0.135                             // 底盘宽度（0.27~0.28）
#define wheel_radius    0.0325                            // 轮半径，单位为米（m）
#define wheel_rpm_ratio (1 / (M_PI * wheel_radius)) // 换算线速度到转子rpm（不是输出rpm，所以要记得乘电机减速比）。得出的线速度的单位取决于轮半径的单位，这里为m/s。

void CalculateTwoWheels(double *moter_speed, double vx, double vw)
{
    moter_speed[0] = (vx + vw * Width_2) * wheel_rpm_ratio;
    moter_speed[1] = -(vx - vw * Width_2) * wheel_rpm_ratio; // 大疆电机speed servo给正值是逆时针转，都给正值时会一个轮往前转一个轮往后转，所以要有一个轮子给计算值的负值
}


/**
 * @brief: PID控制-增量式PID
 * @auther: Chen YiTong 3083697520
 * @param {__IO PID_t} *pid
 * @return {*}
 */

void PID_Calc(__IO PID_t *pid)
{
    pid->cur_error = pid->ref - pid->fdb;
    pid->output += pid->KP * (pid->cur_error - pid->error[1]) + pid->KI * pid->cur_error + pid->KD * (pid->cur_error - 2 * pid->error[1] + pid->error[0]);
    pid->error[0] = pid->error[1];
    pid->error[1] = pid->ref - pid->fdb;
    /*设定输出上限*/
    if (pid->output > pid->outputMax) pid->output = pid->outputMax;
    if (pid->output < -pid->outputMax) pid->output = -pid->outputMax;
}


/**
 * @brief: PID控制-位置式PID
 * @auther: zyt
 * @param {__IO PID_t} *pid
 * @return {*}
 */

void PID_Calc_P(__IO PID_t *pid)
{
    pid->cur_error = pid->ref - pid->fdb;
    pid->integral += pid->cur_error;
    pid->output = pid->KP * pid->cur_error + pid->KI * pid->integral + pid->KD * (pid->cur_error - pid->error[1]);
    pid->error[0] = pid->error[1];  //这句已经没有用了
    pid->error[1] = pid->ref - pid->fdb;
    /*设定输出上限*/
    if (pid->output > pid->outputMax) pid->output = pid->outputMax;
    if (pid->output < -pid->outputMax) pid->output = -pid->outputMax;
}


/**
 * @brief: PID算法-P控制
 * @auther: Chen YiTong 3083697520
 * @param {__IO PID_t} *pid
 * @return {*}
 */
void P_Calc(__IO PID_t *pid)
{
    pid->cur_error = pid->ref - pid->fdb;
    pid->output    = pid->KP * pid->cur_error;
    /*设定输出上限*/
    if (pid->output > pid->outputMax) pid->output = pid->outputMax;
    if (pid->output < -pid->outputMax) pid->output = -pid->outputMax;

    if (fabs(pid->output) < pid->outputMin)
        pid->output = 0;
}

/**
 * @brief: 位置伺服
 * @auther: Chen YiTong 3083697520
 * @param {float} ref 目标位置
 * @param {DJI_t} *motor 电机结构体
 * @return {*}
 */
void positionServo(float ref, DJI_t *motor)
{

    motor->posPID.ref = ref;
    motor->posPID.fdb = motor->AxisData.AxisAngle_inDegree;
    P_Calc(&motor->posPID);
    
    motor->speedPID.ref = motor->posPID.output;
    motor->speedPID.fdb = motor->FdbData.rpm;
    PID_Calc_P(&(motor->speedPID));
}

/**
 * @brief: 速度伺服函数
 * @auther: Chen YiTong 3083697520
 * @param {float} ref 目标值
 * @param {DJI_t} *motor 电机结构体
 * @return {*}
 */
void speedServo(float ref, DJI_t *motor)
{
    motor->speedPID.ref = ref;
    motor->speedPID.fdb = motor->FdbData.rpm;
    PID_Calc(&(motor->speedPID));
}

/**
 * @brief: 位置伺服,使用雷达反馈
 * @param {float} ref 目标位置(距测距雷达的目标距离)
 * @param {DJI_t} *motor 电机结构体
 * @param {LidarData} lidardata 电机结构体
 * @return {*}
 */
void positionServo_lidar(float ref, DJI_t *motor, float distance_aver)
{

    motor->posPID.ref = ref;
    motor->posPID.fdb = distance_aver;
    P_Calc(&motor->posPID);

    motor->speedPID.ref = motor->posPID.output;
    motor->speedPID.fdb = motor->FdbData.rpm;
    PID_Calc_P(&(motor->speedPID));
}

/**
 * @brief: 圆周死区控制
 * @auther: szf
 * @param {double} x
 * @param {double} y
 * @param {double} *new_x
 * @param {double} *new_y
 * @param {double} threshould
 * @return {*}
 */
void DeadBand(double x, double y, double *new_x, double *new_y, double threshould)
{
    double length     = sqrt(x * x + y * y);
    double new_length = length - threshould;

    if (new_length <= 0) {
        *new_x = 0;
        *new_y = 0;
        return;
    }

    double k = new_length / length;

    *new_x = x * k;
    *new_y = y * k;
}

/**
 * @brief: 单轴死区控制
 * @auther: szf
 * @param {double} x
 * @param {double} *new_x
 * @param {double} threshould
 * @return {*}
 */
// void DeadBandOneDimensional(double x, double *new_x, double threshould)
// {

//     double difference_x = fabs(x) - threshould;

//     if (difference_x < 0) {
//         *new_x = 0;
//         return;
//     }

//     double k = difference_x / fabs(x);
//     *new_x   = k * x;
// }

void DeadBandOneDimensional(double x, double *new_x, double threshould)
{

    double difference_x = fabs(x) - threshould;

    if (difference_x < 0) {
        *new_x = 0;
        return;
    }

    return;
}
