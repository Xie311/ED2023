# 导入必要的库
import cv2  # OpenCV库，用于图像处理
import numpy as np  # NumPy库，用于数值计算
import time  # 时间库
import struct
from filterpy.kalman import KalmanFilter
import matplotlib.pyplot as plt
# import serial  # 串口通信库
# 初始化串口
# 参数说明：'COM3'是串口号，115200是波特率，根据实际情况修改
# ser = serial.Serial('COM3', 115200)  

# 初始化摄像头
cap = cv2.VideoCapture(1)  
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 160)   # 设置图像宽度为320
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 120)  # 设置图像高度为240
cap.set(cv2.CAP_PROP_BRIGHTNESS, 1.0)    # 设置亮度，范围0-1
cap.set(cv2.CAP_PROP_AUTO_WB, 0)         # 关闭自动白平衡
# 关闭自动曝光（必须关闭才能手动设置）
cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0.25)  # 0.25 或 0 表示手动曝光
# 调小曝光值（减少进光量）
cap.set(cv2.CAP_PROP_EXPOSURE, -6)  # 典型范围：-8（最暗）到 -1（较亮），或 0.1~0.001（秒）

# 定义感兴趣区域(ROI)
# 格式：(x起始坐标, y起始坐标, 宽度, 高度)
ROI = (0, 0, 160, 120)

# 调节通道强度
lutEqual = np.array([i for i in range(256)]).astype("uint8")
lutRaisen = np.array([int(102+0.6*i) for i in range(256)]).astype("uint8")
# 调节饱和度
# 一个三通道的查找表，其中蓝色通道和红色通道采用了 lutEqual，而绿色通道采用了 lutRaisen。这样就实现了对图像的饱和度进行调节，同时保持了图像的亮度和色调。
lutSRaisen = np.dstack((lutEqual, lutRaisen, lutEqual))  # Saturation raisen

# 定义红色和绿色的HSV颜色范围阈值
# HSV颜色空间中，红色需要两个范围来覆盖
lower_red = np.array([80, 0, 182])   # 红色下限 (H,S,V)
upper_red = np.array([179, 255, 255]) # 红色上限

lower_green = np.array([0, 0, 182])   # 更合理的绿色下限
upper_green = np.array([60, 255, 255]) # 更合理的绿色上限

def detect_black_rectangle(frame, min_area=1000, min_side=20, max_aspect_ratio=5):
    """
    检测图像中的黑色矩形边框（只处理面积最大的一个）
    
    参数:
        frame: 输入图像(BGR格式)
        min_area: 最小矩形面积阈值
        min_side: 最小边长阈值(像素)
        max_aspect_ratio: 最大宽高比
    
    返回:
        processed_frame: 处理后的图像(带标记)
        rectangle: 检测到的最大矩形信息(4个有序角点坐标), 未检测到返回None
    """
    # 转换为HSV颜色空间
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    hsv = cv2.LUT(hsv, lutSRaisen)             # 饱和度增大
    
    # 定义黑色的HSV范围
    lower_black = np.array([0, 0, 0])
    upper_black = np.array([180, 255, 50])
    
    # 创建黑色区域的掩膜
    mask = cv2.inRange(hsv, lower_black, upper_black)
    
    # 形态学操作
    kernel = np.ones((3,3), np.uint8)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel, iterations=1)
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel, iterations=2)
    
    # 查找轮廓
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    processed_frame = frame.copy()
    max_rect = None
    max_area = 0
    
    for contour in contours:
        area = cv2.contourArea(contour)
        
        # 只保留面积最大的轮廓
        if area < min_area or area <= max_area:
            continue
            
        # 多边形逼近
        epsilon = 0.02 * cv2.arcLength(contour, True)
        approx = cv2.approxPolyDP(contour, epsilon, True)
        
        # 放宽四边形检测条件（4-5个边都接受）
        if 4 <= len(approx) <= 5:
            x, y, w, h = cv2.boundingRect(approx)
            aspect_ratio = max(w, h) / min(w, h)
            
            if w > min_side and h > min_side and aspect_ratio < max_aspect_ratio:
                max_area = area
                
                # 获取并排序角点
                corners = approx.reshape(-1, 2)
                center = corners.mean(axis=0)
                diff = corners - center
                angles = np.arctan2(diff[:,1], diff[:,0])
                sorted_idx = np.argsort(angles)
                max_rect = corners[sorted_idx]
    
    # 绘制检测到的最大矩形
    if max_rect is not None:
        # 绘制矩形边框（红色）
        cv2.drawContours(processed_frame, [max_rect.astype(int)], 0, (0, 0, 255), 2)
        
        # 绘制角点（蓝色）
        for (cx, cy) in max_rect:
            cv2.circle(processed_frame, (int(cx), int(cy)), 5, (255, 0, 0), 2)
    
    return processed_frame, max_rect

def red_blob(frame):
    """检测全图中红色光斑（单HSV范围）"""
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    hsv = cv2.LUT(hsv, lutSRaisen)  # 保持饱和度增强
    
    # 单红色范围掩膜（根据实际调整阈值）
    mask = cv2.inRange(hsv, lower_red, upper_red)
    
    # 检测最大轮廓
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        largest = max(contours, key=cv2.contourArea)
        M = cv2.moments(largest)
        if M['m00']!=0:
            cx, cy = int(M['m10']/M['m00']), int(M['m01']/M['m00'])
        return cx, cy
    return 0, 0  # 未检测到

def green_blob(frame):
    """检测全图中绿色光斑（单HSV范围）"""
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    hsv = cv2.LUT(hsv, lutSRaisen)  # 保持饱和度增强
    
    # 绿色掩膜
    mask = cv2.inRange(hsv, lower_green, upper_green)
    
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        largest = max(contours, key=cv2.contourArea)
        M = cv2.moments(largest)
        if M['m00']!=0:
            cx, cy = int(M['m10']/M['m00']), int(M['m01']/M['m00'])
        return cx, cy
    return 0, 0


# 初始化卡尔曼滤波器#######################################################################################
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
    y_observed.append(y_obs)

    # 预测
    kf.predict()
    # 更新（传入观测值）
    kf.update(np.array([x_obs, y_obs]))
    # 记录滤波后的状态
    x_filtered.append(kf.x[0])
    y_filtered.append(kf.x[1])
    return kf.x[0],kf.x[1]

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

init_kalman_filter()
while cap.isOpened():
    # 读取摄像头帧
    ret, frame = cap.read()
    if not ret:
        break
    
    # 创建一个用于绘制的图像副本
    display_frame = frame.copy()
    
    # 检测黑色矩形
    processed_frame, max_rect = detect_black_rectangle(frame)
    # 将检测到的矩形绘制到display_frame上
    if max_rect is not None:
        # 绘制矩形轮廓
        cv2.drawContours(display_frame, [max_rect.astype(int)], -1, (0, 0, 255), 2)
         # 绘制矩形的四个角点
        for point in max_rect:
            cx, cy = point
            cv2.circle(display_frame, (int(cx), int(cy)), 5, (255, 0, 0), 2)

    # 检测红色和绿色光斑
    #x_red, y_red = red_blob(frame) 
    x_green, y_green = green_blob(frame)  
    
    Kalman_filter_cv2(x_green,y_green)
    
    # print(f"Red blob: ({x_red}, {y_red})")
    # print(f"Green blob: ({x_green}, {y_green})")

    # 绘制蓝色圆圈（红色光斑）
    # if x_red is not None and y_red is not None:
    #     cv2.circle(display_frame, (int(x_red), int(y_red)), 6, (255, 0, 0), 2)       # 蓝色空心圆
    # 绘制紫色圆圈（绿色光斑）
    if x_green is not None and y_green is not None:
        cv2.circle(display_frame, (int(x_green), int(y_green)), 6, (255, 0, 255), 2)  # 黄色空心圆

    # 显示最终结果
    cv2.imshow("Detection", display_frame)
    
    key = cv2.waitKey(1)
    if key == 32:
        Figure()
        break  
    if key == 27:
        break  
    

# 释放资源
cap.release()
cv2.destroyAllWindows()