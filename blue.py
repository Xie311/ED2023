from machine import Pin
import sensor, image, time,math,pyb
from pyb import UART
blue_threshold  = (0, 99, -36, 38, -97, -12)
def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob=blob
            max_size = blob[2]*blob[3]
    return max_blob

# 初始化串口
uart=UART(3,115200)

# 初始化摄像头
sensor.reset()
sensor.set_pixformat(sensor.RGB565) # 设置图像色彩格式为RGB565格式
sensor.set_framesize(sensor.QQVGA)  # 设置图像大小为160*120
sensor.set_auto_whitebal(False)      # 设置自动白平衡
sensor.set_brightness(3000)         # 设置亮度为3000
sensor.skip_frames(time = 20)       # 跳过帧
sensor.set_auto_gain(False)  # must be turned off for color tracking

#roi=(0,0,320,120)
#sensor.set_roi(roi)
clock = time.clock()
corner = 0
Nub=None             # 接收题目序号
packet=None          # 接收题目序号
position=None        # 发送点的坐标


while(True):
    clock.tick()

    # 接收当前题目序号
    if uart.any()>=5:
        packet=uart.read(5)   # 读取5个字节

        # 检验包头(0xEE 0xEE) 和包尾 (0xFF 0xFF)
        if packet[0]==0xEE and packet[1]==0xEE and packet[3]==0xFF and packet[4]==0xFF:
            Nub=packet[2]
            print("Receive Nub:",Nub)  # 打印接收到的题目序号

    img = sensor.snapshot()
    # 第二问，追踪黑框------------------------------------------
    # 在图像中寻找矩形
    #img = img.copy(roi=roi)  # 提取 ROI 部分
    blob_x=0
    blob_y=0

    blobs_1 = img.find_blobs([blue_threshold])
    if blobs_1:
        max_blob = find_max(blobs_1)
        img.draw_rectangle(max_blob.rect()) # rect
        img.draw_cross(max_blob.cx(), max_blob.cy()) # cx, cy
        blob_x=max_blob.cx()
        blob_y=max_blob.cy()
        print(1,max_blob.cx(),max_blob.cy())

    position=bytearray([0xAA,0xFF,(blob_x-80),(blob_y-60),0xFF,0xAA])

    uart.write(position)

    # 显示到屏幕上，此部分会降低帧率
    #lcd.show_image(img, 160, 120, 0, 0, zoom=0)  #屏幕显示

    # 打印帧率
    #print(clock.fps())

