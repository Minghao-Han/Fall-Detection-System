package com.example.service;

import org.springframework.stereotype.Service;

import java.io.File;
import java.io.IOException;

@Service
public class VideoProcessingService {

    public void transcodeVideo(File originfile) throws IOException {
        String inputPath, outputPath;
        inputPath = originfile.getPath();
        int pos = inputPath.lastIndexOf('.');
        outputPath = inputPath.substring(0, pos) + ".mp4";
        // 示例：使用命令行执行 FFmpeg 转码
        String ffmpegCmd = "ffmpeg -f rawvideo -pixel_format nv12 -video_size 640x480 -framerate 30 -i " + inputPath + " -c:v libx264 -r 30 " + outputPath;
        Process process = Runtime.getRuntime().exec(ffmpegCmd);
        try {
            process.waitFor();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}