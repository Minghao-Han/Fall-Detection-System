package com.example.service;

import com.example.dao.CameraDao;
import com.example.entity.Camera;
import com.example.entity.Params;
import com.github.pagehelper.PageHelper;
import com.github.pagehelper.PageInfo;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.io.Console;
import java.util.List;

@Service
public class CameraService {

    @Resource
    private CameraDao cameraDao;

    public List<Camera> getCamera() {
        // 3. 使用引入的包
        return cameraDao.selectAll();
    }


    public PageInfo<Camera> findBySearch(Params params) {
        //开启分页查询
        PageHelper.startPage(params.getPageNum(), params.getPageSize());
        //接下来的查询会自动分页
        List<Camera> list= cameraDao.findBySearch(params);
        return PageInfo.of(list);
    }

    public void update(Camera camera) {
        cameraDao.updateByPrimaryKeySelective(camera);
    }

    public void unbindPhone(String serialNumber) {
        cameraDao.unbindPhone(serialNumber);
    }

    public int bind(Camera camera) {
        return cameraDao.bind(camera);
    }

    public int getAffectedRows(Camera camera){
        return cameraDao.getAffectedRows(camera);
    }
}


