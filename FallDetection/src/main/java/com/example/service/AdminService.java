package com.example.service;

import com.example.dao.AdminDao;
import com.example.entity.Admin;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.List;

@Service
public class AdminService {
    @Resource
    private AdminDao adminDao;

    public List<Admin> getUser() {
        //3使用引入的包
        return adminDao.selectAll();
    }
}
