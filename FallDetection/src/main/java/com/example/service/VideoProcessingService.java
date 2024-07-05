package com.example.service;

import org.springframework.stereotype.Service;

import java.io.IOException;

@Service
public class VideoProcessingService {

    public void transcodeVideo(String inputPath, String outputPath) throws IOException {
        // 示例：使用命令行执行 FFmpeg 转码
        String ffmpegCmd = "ffmpeg -i " + inputPath + " -c:v libx264 -c:a aac " + outputPath;
        Process process = Runtime.getRuntime().exec(ffmpegCmd);
        try {
            process.waitFor();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}