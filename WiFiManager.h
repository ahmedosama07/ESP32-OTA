#ifndef WM_H
#define WM_H

#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LITTLEFS.h>
#include <AsyncElegantOTA.h>


#if (ARDDUINO >=100)
  #include "Arduino.h"
#endif

class WiFiManager {
  public:
  
  void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
  void begin_server();
  void create_server(fs::FS &fs);
  String get_credentials(fs::FS &fs, int a);
};

#endif