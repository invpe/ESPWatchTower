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





