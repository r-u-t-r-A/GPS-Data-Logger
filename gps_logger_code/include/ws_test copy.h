const char ws_html[] = R"=====(
<html>
<head>
<script>
var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
connection.onopen = function () {  connection.send('Connect ' + new Date()); }; 
connection.onerror = function (error) {    console.log('WebSocket Error ', error);};
connection.onmessage = function (e) {  console.log('Server: ', e.data);};
connection.on

  window.addEventListener('load', onLoad);
  
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
  }
  function onLoad(event) {
    initstop();
    initstart();
  }
 
  function initstop() {
    document.getElementById('stop').addEventListener('mousedown', StopButton);
    document.getElementById('stop').addEventListener('touchstart', StopButton);
    
  }
   function initstart() {
    document.getElementById('start').addEventListener('mousedown', StartButton);
  
    document.getElementById('start').addEventListener('touchstart', StartButton);
  
  }
  function StartButton(){
    connection.send('StartButton');
  }
  function StopButton(){
    connection.send('StopButton');
  }
   
</script>
<style> 
.start {
    padding: 50px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.start:hover {background-color: #0f8b8d}*/
   .start:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateX(-2px);
   }
   
   .stop {
    padding: 50px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
    margin-top: 50px;
   }
   /*.stop:hover {background-color: #0f8b8d}*/
   .stop:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateX(-2px);
   }
   .buttonB {
    padding: 50px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
    margin:10px;
   }
   /*.buttonB:hover {background-color: #0f8b8d}*/
   .buttonB:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateX(-2px);
   }
   .txtOut {
    width: 100%;
    height: 100%;
    box-sizing: border-box;
    resize: none;
    background-color: rgb(60, 60, 60);
    color: white;
    font: inherit;
    font-size: 0.8em;
}
 
</style>
</head>
<body>  
<p><button id="stop" class="stop">Stop</button></p>
<p><button id="start" class="start">Start</button></p> </br>
<div id="output">
                <textarea readonly id="txtOut"></textarea>
            </div>
</body>
</html>"
)=====";