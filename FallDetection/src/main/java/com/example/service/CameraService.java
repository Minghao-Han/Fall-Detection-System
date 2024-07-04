package com.example.service;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.videoio.VideoCapture;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Service;

@Service
public class CameraService {

    private final SimpMessagingTemplate messagingTemplate;

    public CameraService(SimpMessagingTemplate messagingTemplate) {
        this.messagingTemplate = messagingTemplate;
    }

    public void startCamera() {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME); // 加载OpenCV库

        VideoCapture camera = new VideoCapture(0); // 0代表默认摄像头

        if (!camera.isOpened()) {
            System.out.println("Error: Camera not opened.");
            return;
        }

        while (true) {
            Mat frame = new Mat();
            if (camera.read(frame)) {
                // 发送图像数据到WebSocket客户端
                messagingTemplate.convertAndSend("/topic/camera", frame);
            }
        }
    }

}