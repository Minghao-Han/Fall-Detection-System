
<template>
  <div>

        <div class="box">
          <div style="width: 400px; height: 350px; margin: 150px auto; background-color:rgba(107,149,224,0.5); border-radius: 10px">
            <div style="width: 100%; height: 100px; font-size: 30px; line-height: 100px; text-align: center; color: #4a5ed0">欢迎登录</div>
            <div style="margin-top: 25px; text-align: center; height: 320px;">
              <el-form :model="admin">
                <el-form-item>
                  <el-input v-model="admin.phone" prefix-icon="el-icon-user" style="width: 80%" placeholder="请输入手机号" ></el-input>
                </el-form-item>
                <el-form-item>
                  <el-input v-model="admin.password" prefix-icon="el-icon-lock" style="width: 80%" placeholder="请输入密码" show-password></el-input>
                </el-form-item>
                <el-form-item>
                  <el-button style="width: 80%; margin-top: 10px" type="primary" @click="login()">登录</el-button>

                </el-form-item>
                <el-link type="primary" @click="goRegister()">没有账号？点击注册</el-link>
              </el-form>
            </div>
          </div>
        </div>


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
    goRegister(){
      this.$router.push("/register");
    }
  }
}
</script>
