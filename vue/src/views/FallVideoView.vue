<template>
  <div>
    <div>
      <el-table :data="tableData" stripe style="width: 100%">
        <!--这里的prop的名字应该与entity中设定的变量名相同        -->
        <el-table-column prop="videoName" label="视频名称"></el-table-column>
        <el-table-column label="操作" >
          <template slot-scope="scope">
            <el-button type="primary" @click="play(scope.row.videoName)">播放</el-button>
<!--            <el-popconfirm title="确定删除吗？" @confirm="">
              <el-button slot="reference" type="danger" style="margin-left: 5px">删除</el-button>
            </el-popconfirm>-->
          </template>
        </el-table-column>
      </el-table>
    </div>
    <div class="block">
      <el-pagination
          @size-change="handleSizeChange"
          @current-change="handleCurrentChange"
          :current-page="params.pageNum"
          :page-sizes="[5, 10, 15, 20]"
          :page-size="params.pageSize"
          layout="total, sizes, prev, pager, next, jumper"
          :total="total">
      </el-pagination>
    </div>
    <el-dialog
        :visible.sync="dialogVisible"
        width="60%">
      <div>
          <video-player class="video-player vjs-custom-skin"
                    :options="playerOptions"
          ></video-player>
      </div>
    </el-dialog>
  </div>
</template>

<script>
import request from "@/utils/request";
export default {
  data() {
    return {
      user: localStorage.getItem("user")?JSON.parse(localStorage.getItem("user")):{},
      params:{
        videoName:'',
        pageNum:1,
        pageSize:5,
        phone:'',
      },
      tableData: [],
      total:0,
      dialogVisible:false,
      playerOptions: {
        height: '480',
        width:'640',
        autoplay: true,
        muted: false,
        language: 'en',
        playbackRates: [0.5, 1.0, 1.5, 2.0],
        sources: [{
          type: "video/mp4",
          // mp4
          src: ""
          // src: "https://cdn.theguardian.tv/webM/2015/07/20/150716YesMen_synd_768k_vp8.webm"
        }],
        poster: "https://surmon-china.github.io/vue-quill-editor/static/images/surmon-1.jpg",
      }

    }
  },
  //页面加载的时候做的事情，在created里面
  created() {
    this.params.phone=this.user.phone;
    this.findBySearch();
  },
  //定义一些界面上控件出发的事件并调用方法
  methods:{
    findBySearch(){
      request.get("/video/search", {
        params: this.params
      }).then(res =>{
        if(res.code === '0'){
          this.tableData = res.data.list;
          this.total=res.data.total;
        }else {

        }
      })
    },
    handleSizeChange(pageSize){
      this.params.pageSize = pageSize;
      this.findBySearch();
    },
    handleCurrentChange(pageNum){
      this.params.pageNum = pageNum;
      this.findBySearch();
    },
    play(videoName){
      this.dialogVisible = true;
      request.get("/video/fallClips/"+videoName+".mp4", {
        responseType: "blob"
      })
          .then(res =>{
            const videoBlob = new Blob([res], {type: 'video/mp4'});
            this.playerOptions.sources[0].src = URL.createObjectURL(videoBlob);
          })
          .catch(error =>{
            console.error('Error fetching video', error);
          });
    }
  }
}
</script>
