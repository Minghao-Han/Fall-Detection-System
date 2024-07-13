package com.example.entity;

import javax.persistence.*;

@Table(name = "admin")
public class Admin {

    @Id
    @Column(name="phone")
    private String phone;

    @Column(name = "name")
    private String name;

    @Column(name = "password")
    private String password;

    @Transient
    private String token;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getPhone() {
        return phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

    public String getToken() {return token;}

    public void setToken(String token) {this.token = token;}
}


