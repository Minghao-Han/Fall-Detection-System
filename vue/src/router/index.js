import Vue from 'vue'
import VueRouter from 'vue-router'

Vue.use(VueRouter)

const routes = [
  {
    path: '/login',
    name: 'Login',
    // component: LoginView
    component: () => import('../views/LoginView.vue')
  },
  {
    path: '/register',
    name: 'Register',
    component: () => import('../views/RegisterView.vue')
  },
  {
    path: '/',
    name: 'Layout',
    component: () => import('../views/Layout.vue'),
    children:[
      {
        path: '',
        name: 'home',
        component: () => import('../views/HomeView.vue')
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
      },
      {
        path: 'fallVideo',
        name: 'fallVideo',
        component: () => import(/* webpackChunkName: "about" */ '../views/FallVideoView.vue')
      },
    ]
  },
]

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

router.beforeEach((to ,from, next) => {
  if (to.path === '/login') {
    next();
  } else if (to.path === '/register') {
    next();
  } else {
    const user = localStorage.getItem("user");
    if (!user && to.path !== '/login') {
      return next("/login");
    }
  }
})

export default router
