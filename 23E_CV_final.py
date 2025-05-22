# 导入必要的库
import cv2  # OpenCV库，用于图像处理
import numpy as np  # NumPy库，用于数值计算
import time  # 时间库
import struct
import serial
import serial.tools.list_ports
# import serial  # 串口通信库
# 初始化串口
# 参数说明：'COM3'是串口号，115200是波特率，根据实际情况修改
# ser = serial.Serial('COM3', 115200)

# 初始化摄像头
cap = cv2.VideoCapture(4)  
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

# # 调节通道强度
lutEqual = np.array([i for i in range(256)]).astype("uint8")
# lutRaisen = np.array([int(102+0.6*i) for i in range(256)]).astype("uint8")
# # 调节饱和度
# # 一个三通道的查找表，其中蓝色通道和红色通道采用了 lutEqual，而绿色通道采用了 lutRaisen。这样就实现了对图像的饱和度进行调节，同时保持了图像的亮度和色调。
lutSRaisen = np.dstack((lutEqual, lutEqual, lutEqual))  # Saturation raisen

# 定义红色和绿色的HSV颜色范围阈值
# HSV颜色空间中，红色需要两个范围来覆盖
lower_red = np.array([96,0, 205])   # 红色下限 (H,S,V)
upper_red = np.array([179, 255, 255]) # 红色上限

lower_green = np.array([0, 0, 205])   # 更合理的绿色下限
upper_green = np.array([61, 255, 255]) # 更合理的绿色上限

# 串口定义
serial_port = serial.Serial("/dev/ttyAMA0", 115200, timeout=0.5)
serial_port_state = serial_port.is_open


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
                  顺序: [左上, 右上, 右下, 左下] (还要再倒置)
    """
    # 转换为HSV颜色空间
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # 定义黑色的HSV范围
    lower_black = np.array([0, 0, 0])
    upper_black = np.array([179, 255, 100])

    # 创建黑色区域的掩膜
    mask = cv2.inRange(hsv, lower_black, upper_black)

    # 形态学操作
    kernel = np.ones((3, 3), np.uint8)
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

        # 四边形检测条件（4边则接受）
        if len(approx) == 4:
            x, y, w, h = cv2.boundingRect(approx)
            aspect_ratio = max(w, h) / min(w, h)

            if w > min_side and h > min_side and aspect_ratio < max_aspect_ratio:
                max_area = area

                # 获取并排序角点
                corners = approx.reshape(-1, 2)

                # 按x坐标排序
                x_sorted = corners[np.argsort(corners[:, 0])]

                # 分为左边两个点和右边两个点
                left_points = x_sorted[:2, :]
                right_points = x_sorted[2:, :]

                # 在左边点中，y值较小的为左上，较大的为左下
                left_points = left_points[np.argsort(left_points[:, 1])]
                top_left = left_points[0]
                bottom_left = left_points[1]

                # 在右边点中，y值较小的为右上，较大的为右下
                right_points = right_points[np.argsort(right_points[:, 1])]
                top_right = right_points[0]
                bottom_right = right_points[1]

                # 按顺序组合四个点
                max_rect = np.array([top_left, top_right, bottom_right, bottom_left])

    # 绘制检测到的最大矩形
    if max_rect is not None:
        # 绘制矩形边框（红色）
        cv2.drawContours(processed_frame, [max_rect.astype(int)], 0, (0, 0, 255), 2)

        # 绘制角点（蓝色）并标注序号
        for i, (cx, cy) in enumerate(max_rect):
            cv2.circle(processed_frame, (int(cx), int(cy)), 5, (255, 0, 0), 2)
            cv2.putText(
                processed_frame,
                str(i),
                (int(cx) + 10, int(cy) + 10),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.5,
                (255, 255, 0),
                2,
            )

    return processed_frame, max_rect


def stable_rectangle_detection(cap, num_frames=30, min_consistent=20, max_error=8):
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

####################### 主循环 ##############################################################
stable_rect = stable_rectangle_detection(cap)
while cap.isOpened():
    # 读取摄像头帧
    ret, frame = cap.read()
    if not ret:
        break

    # 掩模处理
    hsv = cv2.cvtColor(frame, cv2.COLOR_RGB2HSV)
    mask_red = cv2.inRange(hsv, lower_red, upper_red)
    cv2.imshow("Mask_red",mask_red)
    mask_green = cv2.inRange(hsv, lower_green, upper_green)
    cv2.imshow("Mask_green",mask_green)

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

    contours_red, _ = cv2.findContours(mask_red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    contours_green, _ = cv2.findContours(mask_green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    if contours_red:
        largest_red = max(contours_red, key=cv2.contourArea)
        M_red = cv2.moments(largest_red)
        if M_red["m00"] != 0:
            x_red, y_red = int(M_red["m10"] / M_red["m00"]), int(M_red["m01"] / M_red["m00"])
            

    if contours_green:
        largest_green = max(contours_green,key=cv2.contourArea)
        M_green = cv2.moments(largest_green)
        if M_green["m00"] != 0:
            x_green, y_green = int(M_green["m10"] / M_green["m00"]), int(M_green["m01"] / M_green["m00"])

    # 绘制蓝色圆圈（红色光斑）
    if x_red>0 and y_red>0:
        cv2.circle(display_frame, (int(x_red), int(y_red)), 6, (255, 0, 0), 2)       # 蓝色空心圆
    # 绘制紫色圆圈（绿色光斑）
    if x_green>0 and y_green>0:
        cv2.circle(display_frame, (int(x_green), int(y_green)), 6, (255, 0, 255), 2)  # 紫色空心圆

    # # 显示最终结果
    cv2.imshow("Detection", display_frame)

    pos_data = [
        stable_rect[0],
        stable_rect[1],
        stable_rect[2],
        stable_rect[3],
        stable_rect[4],
        stable_rect[5],
        stable_rect[6],
        stable_rect[7],
        x_red,
        y_red,
        x_green,
        y_green,
    ]
    pack_data = struct.pack(
        ">BB12BBB",
        0xAA,
        0xFF,
        pos_data[0],     # x1
        pos_data[1],     # y1
        pos_data[2],     # x2
        pos_data[3],     # y2
        pos_data[4],     # x3
        pos_data[5],     # y3
        pos_data[6],     # x4
        pos_data[7],     # y4
        pos_data[8],     # x_red
        pos_data[9],     # y_red
        pos_data[10],    # x_green
        pos_data[11],    # y_green
        0xFF,
        0xAA,
    )
    serial_port.write(pack_data)  # 将数据打包发送到串口

    key = cv2.waitKey(1)
    if key == 27:
        break  


# 释放资源
cap.release()
cv2.destroyAllWindows()
