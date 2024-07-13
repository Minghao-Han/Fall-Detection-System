import Vue from 'vue'
import VueRouter from 'vue-router'
import HomeView from '../views/HomeView.vue'
import LoginView from '../views/LoginView.vue'
import LayoutView from "@/views/Layout.vue";
import RegisterView from "@/views/RegisterView.vue"
import CameraStream from "@/views/CameraStream.vue";

Vue.use(VueRouter)

const routes = [
  {
    path: '/login',
    name: 'Login',
    component: LoginView
  },
  {
    path: '/register',
    name: 'Register',
    component: RegisterView
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

router.beforeEach((to ,from, next) => {
  if (to.path ==='/login') {
    next();
  }
  const user = localStorage.getItem("user");
  if (!user && to.path !== '/login') {
    return next("/login");
  }
  next();
})

export default router
