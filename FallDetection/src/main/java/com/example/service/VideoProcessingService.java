package com.example.service;

import org.springframework.stereotype.Service;

import java.io.File;
import java.io.IOException;
import java.util.concurrent.TimeUnit;

@Service
public class VideoProcessingService {

    public void transcodeVideo(File originfile) throws IOException {
        String inputPath, outputPath;
        inputPath = originfile.getPath();
        int pos = inputPath.lastIndexOf('.');
        outputPath = inputPath.substring(0, pos) + ".mp4";

        // 构建 FFmpeg 命令
        String ffmpegCmd = "ffmpeg -f rawvideo -pixel_format nv12 -video_size 640x480 -framerate 30 -i " + inputPath + " -c:v libx264 -r 30 " + outputPath;

        Process process = null;
        try {
            // 执行 FFmpeg 命令
            process = Runtime.getRuntime().exec(ffmpegCmd);

            boolean isCompleted = process.waitFor(10, TimeUnit.SECONDS);
            if(!isCompleted){
                process.destroy();
            }
             // 等待转码完成
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            if (process != null) {
                process.destroy(); // 结束 ffmpeg 进程
            }
        }
    }

}