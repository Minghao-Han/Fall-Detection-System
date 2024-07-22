<template>
<!--    xmlns="http://www.w3.org/1999/html"-->

  <div>
    <div>
      <el-button @click="table = true" type="primary" style="margin-left: 16px;">
        查看摄像头目录
      </el-button>
      <el-button
          v-if="selectedCamera != null && isPlaying"
          type="danger"
          @click="togglePlayPause"
      >
        暂停
      </el-button>
      <el-button
          v-if="selectedCamera != null && !isPlaying"
          type="success"
          @click="togglePlayPause"
      >
        播放
      </el-button>
<!--      <div style="position: relative; width: 100%; height: 100vh;">-->
<!--        <el-image :src="imageUrl" alt="Image"-->
<!--             v-if="selectedCamera != null"-->
<!--             style="position: absolute; top: 50%; left: 50%; transform: translate(-50%, -75%);-->
<!--             width: 640px; height: 480px"></el-image>-->
<!--      </div>-->
      <div>
        <h1>一、安装播放器</h1>
        <ol>
          <li>点击右侧<a href="https://potplayer.org/" target="_blank">PotPlayer播放器</a></li>
          <li>在网站上找到并下载最新版本的播放器。</li>
          <li>解压下载下来的压缩包</li>
          <li>运行其中的安装.bat</li>
        </ol>
        <h1>二、使用播放器查看摄像头画面</h1>
        <ol>
          <li>打开PotPlayer。</li>
          <li>点击添加->添加链接</li>
          <li>将 rtmp://192.168.43.133:1935/live/streamkey 粘贴到输入框中，点击确定。</li>
          <li>双击播放器右方出现的源，开始查看摄像头画面</li>
        </ol>
      </div>

      <el-drawer
          title="请选择你要播放的摄像头"
          :visible.sync="table"
          :with-header="false"
          direction="rtl"
          size="20%">
        <el-table :data="gridData">
          <el-table-column property="cameraName" label="摄像头位置" width="200"></el-table-column>
          <el-table-column label="操作" >
            <template slot-scope="scope">
              <el-button type="primary"
                         @click="choose(scope.row.serialNumber)"
                         v-if="selectedCamera !== scope.row.serialNumber">选择</el-button>
              <span v-else>已选择</span>
            </template>
          </el-table-column>
        </el-table>
      </el-drawer>
    </div>
  </div>
</template>

<script>

import request from "@/utils/request";

export default {
  data() {
    return {
      user: localStorage.getItem("user")?JSON.parse(localStorage.getItem("user")):{},
      table: false,
      isPlaying:false,
      imageUrl:null,
      intervalId: null,
      refreshInterval: 33,
      gridData: [],
      selectedCamera: null,
      params:{
        cameraName:'',
        phone:'',
        name:'',
        pageNum:1,
        pageSize:5,
      },
    };
  },
  created() {
    this.params.phone=this.user.phone;
    this.findBySearch();
  },
  beforeDestroy() {
    clearInterval(this.intervalId);
  },
  methods: {
    findBySearch(){
      request.get("/camera/search", {
        params: this.params
      }).then(res =>{
        if(res.code === '0'){
          this.gridData = res.data.list;
        }else {
          this.$message({
            message: res.msg,
            type: 'error'
          });
        }
      })
     },
    choose(serialNumber) {
      this.selectedCamera = serialNumber;
      // request.patch("/video/"+serialNumber).then(res => {
      //   if (res.code === '0') {
      //     this.$message({
      //       message: '播放成功',
      //       type: 'success'
      //     });
      //   } else {
      //   }
      // })
      this.togglePlayPause();
      },
    loadImage() {
      // 使用封装好的 Axios 实例发送 GET 请求
      request.get("/camera/stream",{responseType: "arraybuffer"})
          .then(res => {
            // 成功获取图片数据后更新图片名称，触发重新加载图片
            const imageData = this.arrayBufferToBase64(res);
            this.imageUrl = `data:image/jpeg;base64,${imageData}`;
          })
          .catch(error => {
            console.error('Error fetching image:', error);
          });
      },
    arrayBufferToBase64(buffer) {
      let binary = '';
      const bytes = new Uint8Array(buffer);
      const len = bytes.byteLength;
      for (let i = 0; i < len; i++) {
        binary += String.fromCharCode(bytes[i]);
      }
      return window.btoa(binary);
      },
    togglePlayPause() {
      if(this.isPlaying === false) {
        this.$message({
          message: '播放中',
          type: 'success',
          duration: 500
        });
        this.intervalId = setInterval(this.loadImage, this.refreshInterval);
      }
      else {
        this.$message({
          message: '已暂停',
          type: 'warning',
          duration: 500
        });
        clearInterval(this.intervalId);
      }
      this.isPlaying = !this.isPlaying;
      },
     }
  };
</script>
