package com.example.controller;

import com.example.entity.User;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/user")
public class UserController {
    /**
     * controller里的一个方法，它其实就是我们平常说的web项目的一个接口的入口
     * 可以在这个方法上再加上一个url
     * 也可以指定请求方式：GET POST PUT DELETE
     * @return
     */
    @GetMapping("/start")
    public String start() {
        return "终于成功启动了，0点52我还能说什么";
    }

    @GetMapping()
    public User getUser(){
        return new User("宋剑");
    }
}