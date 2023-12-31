#include "WiFiManager.h"

#define FORMAT 23
#define FORMAT_LITTLEFS_IF_FAILED true

WiFiManager wm;

void setup(void) {
  pinMode(FORMAT, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);

  wm.setDebugOutput(3);
  

  if (!wm.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(wm.getSSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  wm.setConfigPortalBlocking(false);
  wm.startWebPortal();
}

void loop(void) {
  wm.process();
  if(digitalRead(FORMAT)==LOW){
    Serial.println("Resetting configuration");
    wm.resetSettings();
    ESP.restart();
  } 
}