#ifndef LITTLEFS_MANAGER_H
#define LITTLEFS_MANAGER_H

#include <FS.h>
#include <LittleFS.h>
#include <Arduino.h>

class LittleFSManager {
public:
    LittleFSManager(const String& logFilename = "/error_log.txt");

    // Initializes LittleFS
    bool begin(bool formatOnFail = true);

    // Logs a message to the log file
    void log(const String& message);

    // Opens the data file in append mode. Creates file and header if it doesn't exist.
    // Returns the File object. Check validity with 'if (file)'
    File openDataFile(const String& dataFilename, const String& header = "timestamp,pressure,temperature,altitude");

    // Writes data to an already opened CSV file. Flushes data for robustness.
    // Returns true on success, false on write error.
    bool writeData(File& dataFile, unsigned long long timestamp, float pressure, float temperature, float altitude);

    // Checks if a file exists
    bool fileExists(const String& filename);

    // Gets filesystem usage information
    String getFSInfo();

private:
    String _logFilename;
    File _logFile;
    bool _isInitialized;
};

// Declare a global instance (extern)
extern LittleFSManager fsManager;

#endif // LITTLEFS_MANAGER_H