<!--VideoPlayer.vue-->
<!--前置网页需要提供视频的名字-->
<template>
  <div>
    <video ref="videoPlayer" :src="videoUrl" controls></video>
  </div>
</template>

<script>
export default {
  props: {
    fileName: {
      type: String,
      required: true
    }
  },
  data() {
    return {
      videoUrl: null
    };
  },
  created() {
    // 组件创建时加载视频
    this.loadVideo();
  },
  watch: {
    fileName: function(newFileName) {
      // 监听 fileName 变化重新加载视频
      this.loadVideo();
    }
  },
  methods: {
    loadVideo() {
      const apiUrl = `/video/fallClip/${this.fileName}`; // 构造请求的视频文件URL

      fetch(apiUrl)
          .then(response => {
            if (!response.ok) {
              throw new Error('Network response was not ok');
            }
            return response.blob();
          })
          .then(blob => {
            const videoObjectURL = URL.createObjectURL(blob);
            this.videoUrl = videoObjectURL;

            // 加载并播放视频
            this.$refs.videoPlayer.load();
            this.$refs.videoPlayer.play();
          })
          .catch(error => {
            console.error('Error fetching the video:', error);
          });
    }
  }
};
</script>

