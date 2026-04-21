#ifndef EDUROAM_CLIENT_H
#define EDUROAM_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_wpa2.h>

struct eduroam_auth
{
    const char *ssid;
    const char *identity;
    const char *password;
};
struct psk_auth
{
    const char *ssid;
    const char *password;
};

class WifiClient
{
public:
    WifiClient(const psk_auth& psk, const eduroam_auth& eduroam);
    struct WifiStatus
    {
        bool isConnected;
        int signalStrength;
        String ipAddress;
        unsigned long lastUpdateTime;
    };

    // Method to try PSK first, then Eduroam
    bool connectAuto(uint32_t psk_timeout_ms = 30000, uint32_t eduroam_timeout_ms = 30000);

    // for Enterprise/Eduroam - uses internal credentials
    bool connectEduroam(uint32_t timeout_ms = 30000);

    // for WPA2-PSK - uses internal credentials
    bool connectPSK(uint32_t timeout_ms = 30000);

    bool isConnected();
    String getLastError();

    const WifiStatus &getWifiStatus();
    static const unsigned long UPDATE_INTERVAL = 30*60*1000; // 30 minutes
    void updateWifiStatus();

private:
    eduroam_auth _eduroamAuth;
    psk_auth _pskAuth;
    String _lastError;
    bool _isConnected;
    WifiStatus _wifiStatus;
    // Helper to configure WiFi basics
    void setupWiFiBasics();
};
#endif // EDUROAM_CLIENT_H