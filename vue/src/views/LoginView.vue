
<template>
  <div class="full-screen-background"style="width: content-box;height: content-box;background-image: url('/background.png')">
    <el-container>
          <div style="width: 900px; height: 450px; margin: 200px auto;
          background-color:rgba(252,252,252,1); border-radius: 10px;display: flex;">
            <h2 style="position: absolute; top: 70px; left: 50%; transform: translateX(-50%);">独居老人检测系统</h2>
            <div style="flex: 1" class="el-container">
              <div class="background" :style="{ backgroundImage:'url(' + require('@/assets/old.gif') + ')' }">
              </div>
            </div>
            <div style="width: 1px; background-color: rgba(204, 204, 204, 0.5);"></div>
            <div style="width: 100%; height: 100%; text-align: center;flex: 1">
              <div style="width: 80%; margin: 70px auto; text-align: center;">
                <div style="display: inline-block;background-color: #7a7ab9; height: 3px; width: 80%; margin-bottom: 5px;"></div>
                <h4 style="margin-left: 40px; margin-bottom: 30px ;text-align: left; width: 80% ; font-weight: normal">请登录您的账号</h4>
                <el-form :model="admin">
                  <el-form-item>
                    <el-input size="small" maxlength="11" v-model="admin.phone" prefix-icon="el-icon-phone" style="width: 80%;opacity:70%"placeholder="请输入您的手机号" ></el-input>
                  </el-form-item>
                  <el-form-item>
                    <el-input size="small" v-model="admin.password" prefix-icon="el-icon-lock" style="width: 80%;opacity: 70%" placeholder="请输入您的密码" show-password></el-input>
                  </el-form-item>
                  <el-form-item>
                    <el-button round style="width: 80%; height: 40px ;margin-top: 10px" type="primary" @click="login()">登录</el-button>
                  </el-form-item>
                  <el-form-item>
                    <el-button round style="width: 80%; height: 40px " type="primary" @click="jumpToRegister()">注册</el-button>
                  </el-form-item>
                </el-form>
              </div>
            </div>
          </div>
    </el-container>
  </div>
</template>

<style>
.el-carousel__item h3 {
  color: #475669;
  font-size: 14px;
  opacity: 0.75;
  line-height: 200px;
  margin: 0;
}

.background {
  width: 100%;
  height: 100%;
  background-size: cover;
  background-repeat: no-repeat;
}

.full-screen-background {
  width: 100%;
  height: 100vh; /* 使用视口高度来充满整个屏幕 */
  background-size: cover; /* 确保背景图片充满容器 */
}

.el-carousel__item:nth-child(2n) {
  background-color: #99a9bf;
}

.el-carousel__item:nth-child(2n+1) {
  background-color: #d3dce6;
}
</style>

<script>

import request from "@/utils/request.js";
export default {
  name: "Login",
  data() {
    return {
      admin:{}
    }
  },
  //页面加载的时候做的事情在created里面
  created() {
  },
  //定义页面上一些控件出发的事件调用的方法
  methods: {
    login(){
      request.post("/admin/login", this.admin).then(res => {
        if (res.code === '0') {
          this.$message({
            message: '登录成功',
            type: 'success'
          });
          localStorage.setItem("user",JSON.stringify(res.data));
          this.$router.push("/");
        } else {
          this.$message({
            message: res.msg,
            type: 'error'
          });
        }
      })
    },
    jumpToRegister(){
      this.$router.push("/register");
    }
  }
}
</script>
