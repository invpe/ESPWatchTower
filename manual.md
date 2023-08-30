# 📎 Setup and instruction

So you've decided to play with WatchTower, that's great 👍
Follow this tutorial to get yourself started.


## 🥩 Sources

Ok, assuming you have Arduino IDE setup and working properly:

Download latest sketch folder from [here](https://github.com/invpe/ESPWatchTower/tree/main/WatchTower) to your desktop.
Open up with Arduino IDE, choose your board and compile / upload to your ESP32.

![image](https://github.com/invpe/ESPWatchTower/assets/106522950/064f511f-64cf-46b2-86ec-a46d4c5768db)


## 📦 Release

The easiest way is to upload binary directly to the ESP device, so:
- Grab latest release
- Use [esp.huhn.me](esp.huhn.me) to upload the binary over WWW

## 📓 Important

### 🛑 Red LED

WatchTower was written with the use of M5Stack AtomU which has a RGB led built in,
but it also uses `LED_BUILTIN` on PIN `2` for notification. If none is working, you might consider updating the source code to fit your needs.

### 💾 SPIFFS

WatchTower uses SPIFFS for storage, do ensure that when working with sources, you set a proper partitioning there to allow file storage and OTA

### ⏬ OTA

WatchTower supports OTA, again as above, ensure partition is set properly before uploading.

# 🧮 Setup

Once you're done with uploading the software to your ESP32 device, it's time to set it up.
Grab yourself a ☕ and lets begin.

### Initial setup (reset) - fresh

When just uploaded or reset, WatchTower will wait for your inputs on Serial.
I will use great online Serial Terminal to guide you through, so if you enjoy new tools - follow me.

1. Open up [https://serial.huhn.me/](https://serial.huhn.me/)
2. Plug in your WatchTower
3. Click Connect

![image](https://github.com/invpe/ESPWatchTower/assets/106522950/81f9081b-1521-4df6-88ed-76fc538a0a29)

4. Select serial port and hit Connect

![image](https://github.com/invpe/ESPWatchTower/assets/106522950/fea3932b-9b34-4296-865d-8df6973365b7)

5. Follow the instructions on the terminal

   ![image](https://github.com/invpe/ESPWatchTower/assets/106522950/fa87c25a-5677-4328-bd28-502d5a0b7403)

6. Hit Enter to start
7. Provide your WiFi
8. Provide your WiFi Password

![image](https://github.com/invpe/ESPWatchTower/assets/106522950/833b18b3-4384-4e48-bc76-ef6dbdd2540e)

9. You're done, WatchTower will now connect to your WiFi and start monitoring it.
10. Close the webterminal page.

    



   



