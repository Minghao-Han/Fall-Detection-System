package com.example.controller;

import com.example.common.Result;
import com.example.entity.Admin;
import com.example.service.AdminService;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.Resource;
import java.util.List;

@RestController
@RequestMapping("/user")
public class AdminController {
    @Resource
    private AdminService adminService;
    /**
     * controller里的一个方法，它其实就是我们平常说的web项目的一个接口的入口
     * 可以在这个方法上再加上一个url
     * 也可以指定请求方式：GET POST PUT DELETE
     * @return
     */
    @GetMapping("/start")
    public Result Start(){
        return Result.success("能成功吗");
    }
    @GetMapping("/getUser")
    public Result getUser(){
       List<Admin> list=adminService.getUser();
       return  Result.success(list);
    }
}