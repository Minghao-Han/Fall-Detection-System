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


@RestController
@CrossOrigin
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
        if ((camera.getCameraName() != null && !camera.getCameraName().isEmpty()) &&
                (camera.getName() != null && !camera.getName().isEmpty())) {
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

    @GetMapping("/startCamera")
    public void startCamera() {
        cameraService.startCamera();
    }

}



