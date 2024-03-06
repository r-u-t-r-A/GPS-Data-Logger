const char ws_html[] = R"=====(
<html>
  <head>
    <script>
      var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
      connection.onopen = function () {  
        connection.send('data'); 
      }; 
      function requestData() {
        connection.send('data');
      };
      setInterval(requestData, 1000);
      connection.onerror = function (error) {    
        console.log('WebSocket Error ', error);
      };

    connection.onmessage = function onMessage(event) {
      console.log(event.data);
      var myObj = JSON.parse(event.data);
      var keys = Object.keys(myObj);

      for (var i = 0; i < keys.length; i++){
          var key = keys[i];
          document.getElementById(key).innerHTML = myObj[key];
      }
    } 

      
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
        initreset();
      
      }
    
      function initstop() {
        document.getElementById('stop').addEventListener('mousedown', StopButton);
        document.getElementById('stop').addEventListener('touchstart', StopButton);
        
      }
      function initstart() {
        document.getElementById('start').addEventListener('mousedown', StartButton);
      
        document.getElementById('start').addEventListener('touchstart', StartButton);
      
      }
      function initreset() {
        document.getElementById('reset').addEventListener('mousedown', ResetButton);
      
        document.getElementById('reset').addEventListener('touchstart', ResetButton);
      
      }
      function StartButton(){
        connection.send('StartButton');
      }
      function StopButton(){
        connection.send('StopButton');
      }
      function ResetButton(){
        connection.send('ResetButton');
      }
      
  </script>
<style> 
body {
  background-color: #181a1f;  
}  
.start {
  padding: 60px 60px;
    font-size: 36px;
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
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
     
   }

   .start:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateY(2px);
   }
   
   .stop {
    padding: 60px 60px;
    font-size: 36px;
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
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    
   }

   .stop:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateY(2px);
   }
   .reset {
    padding: 60px 60px;
    font-size: 36px;
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
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    
   }

   .reset:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateY(2px);
   }
   
    .content {
    padding: 50px;
    }
.card-grid {
    
    max-width: 800px;
    margin: auto;
    display: inline-grid;
  grid-template-columns: auto auto auto;
   
    column-gap: 50px;
    row-gap: 50px;


}
.card {
    text-align: center;
    background-color: #0a6163;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
}
.card-title {
    font-size: 36px;
    font-weight: bold;
    color: white;
}
.reading {
    font-size: 36px;
    color: white;
}
.genbut {
  padding: 60px 60px;
    font-size: 36px;
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
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
}

.genbut:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateY(2px);
   }

</style>
</head>
  <body>  
    <div class ="buts">
      
    </div>
    <div class="content">
      <div class="card-grid">
        
        <div class="card">
          <p class="card-title"> Max</p>
          <p class="reading"><span id="max"></span> kmh</p>
        </div>
        <div class="card">
          <p class="card-title"> Current</p>
          <p class="reading"><span id="current"></span> kmh</p>
        </div>
        <div class="card">
          <p class="card-title"> Sats</p>
          <p class="reading"><span id="sats"></span> </p>
        </div>
        <div class="card">
          <p class="card-title"> latitude</p>
          <p class="reading"><span id="lat"></span> </p>
        </div>
        <div class="card">
          <p class="card-title"> longtitude</p>
          <p class="reading"><span id="lon"></span> </p>
        </div>
        <div class="card">
          <p class="card-title"> Date</p>
          <p class="reading"><span id="date"></span> </p>
        </div>
        <p><button id="stop" class="stop">Stop</button></p>
        <p><button id="start" class="start">Start</button></p>
        <p><button id="reset" class="reset">Reset</button></p>
        <p><button class="genbut" onclick="document.location='/'">Term</button></p>
        <p><button class="genbut" onclick="document.location='/'">Home</button></p>
        <p><button class="genbut" onclick="document.location='/update'">Update</button></p> 
      </div>
    </div>
  </body>
</html>
)=====";