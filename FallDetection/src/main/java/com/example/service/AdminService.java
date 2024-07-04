package com.example.service;

import com.example.dao.AdminDao;
import com.example.entity.Admin;
import com.example.exception.CustomException;
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

    public Admin login(Admin admin) {
// 1. 进行一些非空判断
        if (admin.getName() == null || "".equals(admin.getName())) {
            throw new CustomException("用户名不能为空");
        }
        if (admin.getPassword() == null || "".equals(admin.getPassword())) {
            throw new CustomException("密码不能为空");
        }
        // 2. 从数据库里面根据这个用户名和密码去查询对应的管理员信息，
        Admin user = adminDao.findByNameAndPassword(admin.getName(), admin.getPassword());
        if (user == null) {
            // 如果查出来没有，那说明输入的用户名或者密码有误，提示用户，不允许登录
            throw new CustomException("用户名或密码输入错误");
        }
        // 如果查出来了有，那说明确实有这个管理员，而且输入的用户名和密码都对；
        return user;
    }
}
