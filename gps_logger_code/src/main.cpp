#include <Arduino.h>

#include <TinyGPS++.h> // library for GPS module
#include <SoftwareSerial.h>

#include <SD.h>
#include <SPI.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <ElegantOTA.h>
#include <Adafruit_NeoPixel.h>
#include <DNSServer.h>

#include "index.h"

#include <WebSocketsServer.h>

#include "ws_test.h"

#include <Arduino_JSON.h>
//#include <WebSockets.h>

WebSocketsServer webSocket = WebSocketsServer(81);

#define PixelCount 1// this example assumes 4 pixels, making it smaller will cause a failure
#define LED_PIN 15  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128

JSONVar readings;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(15, LED_PIN, NEO_GRB + NEO_KHZ800);

TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(5, 4); // The serial connection to the GPS device - rx - tx

const int SDchipSelect = 16;

const char* ssid = "GPS"; 
const char* password = "123456";

unsigned long old_ms = 0;

float max_speed = 0.0;
float current_speed = 0.0;
int sat = 0;
bool start_logging = false;
bool logging_running = false;
bool run_term = false;
long total_distance = 0;
long distance_from_start = 0;
static float start_lat;
static float start_lng;
static float previous_lat;
static float previous_lng;
int previous_seconds = 0;
String latitude = "";
String longtitude = "";
String altitude = "";
String time_year = "";
String time_month = "";
String time_day = "";
String time_hour = "";
String time_minute = "";
String time_second = "";
String databuffer = "";
String file_name = "def.csv";
String logger_status = "";

ESP8266WebServer server(80);
DNSServer dnsServer;

IPAddress local_IP(10,0,0,1);
IPAddress gateway(10,0,0,1);
IPAddress subnet(255,255,255,0);

//unsigned long ota_progress_millis = 0;
#define batt_res_ratio 21.818
String getSensorReadings(){
  readings["max"] = String(max_speed);
  readings["current"] =  String(current_speed);
  readings["sats"] = String(sat);
  readings["lat"] = latitude;
  readings["lon"] = longtitude;
  readings["date"] = time_day + "-" + time_month + "-" + time_year;
  readings["batt"] = String(analogRead(A0) * (1.0 / 1023) * batt_res_ratio);
  if (distance_from_start > 9999) {
    readings["sdist"] = String(distance_from_start/1000) + " km";
  } else {
    readings["sdist"] = String(distance_from_start) + " m";
  }

  if (total_distance > 9999) {
    readings["tdist"] = String(total_distance / 1000) + " km";
  } else {
    readings["tdist"] = String(total_distance) + " m";
  }
  readings["alt"] = altitude + " m";
  readings["time"] = time_hour + ":" + time_minute + ":" + time_second;
  readings["status"] = logger_status;
  //readings["tdist"] = String(total_distance);
  //readings["sdist"] = String(distance_from_start);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}


void handleTest() {
	databuffer = ws_html;
	server.send(200, "text/html", databuffer);
	databuffer.remove(0);
}

void handleRoot() {

  //databuffer = String("<html><head><meta content=\"width=device-width\"><style> body{font-size: 56px; text-align:center; background-color:#11d0ed;}</style> <h1>GPS LOGGER<br> Max velocity: <a href=""/max"">max speed</a> <br>Current velocity: <a href=""/current"">current speed</a><br>Reset Data: <a href=""/reset"">reset</a><br>Start: <a href=""/start"">start</a><br>Stop log: <a href=""/stop"">stop</a><br>terminal: <a href=""/term.html"">terminal</a><br>update: <a href=""/update"">UPDATE</a></h1> <body>""</body></html>");
  databuffer = ws_html;
  server.send(200, "text/html", databuffer);
  databuffer.remove(0);
}

void handleTerm() {
  databuffer = String("to be implemented");
  server.send(200, "text/html", databuffer);
  databuffer.remove(0);
}
/*
void max_val()  {

  databuffer = String("max value: ") + String(max_speed) + String("kmph") + "\n";
  databuffer = "<html><head><meta http-equiv=\"refresh\" content =\"2\" content=\"width=device-width\"><style> body{font-size: 56px; text-align:center; background-color:#11d0ed;}</style> <body>"
         + databuffer + "</br><a href=""/"">home</a></body></html>";
  server.send(200, "text/html", databuffer);
  databuffer.remove(0);
}

void current_val()  { 

  databuffer = String("current: ") + String(current_speed) + String("kmph") + "<br>" + String("sat: ") + String(sat) + String("") + "<br>";
  databuffer = "<html><head><meta http-equiv=\"refresh\" content =\"2\" content=\"width=device-width\"><style> body{font-size: 56px; text-align:center; background-color:#11d0ed;}</style> <body>" 
                + databuffer + "</br><a href=""/"">home</a></body></html>";
  server.send(200, "text/html", databuffer);
  databuffer.remove(0);
}

void reset()  {
  server.send(200, "text/html", "<html><head><meta content=\"width=device-width\"><style> body{font-size: 56px; text-align:center; background-color:#11d0ed;}</style> <h1></h1> <body><p><a href=/reset>Reset Data</a></p></body></html>");
  max_speed = 0.0;
  //ESP.reset();
  
}

void start_log() {
	server.send(200, "text/html", "<html><head><meta content=\"width=device-width\"><style> body{font-size: 56px; text-align:center; background-color:#11d0ed;}</style> <h1></h1> <body><p><a href=/>home</a></p></body></html>");
	start_logging = true;
	file_name.remove(0);
	file_name = time_day + time_month + "H" + time_hour + time_minute + ".csv";
	File dataFile = SD.open(file_name, FILE_WRITE);
	if (dataFile) {
		dataFile.println("type,latitude,longitude,alt,speed,time,color");
  	dataFile.close();
	}
  	
}

void stop_log() {
	server.send(200, "text/html", "<html><head><meta content=\"width=device-width\"><style> body{font-size: 56px; text-align:center; background-color:#11d0ed;}</style> <h1></h1> <body><p><a href=/>home</a></p></body></html>");
	start_logging = false;
}

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  start_logging = false;
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

void progressCallBack(size_t currSize, size_t totalSize) {
      Serial.printf("CALLBACK:  Update process at %d of %d bytes...\n", currSize, totalSize);
}*/
/*
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "FButton") == 0) {
      FState = !FState;
    }
    if (strcmp((char*)data, "BButton") == 0) {
      BState = !BState;
    }
     if (strcmp((char*)data, "LButton") == 0) {
      LState = !LState;
    }
    if (strcmp((char*)data, "RButton") == 0) {
      RState = !RState;
    }
  }
}
*/
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  Serial.println("WS event");
  switch(type) {
    
    case WStype_TEXT:
      //Serial.printf("got data: %s\n", num, payload);
      if (strcmp((char*)payload, "StartButton") == 0) {
        if (gps.location.isValid() == true ) {
          
          start_lat = gps.location.lat();
          start_lng = gps.location.lng();
          previous_lat = gps.location.lat();
          previous_lng = gps.location.lng();
          Serial.println("start looginh");
        //webSocket.sendTXT(num, "start\n");
          start_logging = true;
          file_name.remove(0);
          file_name = time_day + time_month + "H" + time_hour + time_minute + ".csv";
          File dataFile = SD.open(file_name, FILE_WRITE);
          
          if (dataFile) {
            dataFile.println("type,latitude,longitude,alt,speed,time,tdist,sdist,color");
            dataFile.close();
          }
        } else {
          Serial.println("nope, not starting");
        }
        
      } else if (strcmp((char*)payload, "StopButton") == 0) {
        Serial.println("stop logging");
        //webSocket.sendTXT(num, "stop\n");
        start_logging = false;
      } else if (strcmp((char*)payload, "data") == 0) {
        Serial.println("received a request");

        String values = getSensorReadings();
        webSocket.sendTXT(num, values);
      } else if (strcmp((char*)payload, "ResetButton") == 0) {
        Serial.println("reset");
        //webSocket.sendTXT(num, "stop\n");
        max_speed = 0.0;
        total_distance = 0;
        distance_from_start = 0;
      }   
      break;
      case WStype_ERROR:
        Serial.println("error");
      break;
      case WStype_DISCONNECTED:
        Serial.println("Disconnected!\n");
      break;
      case WStype_CONNECTED:         //IPAddress ip = webSocket.remoteIP(num);
        Serial.println(" Connected ws");
    
        // send message to client
        //webSocket.sendTXT(num, "Connected\n");		
      break; 
  
  } 

}

void setup()  {

  Serial.begin(115200);
  ss.begin(4800);
  strip.begin();
  strip.setBrightness(25);
  strip.show(); // Initialize all pixels to 'off'
  WiFi.softAPdisconnect();
  Serial.println("booting!");
  Serial.println("checking sd card:");
  
  if (SD.begin(SDchipSelect)) {
  	FSInfo fs_info;
  	SDFS.info(fs_info);
 
  	Serial.print("Total bytes: ");
  	Serial.println(fs_info.totalBytes);
 
	  Serial.print("Used bytes: ");
  	Serial.println(fs_info.usedBytes);
 
  	Serial.print(F("\nSearch for firmware.."));
  	File firmware =  SD.open("/firmware.bin");
  	if (firmware) {
    	Serial.println(F("found!"));
      Serial.println(F("Try to update!"));
 
     // Update.onProgress(progressCallBack);
 
      Update.begin(firmware.size(), U_FLASH);
      Update.writeStream(firmware);
      if (Update.end())	{
        Serial.println(F("Update finished!"));
      } else {
        Serial.println(F("Update error!"));
        Serial.println(Update.getError());
      }

      firmware.close();
 
      if (SD.rename("/firmware.bin", "/firmware.bak")) {
        Serial.println(F("Firmware rename succesfully!"));
      } else {
        Serial.println(F("Firmware rename error!"));
      }
      delay(2000);

      ESP.reset();
 	  } else {
      Serial.println(F("not found!"));
  	} 
  } else {
    //Serial.println("SD card initialization done");
    //ESP.reset();
  }
//Serial.println("");
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.show();
  delay(500);
  strip.clear();
  strip.show();
  delay(1000);
  
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
 //WiFi.softAP(ssid, "", 10, false, 4);
  Serial.println(WiFi.softAP(ssid, "", 10, false, 4) ? "Ready" : "Failed!");
 // Serial.println(WiFi.softAP(ssid) ? "Ready" : "Failed!");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/term", handleTerm);
  //server.on("/max", max_val);
  //server.on("/current", current_val);
  //server.on("/reset", reset);
  //server.on("/start", start_log);
  //server.on("/stop", stop_log);
  server.on("/test", handleTest);
  server.onNotFound(handleRoot);

  dnsServer.start(53, "*", WiFi.softAPIP());
	webSocket.begin();
	webSocket.onEvent(webSocketEvent);
  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
 // ElegantOTA.onStart(onOTAStart);
  //ElegantOTA.onProgress(onOTAProgress);
 // ElegantOTA.onEnd(onOTAEnd);
  //ws.onEvent
  server.begin();
  //Serial.println("HTTP server started");

  //Serial.println("websocket started");
/*
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  delay(1000);
*/
}

void loop() {
	unsigned long curr_ms = millis();
  if (ss.available()) {//while data is available
    if (gps.encode(ss.read()))  {
		sat = gps.satellites.value();
//		Serial.println("gps");
      if (gps.location.isValid()) {
		/*if (run_term == true) {
			term.println("gps");
		}*/
        latitude = String(gps.location.lat(), 6);
        longtitude = String(gps.location.lng(), 6);
        altitude = String(gps.altitude.meters(), 3);

        time_year = String(gps.date.year());
        time_month = String(gps.date.month());
        time_day = String(gps.date.day());

        time_hour = String(gps.time.hour());
        time_minute = String(gps.time.minute());
        time_second = String(gps.time.second());
        
        current_speed = gps.speed.kmph();
        sat = gps.satellites.value();

        if (current_speed > max_speed) {

          max_speed = current_speed;
        }

        if (start_logging == true && previous_seconds != gps.time.second()) {
          logger_status.remove(0);
          logger_status = "logging";
        
          strip.setPixelColor(0, strip.Color(0, 255, 0));
          strip.show();
          File dataFile = SD.open(file_name, FILE_WRITE);
          
          if (dataFile) {
            dataFile.print("T,");
            dataFile.print(latitude);
            dataFile.print(",");
            dataFile.print(longtitude);
            dataFile.print(",");
            dataFile.print(altitude);
            dataFile.print(",");
            dataFile.print(current_speed);
            dataFile.print(",");
            dataFile.print(time_hour);
            dataFile.print(":");
            dataFile.print(time_minute);
            dataFile.print(":");
            dataFile.print(time_second);
            dataFile.print(",");
            dataFile.print(String(total_distance));
            dataFile.print(",");
            dataFile.print(String(distance_from_start));
            dataFile.println(",blue");
            dataFile.close();
          } else {
             logger_status = logger_status + " !SD";
          } 
          distance_from_start = gps.distanceBetween(start_lat, start_lng, gps.location.lat(), gps.location.lng());
          long temp_dist = gps.distanceBetween(previous_lat, previous_lng, gps.location.lat(), gps.location.lng());
          
          if (temp_dist >= 5 && temp_dist < 100) {
            total_distance = total_distance + temp_dist;
            temp_dist = 0;
            previous_lat = gps.location.lat();
            previous_lng = gps.location.lng();
          }
          previous_seconds = gps.time.second();
          
        } else if (start_logging == false) {
			      strip.setPixelColor(0, strip.Color(0, 0, 255));
			      strip.show();
            logger_status.remove(0);
            logger_status = "rdy";
		    }
	  
      } else {
        logger_status.remove(0);
            logger_status = "w8-4-fix";
      }

    } else {
    //term.println("No GPS available on software term port");
	// strip.setPixelColor(0, strip.Color(255, 0, 0));
	 //strip.show();
    } 
  
  } 

  dnsServer.processNextRequest();
  server.handleClient();
  if ((curr_ms - old_ms) > 10000) {
	  Serial.print("alive! ");
	  Serial.println(curr_ms);


	  old_ms = curr_ms;
  }
  ElegantOTA.loop();
  webSocket.loop();
} 
