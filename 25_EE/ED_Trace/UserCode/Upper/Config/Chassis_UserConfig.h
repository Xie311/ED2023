/*
 * @Author: Chen Yitong
 * @Date: 2023-09-23 12:53:53
 * @LastEditors: x311 
 * @LastEditTime: 2024-04-14 22:47:49
 * @FilePath: \WTR_Omni_Chassis\UserCode\Chassis\Config\Chassis_UserConfig.h
 * @Brief: 自定义宏
 *
 * Copyright (c) 2023 by ChenYiTong, All Rights Reserved.
 */

#ifndef __CHASSIS_USERCONFIG_H__
#define __CHASSIS_USERCONFIG_H__

/**
 * @brief 一些有用的常量，基本上是从 GCC 的 math.h 中复制下来的(因为 ARMCC 的库里貌似没有)
 *
 */

/* Natural log of 2 */
#define _M_LN2   0.693147180559945309417

#define MAXFLOAT 3.40282347e+38F

#define M_E      2.7182818284590452354
#define M_LOG2E  1.4426950408889634074
#define M_LOG10E 0.43429448190325182765
#define M_LN2    _M_LN2
#define M_LN10   2.30258509299404568402
#define M_PI     3.14159265358979323846
#define DEC      (M_PI / 180)

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif // !M_PI_2

#define M_PI_4        0.78539816339744830962
#define M_1_PI        0.31830988618379067154
#define M_2_PI        0.63661977236758134308
#define M_2_SQRTPI    1.12837916709551257390
#define M_SQRT2       1.41421356237309504880
#define M_SQRT1_2     0.70710678118654752440

#define M_TWOPI       (M_PI * 2.0)
#define M_3PI_4       2.3561944901923448370E0
#define M_SQRTPI      1.77245385090551602792981
#define M_LN2LO       1.9082149292705877000E-10
#define M_LN2HI       6.9314718036912381649E-1
#define M_SQRT3       1.73205080756887719000
#define M_IVLN10      0.43429448190325182765 /* 1 / log(10) */
#define M_LOG2_E      _M_LN2
#define M_INVLN2      1.4426950408889633870E0 /* 1 / log(2) */

// 限幅
#define max(a, b)      (a > b ? a : b)
#define min(a, b)      (a < b ? a : b)
#define range(x, a, b) (min(max(x, a), b))

/**
 * @brief 自定义端口 *
 */

#define huart_RemoteCtl huart1
#define hcan_Dji        hcan1

#endif // __CHASSIS_USERCONFIG_H__