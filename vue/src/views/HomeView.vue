<template>
  <div>
    <div>
      <el-radio-group v-model="direction">
      </el-radio-group>

      <el-button @click="table = true" type="primary" style="margin-left: 16px;">
        查看摄像头目录
      </el-button>

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
                         v-if="selectedCamera !== scope.row.serialNumber">播放</el-button>
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
      request.patch("/video/"+serialNumber).then(res => {
        if (res.code === '0') {
          this.$message({
            message: '播放成功',
            type: 'success'
          });
        } else {
        }
      })
      },
     }
  };
</script>
