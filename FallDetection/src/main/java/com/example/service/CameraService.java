package com.example.service;

import com.example.dao.CameraDao;
import com.example.entity.Camera;
import com.example.entity.Params;
import com.github.pagehelper.PageHelper;
import com.github.pagehelper.PageInfo;
import org.springframework.stereotype.Service;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.videoio.VideoCapture;
import org.springframework.messaging.simp.SimpMessagingTemplate;

import javax.annotation.Resource;
import java.io.Console;
import java.util.List;

@Service
public class CameraService {

    @Resource
    private CameraDao cameraDao;

    public List<Camera> getCamera() {
        // 3. 使用引入的包
        return cameraDao.selectAll();
    }


    public PageInfo<Camera> findBySearch(Params params) {
        //开启分页查询
        PageHelper.startPage(params.getPageNum(), params.getPageSize());
        //接下来的查询会自动分页
        List<Camera> list= cameraDao.findBySearch(params);
        return PageInfo.of(list);
    }

    public void update(Camera camera) {
        cameraDao.updateByPrimaryKeySelective(camera);
    }



    public void unbindPhone(Integer id) {
        cameraDao.unbindPhone(id);
    }

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


