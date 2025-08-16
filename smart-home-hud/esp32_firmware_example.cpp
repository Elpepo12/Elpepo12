#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Tu_SSID";
const char* password = "Tu_PASSWORD";

WebServer server(80);
String authToken = "";

struct RoomStatus {
  float temp;
  float hum;
  float gas;
  float lum;
  bool light;
};

RoomStatus living  = {25.0, 40.0, 0,   0, false};
RoomStatus kitchen = {24.0, 0,   300,  0, false};
RoomStatus bedroom = {23.0, 0,   0, 200, false};

bool authorized() {
  if (!authToken.length()) return false;
  if (!server.hasHeader("Authorization")) return false;
  String token = server.header("Authorization");
  token.replace("Bearer ", "");
  return token == authToken;
}

void handleLogin() {
  if (server.method() != HTTP_POST) { server.send(405, ""); return; }
  if (!server.hasArg("plain")) { server.send(400, ""); return; }
  String body = server.arg("plain");
  if (body.indexOf("user":"admin")>=0 && body.indexOf("pass":"1234")>=0) {
    authToken = String(random(0xffff), HEX);
    server.send(200, "application/json", String("{\"token\":\"")+authToken+"\"}");
  } else {
    server.send(401, "text/plain", "Unauthorized");
  }
}

void handleStatus() {
  if (!authorized()) { server.send(401, ""); return; }
  String json = "{";
  json += "\"living\":{";
  json += "\"temp\":" + String(living.temp) + ",";
  json += "\"hum\":" + String(living.hum) + ",";
  json += "\"light\":" + String(living.light ? "true" : "false") + "},";
  json += "\"kitchen\":{";
  json += "\"temp\":" + String(kitchen.temp) + ",";
  json += "\"gas\":" + String(kitchen.gas) + ",";
  json += "\"light\":" + String(kitchen.light ? "true" : "false") + "},";
  json += "\"bedroom\":{";
  json += "\"temp\":" + String(bedroom.temp) + ",";
  json += "\"lum\":" + String(bedroom.lum) + ",";
  json += "\"light\":" + String(bedroom.light ? "true" : "false") + "}";
  json += "}";
  server.send(200, "application/json", json);
}

void toggleLight(RoomStatus& room) {
  if (!authorized()) { server.send(401, ""); return; }
  room.light = !room.light;
  server.send(200, "text/plain", room.light ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado");

  server.on("/login", handleLogin);
  server.on("/status", handleStatus);
  server.on("/toggle-living-light", [](){ toggleLight(living); });
  server.on("/toggle-kitchen-light", [](){ toggleLight(kitchen); });
  server.on("/toggle-bedroom-light", [](){ toggleLight(bedroom); });
  server.begin();
}

void loop() {
  server.handleClient();
}
