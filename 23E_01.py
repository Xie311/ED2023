#接收1 运行(3)(4)，识别黑框
#接收2 发挥部分，识别红色光斑
from machine import Pin
import sensor, image, time,math,pyb
from pyb import UART

# 初始化串口
uart=UART(3,115200)

# 初始化摄像头
sensor.reset()
sensor.set_pixformat(sensor.RGB565)     # 设置图像色彩格式为RGB565格式
sensor.set_framesize(sensor.VGA)       # 设置图像大小为320*240
sensor.set_auto_exposure(False)         # 关闭自动曝光
sensor.set_auto_whitebal(False)         # 关闭自动白平衡
sensor.set_auto_gain(False)             # 关闭增益（色块识别时必须要关）
sensor.set_brightness(1000)             # 设置亮度为3000
sensor.skip_frames(time = 20)           # 跳过帧

clock = time.clock()
corner = 0
Nub=None             # 接收题目序号
packet=None          # 接收题目序号
position=None        # 发送点的坐标

ROI = (0,0,160,120)
threshold_red = [(0, 100, 14, 52, -16, 1)]    # 怎么这么亮呃
threshold_green = [(100, 29, -26, -42, -4, 127)]

def red_blob(img, threshold_red,ROI):

    #识别红色激光光斑
    blobs = img.find_blobs(threshold_red, roi=ROI, x_stride=2, y_stride=2, area_threshold=0, pixels_threshold=0,merge=True,margin=10)
    if len(blobs)>=0.1 :#有色块
        # 选择像素最多的色块
        largest_blob = max(blobs, key=lambda b: b.pixels())
        # 获取中心坐标
        cx = largest_blob.cx()
        cy = largest_blob.cy()
        # 在图像上画圆标记光斑
        img.draw_circle(cx, cy, 3, color=(255, 0, 0))

        return cx, cy
    return 0, 0 #表示没有找到

def green_blob(img, threshold_green, ROI):

    #识别绿色激光光斑
    blobs = img.find_blobs(threshold_green, roi=ROI, x_stride=2, y_stride=2, area_threshold=0, pixels_threshold=0,merge=True,margin=10)
    if len(blobs)>=0.1 :#有色块
        # 选择像素最多的色块
        largest_blob = max(blobs, key=lambda b: b.pixels())
        # 获取中心坐标
        cx = largest_blob.cx()
        cy = largest_blob.cy()
        # 在图像上画圆标记光斑
        img.draw_circle(cx, cy, 3, color=(0, 0, 255))

        return cx, cy
    return 0, 0 #表示没有找到


while(True):
    clock.tick()

#    # 接收当前题目序号
#    if uart.any()>=5:
#        packet=uart.read(5)   # 读取5个字节

#        # 检验包头(0xEE 0xEE) 和包尾 (0xFF 0xFF)
#        if packet[0]==0xEE and packet[1]==0xEE and packet[3]==0xFF and packet[4]==0xFF:
#            Nub=packet[2]
#            print("Receive Nub:",Nub)  # 打印接收到的题目序号

    img = sensor.snapshot()
    #------------第三问，追踪黑框-----------------------------------------------------------
    # 在图像中寻找矩形
    rectangles = img.find_rects(threshold=10000)

    # 如果有找到矩形
    if rectangles:
        for r in img.find_rects(threshold = 10000):
            # 判断矩形边长是否符合要求
            if r.w() > 20 and r.h() > 20:
                # 在屏幕上框出矩形
                img.draw_rectangle(r.rect(), color = (255, 0, 0), scale = 4)
                # 获取矩形角点位置
                corner = r.corners()
                # 在屏幕上圈出矩形角点
                img.draw_circle(corner[0][0], corner[0][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
                img.draw_circle(corner[1][0], corner[1][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
                img.draw_circle(corner[2][0], corner[2][1], 5, color = (0, 0, 255), thickness = 2, fill = False)
                img.draw_circle(corner[3][0], corner[3][1], 5, color = (0, 0, 255), thickness = 2, fill = False)

            # 打印四个角点坐标, 角点1的数组是corner[0], 坐标就是(corner[0][0],corner[0][1])
            # 角点检测输出的角点排序每次不一定一致，矩形左上的角点有可能是corner0,1,2,3其中一个
            #------------第三问，识别红色光斑-----------------------------------------------------------
            x, y = red_blob(img, threshold_red, ROI)           # 红光坐标
            x1, y1 = green_blob(img, threshold_green, ROI)     # 绿光坐标
            #print(x,y)

            # 构造数据包，包头为（0xAA 0xFF）+顶点坐标(x,y)+包尾（0xFF 0xAA）
#            position = bytearray([
#                0xAA, 0xFF,
#                corner[0][0], corner[0][1],  # 角点1
#                corner[1][0], corner[1][1],  # 角点2
#                corner[2][0], corner[2][1],  # 角点3
#                corner[3][0], corner[3][1],  # 角点4
#                x, y,                          # 红光坐标
#                0xFF, 0xAA
#            ])
            # uart.write(position)

    #------------发展题 绿光跟随红光-----------------------------------------------------------


    # 显示到屏幕上，此部分会降低帧率
    # lcd.show_image(img, 160, 120, 0, 0, zoom=0)  #屏幕显示

    # 打印帧率
    #print(clock.fps())

