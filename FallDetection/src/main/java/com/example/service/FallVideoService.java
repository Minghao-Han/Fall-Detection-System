package com.example.service;

import com.example.dao.FallVideoDao;
import com.example.entity.FallVideo;
import com.example.entity.Params;
import com.github.pagehelper.PageHelper;
import com.github.pagehelper.PageInfo;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.List;
import java.io.File;
import java.io.IOException;
import java.util.concurrent.TimeUnit;

@Service
public class FallVideoService {

    @Resource
    private FallVideoDao fallVideoDao;

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

    public PageInfo<FallVideo> findBySearch(Params params) {
        //开启分页查询
        PageHelper.startPage(params.getPageNum(), params.getPageSize());
        //接下来的查询会自动分页
        List<FallVideo> list = fallVideoDao.findBySearch(params);
        return PageInfo.of(list);
    }

    public void addData(String serialNumber,String fileName) {
        FallVideo fallVideo =new FallVideo();
        fallVideo.setSerialNumber(serialNumber);
        //插入记录并获取自增的ID
        fallVideo.setVideoName(fileName);
        fallVideoDao.insertSelective(fallVideo);
        // 根据 serialNumber 从 camera 表中查找 phone
        int videoId=fallVideoDao.getVideoId(serialNumber);
        String phone = fallVideoDao.getPhoneBySerialNumber(serialNumber);
        fallVideo.setPhone(phone);
        fallVideoDao.updateByPrimaryKey(fallVideo);
    }
}
