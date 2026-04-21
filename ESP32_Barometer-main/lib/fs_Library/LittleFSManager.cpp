#include "LittleFSManager.h"
#include <sys/time.h> // For gettimeofday in logger (optional, could use millis())

// Function to get current timestamp string for logging
String getCurrentTimestampString()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t now = tv.tv_sec;
    struct tm timeinfo;
    localtime_r(&now, &timeinfo); // 如果使用 FreeRTOS 任务，使用线程安全版本
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    char finalBuffer[40];
    // 使用 %03ld 来确保毫秒始终是三位数，不足的前面补零
    snprintf(finalBuffer, sizeof(finalBuffer), "%s.%03ld", buffer, tv.tv_usec / 1000);
    return String(finalBuffer);
}

LittleFSManager::LittleFSManager(const String &logFilename)
    : _logFilename(logFilename), _isInitialized(false) {}

bool LittleFSManager::begin(bool formatOnFail)
{
    if (!LittleFS.begin(formatOnFail))
    {
        Serial.println("ERROR: LittleFS Mount Failed!");
        _isInitialized = false;
        return false;
    }
    _isInitialized = true;
    _logFile = LittleFS.open(_logFilename, "a");
    return true;
}

void LittleFSManager::log(const String &message)
{
    if (!_isInitialized)
    {
        Serial.println("ERROR: LittleFS not initialized. Msg: " + message);
        return;
    }
    if (!_logFile)
    {
        Serial.println("ERROR: Failed to open log file for writing: " + _logFilename);
        return;
    }

    String timestamp = getCurrentTimestampString();
    _logFile.println("[" + timestamp + "] " + message);
    _logFile.flush();
}

bool LittleFSManager::fileExists(const String &filename)
{
    if (!_isInitialized)
    {
        log("ERROR: Cannot check file existence, LittleFS not initialized.");
        return false;
    }
    return LittleFS.exists(filename);
}

File LittleFSManager::openDataFile(const String &dataFilename, const String &header)
{
    if (!_isInitialized)
    {
        log("ERROR: Cannot open data file, LittleFS not initialized.");
        return File();
    }

    File dataFile;
    bool needsHeader = !fileExists(dataFilename);

    if (needsHeader)
    {
        dataFile = LittleFS.open(dataFilename, "w");
        if (!dataFile)
        {
            log("ERROR: Failed to create file for header: " + dataFilename);
            return File();
        }
        dataFile.println(header);
        dataFile.close();
        log("Created file and wrote header: " + dataFilename);

        dataFile = LittleFS.open(dataFilename, "a");
        if (!dataFile)
        {
            log("ERROR: Failed to reopen data file in append mode after creating header: " + dataFilename);
            return File();
        }
        log("Opened data file in append mode: " + dataFilename);
    }
    else
    {
        dataFile = LittleFS.open(dataFilename, "a");
        if (!dataFile)
        {
            log("ERROR: Failed to open existing data file for appending: " + dataFilename);
            return File();
        }
    }

    return dataFile;
}

bool LittleFSManager::writeData(File &dataFile,
                                unsigned long long timestamp,
                                float pressure,
                                float temperature,
                                float altitude)
{
    if (!_isInitialized)
    {
        log("Error: Cannot write data, LittleFS not initialized.");
        return false;
    }

    if (!dataFile)
    {
        log("ERROR: Attempted to write to an invalid/closed data file handle.");
        return false;
    }

    char buffer[128]; // 足够容纳所有数据的缓冲区
    
    // 使用%.2f来确保保留2位小数，匹配原始print(value, 2)的格式
    int len = snprintf(buffer, sizeof(buffer), "%llu,%.2f,%.2f,%.2f\n", 
                      timestamp, pressure, temperature, altitude);
    
    if (len < 0 || len >= sizeof(buffer))
    {
        log("ERROR: Buffer overflow when formatting data for file: " + String(dataFile.name()));
        return false;
    }
    
    size_t bytesWritten = dataFile.write((uint8_t*)buffer, len);

    if (bytesWritten == 0 && (timestamp != 0 || pressure != 0 || temperature != 0 || altitude != 0))
    {
        log("ERROR: Zero bytes written to data file: " + String(dataFile.name()) + ". Filesystem might be full.");
        return false;
    }

    dataFile.flush();

    return true;
}

String LittleFSManager::getFSInfo()
{
    if (!_isInitialized)
    {
        return "LittleFS not initialized.";
    }
    size_t totalBytes = LittleFS.totalBytes();
    size_t usedBytes = LittleFS.usedBytes();
    String info = String(usedBytes) + " used / " + String(totalBytes) + " total bytes\n";
    info += "Files in root directory:\n";

    File root = LittleFS.open("/");
    if (!root)
    {
        info += "  ERROR: Failed to open root directory.\n";
        return info;
    }
    if (!root.isDirectory())
    {
        info += "  ERROR: Root path is not a directory.\n";
        root.close();
        return info;
    }

    File file = root.openNextFile();
    bool filesFound = false;
    while (file)
    {
        // LittleFS on ESP32 doesn't really have directories in the same way
        // as a standard filesystem. openNextFile() iterates through all entries.
        // We check if it's *not* a directory, though typically all entries
        // created via standard file operations will be files.
        if (!file.isDirectory())
        {
            filesFound = true;
            info += "  - ";
            // Print file name (remove leading '/' if present, LittleFS usually includes it)
            const char* fileName = file.name();
            if (fileName && fileName[0] == '/') {
                info += String(fileName + 1);
            } else {
                info += String(fileName);
            }
            info += " (" + String(file.size()) + " bytes)\n";
        }
        file.close(); // Close the file handle
        file = root.openNextFile();
    }

    if (!filesFound) {
        info += "  (No files found in root)\n";
    }

    root.close(); // Close the root directory handle
    return info;
}