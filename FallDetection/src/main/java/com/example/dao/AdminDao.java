package com.example.dao;

import com.example.entity.Admin;
import org.apache.ibatis.annotations.Param;
import org.apache.ibatis.annotations.Select;
import org.springframework.stereotype.Repository;
import tk.mybatis.mapper.common.Mapper;

import java.util.List;

@Repository
public interface AdminDao extends Mapper<Admin> {
    //基于注解的方式
    //@Select("select * from admin")
    List<Admin> findAll();

    @Select("select * from admin where phone = #{phone} and password = #{password} limit 1")
    Admin findByPhoneAndPassword(@Param("phone") String phone, @Param("password") String password);

    @Select("select * from admin where name = #{name} limit 1")
    Admin findByName(@Param("name") String name);
}
