# 导入必要的库
import cv2  # OpenCV库，用于图像处理
import numpy as np  # NumPy库，用于数值计算
import time  # 时间库
import struct
from filterpy.kalman import KalmanFilter
import matplotlib.pyplot as plt
from collections import deque
# import serial  # 串口通信库
# 初始化串口
# 参数说明：'COM3'是串口号，115200是波特率，根据实际情况修改
# ser = serial.Serial('COM3', 115200)

# 初始化摄像头
cap = cv2.VideoCapture(1)  
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 160)   # 设置图像宽度为160
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 120)  # 设置图像高度为120
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
lower_red = np.array([104, 107, 124])   # 红色下限 (H,S,V)
upper_red = np.array([179, 255, 255]) # 红色上限

lower_green = np.array([0, 79, 203])   # 更合理的绿色下限
upper_green = np.array([63, 255, 255]) # 更合理的绿色上限

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
    # 线性查找表（保持蓝色通道不变）
    lutEqual = np.array([i for i in range(256)]).astype("uint8")

    # 增强红色通道的查找表（可调整参数）
    lutRaisen_red = np.array([int(50 + 0.8 * i) for i in range(256)]).astype("uint8")  # 增强红色

    # 增强绿色通道的查找表（可调整参数）
    lutRaisen_green = np.array([int(50 + 0.8 * i) for i in range(256)]).astype("uint8")  # 增强绿色

    # 组合成三通道 LUT（R:增强, G:增强, B:不变）
    lutSRG = np.dstack((lutRaisen_red, lutRaisen_green, lutEqual))  # 增强红、绿，蓝不变
    hsv = cv2.LUT(hsv, lutSRG)  # 红绿饱和度增大

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

        # 放宽四边形检测条件（4边则接受）
        if len(approx) == 4:
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


def stable_rectangle_detection(cap, num_frames=20, min_consistent=15, max_error=5):
    """
    稳定检测黑色矩形（多次检测取平均值）

    参数:
        cap: cv2.VideoCapture 对象
        num_frames: 总检测次数（默认20次）
        min_consistent: 最小稳定次数（默认15次）
        max_error: 允许的最大坐标误差（默认5像素）

    返回:
        avg_corners: 稳定的平均角点坐标（4x2数组），未检测到返回None
    """
    # 存储检测到的矩形角点
    detected_rects = []

    for _ in range(num_frames):
        ret, frame = cap.read()
        if not ret:
            continue

        _, rect = detect_black_rectangle(frame)

        if rect is not None:
            detected_rects.append(rect)

    # 如果检测次数不足，返回None
    if len(detected_rects) < min_consistent:
        return None

    # 计算角点的平均值
    avg_corners = np.mean(detected_rects, axis=0)

    # 检查稳定性（各次检测与平均值的误差是否 < max_error）
    stable_count = 0
    for rect in detected_rects:
        error = np.mean(np.abs(rect - avg_corners))
        if error < max_error:
            stable_count += 1

    # 如果稳定次数足够，返回平均坐标
    if stable_count >= min_consistent:
        return avg_corners.astype(int)
    else:
        return None

####################### 处理红、绿光点 ##############################################################
# 历史坐标缓存（存储最近5次有效坐标）
red_history = deque(maxlen=5)
green_history = deque(maxlen=5)


def smooth_coordinate(new_val, history, max_jump=10):
    """
    平滑坐标，滤除突变点并估算噪声点坐标
    参数:
        new_val: 新检测到的坐标 (x, y)
        history: 历史坐标队列（deque）
        max_jump: 允许的最大跳跃距离（像素）
    返回:
        平滑后的坐标（优先用新坐标，噪声点则用估算值）
    """
    if not history:  # 第一次检测，直接接受
        history.append(new_val)
        return new_val

    # 计算历史移动趋势（加权平均，近期点权重更高）
    weights = np.linspace(0.5, 1.5, len(history))  # 权重递增
    trend = np.average(history, axis=0, weights=weights)

    # 计算新坐标与趋势的距离
    distance = np.linalg.norm(new_val - trend)

    if distance <= max_jump:
        # 正常点：更新历史记录并返回新坐标
        history.append(new_val)
        return new_val
    else:
        # 噪声点：基于历史趋势估算当前坐标
        if len(history) >= 2:
            # 计算移动向量（最近两点的差值）
            dx = history[-1][0] - history[-2][0]
            dy = history[-1][1] - history[-2][1]
            estimated = (history[-1][0] + dx, history[-1][1] + dy)
        else:
            estimated = trend  # 不足两点时直接用趋势值

        # 将估算值加入历史（但不完全信任）
        history.append(estimated)
        return estimated

################ 检测红色光点 ################################################################
def red_blob(frame):
    """检测红色光斑（改进版：先检测完整圆，若无则检测环形结构）"""
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    hsv = cv2.LUT(hsv, lutSRaisen)
    cx, cy = -1, -1  # 默认值

    # 红色区域掩膜
    mask = cv2.inRange(hsv, lower_red, upper_red)

    # 策略1：检测完整圆形（霍夫圆变换）
    circles = cv2.HoughCircles(
        mask,
        cv2.HOUGH_GRADIENT,
        dp=1,
        minDist=50,
        param1=100,
        param2=30,
        minRadius=1,
        maxRadius=300,
    )

    if circles is not None:
        circles = np.uint16(np.around(circles))
        largest = max(circles[0, :], key=lambda x: x[2])
        return largest[0], largest[1]

    # 策略2：检测环形结构
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if not contours:
        return -1, -1

    # 找到最大轮廓
    largest = max(contours, key=cv2.contourArea)
    contour_area = cv2.contourArea(largest)

    # 计算凸包面积
    hull = cv2.convexHull(largest)
    hull_area = cv2.contourArea(hull)

    # 环形判定条件（仅用凸包面积比）
    if hull_area > 0 and contour_area / hull_area < 0.85:  # 阈值可调
        # 方法：检测空心部分的圆心
        # 1. 创建仅包含最大轮廓的掩膜
        contour_mask = np.zeros_like(mask)
        cv2.drawContours(contour_mask, [largest], -1, 255, -1)

        # 2. 获取空心部分（轮廓内非红色区域）
        hollow_area = cv2.bitwise_and(cv2.bitwise_not(mask), contour_mask)

        # 3. 检测空心部分的圆（霍夫变换）
        circles = cv2.HoughCircles(
            hollow_area,
            cv2.HOUGH_GRADIENT,
            dp=1,
            minDist=50,
            param1=50,
            param2=20,  # 较低阈值提高检测率
            minRadius=5,
            maxRadius=int(np.sqrt(contour_area / np.pi) * 0.8),  # 限制最大半径
        )

        if circles is not None:
            circles = np.uint16(np.around(circles))
            # 返回最大的内接圆圆心
            largest_inner = max(circles[0, :], key=lambda x: x[2])
            return largest_inner[0], largest_inner[1]

        # 如果霍夫变换失败，返回凸包的中心（备选方案）
        M = cv2.moments(hull)
        if M["m00"] != 0:
            return int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"])

    # 如果不是环形，返回轮廓的质心
    M = cv2.moments(largest)
    if M["m00"] != 0:
        return int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"])

    return -1, -1


def green_blob(frame):
    """检测绿色光斑（改进版：先检测完整圆，若无则检测环形结构）"""
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    hsv = cv2.LUT(hsv, lutSRaisen)
    cx, cy = -1, -1  # 默认值

    # 红色区域掩膜
    mask = cv2.inRange(hsv, lower_green, upper_green)

    # 策略1：检测完整圆形（霍夫圆变换）
    circles = cv2.HoughCircles(
        mask,
        cv2.HOUGH_GRADIENT,
        dp=1,
        minDist=50,
        param1=100,
        param2=30,
        minRadius=1,
        maxRadius=300,
    )

    if circles is not None:
        circles = np.uint16(np.around(circles))
        largest = max(circles[0, :], key=lambda x: x[2])
        return largest[0], largest[1]

    # 策略2：检测环形结构
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if not contours:
        return -1, -1

    # 找到最大轮廓
    largest = max(contours, key=cv2.contourArea)
    contour_area = cv2.contourArea(largest)

    # 计算凸包面积
    hull = cv2.convexHull(largest)
    hull_area = cv2.contourArea(hull)

    # 环形判定条件（仅用凸包面积比）
    if hull_area > 0 and contour_area / hull_area < 0.85:  # 阈值可调
        # 方法：检测空心部分的圆心
        # 1. 创建仅包含最大轮廓的掩膜
        contour_mask = np.zeros_like(mask)
        cv2.drawContours(contour_mask, [largest], -1, 255, -1)

        # 2. 获取空心部分（轮廓内非红色区域）
        hollow_area = cv2.bitwise_and(cv2.bitwise_not(mask), contour_mask)

        # 3. 检测空心部分的圆（霍夫变换）
        circles = cv2.HoughCircles(
            hollow_area,
            cv2.HOUGH_GRADIENT,
            dp=1,
            minDist=50,
            param1=50,
            param2=20,  # 较低阈值提高检测率
            minRadius=5,
            maxRadius=int(np.sqrt(contour_area / np.pi) * 0.8),  # 限制最大半径
        )

        if circles is not None:
            circles = np.uint16(np.around(circles))
            # 返回最大的内接圆圆心
            largest_inner = max(circles[0, :], key=lambda x: x[2])
            return largest_inner[0], largest_inner[1]

        # 如果霍夫变换失败，返回凸包的中心（备选方案）
        M = cv2.moments(hull)
        if M["m00"] != 0:
            return int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"])

    # 如果不是环形，返回轮廓的质心
    M = cv2.moments(largest)
    if M["m00"] != 0:
        return int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"])

    return -1, -1


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
stable_rect = stable_rectangle_detection(cap)
while cap.isOpened():
    # 读取摄像头帧
    ret, frame = cap.read()
    if not ret:
        break

    # 创建一个用于绘制的图像副本
    display_frame = frame.copy()

    # 如果已经检测到稳定矩形，直接绘制
    if stable_rect is not None:
        # 绘制矩形轮廓
        cv2.drawContours(display_frame, [stable_rect.astype(int)], -1, (0, 0, 255), 2)
        # 绘制矩形的四个角点
        for point in stable_rect:
            cx, cy = point
            cv2.circle(display_frame, (int(cx), int(cy)), 5, (255, 0, 0), 2)

    # 检测红色和绿色光斑
    x_red, y_red = red_blob(frame) 
    x_green, y_green = green_blob(frame)

    # Kalman_filter_cv2(x_green,y_green)
    # print(f"Red blob: ({x_red}, {y_red})")
    # print(f"Green blob: ({x_green}, {y_green})")

    # 绘制蓝色圆圈（红色光斑）
    if x_red>0 and y_red>0:
        cv2.circle(display_frame, (int(x_red), int(y_red)), 6, (255, 0, 0), 2)       # 蓝色空心圆
    # 绘制紫色圆圈（绿色光斑）
    if x_green>0 and y_green>0:
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
