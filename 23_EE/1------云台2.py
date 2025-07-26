#而第二三问需要识别黑线上的激光，曝光60000
#接收1，运行发挥部分第一问
#接收2，运行发挥部分第二三问
#接收3，发挥部分使用第二套绿色阈值

import sensor, image, time, lcd,pyb
from pyb import Pin, Timer, LED, UART

sensor.reset()
sensor.set_auto_gain(False)
sensor.set_pixformat(sensor.RGB565) # or sensor.RGB565
sensor.set_framesize(sensor.QVGA) # or sensor.QVGA (or others)
sensor.skip_frames(time=900) # Let new settings take affect.
sensor.set_auto_whitebal(False) # turn this off.
sensor.set_auto_gain(False) # 关闭增益（色块识别时必须要关）
lcd.init()
LED(1).on()
LED(2).on()
LED(3).on()
uart = UART(3, 115200)

#ROI = (95,65,140,140)
#ROI = (60,30,200,170)
ROI = (0,0,320,240)
#threshold_red = [(0, 100, 127, 8, -15, 114)]       #红色激光   二三问曝光30000
threshold_red = [(15, 75, 18, 90, 0, 60)]
#threshold_green2 = [(85, 100, -35, -10, -5, 127)]  #绿色激光   二三问曝光30000
#threshold_green2 = [(40, 100, -128, -30, -5, 127)]    #曝光30000
threshold_green2 = [(53, 100, -128, -12, -5, 127)]     #变焦镜头
threshold_green = [(100, 29, -26, -42, -4, 127)]

threshold_red1 = [(0, 100, 127, 1, 2, 114)]        #第一问曝光3000
#threshold_green1 = [(35, 100, -56, -28, -5, 127)]  #第一问曝光3000
#threshold_green1 = [(35, 100, -56, -80, -5, 127)]    #曝光30000
threshold_green1 = [(35, 99, -108, 17, -5, 127)]    #变焦镜头

threshold_red3 = [(0, 100, 127, 20, -15, 114)]       #发挥部分曝光10000
threshold_green3 = [(40, 100, -128, -30, -5, 127)]  #发挥部分曝光10000

threshold_black=[(0, 42, -105, 6, -70, 40)]

def red_blob(img, threshold_red,ROI):

    #识别红色激光光斑
    blobs = img.find_blobs(threshold_red, roi=ROI, x_stride=1, y_stride=1, area_threshold=0, pixels_threshold=0,merge=True,margin=10)
    if len(blobs)>=1 :#有色块
        largest_blob = max(blobs, key=lambda b: b.pixels())
        # Draw a rect around the blob.
        img.draw_circle(largest_blob[5], largest_blob[6], 8, color=(255,0,0))
        cx = largest_blob[5]
        cy = largest_blob[6]
        cx=int(cx/2)
        cy=int(cy)
        return int(cx), int(cy)
    return 0, 0 #表示没有找到

def green_blob(img, threshold_green, ROI):

    #识别绿色激光光斑
    blobs = img.find_blobs(threshold_green, roi=ROI, x_stride=1, y_stride=1, area_threshold=0, pixels_threshold=1,merge=True,margin=1)
    if len(blobs)>=1 :#有色块
        largest_blob = max(blobs, key=lambda b: b.pixels())
        # Draw a rect around the blob.
        img.draw_circle(largest_blob[5], largest_blob[6], 8, color=(0,255,0))
        cx = largest_blob[5]
        cy = largest_blob[6]
        cx=int(cx/2)
        cy=int(cy)
        return int(cx), int(cy)
    return 0, 0 #表示没有找到


problem_num  = '0'
while(True):


    #识别激光
    x5 = y5 = x6 = y6 = 0
    if uart.any():
        problem_num = uart.readline()
    #第一问，曝光3000，------------------------------------------
    if problem_num == b'\x01':
        sensor.set_auto_exposure(False, 3000)
        img = sensor.snapshot()         # Take a picture and return the image.
        img.lens_corr(2)
        img.draw_cross((60,30), color=(255,255,255))
        img.draw_cross((260,30), color=(255,255,255))
        img.draw_cross((60,200), color=(255,255,255))
        img.draw_cross((260,200), color=(255,255,255))
        [x5,y5] = red_blob(img,threshold_red1,ROI)
        [x6,y6] = green_blob(img,threshold_green1,ROI)
        #print(x5, y5, x6, y6)
        data = bytearray([0x2c, 0x12, x5, y5, x6, y6, 0x5B])
        uart.write(data)
        #print(data)
    #第二问，曝光30000，------------------------------------------
    if problem_num == b'\x02':
        img = sensor.snapshot()         # Take a picture and return the image.
        sensor.set_auto_exposure(False,20000)
        img.lens_corr(1)
        img.draw_cross((60,30), color=(255,255,255))
        img.draw_cross((260,30), color=(255,255,255))
        img.draw_cross((60,200), color=(255,255,255))
        img.draw_cross((260,200), color=(255,255,255))
        [x5,y5] = red_blob(img,threshold_red,ROI)
        [x6,y6] = green_blob(img,threshold_green2,ROI)
        #print(x5, y5, x6, y6)
        data = bytearray([0x2c, 0x12, x5, y5, x6, y6, 0x5B])
        uart.write(data)
        #print(data)
    #发挥部分，追踪红色激光点，曝光10000，------------------------------------------
    if problem_num == b'\x03':
        img = sensor.snapshot()         # Take a picture and return the image.
        sensor.set_auto_exposure(False,10000)
        img.lens_corr(2)
        [x5,y5] = red_blob(img,threshold_red3,ROI)
        [x6,y6] = green_blob(img,threshold_green3,ROI)
        #print(x5, y5, x6, y6)
        data = bytearray([0x2c, 0x12, x5, y5, x6, y6, 0x5B])
        uart.write(data)
        #print(data)
    #绿色激光点识别不好时的备用方案，第二套绿色阈值------------------------------------
    if problem_num == b'\x04':
        img = sensor.snapshot()         # Take a picture and return the image.
        sensor.set_auto_exposure(False,60000)
        img.lens_corr(2)
        img.draw_cross((60,30), color=(255,255,255))
        img.draw_cross((260,30), color=(255,255,255))
        img.draw_cross((60,200), color=(255,255,255))
        img.draw_cross((260,200), color=(255,255,255))
        [x5,y5] = red_blob(img,threshold_red,ROI)
        [x6,y6] = green_blob(img,threshold_green,ROI)
        #print(x5, y5, x6, y6)
        data = bytearray([0x2c, 0x12, x5, y5, x6, y6, 0x5B])
        uart.write(data)
        print(data)
    #--------------------------------------------------------------------------
    if problem_num == '0':
        continue





