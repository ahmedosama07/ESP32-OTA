#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "WiFiManager.h"


#define FORMAT 23
#define SPIFFS LITTLEFS
#define FORMAT_LITTLEFS_IF_FAILED true

long delay_time = 0;



WiFiManager routerAP;

void setup(void) {
  pinMode(FORMAT, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);

  if (!SPIFFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println("LittleFS mount failed");
    return;
  }

  String ssid = routerAP.get_credentials(SPIFFS, 0); 
  String pass = routerAP.get_credentials(SPIFFS, 1);
    //ssid = "null";
  if (ssid == "null") { // if the file does not exist ssid will be null
start_webserver:
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    WiFi.softAP("Fat7y Repeater Access Point");
    Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());
    routerAP.create_server(SPIFFS);
    
    routerAP.begin_server();
    Serial.println("HTTP server started");
    delay_time=1000; // blink every sec if webserver is active
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass); // check function to understand
    int timeout_counter=0;
    while (WiFi.status() != WL_CONNECTED) {
      if(timeout_counter>=120){
        goto start_webserver; // if it fails to connect start_webserver
      }

      Serial.print('.');
      timeout_counter++;
      digitalWrite(LED_BUILTIN,0);// leave led on when trying to connect
      delay(500);
    }



    Serial.printf("\nSTA: %s (dns: %s / %s)\n",
                  WiFi.localIP().toString().c_str(),
                  WiFi.dnsIP(0).toString().c_str(),
                  WiFi.dnsIP(1).toString().c_str());
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  routerAP.begin_server();
}

void loop(void) {
  if(digitalRead(FORMAT)==LOW){
    Serial.println("Resetting configuration");
    SPIFFS.format();
    ESP.restart();
  }
}