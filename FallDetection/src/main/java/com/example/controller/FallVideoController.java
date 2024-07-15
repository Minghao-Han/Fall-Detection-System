package com.example.controller;

import com.example.common.Result;
import com.example.entity.Camera;
import com.example.entity.FallVideo;
import com.example.entity.Params;
import com.example.service.FallVideoService;
import com.github.pagehelper.PageInfo;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.Resource;

@CrossOrigin
@RestController
@RequestMapping("/fallVideo")
public class FallVideoController {
    @Resource
    private FallVideoService fallVideoService;

    @GetMapping("/search")
    public Result findBySearch(Params params){
        PageInfo<FallVideo> info=fallVideoService.findBySearch(params);
        return Result.success(info);
    }
}
