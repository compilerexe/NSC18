#include <Wire.h>
#include "LiquidCrystal_I2C.h"
#include <SPI.h>
#include "RTClib.h"
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <Esp.h>
#include <EEPROM.h>

#define DHTPIN      D4
#define DHTTYPE     DHT22

#define BTN_LEFT    D5
#define BTN_RIGHT   D6
#define BTN_CENTER  D7
#define BTN_BACK    D8

#define I2C_SCL     D1
#define I2C_SDA     D2

char* boot_mode           = ""; // if setting don't forgot remove.
char* directory           = ""; // directory in current
char* select_current      = "Select timer"; // first select

/* Setting Timer */
int reboot_time           = 6; // second
int move_right            = 0;
int hour                  = 0;
int minute                = 0;
char display_h[2];
char display_m[2];
char display_reboot[2];

/* Dht22  */
int dht_counting_fail     = 0;

/* detect button */
char* run_left            = "";
char* run_right           = "";
char* run_center          = "";
char* run_back            = "";

// const char* ssid          = "apple";
// const char* password      = "0845178386";

const char* ssid          = "WIFI-MAXNET";
const char* password      = "macrol3ios";

// const char* ssid          = "CMMC-MEMBER";
// const char* password      = "devicenetwork";

const char* host          = "api.thingspeak.com";
const char* writeAPIKey   = "2DTO3Y4QC93L2ZR9";
const int httpPort        = 80;

EspClass Esp;

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 RTC;
LiquidCrystal_I2C lcd(0x27, 20, 4);

unsigned long previousMillis  = 0;
unsigned long currentMillis;
const long    interval        = 2000; 

/* ICON */
byte ICON_TIME[8] {B11111,B11001,B11001,B01110,B01110,B10011,B10011,B11111};
byte ICON_TEMP[8] {B00100,B01010,B01010,B01110,B01110,B11111,B11111,B01110};
byte ICON_HUMID[8] {B00100,B00100,B01010,B01010,B10001,B10001,B10001,B01110};
byte ICON_SELECT[8] {B00100,B00110,B00111,B11111,B11111,B00111,B00110,B00100};

void setup()
{

  Serial.begin(115200);

  pinMode(BTN_LEFT, INPUT); // detect boot

  if (digitalRead(BTN_LEFT) == 1 || boot_mode == "setting") {
    boot_mode = "setting";

    pinMode(BTN_RIGHT,  INPUT); // RIGHT
    pinMode(BTN_CENTER, INPUT); // ENTER
    pinMode(BTN_BACK,   INPUT); // BACK

    EEPROM.begin(512);
    lcd.begin();
    lcd.createChar(4, ICON_SELECT);
    lcd.clear();

    Serial.println("Boot Setting");
  } else {

    pinMode(BTN_LEFT,   OUTPUT);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    EEPROM.begin(512);
    dht.begin();
    delay(100);
    lcd.begin();
    delay(100);
    RTC.begin();
  //  RTC.adjust(DateTime(__DATE__, __TIME__));

    if (! RTC.isrunning()) {
      Serial.println("RTC is NOT running!");
      RTC.adjust(DateTime(__DATE__, __TIME__));
    }

    DateTime now = RTC.now();
  //  RTC.setAlarm1Simple(23, 9);

  //  RTC.turnOnAlarm(1);

  //  if (RTC.checkAlarmEnabled(1)) {
  //    Serial.println("Alarm Enabled");
  //  }

    lcd.createChar(1, ICON_TIME);
    lcd.createChar(2, ICON_TEMP);
    lcd.createChar(3, ICON_HUMID);
    lcd.createChar(4, ICON_SELECT);
    lcd.clear();

  }
  
}

void SENT_THINGSPEAK(float temp, float humid) {
  String url = "/update?api_key=";
  url += writeAPIKey;
  url += "&field1=";
  url += temp;
  url += "&field2=";
  url += humid;
  
  if (!client.connect(host, httpPort)) {
    Serial.println("retry connection");
    return;
  } else {
    Serial.println("connection success");
  }

  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
   "Host: " + host + "\r\n" + 
   "Connection: close\r\n\r\n");
  
}

void* FILTER_NUMBER(int n) {
  char* v;
  switch (n) {
    case 1 : v = "01"; break;
    case 2 : v = "02"; break;
    case 3 : v = "03"; break;
    case 4 : v = "04"; break;
    case 5 : v = "05"; break;
    case 6 : v = "06"; break;
    case 7 : v = "07"; break;
    case 8 : v = "08"; break;
    case 9 : v = "09"; break;
    case 0 : v = "00"; break;
  }
  return v;
}

void LCD_DISPLAY(float temp, float humid) {

  DateTime now = RTC.now();
  
  lcd.setCursor(0, 0);

  if (int(now.day()) <= 9) {
    lcd.print((char*)FILTER_NUMBER(int(now.day())));
  } else {
    lcd.print(now.day(), DEC);
  }

  lcd.print("/");

  if (int(now.month()) <= 9) {
    lcd.print((char*)FILTER_NUMBER(int(now.month())));
  } else {
    lcd.print(now.month(), DEC);
  }

  lcd.print("/");

  lcd.print(now.year(), DEC);
  lcd.print("  ");

  if (int(now.hour()) <= 9) {
    lcd.print((char*)FILTER_NUMBER(int(now.hour())));
  } else {
    lcd.print(now.hour(), DEC);
  }

  lcd.print(":");
  
  if (int(now.minute()) <= 9) {
    lcd.print((char*)FILTER_NUMBER(int(now.minute())));
  } else {
    lcd.print(now.minute(), DEC);
  }
  
  lcd.print(" ");
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.write(2);
  lcd.print(" Temp     ");
  lcd.print(temp);
  lcd.print(" C");
  lcd.setCursor(0, 2);
  lcd.print(" ");
  lcd.write(3);
  lcd.print(" Humid    ");
  lcd.print(humid);
  lcd.print(" %");
}

void DEBUG(float temp, float humid) {

  DateTime now = RTC.now();
  
  Serial.print("Temp : ");
  Serial.println(temp);
  Serial.print("Humid : ");
  Serial.println(humid);
  Serial.print("Day : ");
  Serial.println(now.day(), DEC);
  Serial.print("Month : ");
  Serial.println(now.month(), DEC);
  Serial.print("Year : ");
  Serial.println(now.year(), DEC);
  Serial.print("H : ");
  Serial.println(now.hour(), DEC);
  Serial.print("M : ");
  Serial.println(now.minute(), DEC);
}

void FUNCTION_WRITE_EEPROM() {
  reboot_time--;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Save success.");
  lcd.setCursor(0,2);
  lcd.print("Reboot in ");
  sprintf(display_reboot, "%d", reboot_time);
  lcd.print(display_reboot);
  lcd.print(" second");
  if (reboot_time == 0) {
    lcd.clear();
    Esp.reset();
  }
  delay(1000);
}

void FUNCTION_READ_EEPROM() {
  Serial.println();
  Serial.print("Read eeprom");
  Serial.println();
  Serial.print("Hour : ");
  Serial.println(EEPROM.read(256));
  Serial.print("Minute : ");
  Serial.println(EEPROM.read(257));
}

void FUNCTION_NORMAL() {

  currentMillis = millis();
  
  FUNCTION_READ_EEPROM();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    int humid = dht.readHumidity();
    int temp  = dht.readTemperature();
    
  //      if (RTC.checkIfAlarm(1)) {
  //        Serial.println("Alarm Triggered");
  //      }

    Serial.println();
    
    lcd.clear();
    
    if (isnan(h) || isnan(t)) {

      if (dht_counting_fail == 20) {
        Esp.reset();
      }

      dht_counting_fail++;
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    LCD_DISPLAY(t, h);

    SENT_THINGSPEAK(t, h);
    
    DEBUG(t, h);

  }
}

void FUNCTION_SETTING() {
  directory = "root";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Slect Mode");
  lcd.setCursor(0, 2);
  lcd.write(4);
  lcd.print(" Set Timer"); 

}

void FUNCTION_SET_TIMER() {
  directory = "root/set_timer";
  
  if (reboot_time < 6) {
    FUNCTION_WRITE_EEPROM();
  } else {

    if (select_current == "Select hour" || select_current == "Set hour is active.") {
      lcd.setCursor(0,0);
      lcd.print("Set Timer (24 Hour)");
      lcd.setCursor(0,1);
      lcd.print("  Day    : auto");
      lcd.setCursor(0,2);
      lcd.write(4);
      lcd.print(" Hour   : ");
      sprintf(display_h, "%d", hour);
      lcd.print(display_h);
      lcd.setCursor(0,3);
      lcd.print("  Minute : ");
      sprintf(display_m, "%d", minute);
      lcd.print(display_m);

      if (minute > 9) {
        lcd.print("   ");
      } else {
        lcd.print("    ");
      }

      lcd.print("Save");
    }

    if (select_current == "Select minute" || select_current == "Set minute is active.") {
      lcd.setCursor(0,0);
      lcd.print("Set Timer (24 Hour)");
      lcd.setCursor(0,1);
      lcd.print("  Day    : auto");
      lcd.setCursor(0,2);
      lcd.print("  Hour   : ");
      sprintf(display_h, "%d", hour);
      lcd.print(display_h);
      lcd.setCursor(0,3);
      lcd.write(4);
      lcd.print(" Minute : ");
      sprintf(display_m, "%d", minute);
      lcd.print(display_m);
      
      if (minute > 9) {
        lcd.print("   ");
      } else {
        lcd.print("    ");
      }
      
      lcd.print("Save");
    }

    if (select_current == "Select save") {
      lcd.setCursor(0,0);
      lcd.print("Set Timer (24 Hour)");
      lcd.setCursor(0,1);
      lcd.print("  Day    : auto");
      lcd.setCursor(0,2);
      lcd.print("  Hour   : ");
      sprintf(display_h, "%d", hour);
      lcd.print(display_h);
      lcd.setCursor(0,3);
      lcd.print("  Minute : ");
      sprintf(display_m, "%d", minute);
      lcd.print(display_m);
      
      if (minute > 9) {
        lcd.print("  ");
      } else {
        lcd.print("  ");
      }

      lcd.write(4);
      lcd.print(" Save");
      Serial.println("test");
    }

  } // End check reboot time

}

void BTN_STATE() {
  
  if (digitalRead(BTN_LEFT) == 1) {

    run_left = "detect_left";

  } else {

    if (run_left == "detect_left") { // leave button for run
      
      Serial.println("LEFT");

      if (directory == "root/set_timer") {
        
        if (select_current == "Set hour is active.") {
          hour--;
          if (hour < 0) {
              hour = 23;
          }
        }

        if (select_current == "Set minute is active.") {
          minute--;
          if (minute < 0) {
              minute = 59;
          }
        }

        lcd.clear();
        FUNCTION_SET_TIMER();
      }

      run_left = "";

    }

  }

  if (digitalRead(BTN_RIGHT) == 1) {

    run_right = "detect_right";

  } else {

    if (run_right == "detect_right") { // leave button for run
      
      Serial.println("RIGHT");

      if (directory == "root/set_timer") {
        
        if (select_current != "Set hour is active." && select_current != "Set minute is active.") {
          move_right++;

          if (move_right == 3) {
              select_current = "Select hour";
              move_right = 0;
          }

          if (move_right == 2) {
              select_current = "Select save";
          }

          if (move_right == 1) {
              select_current = "Select minute";
          }
        }

        if (select_current == "Set hour is active.") {
          hour++;
          if (hour > 23) {
              hour = 0;
          }
        }

        if (select_current == "Set minute is active.") {
          minute++;
          if (minute > 59) {
              minute = 0;
          }
        }

        lcd.clear();
        FUNCTION_SET_TIMER();
      }

      run_right = "";

    }

  }

  if (digitalRead(BTN_CENTER) == 1) {
    
    run_center = "detect_center";

  } else {

    if (run_center == "detect_center") { // leave button for run
      
      Serial.println("CENTER");
      
      if (directory == "root/set_timer") {

        if (select_current == "Select hour") {
          select_current = "Set hour is active.";
        }

        if (select_current == "Select minute") {
          select_current = "Set minute is active.";
        }

        if (select_current == "Select save") {
          select_current = "Save data to eeprom.";
          Serial.println("Write");
          EEPROM.write(256, hour);
          EEPROM.write(257, minute);
          EEPROM.commit();
          FUNCTION_WRITE_EEPROM();
        }

      }

      if (select_current == "Select timer") { //select timer
        select_current = "Select hour";
        lcd.clear();
        FUNCTION_SET_TIMER();
      }

      run_center = "";

    }

  }

  if (digitalRead(BTN_BACK) == 1) {
    
    run_back = "detect_back";

  } else {

    if (run_back == "detect_back") { // leave button for run
      
      Serial.println("BACK");
      
      if (directory == "root/set_timer") {

        if (select_current == "Set hour is active.") {
          select_current = "Select hour";
          lcd.clear();
          FUNCTION_SET_TIMER();
        }

        if (select_current == "Set minute is active.") {
          select_current = "Select minute";
          lcd.clear();
          FUNCTION_SET_TIMER();
        }

      }
   
      run_back = "";

    }

  }

}

void get_heap() {
  Serial.print("Heap : ");
  Serial.println(Esp.getFreeHeap());
}

void loop() {

  if (boot_mode == "setting") {

    // get_heap();

    if (reboot_time < 6) {
      FUNCTION_WRITE_EEPROM();
    } else {

      if (directory == "") {
        select_current = "Select timer";
        FUNCTION_SETTING();
      }

      BTN_STATE();

    }

    Serial.println("==============");
    Serial.print("directory : ");
    Serial.println(directory);
    Serial.print("select current : ");
    Serial.println(select_current);
    Serial.print("move right : ");
    Serial.println(move_right);
    Serial.println("==============");

  } else {

    // get_heap();

    FUNCTION_NORMAL();

  } // END FUNCTION_STATE_MODE
  
  

  delay(100);
}
