import cv2

# 检查可用摄像头数量
num_cameras = 0
for i in range(10):
    cap = cv2.VideoCapture(i)
    if cap.isOpened():
        print(f"找到摄像头 #{i}")
        num_cameras += 1
        cap.release()

if num_cameras == 0:
    print("未检测到任何摄像头！")
else:
    print(f"共找到 {num_cameras} 个摄像头")