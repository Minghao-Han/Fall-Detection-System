package com.example.controller;
import org.springframework.core.io.InputStreamResource;
import org.springframework.http.*;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;

import java.io.*;

@RestController
@RequestMapping("/video")
public class VideoController {

    // 用于接收上传的视频流，并进行处理
    @PostMapping("/upload")
    public ResponseEntity<String> uploadVideo(@RequestParam("file") MultipartFile file) {
        // 这里可以保存上传的视频到服务器或者其他处理
        try {
            // 示例中直接返回成功信息
            return ResponseEntity.ok("Video uploaded successfully: " + file.getOriginalFilename());
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body("Failed to upload video");
        }
    }

    // 用于推送视频流到客户端，实现直播功能
    @GetMapping(value = "/stream/{videoName}", produces = MediaType.APPLICATION_OCTET_STREAM_VALUE)
    public ResponseEntity<InputStreamResource> streamVideo(@PathVariable String videoName) throws IOException {
        // 这里可以从文件系统或者数据库中读取视频流并返回
        File videoFile = new File("/path/to/your/video/" + videoName);
        InputStreamResource videoStream = new InputStreamResource(new FileInputStream(videoFile));

        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_OCTET_STREAM);
        headers.setContentLength(videoFile.length());
        headers.setContentDispositionFormData("attachment", videoName);

        return new ResponseEntity<>(videoStream, headers, HttpStatus.OK);
    }
}
