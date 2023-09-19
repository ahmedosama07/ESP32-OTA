#include "WiFiManager.h"


AsyncWebServer server(80);
DynamicJsonDocument Config(2048);
JsonObject obj = Config.as<JsonObject>();

    void WiFiManager::begin_server(){
     server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "يخربيت كووووووبره");
      });

      AsyncElegantOTA.begin(&server);    // Start ElegantOTA
      server.begin();
      Serial.println("HTTP server started");
    }
    void WiFiManager::listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
      Serial.printf("Listing directory: %s\n", dirname);

      File root = fs.open(dirname);

      File file;

      do {
        file = root.openNextFile("r");
        Serial.print("  FILE: ");
        Serial.print(root.name());
        Serial.print("  SIZE: ");
        Serial.print(file.size());
        file.close();

      } while (file);
    }

    void WiFiManager::create_server(fs::FS &fs) {

      server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {

        // scan for networks and get ssid
        String network_html = "";

        int n = WiFi.scanComplete();
        if (n == -2) {
          WiFi.scanNetworks(true);
        } else if (n) {
          for (int i = 0; i < n; ++i) {
            String router = WiFi.SSID(i);
            Serial.println(router);
            network_html += "<input type=\"radio\" id=\"#radiobuttonex\" name=\"ssid\" value=" + router + " required ><label for=\"html\">" + router + "</label><<br>";

          }
          WiFi.scanDelete();
          if (WiFi.scanComplete() == -2) {
            WiFi.scanNetworks(true);
          }
        }

        String html = "<!DOCTYPE html><html>";
        html+= "<head>";
        html+=" <link rel=\"stylesheet\" href=\"styles.css\">";
        html+= "</head>";
        html += "<body>";
        html+= "<div>";
        html += "<h1>Fat7y Configuration page</h1>";
        html += "<button onclick=\"window.location.href='/';\">Scan </button>";
        html += "<p>networks found </p>";
        html += "<form action=\"/credentials\">";
        html += "<p>Please select a WiFi network:</p>" + network_html;
        html += "<input type=\"password\" id=\"pass\" name=\"pass\" value=\"\" required ><label for=\"pass\">password</label><br><br>";
        html += "<input type=\"submit\" value=\"Submit\">";
        html += "</form></body></html>";
        html+= "</div>";

        request->send(200, "text/html", html);
      });

      // css style from grepper
      server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
        String style =  "#radiobuttonex{\n"
                        "  border: 2px solid rgb(255,1,199);\n"
                        "  text-align: center;\n"
                        "  font-family: sans serif;\n"
                        "  width: 305px;\n"
                        "  background: rgb(50,50,100);\n"
                        "  border-radius: 40px;\n"
                        "}";
        
       request->send(200, "text/css", style);
        
        });

      // Send a GET request to <IP>/get?message=<message> 
      server.on("/credentials", HTTP_GET, [](AsyncWebServerRequest * request) {
        String param = "ssid";

        if (request->hasParam(param)) {
          String ssid = request->getParam(param)->value();
          Config["ssid"] = ssid;
          Serial.println(ssid);
        } else {
          Serial.println("No " + param + " sent");
        }

        param = "pass";
        if (request->hasParam(param)) {
          String pass = request->getParam(param)->value();
          Config["pass"] = pass;
          Serial.println(pass);
        } else {
          Serial.println("No " + param + " sent");
        }

        String output;
        serializeJson(Config, output);
        Serial.print(output);

        String path = "/config.json";

        Serial.printf("Writing file: %s\n", path);

        File file = LITTLEFS.open(path, FILE_WRITE);
        if (!file) {
          Serial.println("Failed to open file for writing");
          return "null";
        }
        if (file.print(output)) {
          Serial.println("File written");
          ESP.restart();
        } else {
          Serial.println("Write failed");
        }
        file.close();

        return output.c_str();
      });

    }

    String WiFiManager::get_credentials(fs::FS &fs, int a) {
      // a: 0=ssid, 1=pass;
      String path = "/config.json";
      String credentials = "";

      Serial.print("reading file ");
      Serial.println(path);

      File file = fs.open(path);
      if (!file) {
        Serial.println("Failed to open file for reading");
        Serial.println("this is probally first usage, so the file does not exist");
        return "null";
      }

      Serial.print("Read from file: ");
      while (file.available()) {
        credentials += file.readString();

      }
      Serial.println(credentials);
      deserializeJson(Config, credentials);
      file.close();
      String credential_array [2] = {Config["ssid"], Config["pass"]};
      return credential_array[a];
    }