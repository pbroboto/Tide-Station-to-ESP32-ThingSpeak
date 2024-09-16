**Tide Station to ESP32 ThingSpeak**

The Tide Station utilizes the Valeport TideMaster system to monitor water levels. The water level sensor measures pressure and converts it into water level readings. The sensor is connected to a Scannex device, which collects the data and transmits it to the cloud using a SIM card. The SIM card is leased, and the service fee is approximately 1,000 Baht per month.

Cloud services are provided by a vendor who requires the purchase of their cloud package, which costs around 80,000 Baht per year. After the third year, it’s worth reconsidering whether to continue renting, as the cost is quite high. The service itself is fairly basic, offering unlimited downloads of historical data and access to real-time data. From a DIY perspective, it’s possible to replicate this service at a lower cost.

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak./blob/main/Tide5-768x1024.webp?raw=true)

**Selecting a Microcontroller Board**

The next step was to choose a microcontroller that could connect to the internet and receive water level data via a UART serial connection—both basic features for most microcontrollers. The board would then send the data to the cloud. Initially, I considered the Pico W, but in my experience, it had difficulties maintaining a stable Wi-Fi connection, even when placed close to the router. I then looked into the Arduino Mega2560 Wireless R3 and the ESP32. After testing both, I found that I preferred the ESP32 for its performance.

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak./blob/main/esp32-wroom-32b.jpg?raw=true)

**LoRa Radio (LoRa SX1276) to the Rescue**

I ordered two sets of LoRa SX1276 radios from mainland China, specifically the Waveshare brand, model SX1276 DTU HF. When I received them, I was surprised by their size—they were so small they didn’t even cover the length of my palm, and the antenna was equally tiny, shorter than a cubit. I wondered if they could really handle transmission and reception. The distance from the water level station to the office was about 1.5 km, with several obstacles in between, as it was located in an industrial estate. According to the specs, this model can transmit up to 5 km.

I started by testing data reception, and it worked. Next, I connected the LoRa radio to the serial port of the water level meter. At the office, I connected the prepared microcontroller circuit to the LoRa radio. To my relief, the water level data was transmitted smoothly into the circuit.

An added benefit is that with the microcontroller circuit located in the office, I can easily edit and upload programs to the board whenever needed.


![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak./blob/main/waveshare_lora_sx1276_dtu_hf.webp?raw=true)

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak./blob/main/lora_sx1276_valeport_tidemaster_and_esp32.jpg?raw=true)

**Wiring Diagram**

This is the circuit diagram showing the connection between the microcontroller board and the LoRa radio. I used Fritzing to create the diagram, but since the Fritzing library doesn’t include the extension board I used, I substituted a breadboard to clearly display the 5V and ground connections.

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak./blob/main/ESP32 Tide Station_bb.jpg?raw=true)
