/**
 * Created by R00118979 on 13/03/2017.
 */

var webserverAddress = "192.168.172.1"

var socket = new WebSocket("ws://"+ webserverAddress + ":8081/ws");
var received = $('#received');
socket.onopen = function(){
    console.log("connected");
};
socket.onmessage = function (message) {
    console.log("receiving: " + message.data);
};

socket.onclose = function(){
    console.log("disconnected");
};

var sendMessage = function(message) {
    socket.send(message.data);
};