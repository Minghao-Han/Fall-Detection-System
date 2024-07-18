package com.example.service;

import com.example.entity.VideoFrame;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.scheduling.annotation.Async;
import org.springframework.scheduling.annotation.EnableAsync;
import org.springframework.stereotype.Service;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

@Service
@EnableAsync
public class FrameProcessor {
    @Value("${camera.stream.directory}")
    private String saveDirectory;

    private BlockingQueue<VideoFrame> frameQueue;

    private boolean isInitialized = false;

    public boolean getStatus(){
        return isInitialized;
    }

//    @PostConstruct
    public void init() {
        frameQueue = new LinkedBlockingQueue<>();
        startProcessing();
        isInitialized = true;
    }

    @PreDestroy
    public void cleanUp() {
        // Optionally add cleanup logic here
    }

    // Method to receive video frame data
    public void receiveFrame(byte[] frameData) {
        VideoFrame videoFrame = new VideoFrame();
        videoFrame.setFrameData(frameData);
        frameQueue.offer(videoFrame); // Put frame into queue
    }

    // Async method to process frames from the queue
    @Async
    public void startProcessing() {
        while (true) {
            try {
                VideoFrame videoFrame = frameQueue.take(); // Blocking call to take frame from queue
                convertAndSaveFrame(videoFrame);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }

    // Method to convert NV12 frame to JPEG and save to file
    private void convertAndSaveFrame(VideoFrame videoFrame) {
        byte[] nv12Data = videoFrame.getFrameData();
        // Perform NV12 to JPEG conversion
        // This is a placeholder example, actual conversion logic needs to be implemented

        try {
            BufferedImage img = convertNV12toBufferedImage(nv12Data, 640, 480);
            if (img != null) {
                File outputFile = new File(saveDirectory + "currentFrame.jpg");
                ImageIO.write(img, "jpg", outputFile);
                System.out.println("Converted and saved frame as: " + outputFile.getAbsolutePath());
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Placeholder method for NV12 to BufferedImage conversion
    private BufferedImage convertNV12toBufferedImage(byte[] nv12Data, int width, int height) {
        // Implement NV12 to BufferedImage conversion logic here
        // Example: Using Java's ImageIO to read NV12 data and convert to BufferedImage
        // This depends on how NV12 data is structured and decoded
        // Placeholder implementation
        try {
            ByteBuffer buffer = ByteBuffer.wrap(nv12Data);
            BufferedImage img = new BufferedImage(width, height, BufferedImage.TYPE_3BYTE_BGR);
            return img;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
}

