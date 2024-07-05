import Vue from 'vue'
import VueRouter from 'vue-router'
import HomeView from '../views/HomeView.vue'
import LoginView from '../views/LoginView.vue'
import LayoutView from "@/views/Layout.vue";
import CameraStream from "@/views/CameraStream.vue";

Vue.use(VueRouter)

const routes = [
  {
    path: '/login',
    name: 'login',
    component: LoginView
  },
  {
    path: '/',
    name: 'Layout',
    component: LayoutView,
    children:[
      {
        path: '',
        name: 'home',
        component: HomeView
      },
      {
        path: 'camera',
        name: 'camera',
        component: () => import(/* webpackChunkName: "about" */ '../views/CameraView.vue')
      },
      {
        path: 'stream',
        name: 'stream',
        component: () => import('../views/CameraStream.vue')
      }
    ]
  },
]

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

export default router
