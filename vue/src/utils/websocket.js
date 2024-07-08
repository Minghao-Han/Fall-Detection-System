// websocket.js

import SockJS from 'sockjs-client';
import Stomp from 'stompjs';

let stompClient = null;

export function initializeWebSocket() {
    const socket = new SockJS('http://localhost:8080/ws-alerts'); // WebSocket endpoint
    stompClient = Stomp.over(socket);

    stompClient.connect({}, frame => {
        console.log('Connected: ' + frame);
        stompClient.subscribe('/topic/alerts', alertMessage => {
            // 收到警告消息
            handleAlert(JSON.parse(alertMessage.body));
        });
    });
}

export function disconnectWebSocket() {
    if (stompClient !== null) {
        stompClient.disconnect();
    }
    console.log("Disconnected");
}

function handleAlert(alertMessage) {
    // 处理警告消息，例如弹窗
    alert(`Received alert: ${alertMessage}`);
}
