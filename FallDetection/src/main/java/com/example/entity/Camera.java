package com.example.entity;

import com.fasterxml.jackson.annotation.JsonFormat;

import javax.persistence.*;
import java.util.Date;

@Table(name = "camera")
public class Camera {
    @Id
    @Column(name="serialNumber")
    private String serialNumber;
    /*    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Integer id;*/

    @Column(name = "cameraName")
    private String cameraName;

/*    @Column(name = "serialNumber")
    private String serialNumber;*/

    @Column(name = "phone")
    private String phone;

    @Column(name = "name")
    private String name;

/*    @Column(name="id")
    private Integer id;*/

    @Column(name = "dateTime")
    @Temporal(TemporalType.DATE)
    @JsonFormat(shape = JsonFormat.Shape.STRING, pattern = "yyyy-MM-dd",timezone = "GMT+8")
    private Date dateTime;

/*    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }*/

    public String getCameraName() {
        return cameraName;
    }

    public void setCameraName(String cameraName) {
        this.cameraName = cameraName;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public String getPhone() {
        return phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

    public Date getDateTime() {
        return dateTime;
    }

    public void setDateTime(Date dateTime) {
        this.dateTime = dateTime;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
