/*
 * ESP32 WatchTower an ESP32 Wireless Intrusion Detection eXperiment
 * https://github.com/invpe/ESPWatchTower
 * (c) invpe 2k23 
 */
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <vector>
#include <ctime>
#include <esp_task_wdt.h>
#include <map>
#include <Adafruit_NeoPixel.h>
#include "esp_wifi.h"
#include "SPIFFS.h"
#include "FS.h"
///////////////////////////
#define WIFI_MAX_CHANNELS     14
#define SNAP_LEN              293
#define TYPE_MANAGEMENT       0x00
#define TYPE_CONTROL          0x01
#define TYPE_DATA             0x02
#define SUBTYPE_BEACONS       0x08
#define SUBTYPE_PROBE_REQ     0x04
#define SUBTYPE_DEAUTH        0xC0
#define GET_BIT(X,N) ( ( (X) >> (N) ) & 1 )
#define PBUF_PAYLOAD_OFF(p, T, off) reinterpret_cast<T*>(reinterpret_cast< uint8_t*>(p->payload) + (off))
///////////////////////////
#define ADA_PIN       27
#define ADA_NUMPIXELS 1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(ADA_NUMPIXELS, ADA_PIN, NEO_GRB + NEO_KHZ800);
///////////////////////////
#define ANSI_CLEAR_SCREEN "\x1B[2J\x1B[H" // Clear screen and move cursor to home position
#define ANSI_RESET "\x1B[0m"
#define ANSI_BLACK "\x1B[30m"
#define ANSI_RED "\x1B[31m"
#define ANSI_GREEN "\x1B[32m"
#define ANSI_YELLOW "\x1B[33m"
#define ANSI_BLUE "\x1B[34m"
#define ANSI_MAGENTA "\x1B[35m"
#define ANSI_CYAN "\x1B[36m"
#define ANSI_WHITE "\x1B[37m"
#define ANSI_GREY "\x1B[90m"
///////////////////////////
#define LED_BUILTIN 2
///////////////////////////
WiFiServer TelnetServer(23);
WiFiClient TelnetClient;
///////////////////////////
String strVersion = "WATCHTOWER_01";
bool bOTA = true;
String strWiFiA = "";
String strWiFiP = "";
String strAccessPointMAC = "";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
uint32_t uiStatsTimer;
uint64_t uiCountOfFrames = 0;
time_t tTimeSinceEpoch;
tm tTime;
tm tStartTime;
///////////////////////////
struct tDevice
{
  time_t m_tFirstSeen;
  time_t m_tLastSeen;
  int m_iRSSI;
  bool m_bTrusted;
  String m_strName;
  tDevice()
  {
    m_iRSSI       = 0;
    m_tFirstSeen  = 0;
    m_tLastSeen   = 0;
    m_bTrusted    = false;
    m_strName     = "Unknown";
  }
};
std::map<String, tDevice> mDevices;
// Helper structures
struct frame_control
{
  uint8_t protocol_version: 2;
  uint8_t type: 2;
  uint8_t subtype: 4;
  uint8_t to_ds: 1;
  uint8_t from_ds: 1;
  uint8_t more_frag: 1;
  uint8_t retry: 1;
  uint8_t pwr_mgt: 1;
  uint8_t more_data: 1;
  uint8_t protected_frame: 1;
  uint8_t order: 1;
};
struct ieee80211_hdr {
  struct frame_control mframe_control;
  uint16_t duration_id;
  uint8_t addr1[6];
  uint8_t addr2[6];
  uint8_t addr3[6];
  uint16_t seq_ctrl;
  uint8_t addr4[6];
};
///////////////////////////
void PassiveScan(void* buf, wifi_promiscuous_pkt_type_t type)
{
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;

  if (type == WIFI_PKT_MISC)
    return;   // wrong packet type

  if (ctrl.sig_len > SNAP_LEN)
    return; // packet too long    if (ctrl.sig_len > SNAP_LEN) return;

  if (ctrl.rx_state != 0)
    return;

  uint32_t packetLength = ctrl.sig_len;
  struct ieee80211_hdr *pHeader = (struct ieee80211_hdr*) pkt->payload;

  char cSource[32];
  char cDestination[32];
  char cBSSID[32];


  //ToDS FromDS BSSID
  // 0 0
  if (pHeader->mframe_control.to_ds == 0 &&  pHeader->mframe_control.from_ds == 0)
  {
    char cData[32];
    sprintf(cBSSID, "%02x%02x%02x%02x%02x%02x", pHeader->addr3[0], pHeader->addr3[1], pHeader->addr3[2], pHeader->addr3[3], pHeader->addr3[4], pHeader->addr3[5]);
    sprintf(cDestination, "%02x%02x%02x%02x%02x%02x", pHeader->addr1[0], pHeader->addr1[1], pHeader->addr1[2], pHeader->addr1[3], pHeader->addr1[4], pHeader->addr1[5]);
    sprintf(cSource, "%02x%02x%02x%02x%02x%02x", pHeader->addr2[0], pHeader->addr2[1], pHeader->addr2[2], pHeader->addr2[3], pHeader->addr2[4], pHeader->addr2[5]);
  }
  //
  // 0 1
  else if (pHeader->mframe_control.to_ds == 0 &&  pHeader->mframe_control.from_ds == 1)
  {
    char cData[32];
    sprintf(cBSSID, "%02x%02x%02x%02x%02x%02x", pHeader->addr2[0], pHeader->addr2[1], pHeader->addr2[2], pHeader->addr2[3], pHeader->addr2[4], pHeader->addr2[5]);
    sprintf(cDestination, "%02x%02x%02x%02x%02x%02x", pHeader->addr1[0], pHeader->addr1[1], pHeader->addr1[2], pHeader->addr1[3], pHeader->addr1[4], pHeader->addr1[5]);
    sprintf(cSource, "%02x%02x%02x%02x%02x%02x", pHeader->addr3[0], pHeader->addr3[1], pHeader->addr3[2], pHeader->addr3[3], pHeader->addr3[4], pHeader->addr3[5]);
  }
  //
  // 1 0
  else if (pHeader->mframe_control.to_ds == 1 &&  pHeader->mframe_control.from_ds == 0)
  {
    char cData[32];
    sprintf(cBSSID, "%02x%02x%02x%02x%02x%02x", pHeader->addr1[0], pHeader->addr1[1], pHeader->addr1[2], pHeader->addr1[3], pHeader->addr1[4], pHeader->addr1[5]);
    sprintf(cDestination, "%02x%02x%02x%02x%02x%02x", pHeader->addr3[0], pHeader->addr3[1], pHeader->addr3[2], pHeader->addr3[3], pHeader->addr3[4], pHeader->addr3[5]);
    sprintf(cSource, "%02x%02x%02x%02x%02x%02x", pHeader->addr2[0], pHeader->addr2[1], pHeader->addr2[2], pHeader->addr2[3], pHeader->addr2[4], pHeader->addr2[5]);
  }
  // 1 1
  else if (pHeader->mframe_control.to_ds == 1 &&  pHeader->mframe_control.from_ds == 1)
  {
  }

  //
  if (type == WIFI_PKT_DATA)
  {
    String strSource      = String(cSource);
    String strDestination = String(cDestination);
    String strBSSID       = String(cBSSID);

    // Through AP
    if (strDestination == strAccessPointMAC )
    {
      uiCountOfFrames++;
      // Unknown, add
      if ( mDevices.find(strSource) == mDevices.end() )
      {
        tDevice _new;
        _new.m_iRSSI = pkt->rx_ctrl.rssi;
        _new.m_tFirstSeen = tTimeSinceEpoch;
        _new.m_tLastSeen  = tTimeSinceEpoch;
        mDevices[strSource] = _new;
      }
      else
      {
        mDevices[strSource].m_iRSSI = pkt->rx_ctrl.rssi;
        mDevices[strSource].m_tLastSeen = tTimeSinceEpoch;
      }
    }
  }
}

bool IsSecure()
{
  for (std::map<String, tDevice>::const_iterator it = mDevices.begin(); it != mDevices.end(); ++it) {
    const String& key = it->first;
    tDevice device = it->second;
    if (device.m_bTrusted == false) return false;
  }
  return true;
}
void setup()
{
  pixels.begin();
  pixels.setBrightness(255);
  pixels.setPixelColor(0, 0, 0, 0);
  pixels.show();
  Serial.begin(115200);
  Serial.println("Preparing SPIFFS");

  while (!SPIFFS.begin())
  {
    SPIFFS.format();
    Serial.println(".");
    delay(1000);
  }
  Serial.println("SPIFFS Done");

  //
  LoadConfig();

  // See if we have config available if not wait for serial config to happen
  // Use https://esp.huhn.me/
  if (strWiFiA == "")
  {
    Serial.println("Fresh start, needs config over serial.");
    Serial.println("--------------------------------------");
    Serial.println("Hit any key to start...");

    // Wait for user readiness
    while (!Serial.available()) {
      // Wait for input
    }
    Serial.readString();

    // Wait for user to provide WiFi name
    Serial.println("Please enter WiFi name:");
    while (!Serial.available()) {
      // Wait for input
    }
    strWiFiA = Serial.readString();
    strWiFiA.trim(); // Remove leading/trailing spaces

    // Wait for user to provide WiFi password
    Serial.println("Please enter WiFi password:");
    while (!Serial.available()) {
      // Wait for input
    }
    strWiFiP = Serial.readString();
    strWiFiP.trim(); // Remove leading/trailing spaces

    Serial.println("Configuration stored: " + strWiFiA + "/" + strWiFiP);

    // Save config
    SaveConfig();
  }

  Serial.println("Connecting to WIFI " + strWiFiA);
  WiFi.setHostname(strVersion.c_str());
  WiFi.begin(strWiFiA.c_str(), strWiFiP.c_str());

  // Give it 10 seconds to connect, otherwise reboot
  uint8_t iRetries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
    iRetries += 1;

    if (iRetries >= 10)
      ESP.restart();
  }
  Serial.println("Connected " + WiFi.localIP().toString());

  strAccessPointMAC = WiFi.BSSIDstr();
  strAccessPointMAC.replace(":", "");
  strAccessPointMAC.toLowerCase();

  Serial.println("Will look after " + WiFi.SSID() + " " + strAccessPointMAC);


  Serial.println("OTA starting");
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else  // U_SPIFFS
      type = "filesystem";
  })
  .onEnd([]() {
    Serial.println(F("\nEnd"));
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR)
      Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR)
      Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR)
      Serial.println(F("End Failed"));
    esp_restart();
  });

  ArduinoOTA.setHostname(strVersion.c_str());
  ArduinoOTA.begin();



  //
  Serial.println("NTP Setup");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  while (!getLocalTime(&tTime)) {
    Serial.println("Failed to obtain time");
    delay(1000);
  }
  tStartTime = tTime;
  tTimeSinceEpoch = mktime(&tTime);


  // See if we have devices already stored if not gop to auto mapping mode
  LoadDevices();
  Serial.println("Loaded " + String(mDevices.size()) + " devices");

  if (mDevices.size() == 0)
  {
    // Check if we have any devices mapped if not, go into auto-map mode
    Serial.println("Entering auto map mode since no devices defined");
    esp_wifi_set_promiscuous_rx_cb(&PassiveScan);
    esp_wifi_set_promiscuous(true);
    uint64_t uiWait = millis();
    while (millis() - uiWait <= 10000)
    {
      yield();
      esp_task_wdt_reset();
    }
    esp_wifi_set_promiscuous(false);
    Serial.println("Auto map ended with " + String(mDevices.size()) + " devices");

    // Mark them all as trusted
    for (std::map<String, tDevice>::iterator it = mDevices.begin(); it != mDevices.end(); ++it) {
      const String& key = it->first;
      it->second.m_bTrusted = true;
    }
  }
  // Start telnet
  TelnetServer.begin();


  // Start promisc forever
  esp_wifi_set_promiscuous_rx_cb(&PassiveScan);
  esp_wifi_set_promiscuous(true);


  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("Rolling, connect through telnet to " + WiFi.localIP().toString());


}
// Tokenize a String and store tokens in a vector
std::vector<String> TokenizeString(const String& input, char delimiter) {
  std::vector<String> tokens;
  char* token = strtok(const_cast<char*>(input.c_str()), &delimiter);
  while (token != nullptr) {
    tokens.push_back(token);
    token = strtok(nullptr, &delimiter);
  }
  return tokens;
}

void SaveDevices()
{
  File configFile = SPIFFS.open("/devices.txt", "w");
  if (!configFile)
  {
    return;
  }

  for (std::map<String, tDevice>::const_iterator it = mDevices.begin(); it != mDevices.end(); ++it) {
    const String& key = it->first;
    tDevice device = it->second;
    configFile.print(key + "," + device.m_strName + "," + String(device.m_bTrusted) + "," + String(device.m_tFirstSeen) + "," + String(device.m_tLastSeen) + "\n");
  }
  configFile.close();
}
void LoadDevices()
{
  File configFile = SPIFFS.open("/devices.txt", "r");
  if (!configFile)
  {
    return;
  }
  while (configFile.available()) {
    String line = configFile.readStringUntil('\n');
    std::vector<String> vArguments = TokenizeString(line, ',');
    tDevice _new;
    String strMac = vArguments[0];
    _new.m_strName = vArguments[1];
    _new.m_bTrusted = vArguments[2].toInt();
    _new.m_tFirstSeen = vArguments[3].toInt();
    _new.m_tLastSeen = vArguments[4].toInt();
    mDevices[strMac] = _new;
  }

  configFile.close();
}
void SaveConfig()
{
  File configFile = SPIFFS.open("/config.txt", "w");
  if (!configFile)
  {
    return;
  }
  //
  configFile.print(strWiFiA + "\n");
  configFile.print(strWiFiP + "\n");
  configFile.print(String(bOTA) + "\n");
  configFile.close();
}
bool LoadConfig()
{
  File configFile = SPIFFS.open("/config.txt", "r");
  if (!configFile)
    return false;

  strWiFiA = configFile.readStringUntil('\n') ;
  strWiFiP = configFile.readStringUntil('\n') ;
  bOTA = configFile.readStringUntil('\n').toInt();
  configFile.close();

  return true;
}
void ProcessCommand(const String& rstrCommand)
{
  // RESET all configuration
  if (rstrCommand.startsWith("RESET"))
  {
    SPIFFS.remove("/config.txt");
    SPIFFS.remove("/devices.txt");
    TelnetClient.println("All data removed, rebooting device");
    TelnetClient.stop();
    ESP.restart();
  }
  // STAT
  else if (rstrCommand.startsWith("STATUS"))
  {
    TelnetClient.println("WatchTower Status");
    TelnetClient.println("------------------------------------------------------------");
    TelnetClient.println("Version     : "ANSI_WHITE + strVersion + ANSI_RESET);
    TelnetClient.println("Started at  : "ANSI_WHITE + String(tStartTime.tm_year + 1900) + String(tStartTime.tm_mon + 1) + String(tStartTime.tm_mday) + " " + String(tStartTime.tm_hour) + ":" + String(tStartTime.tm_min) + ANSI_RESET);
    TelnetClient.println("Memory Free : "ANSI_WHITE + String(ESP.getFreeHeap()) + ANSI_RESET);
    TelnetClient.println("SPIFFS Total: "ANSI_WHITE + String(SPIFFS.totalBytes()) + ANSI_RESET);
    TelnetClient.println("SPIFFS Free : "ANSI_WHITE + String(SPIFFS.totalBytes() - SPIFFS.usedBytes()) + ANSI_RESET);
    TelnetClient.println("WIFI Network: "ANSI_WHITE + strWiFiA + ANSI_RESET);
    TelnetClient.println("WIFI RSSI   : "ANSI_WHITE + String(WiFi.RSSI()) + ANSI_RESET);
    TelnetClient.println("Devices     : "ANSI_WHITE + String(mDevices.size()) + ANSI_RESET);
    TelnetClient.println("Frames/m    : "ANSI_WHITE + String(uiCountOfFrames) + ANSI_RESET);

  }
  // REBOOT
  else if (rstrCommand.startsWith("REBOOT"))
  {
    TelnetClient.println("Rebooting device");
    TelnetClient.stop();
    ESP.restart();
  }
  // SAVE
  else if (rstrCommand.startsWith("SAVE"))
  {
    SaveConfig();
    SaveDevices();
    TelnetClient.println("Data saved");
  }
  // OTA off OTA on
  else if (rstrCommand.startsWith("OTA"))
  {
    bOTA = !bOTA;
    TelnetClient.println("OTA is now "ANSI_WHITE + String(bOTA) + ANSI_RESET);
  }
  // TRUST ALL DEVICES
  else if (rstrCommand.startsWith("ALLTRUST"))
  {
    for (std::map<String, tDevice>::iterator it = mDevices.begin(); it != mDevices.end(); ++it)
      it->second.m_bTrusted = true;

    TelnetClient.println("All devices marked as "ANSI_WHITE"trusted"ANSI_RESET);
  }
  // ADD mac name
  else if (rstrCommand.startsWith("ADD"))
  {
    std::vector<String> vArguments = TokenizeString(rstrCommand, ' ');
    if (vArguments.size() == 3) {
      if ( mDevices.find(vArguments[1]) == mDevices.end() )
        TelnetClient.println("Added device "ANSI_WHITE + vArguments[1] + ANSI_RESET " as "ANSI_WHITE"trusted"ANSI_RESET" with name " + ANSI_WHITE + vArguments[2] + ANSI_RESET);
      else
        TelnetClient.println("Device already exists");
    }
  }
  // SETNAME mac name
  else if (rstrCommand.startsWith("SETNAME"))
  {
    std::vector<String> vArguments = TokenizeString(rstrCommand, ' ');
    if (vArguments.size() == 3) {
      if ( mDevices.find(vArguments[1]) == mDevices.end() )
        TelnetClient.println("Can't find device with MAC of "ANSI_WHITE + vArguments[1] + ANSI_RESET);
      else
      {
        String strName = vArguments[2];
        strName.substring(0, 7);
        mDevices[vArguments[1]].m_strName = strName;
        TelnetClient.println("Setting name of device "ANSI_WHITE + vArguments[1] + ANSI_RESET " to "ANSI_WHITE + vArguments[2] + ANSI_RESET);
      }
    }
  }
  // DELETE mac
  else if (rstrCommand.startsWith("DELETE"))
  {
    std::vector<String> vArguments = TokenizeString(rstrCommand, ' ');
    if (vArguments.size() == 2) {
      if ( mDevices.find(vArguments[1]) == mDevices.end() )
        TelnetClient.println("Can't find device with MAC of "ANSI_WHITE + vArguments[1] + ANSI_RESET);
      else
      {
        mDevices.erase(vArguments[1]);
        TelnetClient.println("Deleted device "ANSI_WHITE + vArguments[1] + ANSI_RESET);
      }
    }
  }
  // TRUST mac
  else if (rstrCommand.startsWith("TRUST"))
  {
    std::vector<String> vArguments = TokenizeString(rstrCommand, ' ');
    if (vArguments.size() == 2) {
      if ( mDevices.find(vArguments[1]) == mDevices.end() )
        TelnetClient.println("Can't find device with MAC of "ANSI_WHITE + vArguments[1] + ANSI_RESET);
      else
      {
        mDevices[vArguments[1]].m_bTrusted = true;
        TelnetClient.println("Device "ANSI_WHITE + vArguments[1] + ANSI_RESET " is marked "ANSI_WHITE "trusted" ANSI_RESET);
      }
    }
  }
  // UNTRUST mac
  else if (rstrCommand.startsWith("UNTRUST"))
  {
    std::vector<String> vArguments = TokenizeString(rstrCommand, ' ');
    if (vArguments.size() == 2) {
      if ( mDevices.find(vArguments[1]) == mDevices.end() )
        TelnetClient.println("Can't find device with MAC of "ANSI_WHITE + vArguments[1] + ANSI_RESET);
      else
      {
        mDevices[vArguments[1]].m_bTrusted = false;
        TelnetClient.println("Device "ANSI_WHITE + vArguments[1] + ANSI_RESET " is marked "ANSI_RED "untrusted" ANSI_RESET);
      }
    }
  }
  // LIST devices found
  else if (rstrCommand.startsWith("LIST"))
  {
    int iCount = 0;
    int iCountNotTrusted = 0;

    TelnetClient.println("Index\tMAC\t\tName\tRSSI\tFirstSeen\t\tLastSeen\t\tTrusted");
    for (std::map<String, tDevice>::const_iterator it = mDevices.begin(); it != mDevices.end(); ++it) {
      const String& key = it->first;
      tDevice device = it->second;
      if (device.m_bTrusted == false)
        iCountNotTrusted++;


      char timeFirstString[30];
      strftime(timeFirstString, sizeof(timeFirstString), "%Y-%m-%d %H:%M:%S", gmtime(&device.m_tFirstSeen));

      char timeLastString[30];
      strftime(timeLastString, sizeof(timeLastString), "%Y-%m-%d %H:%M:%S", gmtime(&device.m_tLastSeen));

      // Now check if was seen long ago (min 30m ago)
      TelnetClient.print(String(iCount) + "\t"); TelnetClient.print(ANSI_RESET);

      if (device.m_bTrusted == true)TelnetClient.print(ANSI_GREEN);
      else TelnetClient.print(ANSI_RED);
      TelnetClient.print(key + "\t"); TelnetClient.print(ANSI_RESET);

      TelnetClient.print(device.m_strName + "\t"); TelnetClient.print(ANSI_RESET);

      if (device.m_iRSSI < 0)TelnetClient.print(ANSI_GREEN);
      if (device.m_iRSSI < -40)TelnetClient.print(ANSI_YELLOW);
      if (device.m_iRSSI < -60)TelnetClient.print(ANSI_RED);
      TelnetClient.print(String(device.m_iRSSI) + "\t"); TelnetClient.print(ANSI_RESET);


      TelnetClient.print(String(timeFirstString) + "\t"); TelnetClient.print(ANSI_RESET);

      if (tTimeSinceEpoch - device.m_tLastSeen > 30 * 60)TelnetClient.print(ANSI_GREY);
      else TelnetClient.print(ANSI_WHITE);
      TelnetClient.print(String(timeLastString) + "\t"); TelnetClient.print(ANSI_RESET);

      if (device.m_bTrusted == true)TelnetClient.print(ANSI_GREEN);
      else TelnetClient.print(ANSI_RED);
      TelnetClient.print(String(device.m_bTrusted)); TelnetClient.print(ANSI_RESET);

      TelnetClient.println("");

      iCount++;
    }

    TelnetClient.println("------------------------------------------------------------");
    TelnetClient.println("NotTrusted: "ANSI_WHITE + String(iCountNotTrusted) + ANSI_RESET" Trusted: "ANSI_WHITE + String(mDevices.size() - iCountNotTrusted) + ANSI_RESET" Total: "ANSI_WHITE + String(mDevices.size()) + ANSI_RESET);
  }
  else {
    // Unknown command
    Serial.println("Unknown command.");
  }
}
void loop()
{
  // Check if WiFi available, if not just boot.
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WIFI Bad, rebooting");
    ESP.restart();
  }

  // Handle OTA
  if (bOTA)
    ArduinoOTA.handle();

  // Extract the time from NTP server
  if (!getLocalTime(&tTime)) {
  } else
    tTimeSinceEpoch = mktime(&tTime);

  // Handle Telnet communication
  if (TelnetServer.hasClient())
  {
    TelnetClient = TelnetServer.available();
    TelnetClient.println(ANSI_CLEAR_SCREEN);
    TelnetClient.println("Welcome to the "ANSI_CYAN"" + strVersion + ""ANSI_RESET", your IP is "ANSI_CYAN"" + TelnetClient.remoteIP().toString() + ""ANSI_RESET);
    TelnetClient.println("The project is hosted on GitHub "ANSI_WHITE"https://github.com/invpe/WatchTower"ANSI_RESET);
    if (bOTA)
      TelnetClient.println("This WatchTower can be updated over OTA");
    TelnetClient.println("------------------------------------------------------------");

  }
  if (TelnetClient && TelnetClient.connected()) {
    if (TelnetClient.available()) {
      String strCommand = TelnetClient.readStringUntil('\n');
      strCommand.trim();
      ProcessCommand(strCommand);
    }
  }
  else if (TelnetClient) TelnetClient.stop();


  // One minute timer
  if (millis() - uiStatsTimer > 60000)
  {

    // The logic of turning ON / OFF the led if untrusted device found
    if (IsSecure() == false)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      pixels.setPixelColor(0, 255, 0, 0);
      pixels.show();
    }
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
      pixels.setPixelColor(0, 0, 0, 0);
      pixels.show();
    }

    // Some quick stats dump
    Serial.println("Devices:" + String(mDevices.size()) + " Frames:" + String(uiCountOfFrames) + "/m");

    //
    uiStatsTimer = millis();
    uiCountOfFrames = 0;
    SaveDevices();
  }
}
