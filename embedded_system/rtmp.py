import sys
import signal
import os
import time

from hobot_vio import libsrcampy
import subprocess
width = 640
height = 480
# 设置ffmpeg管道命令
# 设置ffmpeg管道命令
is_continue=True

def signal_handler(signal, frame):
    sys.exit(0) 
    global is_stop
    print("Stopping!\n")
    is_continue=False

command = [
    'ffmpeg',
    '-y',  # 覆盖输出文件
    '-f', 'rawvideo',
    '-vcodec', 'rawvideo',
    '-s', f'{width}x{height}',  # 输入视频尺寸
    '-pix_fmt', 'nv12',  # 像素格式
    '-r', str(30),  # 帧率
    '-i', '-',  # 从管道读取
    '-an',  # 无音频
    '-c:v', 'libx264',  # 编码器
    '-pix_fmt', 'yuv420p',  # 像素格式
    '-f', 'flv',  # 输出格式
    'rtmp://192.168.43.133:1935/live/streamkey'  # RTMP 服务器 URL
]

# 打开ffmpeg进程
process = subprocess.Popen(command, stdin=subprocess.PIPE, stderr=subprocess.PIPE)

def test_camera():
    cam = libsrcampy.Camera()

    #open MIPI camera, fps: 30, solution: 1080p
    ret = cam.open_cam(0, -1, 30, width, height)
    print("Camera open_cam return:%d" % ret)

    # wait for 1s
    time.sleep(1)
    while is_continue:
        #get one image from camera   
        img = cam.get_img(2)
        if img is not None:
            #save file
            process.stdin.write(img)
            #print("camera output success")
        else:
            print("camera output failed")
    
    #close MIPI camera
    cam.close_cam()
    print("test_camera done!!!")


if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)
    test_camera()
