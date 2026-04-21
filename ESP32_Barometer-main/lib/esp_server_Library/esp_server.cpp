#include "esp_server.h"
#include <set>

std::set<String> client_IPs;

ESPServer::ESPServer(WebServer &server, HP206C &sensor)
    : _server(server), _sensor(sensor)
{
}

void ESPServer::begin()
{
    // 设置路由处理函数
    _server.on("/", HTTP_GET, [this]()
               { handleRoot(); });
    _server.on("/clientip", HTTP_POST, [this]()
               { handleClientIP(); });
    _server.on("/synctime", HTTP_POST, [this]()
               { handleSyncTime(); });
    _server.on("/localpressure", HTTP_POST, [this]()
               { handleLocalPressure(); });

    // 设置404处理
    _server.onNotFound([this]()
                       { _server.send(404, "text/plain", "Not Found"); });

    _server.begin();
}

void ESPServer::handleRoot()
{
    JsonDocument doc;
    doc["status"] = "success";
    doc["message"] = "Welcome to ESP32 Web Server!";
    doc["timestamp"] = millis();
    doc["ip"] = WiFi.localIP().toString();
    doc["mac"] = WiFi.macAddress();
    doc["rssi"] = WiFi.RSSI();
    doc["heap"] = ESP.getFreeHeap();

    String response;
    serializeJson(doc, response);
    _server.send(200, "application/json", response);
}

void ESPServer::handleClientIP()
{
    String clientIP = _server.client().remoteIP().toString();
    if (client_IPs.find(clientIP) == client_IPs.end())
    {
        client_IPs.insert(clientIP);
    }
    JsonDocument doc;
    doc["status"] = "success";
    doc["your_client_ip"] = clientIP;
    String response;
    serializeJson(doc, response);
    _server.send(200, "application/json", response);
}

void ESPServer::handleSyncTime()
{
    if (_server.method() == HTTP_POST)
    {
        // 处理同步时间的逻辑
        JsonDocument doc;
        doc["status"] = "success";
        doc["message"] = "Time synchronized";
        String response;
        serializeJson(doc, response);
        _server.send(200, "application/json", response);
    }
}

void ESPServer::handleLocalPressure()
{
    if (_server.method() == HTTP_POST)
    {
    }
}

void ESPServer::handleClient()
{
    _server.handleClient();
}
