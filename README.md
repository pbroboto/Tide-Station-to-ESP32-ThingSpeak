**Tide Station to ESP32 ThingSpeak**

The Tide Station utilizes the Valeport TideMaster system to monitor water levels. The water level sensor measures pressure and converts it into water level readings. The sensor is connected to a Scannex device, which collects the data and transmits it to the cloud using a SIM card. The SIM card is leased, and the service fee is approximately 1,000 Baht per month.

Cloud services are provided by a vendor who requires the purchase of their cloud package, which costs around 80,000 Baht per year. After the third year, it’s worth reconsidering whether to continue renting, as the cost is quite high. The service itself is fairly basic, offering unlimited downloads of historical data and access to real-time data. From a DIY perspective, it’s possible to replicate this service at a lower cost.

**Selecting a Microcontroller Board**

The next step was to choose a microcontroller that could connect to the internet and receive water level data via a UART serial connection—both basic features for most microcontrollers. The board would then send the data to the cloud. Initially, I considered the Pico W, but in my experience, it had difficulties maintaining a stable Wi-Fi connection, even when placed close to the router. I then looked into the Arduino Mega2560 Wireless R3 and the ESP32. After testing both, I found that I preferred the ESP32 for its performance.

![alt text](https://github.com/pbroboto/Pico-Echo-Sounder/blob/main/airmar-d800-pico-w-_bb-scaled.jpg?raw=true)
