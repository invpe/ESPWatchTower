![image](https://github.com/invpe/ESPWatchTower/assets/106522950/c9823da1-eb47-4889-aae4-93dc9db5ff06)

Congratulations, you're ready to start working with your WatchTower.

# 🌓 First run after installation

When there are no devices defined, WatchTower will run a `10 seconds` scan and add whatever it currently 
sees speaking to your AP as trusted devices. This is to help you out with filling in devices a bit 😄


# 🔌 Connect to WatchTower

Simply `telnet ip_address_of_your_watchtower`

If you don't know the IP of your device, you can find it in serial monitor once rebooted.

```19:40:05.261 -> Connected 192.168.1.211```

# ✨ Connected, now what

Once you're connected you are in control, start by `LIST`ing devices that were found so far.
Type `LIST` and hit `ENTER`

![image](https://github.com/invpe/ESPWatchTower/assets/106522950/fa70d423-99ec-4a58-acbb-3b9252674347)

This is the main screen that you will be looking into, when WatchTower turns 🛑 (red).

## What do i see in the `LIST`

It is a list of all devices that were sending frames to/through your Access Point that you have connected to.
All data is put into a table view so it is easier to read but it all boils down to :

- `MAC` - which is a MAC address of the device seen
- `Name` - which is a NAME of the device that you can set
- `RSSI` - which is the RSSI (signal) as noted from the WatchTower position
- `FirstSeen` - which is the first time it was seen
- `LastSeen` - which is the last time it was seen
- `Trusted` - which is a flag you set for devices you trust

## Updating device names

A device is easier to read when it has a name, by default WatchTower names all as `Unknown`.
Simply type `SETNAME mac_address name` to update the name of the device.

Example:

```
LIST
Index	MAC		Name	RSSI	FirstSeen		LastSeen		Trusted
0	xxxxxxxxxxxx	Unknown	-74	2023-08-30 17:40:09	2023-08-30 17:49:43	0

SETNAME xxxxxxxxxxxx IPAD
LIST

Index	MAC		Name	RSSI	FirstSeen		LastSeen		Trusted
0	xxxxxxxxxxxx	IPAD	-74	2023-08-30 17:40:09	2023-08-30 17:49:43	0
```


## Updating trust flag
If you see a device on the list that you know, simply mark it trusted.
It will display in green the next time you perform `LIST`ing.

To mark a device trusted type:

 
``` 
TRUST xxxxxxxxxxxx


Index	MAC		Name	RSSI	FirstSeen		LastSeen		Trusted
0	xxxxxxxxxxxx	IPAD	-74	2023-08-30 17:40:09	2023-08-30 17:49:43	1
```

The flag will change to 1 and device will be rendered green.
Once device is set to trusted, it will not fire a 🛑 red alert.

## Adding device manually

Sometimes you want to add or remove a device, there are few situations in which this can come handy.

1. You know a device is out of range, and you want to add it
2. You want to delete an unknown device to see if it comes back

Use the Commands table to perform these operations.



# 📝 Commands

| Command  | Arguments | Description | 
| ------------- | ------------- |------------- |
| STATUS  |   | Display the status of the WatchTower |
| REBOOT  |   | Reboot the device |
| RESET   |   | Remove settings and reset the device to defaults |
| SAVE   |   | Save changes |
| OTA   |   | Turn OTA ON/OFF |
| LIST  |   | List all devices seen |
| ADD | MAC NAME | Add device with MAC and NAME |
| DELETE | MAC | Delete device with MAC |
| TRUST | MAC | Mark device with MAC as trusted |
| UNTRUST | MAC | Mark device with MAC as untrusted |
| ALLTRUST |  | Mark all devices as trusted |
| SETNAME | MAC NAME | Set NAME of a device with MAC |
| SETWIFI | WIFI_NAME | Set WIFI Name to connect to |
| SETPASS | WIIF_PASSWORD | Set WIFI Password |



# 🚢 Operating WatchTower

Once you have added/updated and marked your devices as trusted, this data will be saved in the WatchTower device.
If a device that is untrusted (unknown) is sending data through your AP, WatchTower will go 🛑 to notify you.
It simply checks if a devices is on your list marked as `trusted` if not, it goes red-red-red.

# Once 🔴 happens:

- Definitely revisit your `LIST` and check the device marked as untrusted
- See if this is a device that you know but forgot to flag `trusted` if so, update it.
- If you don't know this device a 🔴 red alert should now pop above your head
  * Check the RSSI, does it ring a bell ? (low RSSI might mean signal is coming from far away), compare it to the other trusted signals
  * Check [OUI](https://www.wireshark.org/tools/oui-lookup.html) tool to see if you know if you have a vendor that is assigned to this MAC
  * Note it down, you can check against it later
  * If you really don't recognize it, maybe it is the time to update your WIFI password - Use [https://www.avast.com/random-password-generator#pc](https://www.avast.com/random-password-generator#pc) to help you pick one that is safe.

  * When decided to change password, update WatchTower first then your WiFi AP/router  
  * When you change it, wait to see if the device comes back - if it was an intruder, it shouldnt appear anymore. If that's the case, good job ! You found someone is using your internet!
    

This is just basic, there are way more things you might want to consider to play it even better, some of them are:

- Deploy few instances to check for suspicious connections covering larger area of your house
- Maybe you spot it happening only from a room upstairs, that would be good clue.
- Monitor RSSI of the devices vs 🔴 red ones, do you see any pattern ?
- Mark down the `LastSeen` and check if it comes during grey hours like early in the morning, or when your out of home
- Check if the suspected device is connecting anywhere else too - you can use tools available online for that
  

 
# 📱 False positives

## MAC Address Randomization
Some devices have a mechanism called "MAC address randomization".
Ensure to disable it in your mobile phones, tablets or other devices when using WatchTower for your WiFi network as this function randomizes MAC addresses of your devices,
so that whenever they connect, they will use a different address which by WatchTower will be marked untrusted.

Here is how to do this 
- IOS [https://www.linksys.com/support-article?articleNum=317709](https://www.linksys.com/support-article?articleNum=317709) 
- ANDROID [https://www.linksys.com/support-article?articleNum=317710](https://www.linksys.com/support-article?articleNum=317710)


# 🎇 Ideas ?

Feel free to share !





