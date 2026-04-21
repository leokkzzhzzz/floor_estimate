#ifndef ESP_SERVER_H
#define ESP_SERVER_H

#include <WebServer.h>
#include <ArduinoJson.h>
#include "HP206C.h"

class ESPServer {
public:
    ESPServer(WebServer& server, HP206C& sensor);
    void begin();
    void handleClient();

private:
    WebServer& _server;
    HP206C& _sensor;
    
    void handleRoot();
    void handleClientIP();
    void handleSyncTime();
    void handleLocalPressure();
};

#endif