**Tide Station to ESP32 ThingSpeak**

The Tide Station utilizes the Valeport TideMaster system to monitor water levels. The water level sensor measures pressure and converts it into water level readings. The sensor is connected to a Scannex device, which collects the data and transmits it to the cloud using a SIM card. The SIM card is leased, and the service fee is approximately 1,000 Baht per month.

Cloud services are provided by a vendor who requires the purchase of their cloud package, which costs around 80,000 Baht per year. After the third year, it’s worth reconsidering whether to continue renting, as the cost is quite high. The service itself is fairly basic, offering unlimited downloads of historical data and access to real-time data. From a DIY perspective, it’s possible to replicate this service at a lower cost.

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak/blob/main/Tide5-768x1024.webp?raw=true)

**Selecting a Microcontroller Board**

The next step was to choose a microcontroller that could connect to the internet and receive water level data via a UART serial connection—both basic features for most microcontrollers. The board would then send the data to the cloud. Initially, I considered the Pico W, but in my experience, it had difficulties maintaining a stable Wi-Fi connection, even when placed close to the router. I then looked into the Arduino Mega2560 Wireless R3 and the ESP32. After testing both, I found that I preferred the ESP32 for its performance.

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak/blob/main/esp32-wroom-32b.jpg?raw=true)

**LoRa Radio (LoRa SX1276) to the Rescue**

I ordered two sets of LoRa SX1276 radios from mainland China, specifically the Waveshare brand, model SX1276 DTU HF. When I received them, I was surprised by their size—they were so small they didn’t even cover the length of my palm, and the antenna was equally tiny, shorter than a cubit. I wondered if they could really handle transmission and reception. The distance from the water level station to the office was about 1.5 km, with several obstacles in between, as it was located in an industrial estate. According to the specs, this model can transmit up to 5 km.

I started by testing data reception, and it worked. Next, I connected the LoRa radio to the serial port of the water level meter. At the office, I connected the prepared microcontroller circuit to the LoRa radio. To my relief, the water level data was transmitted smoothly into the circuit.

An added benefit is that with the microcontroller circuit located in the office, I can easily edit and upload programs to the board whenever needed.


![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak/blob/main/waveshare_lora_sx1276_dtu_hf.webp?raw=true)

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak/blob/main/lora_sx1276_valeport_tidemaster_and_esp32.jpg?raw=true)

**Wiring Diagram**

This is the circuit diagram showing the connection between the microcontroller board and the LoRa radio. I used Fritzing to create the diagram, but since the Fritzing library doesn’t include the extension board I used, I substituted a breadboard to clearly display the 5V and ground connections.

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak/blob/main/ESP32_Tide_Station_bb.jpg?raw=true)

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak/blob/main/esp32_thingspeak_wiring_diagram.webp?raw=true)

![alt text](https://github.com/pbroboto/Tide-Station-to-ESP32-ThingSpeak/blob/main/esp32_thingspeak_box.webp?raw=true)

**Integrated Development Environment (IDE)**

For programming, I used the Arduino IDE, which is based on the C language. It offers various libraries to easily connect to peripherals such as the LCD screen, SD card, Wi-Fi, and Thingspeak. Since the Arduino IDE supports a wide range of boards, it’s important to select the correct one. In this case, I chose the ESP32 Dev Module.

**Capturing Data from the LoRa Radio**

Capturing string data from the LoRa radio is straightforward. Each character is received and combined into a complete word. If the data matches the NMEA sentence pattern, the program continues capturing until the end of the sentence, verifying the checksum to ensure the data was transmitted correctly. If the checksum matches, the sentence is considered valid.

For processing NMEA strings, I modified the TinyGPSPlus library, originally written by Mikal Hart, by adding support for the $PVTMA, $SDDBT, and $SDDPT sentences. I then renamed the library to TinyNMEA.

$PVTMA,01,20240912,150400,2.270,0.009,m,6.045,13.485,0,3.410,,*16
$PVTMA,01,20240912,150500,2.272,0.008,m,6.045,13.485,0,3.412,,*16
$PVTMA,01,20240912,150600,2.276,0.009,m,6.045,13.492,0,3.415,,*11
$PVTMA,01,20240912,150700,2.279,0.011,m,6.045,13.485,0,3.419,,*1c
$PVTMA,01,20240912,150800,2.283,0.009,m,6.049,13.477,0,3.422,,*16

**ThingSpeak Cloud Project**

ThingSpeak is a web application that provides cloud data collection services for sensor data via a microcontroller board. It allows users to send and store values in a cloud-based area that is accessible for various uses. Additionally, data can be accessed from any web browser with an internet connection or through a mobile app, making it a convenient solution for IoT (Internet of Things) applications.

To get started with ThingSpeak, register by visiting ThingSpeak.com and clicking the “Get Started For Free” button. Complete the registration form to activate your account. Once registered, sign in to create a channel by clicking My Channels and selecting Create New Channel. Make sure to note your User ID, Channel ID, and API Key, as these are needed for your program. Detailed instructions for creating a channel can be found online in both Thai and English.

ThingSpeak offers both free and paid plans. The free plan limits data logging to a maximum rate of once every 15 seconds.

For this project, water level readings from the water station will be sent to ThingSpeak every minute. The data includes 8 fields: Water Level Station ID, Date, Time, Water Level, Units, Standard Deviation, Internal Voltage, and External Voltage. Since the water level station sends data once per minute, this meets the requirements of the free service plan.

**Checking Data on ThingSpeak Cloud**

To view your data on ThingSpeak, you can use a web browser or a mobile app. For example, on my Android phone, I installed the ThingView Free and ThingShow apps. These apps are user-friendly; simply enter your User ID to see the results displayed according to the format you've set up in ThingSpeak. While the apps do have ads, they are not intrusive.

**Conclusion**

In summary, the cost of creating a DIY microcontroller project to send water level data to the cloud is approximately 500 Baht for the materials, 2,000 Baht for the LoRa radio, and 1,500 Baht for a new radio antenna to enhance transmission, totaling around 4,000 Baht. The cloud service itself incurs no additional charges. This is a significant saving compared to renting a cloud service, which costs over 80,000 Baht per year. Stay tuned for the next article for more details.
