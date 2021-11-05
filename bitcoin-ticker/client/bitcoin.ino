#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <Segment7Display.h> // library source unknown
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#define DIN D8  // Data pin
#define CLK D7 // Clock pin
#define CS D6  // Strobe pin

Segment7Display display(DIN, CLK, CS);
const IPAddress apIP(192, 168, 1, 1);
const char* apSSID = "TICKERBOXX";
boolean settingMode;
String ssidList;

DNSServer dnsServer;
ESP8266WebServer webServer(80);

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  display.reset();
  delay(10);
  if (restoreConfig()) {
    if (checkConnection()) {
      WiFi.softAPdisconnect(true);
      settingMode = false;
      startWebServer();
      display.reset();
      return;
    }
  }
  settingMode = true;
  setupMode();
}

int period = 10000;
bool firstRequest = true;
unsigned long time_now = 0;
String currency = "USD";

void loop() {
  if (settingMode) {
    dnsServer.processNextRequest();
  }
  webServer.handleClient();
  if(millis() > time_now + period or firstRequest == true){
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      time_now = millis();
      firstRequest = false;
      WiFiClient wifiClient;
      HTTPClient http;  //Declare an object of class HTTPClient
      http.begin(wifiClient, "https://tickerboxx-api.herokuapp.com/api/v1/btc");  //Specify request destination
      int httpCode = http.GET();
      if (EEPROM.read(96) != 0) {
        currency = "";
        for (int i = 96; i < 99; ++i) {
          currency += char(EEPROM.read(i));
        }
        Serial.println("currency from eprom: ");
        Serial.println(currency);
      }
   
      if (httpCode > 0) { //Check the returning code
   
        const size_t bufferSize = JSON_OBJECT_SIZE(1) + 30;
        DynamicJsonBuffer jsonBuffer(bufferSize);
        JsonObject& root = jsonBuffer.parseObject(http.getString());
        float BTCUSD = root["BTCUSD"];
        float BTCGBP = root["BTCGBP"];
        float BTCEUR = root["BTCEUR"];
        float price = BTCUSD;
        if (currency == "GBP")
        {
          price = BTCGBP;
        }
        if (currency == "EUR")
        {
          price = BTCEUR;
        }
        Serial.println(currency);        
        Serial.println(price);
        display.setBrightness(0);
        display.printFloat(price,0);
      }
      http.end();   //Close connection
    }
  }
}

boolean restoreConfig() {
  Serial.println("Reading EEPROM...");
  String ssid = "";
  String pass = "";
  if (EEPROM.read(0) != 0) {
    for (int i = 0; i < 32; ++i) {
      ssid += char(EEPROM.read(i));
    }
    Serial.print("SSID: ");
    Serial.println(ssid);
    for (int i = 32; i < 96; ++i) {
      pass += char(EEPROM.read(i));
    }
    Serial.print("Password: ");
    Serial.println(pass);
    WiFi.begin(ssid.c_str(), pass.c_str());
    return true;
  }
  else {
    Serial.println("Config not found.");
    return false;
  }
}

boolean checkConnection() {
  int count = 0;
  Serial.print("Waiting for Wi-Fi connection");
  while ( count < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Connected!");
      return (true);
    }
    delay(500);
    Serial.print(".");
    count++;
  }
  Serial.println("Timed out.");
  return false;
}

void startWebServer() {
  if (settingMode) {
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.softAPIP());
    webServer.on("/settings", []() {
      String s = "<h1 style=\"color:#40414B;font-size:40px;font-family:Verdana;\">WiFi Settings</h1><p>Please select your WiFi SSID and enter the password.</p>";
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += ssidList;
      s += "</select><br>Password: <input name=\"pass\" length=64 type=\"password\"><input type=\"submit\"></form>";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
    });
    webServer.on("/setap", []() {
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
      }
      String ssid = urlDecode(webServer.arg("ssid"));
      Serial.print("SSID: ");
      Serial.println(ssid);
      String pass = urlDecode(webServer.arg("pass"));
      Serial.print("Password: ");
      Serial.println(pass);
      Serial.println("Writing SSID to EEPROM...");
      for (int i = 0; i < ssid.length(); ++i) {
        EEPROM.write(i, ssid[i]);
      }
      Serial.println("Writing Password to EEPROM...");
      for (int i = 0; i < pass.length(); ++i) {
        EEPROM.write(32 + i, pass[i]);
      }
      EEPROM.commit();
      Serial.println("Write EEPROM done!");
      String s = "<h1>Setup complete.</h1><p>device will be connected to \"";
      s += ssid;
      s += "\" after the restart.";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
      ESP.restart();
    });
    webServer.onNotFound([]() {
      String s = "<h1 style=\"color:#40414B;font-size:40px;font-family:Verdana;\">AP mode</h1><p><a href=\"/settings\">Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage("AP mode", s));
    });
  }
  else {
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.localIP());
    if (!MDNS.begin("TICKERBOXX")) {             // Start the mDNS responder for TICKERBOXX.local
    Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");
    webServer.on("/", []() {
      String s = "<h1 style=\"color:#40414B;font-size:40px;font-family:Verdana;\"> TICKERBOXX</h1>";
      s += "<p style=\"color:#40414B;font-size:20px;font-family:Verdana;\">Select display currency.</p>";
      s += "<form method=\"get\" action=\"/\"><select name=\"currency\">";
      s += "<option value=\"USD\">USD</option><option value=\"GBP\">GBP</option><option value=\"EUR\">EUR</option>";
      s += "</select><input type=\"submit\"></form><p style=\"font-size:20px;font-family:Verdana;\"><a href=\"/reset\">Reset Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage("TICKERBOXX", s));
      String currency = urlDecode(webServer.arg("currency"));
      Serial.print("set currency to : ");
      Serial.println(currency);
      Serial.println("Writing Currency to EEPROM...");
      for (int i = 0; i < currency.length(); ++i) {
        EEPROM.write(96 + i, currency[i]);
      }
      EEPROM.commit();
      firstRequest = true;
    });
    webServer.on("/reset", []() {
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
      String s = "<h1>Wi-Fi settings reset.</h1><p>Please disconnect USB and reconnect to restart device in AP mode. Then connect to \"TICKERBOXX\" SSID</p>";
      webServer.send(200, "text/html", makePage("Reset Wi-Fi Settings", s));
    });
  }
  webServer.begin();
}

void setupMode() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  delay(100);
  Serial.println("");
  for (int i = 0; i < n; ++i) {
    ssidList += "<option value=\"";
    ssidList += WiFi.SSID(i);
    ssidList += "\">";
    ssidList += WiFi.SSID(i);
    ssidList += "</option>";
  }
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apSSID);
  dnsServer.start(53, "*", apIP);
  startWebServer();
  Serial.print("Starting Access Point at \"");
  Serial.print(apSSID);
  Serial.println("\"");
  display.printString("AP");
}

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
  s += "<title>";
  s += title;
  s += "</title></head><body>";
  s += contents;
  s += "</body></html>";
  return s;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}
