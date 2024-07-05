// cameraStream.js
var stompClient = null;

function connect() {
    var socket = new SockJS('/live-camera');
    stompClient = Stomp.over(socket);
    stompClient.connect({}, function(frame) {
        console.log('Connected: ' + frame);
        stompClient.subscribe('/topic/camera', function(message) {
            var imageData = JSON.parse(message.body);
            // 在页面上显示图像数据
            document.getElementById('camera-view').src = 'data:image/jpeg;base64,' + imageData;
        });
    });
}

function disconnect() {
    if (stompClient !== null) {
        stompClient.disconnect();
    }
    console.log("Disconnected");
}

// 暴露 connect 和 disconnect 函数给外部使用
export { connect, disconnect };
