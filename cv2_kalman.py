import numpy as np
import cv2
from filterpy.kalman import KalmanFilter
import matplotlib.pyplot as plt


# 初始化卡尔曼滤波器
def init_kalman_filter():
    kf = KalmanFilter(dim_x=4, dim_z=2)  # 状态量(x, y, vx, vy)，观测量(x, y)

    # 状态转移矩阵 (假设匀速运动模型)
    kf.F = np.array([[1, 0, 1, 0],
                     [0, 1, 0, 1],
                     [0, 0, 1, 0],
                     [0, 0, 0, 1]])

    # 观测矩阵 (只能观测位置)
    kf.H = np.array([[1, 0, 0, 0],
                     [0, 1, 0, 0]])

    # 过程噪声协方差 (调整参数以控制滤波强度)
    kf.Q = np.eye(4) * 0.01

    # 观测噪声协方差 (根据实际噪声调整)
    kf.R = np.array([[10, 0],
                     [0, 10]])

    # 初始状态和协方差
    kf.x = np.zeros(4)  # [x, y, vx, vy]
    kf.P = np.eye(4) * 100

    return kf

x_filtered, y_filtered = [], []
x_observed,y_observed = [], []

# 主程序
def Kalman_filter_cv2(x_obs,y_obs):
    kf = init_kalman_filter()
    x_observed.append(x_obs)
    y_observed.append(Y_obs)

    # 预测
    kf.predict()
    # 更新（传入观测值）
    kf.update(np.array([x_obs, y_obs]))
    # 记录滤波后的状态
    x_filtered.append(kf.x[0])
    y_filtered.append(kf.x[1])

def Figure()  :
    plt.figure(figsize=(12, 6))
    plt.plot(x_observed,y_observed, 'r.', label='Noisy Observations')
    plt.plot(x_filtered, y_filtered, 'b-', linewidth=2, label='Kalman Filtered')
    plt.legend()
    plt.title("Kalman Filter for Trajectory Smoothing")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.grid()
    plt.show()


