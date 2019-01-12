var rainbowEnable = false;
var connection = new WebSocket("ws://" + location.hostname + ":81/", ["arduino"]);
connection.onopen = function () {
  connection.send("Connect " + new Date());
};
connection.onerror = function (error) {
    console.log("WebSocket Error ", error);
    
};
connection.onmessage = function (e) {  
    console.log("Server: ", e.data);};
connection.onclose = function () {
  console.log("WebSocket connection closed");
};
function sendInterval () {
  var i = document.getElementById("i").value;
  var intervalstring = "?" + i.toString();
  console.log("Interval: " + intervalstring);
  connection.send(intervalstring);
}

function sendBrightness () {
  var b = document.getElementById("b").value;
  var brightnessString = "%" + b.toString();
  console.log("Brightness: " + brightnessString);
  connection.send(brightnessString);
}


