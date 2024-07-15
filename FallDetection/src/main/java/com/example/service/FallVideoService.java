package com.example.service;

import com.example.dao.FallVideoDao;
import com.example.entity.FallVideo;
import com.example.entity.Params;
import com.github.pagehelper.PageHelper;
import com.github.pagehelper.PageInfo;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.List;

@Service
public class FallVideoService {

    @Resource
    private FallVideoDao fallVideoDao;

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
