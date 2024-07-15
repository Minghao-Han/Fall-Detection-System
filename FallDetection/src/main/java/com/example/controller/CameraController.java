package com.example.controller;

import com.example.exception.CustomException;
import com.example.service.CameraService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;
import com.example.common.Result;
import com.example.entity.Camera;
import com.example.entity.Params;
import com.github.pagehelper.PageInfo;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.io.Console;
import java.util.List;

@CrossOrigin
@RestController
@RequestMapping("/camera")
public class CameraController {
    @Resource
    private CameraService cameraService;

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
/*            Params params=new Params();
            params.setSerialNumber(camera.getSerialNumber());
            params.setCameraName(camera.getCameraName());
            params.setName(camera.getName());
            params.setPhone(camera.getPhone());*/
            int rows = cameraService.bind(camera);
            if (rows !=1){
                return Result.error("该序列号已经被绑定");
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

    @GetMapping("/startCamera")
    public void startCamera() {
        cameraService.startCamera();
    }

}



