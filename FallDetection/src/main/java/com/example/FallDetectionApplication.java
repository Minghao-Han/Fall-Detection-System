package com.example;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import tk.mybatis.spring.annotation.MapperScan;

@SpringBootApplication
@MapperScan("com.example.dao")
public class FallDetectionApplication {

    public static void main(String[] args) {
        SpringApplication.run(FallDetectionApplication.class, args);
    }

}
