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
sensor.set_framesize(sensor.QQVGA)       # 设置图像大小为320*240
sensor.set_auto_exposure(False)         # 关闭自动曝光
sensor.set_auto_whitebal(False)         # 关闭自动白平衡
sensor.set_auto_gain(False)             # 关闭增益（色块识别时必须要关）
sensor.set_brightness(3000)             # 设置亮度为3000
sensor.skip_frames(time = 20)           # 跳过帧

clock = time.clock()
corner = 0
Nub=None             # 接收题目序号
packet=None          # 接收题目序号
position=None        # 发送点的坐标

ROI = (0,0,160,120)
threshold_red = [(0, 100, 127, 20, -15, 114)]
threshold_green = [(76, 100, -26, 12, -17, 20)]

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
    blobs = img.find_blobs(threshold_green, roi=ROI, x_stride=2, y_stride=2, area_threshold=0, pixels_threshold=0,merge=True,margin=1)
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
    #------------发展题 绿光追踪红光-----------------------------------------------------------
    x, y = red_blob(img, threshold_red, ROI)           # 红光坐标
    x1, y1 = green_blob(img, threshold_green, ROI)     # 绿光坐标
    #print(x,y)

    # 构造数据包，包头为（0xAA 0xFF）+顶点坐标(x,y)+包尾（0xFF 0xAA）
#   led_position = bytearray([
#                0xAA, 0xFF,
#                x, y,                          # 红光坐标
#                0xFF, 0xAA
#            ])
    # uart.write(position)

    #------------发展题 绿光跟随红光-----------------------------------------------------------

    # 显示到屏幕上，此部分会降低帧率
    # lcd.show_image(img, 160, 120, 0, 0, zoom=0)  #屏幕显示

    # 打印帧率
    #print(clock.fps())

