package com.example.controller;
import com.example.common.Message;
import com.example.common.Result;
import com.example.service.FallVideoService;
import com.example.service.VideoProcessingService;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.core.io.Resource;
import org.springframework.core.io.UrlResource;
import org.springframework.http.*;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;

import java.io.*;

@RestController
@CrossOrigin
@RequestMapping("/video")
public class VideoController {

    @javax.annotation.Resource
    private FallVideoService fallVideoService;
    @javax.annotation.Resource
    private VideoProcessingService videoProcessingService;

    @Value("${video.upload.directory}")
    private String uploadDir;
    // 用于接收上传的视频流，并进行处理
    @PostMapping("/upload")
    public ResponseEntity<String> uploadVideo(@RequestParam("file") MultipartFile file) {
        try {
            // 保存上传的视频文件到服务器
            if (!file.isEmpty()) {
                byte[] bytes = file.getBytes();
                File uploadFile = new File(uploadDir + file.getOriginalFilename());
                BufferedOutputStream stream = new BufferedOutputStream(new FileOutputStream(uploadFile));
                stream.write(bytes);
                stream.close();
                String videoName = uploadFile.getName();
                int idx = videoName.lastIndexOf('.');
                videoName = videoName.substring(0,idx);
                fallVideoService.addData("G7kL0mN2pZ8rT4uV",videoName);//里面应该有相关参数，只包括序列号似乎即可
                //短信发送接口的调用
//                Message.sendMessage((int)(Math.random()*1000000)+"", "13657408690");
                videoProcessingService.transcodeVideo(uploadFile);
                return new ResponseEntity<>("Video uploaded successfully", HttpStatus.OK);
            } else {
                return new ResponseEntity<>("Video upload failed: File is empty", HttpStatus.BAD_REQUEST);
            }
        } catch (IOException e) {
            return new ResponseEntity<>("Video upload failed: " + e.getMessage(), HttpStatus.INTERNAL_SERVER_ERROR);
        }
    }

    @GetMapping("/fallClips/{fileName:.+}")
    public ResponseEntity<Resource> getVideo(@PathVariable String fileName) throws IOException {
        // 返回存储在服务器上的视频文件，前端可以通过URL访问这个接口来获取视频流
        File file = new File(uploadDir + fileName);
        Resource resource = new UrlResource(file.toURI());
        if (resource.exists() || resource.isReadable()) {
            return ResponseEntity.ok()
                    .contentType(MediaType.parseMediaType("video/mp4"))
                    .body(resource);
        } else {
            return ResponseEntity.notFound().build();
        }
    }

    @PatchMapping("/{serialNumber}")
    public Result choose(@PathVariable String serialNumber){
        //这里需要填写切换播放器的相关代码，如果有播放源，就播放视频，没有播放源就放一张无播放源的图片。参数serialNumber代表序列号，每台摄影机各不相同
        return Result.success();
    }

}
