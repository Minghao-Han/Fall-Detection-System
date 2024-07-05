<template>
  <div>
    <h1>Live Camera Stream</h1>
    <img id="camera-view" :src="imageSrc" style="max-width: 100%;" />
  </div>
</template>

<script>
import { connect, disconnect } from "@/utils/cameraStream.js"; // 导入之前创建的 JavaScript 文件

export default {
  data() {
    return {
      imageSrc: ''
    };
  },
  mounted() {
    connect(); // 在组件加载后连接到 WebSocket
  },
  beforeDestroy() {
    disconnect(); // 在组件销毁前断开 WebSocket 连接
  },
  methods: {
    updateImage(imageData) {
      this.imageSrc = 'data:image/jpeg;base64,' + imageData;
    }
  },
  created() {
    stompClient.subscribe('/topic/camera', (message) => {
      var imageData = JSON.parse(message.body);
      this.updateImage(imageData);
    });
  }
};
</script>
