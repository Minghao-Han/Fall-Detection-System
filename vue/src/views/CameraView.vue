<template>
  <div>
    <div>
      <el-input v-model="params.cameraName" style="width: 300px; margin-right: 10px" placeholder="请输入位置" clearable prefix-icon="el-icon-search"></el-input>
      <el-input v-model="params.name" style="width: 300px; margin-right: 10px" placeholder="请输入老人姓名" clearable prefix-icon="el-icon-search"></el-input>
      <el-button type="warning" @click="findBySearch()">查询</el-button>
      <el-button type="primary" @click="showWindow()">绑定</el-button>
    </div>
    <div>
      <el-table :data="tableData" stripe style="width: 100%">
<!--这里的prop的名字应该与entity中设定的变量名相同        -->
        <el-table-column prop="dateTime" label="日期"></el-table-column>
        <el-table-column prop="cameraName" label="位置"></el-table-column>
        <el-table-column prop="name" label="老人姓名"></el-table-column>
        <el-table-column label="操作" >
          <template slot-scope="scope">
            <el-button type="primary" @click="edit(scope.row)">编辑</el-button>
            <el-popconfirm title="确定删除吗？" @confirm="unbind(scope.row.serialNumber)">
              <el-button slot="reference" type="danger" style="margin-left: 5px">解绑</el-button>
            </el-popconfirm>
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
    <div>
      <el-dialog title="请修改信息" :visible.sync="dialogFormVisible" width="40%">
        <el-form :model="form">
          <el-form-item v-if="isShow" label="序列码" prop="serialNumber" label-width="15%">
            <el-input v-model="form.serialNumber" autocomplete="off" style="width:90%"></el-input>
          </el-form-item>
          <el-form-item label="位置" prop="cameraName" label-width="15%">
            <el-input v-model="form.cameraName" autocomplete="off" style="width:90%"></el-input>
          </el-form-item>
          <el-form-item label="老人姓名" prop="name"label-width="15%">
            <el-input v-model="form.name" autocomplete="off" style="width:90%"></el-input>
          </el-form-item>
        </el-form>
        <div slot="footer" class="dialog-footer">
          <el-button @click="dialogFormVisible = false">取 消</el-button>
          <el-button v-if="!isShow" type="primary" @click="submit()">确 定</el-button>
          <el-button v-if="isShow" type="primary" @click="bind()">绑 定</el-button>
        </div>
      </el-dialog>
    </div>
  </div>
</template>

<script>
import request from "@/utils/request";
export default {
  data() {
    return {
      user: localStorage.getItem("user")?JSON.parse(localStorage.getItem("user")):{},
      params:{
        cameraName:'',
        name:'',
        pageNum:1,
        pageSize:5,
        phone:'',
      },
      tableData: [],
      total:0,
      dialogFormVisible:false,
      bindVisible:false,
      form:{},
      isShow:false,
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
      request.get("/camera/search", {
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
    submit() {
      request.post("/camera",this.form).then(res => {
        if (res.code === '0') {
          this.$message({
            message: '操作成功',
            type: 'success'
          });
          this.dialogFormVisible = false;
          this.findBySearch();
        } else {
          this.$message({
            message: res.msg,
            type: 'error'
          });
        }
      })
    },
    bind(){
      request.post("/camera/bind",this.form).then(res => {
        if (res.code === '0') {
          this.$message({
            message: '操作成功',
            type: 'success'
          });
          this.dialogFormVisible = false;
          this.findBySearch();
        } else {
          this.$message({
            message: res.msg,
            type: 'error'
          });
        }
      })
    },
    edit(obj){
      this.isShow=false;
      this.form=JSON.parse(JSON.stringify(obj));
      this.dialogFormVisible=true;
    },
    unbind(serialNumber){
      request.patch("/camera/"+serialNumber).then(res => {
        if (res.code === '0') {
          this.$message({
            message: '解绑成功',
            type: 'success'
          });
          this.findBySearch();
        } else {
          this.$message({
            message: res.msg,
            type: 'error'
          });
        }
      })
    },
    showWindow(){
      this.form={};
      this.form.phone=this.user.phone;
      this.isShow=true;
      this.dialogFormVisible=true;
    }
  }
}
</script>
