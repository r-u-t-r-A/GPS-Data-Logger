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

#include "WiFiTerm.h"

#include "index.h"

#include "led_def.h"

#define PixelCount 1// this example assumes 4 pixels, making it smaller will cause a failure
#define LED_PIN 15  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128

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

int led_mode;
bool led_refreshed = false;

float max_speed = 0.0;
float current_speed = 0.0;
int sat = 0;
bool start_logging = false;
bool logging_running = false;
bool run_term = false;
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

ESP8266WebServer server(80);
DNSServer dnsServer;

IPAddress local_IP(10,0,0,1);
IPAddress gateway(10,0,0,1);
IPAddress subnet(255,255,255,0);

unsigned long ota_progress_millis = 0;

void updateLED(int mode) {
	switch (mode) {
		case RED:
			strip.setPixelColor(0, strip.Color(255, 0, 0));
		break;
		case GREEN:
			strip.setPixelColor(0, strip.Color(0, 255, 0));
		break;
		case BLUE:
			strip.setPixelColor(0, strip.Color(0, 0, 255));
		break;
		case PULSE:
			strip.rainbow(0);
		break;
	}
	strip.show();
}

void handleRoot() {

  databuffer = MAIN_page;

  //databuffer = String("<html><head><meta content=\"width=device-width\"><style> body{font-size: 56px; text-align:center; background-color:#11d0ed;}</style> <h1>GPS LOGGER<br> Max velocity: <a href=""/max"">max speed</a> <br>Current velocity: <a href=""/current"">current speed</a><br>Reset Data: <a href=""/reset"">reset</a><br>Start: <a href=""/start"">start</a><br>Stop log: <a href=""/stop"">stop</a><br>terminal: <a href=""/term.html"">terminal</a><br>update: <a href=""/update"">UPDATE</a></h1> <body>""</body></html>");
  server.send(200, "text/html", databuffer);
  databuffer.remove(0);
}

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
  ESP.reset();
  
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
/*
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
  server.on("/max", max_val);
  server.on("/current", current_val);
  server.on("/reset", reset);
  server.on("/start", start_log);
  server.on("/stop", stop_log);
  server.onNotFound(handleRoot);

  dnsServer.start(53, "*", WiFi.softAPIP());

  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
 // ElegantOTA.onStart(onOTAStart);
  //ElegantOTA.onProgress(onOTAProgress);
 // ElegantOTA.onEnd(onOTAEnd);
  
  server.begin();
  //Serial.println("HTTP server started");

  term.begin(server);
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
				
				//sat = gps.satellites.value();
				
				if (run_term == true) {
					term.print(sat);
					term.print("  ");
					term.print(latitude);
					term.print("  ");
					term.print(longtitude);
					term.print("  ");
					term.print(time_day);
					term.print("-");
					term.print(time_month);
					term.print("-");
					term.print(time_year);
					term.print("  ");
					term.print(time_hour);
					term.print("-");
					term.print(time_minute);
					term.print("-");
					term.println(time_second);
				} 
				
				if (current_speed > max_speed) {

					max_speed = current_speed;
				}

				if (start_logging == true) {
				
					strip.setPixelColor(0, strip.Color(0, 255, 0));
					strip.show();
					File dataFile = SD.open(file_name, FILE_WRITE);
						
					if (run_term == true) {
						term.print("writing to:"); term.println(file_name);
					}
				
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
						dataFile.println(",blue");
						dataFile.close();
					} else {
						/* if (run_term == true) {
						term.println("Can't save data");
						}*/
					} 
				
				} else {
				strip.setPixelColor(0, strip.Color(0, 0, 255));
				strip.show();
				
				}
		
     		}

    	} else {
    	//term.println("No GPS available on software term port");
	 strip.setPixelColor(0, strip.Color(255, 0, 0));
	 strip.show();
  		}	
  
	} 
  	if (term.available()) {
		char read = term.read();
		switch (read) {
			case 'l':
				run_term = true;
				term.println("start log");
				break;
			
			case 's':
				run_term = false;
				term.println("stop log");
				break;
		}
  	}
  
  	if ((curr_ms - old_ms) > 10000) {
	
		Serial.print("alive! ");
		Serial.println(curr_ms);

		if (run_term == true) {
			term.print("alive! ");
			term.println(curr_ms);
		}

		old_ms = curr_ms;
  	}
  
  	ElegantOTA.loop();
  	term.handleClient();
  	dnsServer.processNextRequest();
  	server.handleClient();

  //	updateLED(led_mode);
	//led_refreshed = false;
} 
