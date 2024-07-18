package com.example.controller;

import com.example.exception.CustomException;
import com.example.service.CameraService;
import com.example.service.FrameProcessor;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.io.ClassPathResource;
import org.springframework.core.io.Resource;
import org.springframework.core.io.ResourceLoader;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.beans.factory.annotation.Value;
import com.example.common.Result;
import com.example.entity.Camera;
import com.example.entity.Params;
import com.github.pagehelper.PageInfo;
import org.springframework.web.bind.annotation.*;

import java.awt.*;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;


@RestController
@CrossOrigin
@RequestMapping("/camera")
public class CameraController {
    @javax.annotation.Resource
    private CameraService cameraService;

    @Autowired
    private FrameProcessor frameProcessor;

    @Value("${camera.stream.directory}")
    private String framePath;

    @GetMapping
    public Result getCamera() {
        List<Camera> list=cameraService.getCamera();
        return Result.success(list);
    }

    @GetMapping("/search")
    public Result findBySearch(Params params){
        PageInfo<Camera> info=cameraService.findBySearch(params);
        return Result.success(info);
    }

    //编辑摄像头信息
    @PostMapping
    public Result save(@RequestBody Camera camera) {
        if (camera.getCameraName()!=null && camera.getName()!=null ) {
            cameraService.update(camera);
        }else{
           throw new CustomException("存在空字段");
        }
        return Result.success();
    }

    @PostMapping("/bind")
    public Result bind(@RequestBody Camera camera){
        if (camera.getSerialNumber() != null && camera.getCameraName()!=null && camera.getName()!=null ) {
            int rows = cameraService.bind(camera);
            int affectedRows = cameraService.getAffectedRows(camera);
            if (rows == 0 && affectedRows == 1){
                return Result.error("该序列号已经被绑定");
            }else if (rows ==0 && affectedRows == 0){
                return Result.error("该序列码不合法");
            }
        }else{
            throw new CustomException("存在空字段");
        }
        return Result.success();
    }

    @PatchMapping("/{serialNumber}")
    public Result unbindPhone(@PathVariable String serialNumber){
        cameraService.unbindPhone(serialNumber);
        return Result.success();
    }

    @Autowired
    public CameraController(CameraService cameraService) {
        this.cameraService = cameraService;
    }

    @PostMapping("/receiveFrame")
    public void receiveFrame(@RequestBody byte[] frameData) {
        if(frameProcessor.getStatus() != true){
            frameProcessor.init();
        }
        frameProcessor.receiveFrame(frameData);
    }


    @GetMapping(value = "/stream", produces = MediaType.IMAGE_JPEG_VALUE)
    public ResponseEntity<byte[]> pushStream() throws IOException {
        Path imagePath = Path.of(framePath,"currentFrame.jpg");
        byte[] imageBytes = Files.readAllBytes(imagePath);
        return ResponseEntity.ok().body(imageBytes);
    }

}



