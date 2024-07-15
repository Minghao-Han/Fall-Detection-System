package com.example.entity;


import javax.persistence.*;

@Table(name = "video")
public class FallVideo {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Integer videoId;

    @Column(name = "phone")
    private String phone;

    @Column(name = "serialNumber")
    private String serialNumber;

    @Column(name = "videoName")
    private String videoName;

    public Integer getVideoId() {
        return videoId;
    }

    public void setVideoId(Integer videoId) {
        this.videoId = videoId;
    }

    public String getPhone() {
        return phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public String getVideoName() {
        return videoName;
    }

    public void setVideoName(String videoName) {
        this.videoName = videoName;
    }
}
