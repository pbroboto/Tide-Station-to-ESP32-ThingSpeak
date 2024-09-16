#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "time.h"
#include <ThingSpeak.h>
#include <TinyNMEA.h>
//#include <NTPClient.h>
//#include <WiFiUdp.h>
//Serial UART connect to Arduino Mega 2560
#define RXD2 16
#define TXD2 17

//Connect to SD Card
//MicroSD card module	ESP32
//5V	    5V
//CS	    GPIO 5
//MOSI	  GPIO 23
//CLK	    GPIO 18
//MISO	  GPIO 19
//GND	    GND

//connect to LCD LiquidCrystal I2C
//I2C LCD	  ESP32
//GND	      GND
//VCC	      5V
//SDA	      GPIO 21
//SCL	      GPIO 22

char filename[13] = "00000000.txt";
File dataFile;

// Define NTP Client to get time
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

const char *ssid = "******"; //Input your WiFi SSID
const char *password = "*******"; //Input your password

WiFiClient client;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;

//ThingSpeak
unsigned long myChannelNumber = 1111111;  //Input your own User ID
const char *myWriteAPIKey = "****************"; //Input your own Thingspeak API Key

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
String messageStatic = "ESP32 Tide Master";
String messageToScroll = "This is a ESP32 Microcontroller for Tide Station.";

//TinyNMEA by prajuab riabroy, original by Mikal Heart
TinyNMEA tide;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);  // Serial2.begin(38400) => not work
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(messageStatic);
  //scrollText(1, messageToScroll, 250, lcdColumns);
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  lcd.setCursor(0, 1);
  lcd.print("Wifi Connected.    ");

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  initSDCard();
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  bool newData = false;
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    lcd.setCursor(0, 1);
    lcd.print("Wifi Not Connected.");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      delay(5000);
    }
    Serial.println("\nConnected.");
    lcd.setCursor(0, 1);
    lcd.print("Wifi Connected.    ");
  }

  // set cursor to first column, first row
  //lcd.setCursor(0, 0);
  //lcd.print(messageStatic);
  //scrollText(1, messageToScroll, 250, lcdColumns);
  //getfilename();
  //createfilename();
  while (Serial2.available()) {
    char c = Serial2.read();
    //Serial.print("char =");
    //Serial.println(c);
    if (tide.encode(c)) {
      newData = true;
    }
    if (newData) {
      float flat, flon;
      char tid[9];
      char date[11], time[9], height[7], units[2], stdev[7], intvolt[7], extvolt[7];
      long age;
      //myFile = SD.open("gnss.txt", FILE_WRITE);  // เปิดไฟล์ที่ชื่อ test.txt เพื่อเขียนข้อมูล โหมด FILE_WRITE
      tide.get_tide_id(tid);
      Serial.print(" Tide ID=");
      Serial.println(tid);
      tide.get_tide_date(date);
      Serial.print(" Tide Date=");
      Serial.println(date);
      tide.get_tide_time(time);
      Serial.print(" Tide Time=");
      Serial.println(time);
      tide.get_tide_height(height);
      Serial.print(" Tide Height=");
      Serial.println(height);
      tide.get_tide_height_units(units);
      Serial.print(" Tide Height Units=");
      Serial.println(units);
      tide.get_tide_height_stdev(stdev);
      Serial.print(" Tide Height Stdev=");
      Serial.println(stdev);
      tide.get_tide_int_voltage(intvolt);
      Serial.print(" Tide Internal Voltage=");
      Serial.println(intvolt);
      tide.get_tide_ext_voltage(extvolt);
      Serial.print(" Tide External Voltage=");
      Serial.println(extvolt);
      lcdPrintTide(date, time, height);

      // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
      // pieces of information in a channel.  Here, we write to field 1.

      ThingSpeak.setField(1, tid);
      ThingSpeak.setField(2, date);
      ThingSpeak.setField(3, time);
      float h = strtod(height, NULL);
      char str[16];
      snprintf(str, sizeof(str), "%.3f", h);      
      ThingSpeak.setField(4, str);  //display "NaN" if uncomment.
      ThingSpeak.setField(5, units);
      ThingSpeak.setField(6, stdev);
      ThingSpeak.setField(7, intvolt);
      ThingSpeak.setField(8, extvolt);
      if (WiFi.status() == WL_CONNECTED) {
        // ปัญหาเมื่อส่งเป็น string ไปเว็บดันต่อท้ายต่อ m ต่อท้ายสตริงให้ ทำให้แสดงผล widget "NaN"
        // ต้องบังคับส่งเป็น float ถึงแก้ปัญหาได้
        /*int i = ThingSpeak.writeField(myChannelNumber, 1, tid, myWriteAPIKey);        
        i = ThingSpeak.writeField(myChannelNumber, 2, date, myWriteAPIKey);        
        i = ThingSpeak.writeField(myChannelNumber, 3, time, myWriteAPIKey); 
        i = ThingSpeak.writeField(myChannelNumber, 4, h, myWriteAPIKey);           
        i = ThingSpeak.writeField(myChannelNumber, 5, units, myWriteAPIKey);        
        i = ThingSpeak.writeField(myChannelNumber, 6, stdev, myWriteAPIKey);
        i = ThingSpeak.writeField(myChannelNumber, 7, intvolt, myWriteAPIKey);        
        i = ThingSpeak.writeField(myChannelNumber, 8, extvolt, myWriteAPIKey); */
        int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        if (x == 200) {
          Serial.println("Channel update successful.");
        } else {
          Serial.println("Problem updating channel. HTTP error code " + String(x));
        }
        //}
        //myFile.close();
      }
      newData = false;
    }
  }
}

void initSDCard() {
  if (!SD.begin(5)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  listDir(SD, "/", 0);
  createDir(SD, "/mydir");
  listDir(SD, "/", 0);
  removeDir(SD, "/mydir");
  listDir(SD, "/", 2);
  writeFile(SD, "/hello.txt", "Hello ");
  appendFile(SD, "/hello.txt", "World!\n");
  readFile(SD, "/hello.txt");
  deleteFile(SD, "/foo.txt");
  renameFile(SD, "/hello.txt", "/foo.txt");
  readFile(SD, "/foo.txt");
  testFileIO(SD, "/test.txt");
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char *path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char *path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

// Function to scroll text
// The function acepts the following arguments:
// row: row number where the text will be displayed
// message: message to scroll
// delayTime: delay between each character shifting
// lcdColumns: number of columns of your LCD
void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i = 0; i < lcdColumns; i++) {
    message = " " + message;
  }
  message = message + " ";
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

//Modified from Nick Pyder's code at Arduino forum.
void getfilename() {
  time_t t = time(nullptr);
  struct tm *tm = localtime(&t);
  if (tm->tm_year >= 120) {  // check if we have a valid time
    int second = tm->tm_sec;
    int minute = tm->tm_min;
    int hour = tm->tm_hour;
    int year = 1900 + tm->tm_year;
    int month = tm->tm_mon + 1;
    int day = tm->tm_mday;
    /*Serial.print(" year = ");
    Serial.println(year);  //based on year 1900
    Serial.print(" month = ");
    Serial.println(month);  //based count from zero : 0-11
    Serial.print(" day = ");
    Serial.println(day);  //based on one (1) : 1-31*/

    filename[0] = (year / 1000) % 10 + '0';  //To get 1st digit from year()
    filename[1] = (year / 100) % 10 + '0';   //To get 2nd digit from year()
    filename[2] = (year / 10) % 10 + '0';    //To get 3rd digit from year()
    filename[3] = year % 10 + '0';           //To get 4th digit from year()
    filename[4] = month / 10 + '0';          //To get 1st digit from month()
    filename[5] = month % 10 + '0';          //To get 2nd digit from month()
    filename[6] = day / 10 + '0';            //To get 1st digit from day()
    filename[7] = day % 10 + '0';            //To get 2nd digit from day()
    Serial.print("Using file name: ");
    Serial.println(filename);
  }
}

void serialPrintTide(char *tid, char *date, char *time, char *height, char *stdev, char *units, char *intvolt, char *extvolt) {
  Serial.print(" Tide ID=");
  Serial.println(tid);
  Serial.print(" Tide Date=");
  Serial.println(date);
  Serial.print(" Tide Time=");
  Serial.println(time);
  Serial.print(" Tide Height=");
  Serial.println(height);
  Serial.print(" Tide Height Stdev=");
  Serial.println(stdev);
  Serial.print(" Tide Height Units=");
  Serial.println(units);
  Serial.print(" Tide Internal Voltage=");
  Serial.println(intvolt);
  Serial.print(" Tide External Voltage=");
  Serial.println(extvolt);
}

void lcdPrintTide(char *date, char *time, char *height) {
  //lcd.backlight();
  lcd.setCursor(0, 2);
  insertChar(date, "/", 4);
  insertChar(date, "/", 7);
  lcd.print(date);
  lcd.setCursor(0, 3);
  insertChar(time, ":", 2);
  insertChar(time, ":", 5);
  lcd.print(time);
  lcd.setCursor(9, 3);
  strncat(height, "m", 1);
  lcd.print(height);
  delay(5000);
  //lcd.noBacklight();
}

void serialPrintStats(unsigned long chars, unsigned short sentences, unsigned short failed) {
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CHECKSUM ERR=");
  Serial.println(failed);
}

// creates a new file name each day if it doesn't already exist
void createfilename() {
  //Check if file exists?
  /*if (!SD.exists(filename)) {
    Serial.println("exists");
    Serial.println("appending to existing file");
    } else {
    Serial.println("doesn't exist");
    Serial.println("Creating new file.");
    Serial.println(filename);*/
  dataFile = SD.open(filename, FILE_WRITE);
  dataFile.close();
}


/*void readRTCDateTime() {
  DateTime now = RTC.now();
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}*/

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

// inserts into subject[] at position pos
void insertChar(char subject[], const char insert[], int pos) {
  char buf[50];
  strncpy(buf, subject, pos);
  //printf("%s\n", buf);
  buf[pos] = '\0';
  strcat(buf, insert);
  //printf("%s\n", buf);
  strcat(buf, subject + pos);
  strcpy(subject, buf);
  //printf("%s\n", buf);
}
