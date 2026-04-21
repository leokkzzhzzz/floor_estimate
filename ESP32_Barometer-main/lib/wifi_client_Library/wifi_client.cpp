#include "wifi_client.h"

WifiClient::WifiClient(const psk_auth& psk, const eduroam_auth& eduroam)
    : _eduroamAuth(eduroam), _pskAuth(psk), _isConnected(false)
{
    // Initialize WifiStatus
    _wifiStatus.isConnected = false;
    _wifiStatus.signalStrength = 0;
    _wifiStatus.ipAddress = "";
    _wifiStatus.lastUpdateTime = 0;
}

// Helper function for basic WiFi setup
void WifiClient::setupWiFiBasics()
{
    WiFi.disconnect(true); // Disconnect previous connection and clear config
    delay(100);
    WiFi.mode(WIFI_STA);
}

// Connect using WPA2-Enterprise (Eduroam) - Uses internal _eduroamAuth
bool WifiClient::connectEduroam(uint32_t timeout_ms)
{
    // Check if internal credentials are set (basic check)
    if (!_eduroamAuth.ssid || !_eduroamAuth.identity || !_eduroamAuth.password ||
        strlen(_eduroamAuth.ssid) == 0 || strlen(_eduroamAuth.identity) == 0 || strlen(_eduroamAuth.password) == 0)
    {
        _lastError = "Internal Enterprise SSID, Identity, or Password not set/empty";
        _isConnected = false;
        return false;
    }

    Serial.println("Attempting Eduroam connection...");
    setupWiFiBasics(); // Basic WiFi setup

    // configure WPA2 Enterprise using internal struct members
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)_eduroamAuth.identity, strlen(_eduroamAuth.identity));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)_eduroamAuth.identity, strlen(_eduroamAuth.identity)); // Often same as identity
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)_eduroamAuth.password, strlen(_eduroamAuth.password));
    esp_wifi_sta_wpa2_ent_enable(); // Enable Enterprise mode

    // 开始连接 (SSID only for Enterprise)
    WiFi.begin(_eduroamAuth.ssid);

    // 等待连接完成或超时
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime) < timeout_ms)
    {
        Serial.print("."); // Add progress indicator
        delay(500);
    }
    Serial.println(); // Newline after dots

    // 更新连接状态
    _isConnected = (WiFi.status() == WL_CONNECTED);
    if (!_isConnected)
    {
        _lastError = "Enterprise connection failed or timed out. Status: " + String(WiFi.status());
        Serial.println(_lastError); // Add logging
        // Optional: Disable WPA2 Enterprise mode if connection failed
        // esp_wifi_sta_wpa2_ent_disable(); // Consider disabling if not retrying soon
    }
    else
    {
        _lastError = ""; // Clear error on success
        Serial.println("Eduroam connection successful."); 
    }

    updateWifiStatus(); // Update status immediately after connection attempt
    return _isConnected;
}

// Connect using WPA2-PSK (Shared Key) - Uses internal _pskAuth
bool WifiClient::connectPSK(uint32_t timeout_ms)
{
    // Check if internal credentials are set
    if (!_pskAuth.ssid || !_pskAuth.password ||
        strlen(_pskAuth.ssid) == 0 || strlen(_pskAuth.password) == 0)
    {
        _lastError = "Internal PSK SSID or Password not set/empty";
        _isConnected = false;
        return false;
    }

    Serial.println("Attempting PSK connection..."); 
    setupWiFiBasics(); // Basic WiFi setup

    // Make sure Enterprise mode is disabled if it was previously enabled
    esp_wifi_sta_wpa2_ent_disable();

    // Use internal struct members
    WiFi.begin(_pskAuth.ssid, _pskAuth.password);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime) < timeout_ms)
    {
        Serial.print("."); // Add progress indicator
        delay(500);
    }
    Serial.println(); // Newline after dots

    _isConnected = (WiFi.status() == WL_CONNECTED);
    if (!_isConnected)
    {
        _lastError = "PSK connection failed or timed out. Status: " + String(WiFi.status());
        Serial.println(_lastError); // Add logging
    }
    else
    {
        _lastError = ""; // Clear error on success
        Serial.println("PSK connection successful."); // Add logging
    }

    updateWifiStatus(); // Update status immediately after connection attempt
    return _isConnected;
}

// New method to try PSK then Eduroam
bool WifiClient::connectAuto(uint32_t psk_timeout_ms, uint32_t eduroam_timeout_ms)
{
    Serial.println("Starting automatic connection attempt...");
    _lastError = ""; // Clear previous errors

    // Try PSK first
    if (connectPSK(psk_timeout_ms))
    {
        Serial.println("Connected via PSK.");
        return true; // Success
    }

    // If PSK failed, try Eduroam
    Serial.println("PSK failed, trying Eduroam...");
    if (connectEduroam(eduroam_timeout_ms))
    {
        Serial.println("Connected via Eduroam.");
        return true; // Success
    }

    // If both failed
    Serial.println("Automatic connection failed (both PSK and Eduroam).");
    // _lastError should be set by the last failed attempt (connectEduroam)
    return false; // Failure
}

bool WifiClient::isConnected()
{
    // Check for live status if needed,
    return _isConnected && (WiFi.status() == WL_CONNECTED);
}

String WifiClient::getLastError()
{
    return _lastError;
}

const WifiClient::WifiStatus &WifiClient::getWifiStatus()
{
    // Time-based update
    unsigned long now = millis();
    // Corrected UPDATE_INTERVAL check
    if (!_isConnected || now - _wifiStatus.lastUpdateTime > WifiClient::UPDATE_INTERVAL || _wifiStatus.lastUpdateTime == 0) {
       updateWifiStatus();
    }
    return _wifiStatus;
}

void WifiClient::updateWifiStatus()
{
    // More robust check using WiFi.status() directly here
    bool currentStatus = (WiFi.status() == WL_CONNECTED);

    // Only update _isConnected if the status actually changed,
    // or if it's the first update. Avoids overwriting connection success
    // if WiFi momentarily drops but recovers before next check.
    // However, for simplicity and immediate reflection of WiFi.status(),
    // updating _isConnected directly might be intended.
    _isConnected = currentStatus; // Keep this if immediate reflection is desired

    _wifiStatus.isConnected = currentStatus;
    if (_wifiStatus.isConnected)
    {
        _wifiStatus.signalStrength = WiFi.RSSI();
        _wifiStatus.ipAddress = WiFi.localIP().toString();
    }
    else
    {
        // If disconnected, clear IP and set signal strength to 0 or a specific "disconnected" value
        _wifiStatus.signalStrength = 0; // Or perhaps a negative value like -100
        _wifiStatus.ipAddress = ""; // Clear IP address
    }
    _wifiStatus.lastUpdateTime = millis();
}