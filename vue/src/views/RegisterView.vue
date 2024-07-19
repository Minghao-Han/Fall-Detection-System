<template>
  <div class="full-screen-background"style="width: content-box;height: content-box;background-image: url('/background.png')">
    <el-container>
      <div style="width: 900px; height: 450px; margin: 200px auto;
          background-color:rgba(252,252,252,1); border-radius: 10px;display: flex;">
        <h2 style="position: absolute; top: 70px; left: 50%; transform: translateX(-50%);">独居老人检测系统</h2>
        <div style="width: 100%; height: 100%; text-align: center;flex: 1">
          <div style="width: 80%; margin: 70px auto; text-align: center;">
            <div style="display: inline-block;background-color: #7a7ab9; height: 3px; width: 80%; margin-bottom: 5px;"></div>
            <h4 style="margin-left: 40px; margin-bottom: 30px ;text-align: left; width: 80% ; font-weight: normal">请注册您的账号</h4>
            <el-form :model="admin">
              <el-form-item>
                <el-input size="small" v-model="admin.name" prefix-icon="el-icon-user" style="width: 80%" placeholder="请输入您的用户名"></el-input>
              </el-form-item>
              <el-form-item>
                <el-input size="small" v-model="admin.phone" maxlength="11" prefix-icon="el-icon-phone" style="width: 80%" placeholder="请输入您的手机号"></el-input>
              </el-form-item>
              <el-form-item>
                <el-input size="small" v-model="admin.password" prefix-icon="el-icon-lock" style="width: 80%" placeholder="请输入您的密码"></el-input>
              </el-form-item>
              <el-form-item>
                <el-button round style="width: 80%; height: 40px" type="primary" @click="register()">注册</el-button>
              </el-form-item>
              <el-form-item>
<<<<<<< HEAD
                <el-button round style="width: 80%; height: 40px" type="primary" @click="goLogin()">返回登录界面</el-button>
=======
                <el-button round style="width: 80%; height: 40px" type="primary">返回登录界面</el-button>
>>>>>>> 07b5ca7a7ef17d8f083510ae937a511276b1b83e
              </el-form-item>
            </el-form>
          </div>
        </div>
        <div style="width: 1px; background-color: rgba(204, 204, 204, 0.5);"></div>
        <div style="flex: 1" class="el-container">
          <div class="background" :style="{ backgroundImage:'url(' + require('@/assets/walking.gif') + ')' }">
          </div>
        </div>
      </div>
    </el-container>
  </div>
</template>
<style>
.background {
  width: 100%;
  height: 100%;
  background-size: cover;
  background-repeat: no-repeat;
}
</style>
<script>

import request from "@/utils/request.js";
export default {
  name: "Register",
  data() {
    return {
      admin:{
      }
    }
  },
  //页面加载的时候做的事情在created里面
  created() {
  },
  //定义页面上一些控件出发的事件调用的方法
  methods: {
    register(){
      request.post("/admin/register", this.admin).then(res => {
        if (res.code === '0') {
          this.$message({
            message: '注册成功',
            type: 'success'
          });
          this.$router.push("/login");
        } else {
          this.$message({
            message: res.msg,
            type: 'error'
          });
        }
      })
    },
    goLogin(){
      this.$router.push("/login");
    }
  }
}
</script>
