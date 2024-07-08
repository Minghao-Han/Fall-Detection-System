package com.example.dao;

import com.example.entity.Camera;
import com.example.entity.Params;
import org.apache.ibatis.annotations.Param;
import org.apache.ibatis.annotations.Update;
import org.springframework.stereotype.Repository;
import tk.mybatis.mapper.common.Mapper;

import java.util.List;

@Repository
public interface CameraDao extends Mapper<Camera> {

    List<Camera> findBySearch(@Param("params") Params params);

    @Update("UPDATE camera SET phone = NULL WHERE id = #{id}")
    int unbindPhone(@Param("id") Integer id);

}