package com.example.dao;

import com.example.entity.Camera;
import com.example.entity.FallVideo;
import com.example.entity.Params;
import org.apache.ibatis.annotations.Param;
import org.apache.ibatis.annotations.Select;
import org.springframework.stereotype.Repository;
import tk.mybatis.mapper.common.Mapper;

import java.util.List;

@Repository
public interface FallVideoDao extends Mapper<FallVideo> {

    List<FallVideo> findBySearch(@Param("params") Params params);

    @Select("SELECT phone FROM camera WHERE serialNumber = #{serialNumber}")
    String getPhoneBySerialNumber(@Param("serialNumber") String serialNumber);

    @Select("SELECT videoId FROM video WHERE serialNumber = #{serialNumber}")
    int getVideoId(@Param("serialNumber") String serialNumber);
}
