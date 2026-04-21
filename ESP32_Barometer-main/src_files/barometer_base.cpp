#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include <sys/time.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <set>
#include <map>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "wifi_client.h"
#include "HP206C.h"
#include "esp_server.h"
#include "config.h"

#define DATA_COLLECTION_INTERVAL 500            // 500 milliseconds
#define TIME_SYNC_INTERVAL (60 * 60 * 2 * 1000) // 2 hour in milliseconds
#define WIFI_RECONNECT_INTERVAL (20 * 1000)     // 20 seconds in milliseconds

// define onboard LED pin
constexpr int kOnboardLedPin = 2;
// Led blink loop function
inline void blinkLoop(int delay_ms) {
    while (true) {
        digitalWrite(kOnboardLedPin, !digitalRead(kOnboardLedPin));
        delay(delay_ms);
    }
}

// NTP Configuration (Keep existing ones)
const char *ntpServer = "cn.pool.ntp.org";
const long gmtOffset_sec = 8 * 3600;
const int daylightOffset_sec = 0;

// Structure to hold data to be sent
typedef struct
{
    unsigned long long timestamp_ms;
    float pressure_hpa;
    float temperature_c;
} SensorDataPayload;

// FreeRTOS Queue handle
QueueHandle_t dataQueue;
#define QUEUE_LENGTH 20 // How many data points to buffer if network is slow
#define QUEUE_ITEM_SIZE sizeof(SensorDataPayload) // Size of each item in the queue

HP206C barometer;

String deviceMac;          // Initialized in setup
eduroam_auth eduroamAuth = {
    .ssid = EDUROAM_SSID,
    .identity = EDUROAM_IDENTITY,
    .password = EDUROAM_PASSWORD
};
psk_auth pskAuth = {
    .ssid = SSID_IOT,
    .password = SSID_IOT_PASSWORD
};
WifiClient wifiClient(pskAuth, eduroamAuth); // Initialize WifiClient with PSK and Eduroam credentials
WifiClient::WifiStatus wifi_status;

// Web Server and ESPServer
WebServer server(80);
ESPServer espServer(server, barometer);

// for time synchronization
unsigned long lastDataCollectionTime = 0;
// for WiFi reconnection attempts
unsigned long lastTimeSync = 0;
// for WiFi reconnection attempts
unsigned long lastWifiReconnectAttempt = 0;
// Flag to indicate if time is synchronized
bool timeIsSynchronized = false;

// --- Function Declarations ---
void collectAndSendData();          // Now sends data to queue
void httpSendTask(void *parameter); // The asynchronous sending task
bool requestTimeSync();             // Returns true if time sync was successful
unsigned long long getUnixMillis(); // Returns current Unix time in milliseconds
bool connectWiFi();                 // Connects to WiFi and updates wifi_status.isConnected
void sendMACAddress();              // Sends the device MAC address to the client

// Global set to track connected client IPs
extern std::set<String> client_IPs;

void setup()
{
    Serial.begin(115200);
    Wire.begin(BARO_I2C_SDA_PIN, BARO_I2C_SCL_PIN);
    Serial.printf("INFO: I2C init SDA=%d SCL=%d\n", BARO_I2C_SDA_PIN, BARO_I2C_SCL_PIN);
    delay(1000); // Allow components to stabilize

    // Initialize WiFi and get MAC Address EARLY
    WiFi.mode(WIFI_STA);
    deviceMac = WiFi.macAddress();

    while (!connectWiFi())
    {
        blinkLoop(1000);
    }

    espServer.begin();
    Serial.println("INFO: ESPServer started.");
    
    // Initial Time Sync
    requestTimeSync();

    while (!timeIsSynchronized)
    {
        Serial.println("WARNING: Initial time sync failed. Will retry later.");
        blinkLoop(1000);
    }

    // Initialize barometer
    while (!barometer.begin())
    {
        Serial.println("ERROR: Sensor initialization failed. Please check wiring.");
        blinkLoop(1000);
    }
    // Create FreeRTOS Queue
    dataQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    while (dataQueue == NULL)
    {
        Serial.println("CRITICAL: Failed to create data queue! Halting.");
        blinkLoop(1000);
    }
    Serial.println("Data queue created successfully.");

    // Create FreeRTOS Task for Sending Data
    // Run task on Core 1 (Core 0 is often used by WiFi/BT)
    // Stack size might need adjustment based on complexity (e.g., large JSON buffers)
    TaskHandle_t httpSendTaskHandle = NULL;
    BaseType_t taskCreated = xTaskCreatePinnedToCore(
        httpSendTask,        /* Function to implement the task */
        "HTTPSendTask",      /* Name of the task */
        8192,                /* Stack size in words */
        NULL,                /* Task input parameter */
        1,                   /* Priority of the task */
        &httpSendTaskHandle, /* Task handle. */
        1);                  /* Core where the task should run */

    while (taskCreated != pdPASS)
    {
        Serial.println("CRITICAL: Failed to create HTTP Send Task! Halting.");
        blinkLoop(1000);
    }
    Serial.println("HTTP Send Task created successfully on Core 1.");
}

void loop()
{
    espServer.handleClient();
    unsigned long currentMillis = millis();
    while (!wifi_status.isConnected)
    {
        // Try to reconnect periodically
        if (currentMillis - lastWifiReconnectAttempt >= WIFI_RECONNECT_INTERVAL)
        {
            Serial.println("WiFi disconnected. Attempting reconnect...");
            connectWiFi();                            // Attempt to reconnect
            lastWifiReconnectAttempt = currentMillis; // Update timestamp regardless of success
        }
        blinkLoop(1000); // Blink LED to indicate reconnection attempt
    }

    // Time Sync Check
    if ((!timeIsSynchronized) || (currentMillis - lastTimeSync >= TIME_SYNC_INTERVAL))
    {
        requestTimeSync();
    }

    // Data Collection and Queueing
    if (currentMillis - lastDataCollectionTime >= DATA_COLLECTION_INTERVAL)
    {
        lastDataCollectionTime = currentMillis;
        collectAndSendData();
    }
    // Small delay to allow other tasks (like the HTTP send task) to run
    // Especially important if loop() becomes very busy or DATA_COLLECTION_INTERVAL is small
    vTaskDelay(pdMS_TO_TICKS(100));
} //End of loop()

// Collect Data and Send to Queue
void collectAndSendData()
{
    float temperature, pressure;
    // Use the new readAll method which handles measurement start and data reading
    if (!barometer.readAll(&temperature, &pressure))
    {
        Serial.println("ERROR: Failed to read sensor data!");
        return;
    }
    // Check for valid readings
    if (isnan(temperature) || isnan(pressure))
    {
        Serial.println("ERROR: Invalid sensor readings!");
        return;
    }
    // Get current timestamp from system time
    unsigned long long now = getUnixMillis();
    
    SensorDataPayload payload;
    payload.timestamp_ms = now;
    payload.pressure_hpa = pressure;
    payload.temperature_c = temperature;

    // Use xQueueSend with a small timeout (e.g., 10ms) to prevent blocking if the queue is full
    BaseType_t result = xQueueSend(dataQueue, &payload, pdMS_TO_TICKS(10));

    if (result != pdPASS)
    {
        Serial.println("ERROR: Failed to queue data! Queue might be full.");
    }
}

// FreeRTOS Task for Asynchronous HTTP Sending
void httpSendTask(void *parameter)
{
    SensorDataPayload receivedPayload;
    // 为每个客户端IP创建并维护HTTPClient实例
    std::map<String, HTTPClient*> httpClients;
    // 添加失败计数器
    std::map<String, int> failureCount;
    const int MAX_FAILURE_COUNT = 3; // 连续失败3次后移除客户端
    
    // Create a standard WiFiClient for HTTP requests
    WiFiClient standardClient;
    Serial.println("httpSendTask started.");

    for (;;)
    {
        // Wait indefinitely for an item to arrive in the queue
        if (xQueueReceive(dataQueue, &receivedPayload, portMAX_DELAY) == pdPASS)
        {
            
            // Check WiFi connection BEFORE attempting POST
            if (!wifi_status.isConnected)
            {
                Serial.println("httpSendTask: WiFi disconnected, skipping send.");
                // Data is lost here. Could implement temporary local storage if needed.
                continue; // Go back to waiting for the next queue item
            }
            
            JsonDocument doc;

            // 添加数据到JSON文档
            doc["timestamp_ms"] = receivedPayload.timestamp_ms;
            doc["pressure_hpa"] = receivedPayload.pressure_hpa;
            doc["temperature_c"] = receivedPayload.temperature_c;

            String jsonData;
            serializeJson(doc, jsonData);
            Serial.println("httpSendTask: Sending data payload: " + jsonData);

            // 检查并更新客户端连接
            // 移除已断开的客户端
            for (auto it = httpClients.begin(); it != httpClients.end();)
            {
                if (client_IPs.find(it->first) == client_IPs.end())
                {
                    // 客户端已断开，清理资源
                    it->second->end();
                    delete it->second;
                    it = httpClients.erase(it);
                    Serial.println("httpSendTask: Removed disconnected client: " + it->first);
                }
                else
                {
                    ++it;
                }
            }

            // 为新客户端创建连接
            for (const String& clientIP : client_IPs)
            {
                if (httpClients.find(clientIP) == httpClients.end())
                {
                    // 创建新的客户端连接
                    HTTPClient* newHttpClient = new HTTPClient();
                    
                    String url = "http://" + clientIP + ":8080/data";

                    if (newHttpClient->begin(standardClient, url))
                    {
                        newHttpClient->setReuse(true); // 启用连接重用
                        newHttpClient->setTimeout(5000);
                        newHttpClient->addHeader("Content-Type", "application/json");
                        newHttpClient->addHeader("Connection", "keep-alive");
                        newHttpClient->addHeader("Device-Mac", deviceMac);
                        
                        httpClients[clientIP] = newHttpClient;
                        Serial.println("httpSendTask: Created persistent connection for client: " + clientIP);
                    }
                    else
                    {
                        delete newHttpClient;
                        Serial.println("httpSendTask: Failed to create connection for client: " + clientIP);
                    }
                }
            }

            // 向所有连接的客户端发送数据
            for (auto clientIt = client_IPs.begin(); clientIt != client_IPs.end();)
            {
                String clientIP = *clientIt;
                auto httpIt = httpClients.find(clientIP);
                if (httpIt != httpClients.end())
                {
                    HTTPClient* http = httpIt->second;
                    unsigned long postStartTime = millis();

                    Serial.println("httpSendTask: Sending to client: " + clientIP);

                    int httpResponseCode = http->POST(jsonData);

                    if (httpResponseCode > 0)
                    {
                        String response = http->getString();
                        unsigned long duration = millis() - postStartTime;
                        Serial.printf("httpSendTask: POST to %s response code: %d, Body: %s, Duration: %lu ms\n", 
                                    clientIP.c_str(), httpResponseCode, response.c_str(), duration);

                        if (httpResponseCode == HTTP_CODE_OK || httpResponseCode == 201)
                        {
                            Serial.println("httpSendTask: Data sent successfully to " + clientIP + " (Code " + String(httpResponseCode) + ")");
                            // 成功发送，重置失败计数
                            failureCount[clientIP] = 0;
                            ++clientIt;
                        }
                        else
                        {
                            Serial.println("httpSendTask: Server error from " + clientIP + "! Code: " + String(httpResponseCode) + ", Response: " + response);
                            // 增加失败计数
                            failureCount[clientIP]++;
                            ++clientIt;
                        }
                    }
                    else
                    {
                        unsigned long duration = millis() - postStartTime;
                        Serial.println("httpSendTask: POST to " + clientIP + " failed, error: " + http->errorToString(httpResponseCode) + ", Duration: " + String(duration) + " ms");
                        
                        // 连接失败，删除HTTP连接
                        http->end();
                        delete http;
                        httpClients.erase(clientIP);
                        
                        // 增加失败计数
                        failureCount[clientIP]++;
                        
                        // 检查是否达到最大失败次数
                        if (failureCount[clientIP] >= MAX_FAILURE_COUNT)
                        {
                            Serial.println("httpSendTask: Client " + clientIP + " failed " + String(MAX_FAILURE_COUNT) + " times, removing from registry");
                            failureCount.erase(clientIP);
                            clientIt = client_IPs.erase(clientIt);
                        }
                        else
                        {
                            Serial.println("httpSendTask: Client " + clientIP + " failure count: " + String(failureCount[clientIP]) + "/" + String(MAX_FAILURE_COUNT));
                            ++clientIt;
                        }
                    }
                }
                else
                {
                    ++clientIt;
                }
            }

        } // end if xQueueReceive

        // Small delay to prevent task from hogging CPU if queue becomes empty/full rapidly
        vTaskDelay(pdMS_TO_TICKS(50));

    } // end for(;;) task loop
}


bool connectWiFi()
{
    Serial.println("Attempting WiFi connection...");
    if (wifiClient.connectAuto())
    {
        Serial.println("INFO: WiFi connected. IP: " + wifiClient.getWifiStatus().ipAddress);
        lastWifiReconnectAttempt = millis();
        // Attempt time sync immediately after connecting
        requestTimeSync();
        wifi_status = wifiClient.getWifiStatus();
        return true;
    }
    else
    {
        Serial.println("ERROR: WiFi connection failed. Error: " + wifiClient.getLastError());
    }
    return false;
}

// Get Unix Milliseconds
unsigned long long getUnixMillis()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
}

// Request Time Sync
bool requestTimeSync()
{   
    if (!wifi_status.isConnected)
    {
        Serial.println("WARN: Cannot sync time, WiFi not connected.");
        timeIsSynchronized = false; // Ensure flag is false
        return false;
    }
    Serial.println("INFO: Requesting time sync from NTP server: " + String(ntpServer));

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    int retry = 0;
    const int maxRetries = 15;
    while (!getLocalTime(&timeinfo, 5000))
    { // Use timeout
        Serial.print(".");
        delay(500);
        retry++;
        if (retry >= maxRetries)
            break;
    }
    Serial.println();
    if (retry < maxRetries && timeinfo.tm_year > (2016 - 1900))
    {
        timeIsSynchronized = true;
        lastTimeSync = millis();
        char timeStr[64];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
        Serial.println("INFO: Time synchronized: " + String(timeStr));
        return true;
    }
    else
    {
        timeIsSynchronized = false;
        Serial.println("ERROR: Failed to obtain valid time from NTP.");
        return false;
    }
}

