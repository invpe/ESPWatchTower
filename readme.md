![image](https://github.com/invpe/ESPWatchTower/assets/106522950/1ce50f55-5bde-4de7-a26e-021e15043a4b)

# 🗼 WATCHTOWER

WatchTower is a simple yet very effective WiFi intrusion detection system designed to safeguard your home network against unauthorized access. 
By monitoring incoming data packets and comparing them to a whitelist of known devices, WatchTower swiftly identifies potential intruders. 
When an unknown device is detected, WatchTower triggers a red LED indicator, providing a visual alert. When there is nothing unusual happening, it stays in the shadows
without any sign of life. 

As with any "red" scenario it is good to dive deeper and either mark the device as trusted or change your Access Point password immediately.
WatchTower enables telnet access for you to control its behaviour. 


# 🗞️ Use case

Real world scenario where a friend of mine suspected his AP was used by his neighbour.
Since APs are not user's friendly in terms of notifying you about breaches, i had an idea to simply shine a 🔴 
when an untrusted device is found using his AP.

We went with WatchTower and found it really happening after few hours.

![image](https://github.com/invpe/ESPWatchTower/assets/106522950/265d9c85-8e52-4a30-97bf-c01fe82795b2)


# 🅰️ How does it work

It is really simple, that's why i like it.

WatchTower will continosuly look for packets going through your AP over WIFI.
All devices that happen to send data with your AP will be added to a list.
Devices are marked **untrusted** by default, so you will need some minimal time to isolate the ones you trust.
Then when you're all set, anytime a device that is **untrusted** is seen talking with your AP, you will get a 🔴 

Under the hood, you will have access to a telnet server running on the WatchTower,
there you can set names, trust, untrust, add, delete devices.


# 🔴 What next ?

When you get 🔴 and it's not a device that you know - definitely change your WiFi Access Point password ❕

Use [https://www.avast.com/random-password-generator#pc](https://www.avast.com/random-password-generator#pc) to help you pick one that is safe.


# 📱 False positives

## MAC Address Randomization
Some devices have a mechanism called "MAC address randomization".
Ensure to disable it in your mobile phones, tablets or other devices when using WatchTower for your WiFi network as this function randomizes MAC addresses of your devices,
so that whenever they connect, they will use a different address which by WatchTower will be marked untrusted.

Here is how to do this 
- IOS [https://www.linksys.com/support-article?articleNum=317709](https://www.linksys.com/support-article?articleNum=317709) 
- ANDROID [https://www.linksys.com/support-article?articleNum=317710](https://www.linksys.com/support-article?articleNum=317710)

# 🚶‍♂️ Next steps if you're still interested.

- [Install](https://github.com/invpe/ESPWatchTower/blob/main/install_manual.md)
- [Use](https://github.com/invpe/ESPWatchTower/blob/main/usage_manual.md)
- Have fun ! 🙂

# 🗃️ License
```
MIT License

Copyright (c) 2023 invpe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
 

