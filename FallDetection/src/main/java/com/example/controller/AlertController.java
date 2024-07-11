package com.example.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/alerts")
public class AlertController {
    private final SimpMessagingTemplate messagingTemplate;

    @Autowired
    public AlertController(SimpMessagingTemplate messagingTemplate) {
        this.messagingTemplate = messagingTemplate;
    }

    @PostMapping("/receive")
    public ResponseEntity<String> receiveAlert(@RequestBody String alertMessage) {
        // 处理接收到的警告消息
        // 这里可以根据需要处理消息内容，例如记录日志、处理业务逻辑等

        // 将警告消息推送到前端
        messagingTemplate.convertAndSend("/topic/alerts", alertMessage);

        return ResponseEntity.ok("Alert received and forwarded to frontend.");
    }
}
