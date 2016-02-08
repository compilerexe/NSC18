#include <Wire.h>
#include "LiquidCrystal_I2C.h"
#include <SPI.h>
#include "RTClib.h"
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <Esp.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define DHTPIN            D4
#define DHTTYPE           DHT22

#define ACTIVE_RELEY      D5

#define BTN_LEFT          D5
#define BTN_RIGHT         D6
#define BTN_CENTER        D7
#define BTN_BACK          D8

#define I2C_SCL           D1
#define I2C_SDA           D2

char* boot_mode           = ""; // if setting don't forgot remove.
char* directory           = ""; // directory in current
char* select_current      = "Select timer"; // first select

/* Setting Timer */
const unsigned int eeprom_addr_h         = 1;
const unsigned int eeprom_addr_after_h   = 2;
const unsigned int eeprom_addr_m         = 3;
const unsigned int eeprom_addr_after_m   = 4;

unsigned int reboot_time           = 6; // second
unsigned int move_right            = 0;
unsigned int hour                  = 0;
unsigned int minute                = 0;
unsigned int after_hour            = 0;
unsigned int after_minute          = 0;
char display_h[2];
char display_after_h[2];
char display_m[2];
char display_after_m[2];
char display_reboot[2];

/* Dht22  */
unsigned int dht_counting_fail     = 0;

/* detect button */
char* run_left            = "";
char* run_right           = "";
char* run_center          = "";
char* run_back            = "";

// const char* ssid          = "apple";
// const char* password      = "0845178386";

const char* ssid          = "";
const char* password      = "";

// const char* ssid          = "CMMC-MEMBER";
// const char* password      = "devicenetwork";

const char* host          = "api.thingspeak.com";
const char* writeAPIKey   = "2DTO3Y4QC93L2ZR9";
const unsigned int httpPort        = 80;

/*=== WiFiAccessPoint ===*/
const char* ssidAP = "NSC18-Main";
const char* ssidPass = "";
const char* ap_ip[4] = {"192", "168", "0", "100"};
const char* ap_subnet[4] = {"255", "255", "255", "0"};
const char* ap_gateway[4] = {"192", "168", "0", "1"};

String http_ssid = "";
String http_pass = "";

uint8_t state_internet = 0;

/*=======================*/

EspClass Esp;

WiFiClient client;
ESP8266WebServer server(80);
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

void wifi_ap() {
  
  WiFi.softAP(ssidAP);

  WiFi.softAPConfig(
    IPAddress(atoi(ap_ip[0]), atoi(ap_ip[1]), atoi(ap_ip[2]), atoi(ap_ip[3])),
    IPAddress(atoi(ap_gateway[0]), atoi(ap_gateway[1]), atoi(ap_gateway[2]), atoi(ap_gateway[3])),
    IPAddress(atoi(ap_subnet[0]), atoi(ap_subnet[1]), atoi(ap_subnet[2]), atoi(ap_subnet[3]))
  );
  Serial.println();
  Serial.print("WiFi AP : ");
  Serial.print(ssidAP);
  
  Serial.println();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP Address : ");
  Serial.println(myIP);
  
}

void autoConnect() {

  int address_eeprom = 0;

  ssid = http_ssid.c_str();
  password = http_pass.c_str();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());

  Serial.print("eeprom ssid : ");
  for (int i = 0; i < strlen(ssid); i++) {
    EEPROM.write(address_eeprom, ssid[i]);
    address_eeprom++;
    Serial.print(ssid[i]);
  }

  Serial.println();
  Serial.print("eeprom pass : ");
  for (int j = 0; j < strlen(password); j++) {
    EEPROM.write(address_eeprom, password[j]);
    address_eeprom++;
    Serial.print(password[j]);
  }

  EEPROM.write(256, strlen(ssid));
  EEPROM.write(257, strlen(password));
  EEPROM.commit();
  Esp.reset();

}

void webserver_display() {

  if (state_internet == 0) {
      String codeHtml = "\
      <html>\
        <head>\
          <meta name='viewport' content='initial-scale=1.0, user-scalable=no'>\
          <title>WiFi Config</title>\
        </head>\
        <body>\
          <form method='get'>\
            SSID&ensp;\
            <input type='text' name='SSID'>\
            </select><br><br>\
            PASS&ensp;<input type='text' name='password'><br><br>\
            <input type='submit' value='Connect'>\
          </form>\
        </body>\
      </html>";
  
      server.send(200, "text/html", codeHtml);
  }

  if (server.arg(0) != "" && server.arg(1) != "") {

    http_ssid = server.arg(0);
    http_pass = server.arg(1);

    autoConnect();

  }
    
}

void webserver_config() {
  
  server.on("/", webserver_display);
  server.begin();
  Serial.println("HTTP server started");
  
}

String decryption_ascii(char ascii_code) {

  String buffer_x = "";

  switch (ascii_code) {
    case 32 : buffer_x += " "; break;
    case 33 : buffer_x += "!"; break;
    case 34 : buffer_x += '"'; break;
    case 35 : buffer_x += "#"; break;
    case 36 : buffer_x += "$"; break;
    case 37 : buffer_x += "%"; break;
    case 38 : buffer_x += "&"; break;
    case 39 : buffer_x += "'"; break;
    case 40 : buffer_x += "("; break;
    case 41 : buffer_x += ")"; break;
    case 42 : buffer_x += "*"; break;
    case 43 : buffer_x += "+"; break;
    case 44 : buffer_x += ","; break;
    case 45 : buffer_x += "-"; break;
    case 46 : buffer_x += "."; break;
    case 47 : buffer_x += "/"; break;

    case 48 : buffer_x += "0"; break;
    case 49 : buffer_x += "1"; break;
    case 50 : buffer_x += "2"; break;
    case 51 : buffer_x += "3"; break;
    case 52 : buffer_x += "4"; break;
    case 53 : buffer_x += "5"; break;
    case 54 : buffer_x += "6"; break;
    case 55 : buffer_x += "7"; break;
    case 56 : buffer_x += "8"; break;
    case 57 : buffer_x += "9"; break;

    case 58 : buffer_x += ":"; break;
    case 59 : buffer_x += ";"; break;
    case 60 : buffer_x += "<"; break;

    case 61 : buffer_x += "="; break;
    case 62 : buffer_x += ">"; break;
    case 63 : buffer_x += "?"; break;
    case 64 : buffer_x += "@"; break;

    case 65 : buffer_x += "A"; break;
    case 66 : buffer_x += "B"; break;
    case 67 : buffer_x += "C"; break;
    case 68 : buffer_x += "D"; break;
    case 69 : buffer_x += "E"; break;
    case 70 : buffer_x += "F"; break;
    case 71 : buffer_x += "G"; break;
    case 72 : buffer_x += "H"; break;
    case 73 : buffer_x += "I"; break;
    case 74 : buffer_x += "J"; break;
    case 75 : buffer_x += "K"; break;
    case 76 : buffer_x += "L"; break;
    case 77 : buffer_x += "M"; break;
    case 78 : buffer_x += "N"; break;
    case 79 : buffer_x += "O"; break;
    case 80 : buffer_x += "P"; break;
    case 81 : buffer_x += "Q"; break;
    case 82 : buffer_x += "R"; break;
    case 83 : buffer_x += "S"; break;
    case 84 : buffer_x += "T"; break;
    case 85 : buffer_x += "U"; break;
    case 86 : buffer_x += "V"; break;
    case 87 : buffer_x += "W"; break;
    case 88 : buffer_x += "X"; break;
    case 89 : buffer_x += "Y"; break;
    case 90 : buffer_x += "Z"; break;
    
    case 97 : buffer_x += "a"; break;
    case 98 : buffer_x += "b"; break;
    case 99 : buffer_x += "c"; break;
    case 100 : buffer_x += "d"; break;
    case 101 : buffer_x += "e"; break;
    case 102 : buffer_x += "f"; break;
    case 103 : buffer_x += "g"; break;
    case 104 : buffer_x += "h"; break;
    case 105 : buffer_x += "i"; break;
    case 106 : buffer_x += "j"; break;
    case 107 : buffer_x += "k"; break;
    case 108 : buffer_x += "l"; break;
    case 109 : buffer_x += "m"; break;
    case 110 : buffer_x += "n"; break;
    case 111 : buffer_x += "o"; break;
    case 112 : buffer_x += "p"; break;
    case 113 : buffer_x += "q"; break;
    case 114 : buffer_x += "r"; break;
    case 115 : buffer_x += "s"; break;
    case 116 : buffer_x += "t"; break;
    case 117 : buffer_x += "u"; break;
    case 118 : buffer_x += "v"; break;
    case 119 : buffer_x += "w"; break;
    case 120 : buffer_x += "x"; break;
    case 121 : buffer_x += "y"; break;
    case 122 : buffer_x += "z"; break;

    case 123 : buffer_x += "{"; break;
    case 124 : buffer_x += "|"; break;
    case 125 : buffer_x += "}"; break;
    case 126 : buffer_x += "~"; break;
  }

  return buffer_x;

}

void setup()
{

  Serial.begin(115200);

  pinMode(BTN_LEFT,  INPUT);  // detect boot
  pinMode(BTN_RIGHT, INPUT); // detect boot setting wifi

  if (digitalRead(BTN_LEFT) == 1 || boot_mode == "setting") {
    boot_mode = "setting";

    //pinMode(BTN_RIGHT,  INPUT); // RIGHT
    pinMode(BTN_CENTER, INPUT); // ENTER
    pinMode(BTN_BACK,   INPUT); // BACK

    EEPROM.begin(512);
    lcd.begin();
    lcd.createChar(4, ICON_SELECT);
    lcd.clear();
    Serial.println();
    Serial.println();
    Serial.println("Boot setting");
  } else {

    if (digitalRead(BTN_RIGHT) == 1) {
      boot_mode = "setting wifi";
      EEPROM.begin(512);
      lcd.begin();
      lcd.clear();
      
      Serial.println();
      Serial.println();
      Serial.println("Boot setting wifi.");
      pinMode(BTN_LEFT,   OUTPUT);
      pinMode(BTN_RIGHT,  OUTPUT);
      
      wifi_ap();
      webserver_config();

    } else {

      EEPROM.begin(512);

      pinMode(BTN_LEFT,   OUTPUT);
      pinMode(BTN_RIGHT,  OUTPUT);

      digitalWrite(ACTIVE_RELEY, 1);

      //=======

      int len_ssid = EEPROM.read(256);
      int len_pass = EEPROM.read(257);

      int address_eeprom = 0;

      char buff_ssid[20];
      char buff_pass[20];

      Serial.println();

      String decryp_ssid = "";
      String decryp_pass = "";

      for (int i = 0; i < len_ssid; i++) {

        int ascii_code = EEPROM.read(address_eeprom);
        decryp_ssid += decryption_ascii(ascii_code);
        address_eeprom++;
      }

      for (int j = 0; j < len_pass; j++) {
        int ascii_code = EEPROM.read(address_eeprom);
        decryp_pass += decryption_ascii(ascii_code);
        address_eeprom++;
      }

      Serial.println();
      Serial.print("decryp ssid : ");
      Serial.print(decryp_ssid);
      Serial.print(" / len : ");
      Serial.print(decryp_ssid.length());
      Serial.println();
      Serial.print("decryp pass : ");
      Serial.print(decryp_pass);
      Serial.print(" / len : ");
      Serial.print(decryp_pass.length());
      Serial.println();

      // ssid = decryp_ssid.c_str();
      // password = decryp_ssid.c_str();

      //=======

      Serial.println();
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(decryp_ssid);
      
      WiFi.begin(decryp_ssid.c_str(), decryp_pass.c_str());
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      Serial.println("");
      Serial.println("WiFi connected");  
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      
      dht.begin();
      lcd.begin();
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

  DateTime now = RTC.now();

  if (EEPROM.read(eeprom_addr_h) > 0) {
    if (EEPROM.read(eeprom_addr_h) == now.hour() && EEPROM.read(eeprom_addr_m) == now.minute()) {
      digitalWrite(ACTIVE_RELEY, 0);
    }
    if (EEPROM.read(eeprom_addr_after_h) == now.hour() && EEPROM.read(eeprom_addr_after_m) == now.minute()) {
      digitalWrite(ACTIVE_RELEY, 1);
    }
  }

  Serial.println();
  Serial.print("Read eeprom");
  Serial.println();
  Serial.print("Hour : ");
  Serial.print(EEPROM.read(eeprom_addr_h));
  Serial.print("-");
  Serial.println(EEPROM.read(eeprom_addr_m));

  Serial.print("Hour : ");
  Serial.print(EEPROM.read(eeprom_addr_after_h));
  Serial.print("-");
  Serial.println(EEPROM.read(eeprom_addr_after_m));
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

void FUNCTION_SET_TIMER_HOUR() {
  lcd.setCursor(0,0);
  lcd.print("Set Timer (24 Hour)");
  lcd.setCursor(0,1);
  lcd.print(" Day   : every day");
  lcd.setCursor(0,2);
  lcd.write(4);

  lcd.print("Hour  : ");
  sprintf(display_h, "%d", hour);
  lcd.print(display_h);
  lcd.print("-");
  sprintf(display_after_h, "%d", after_hour);
  lcd.print(display_after_h);

  lcd.setCursor(0,3);
  lcd.print(" Minute: ");
  sprintf(display_m, "%d", minute);
  lcd.print(display_m);
  lcd.print("-");
  sprintf(display_after_m, "%d", after_minute);
  lcd.print(display_after_m);

  if (minute >= 10 && after_minute < 10) {
    lcd.print("   ");
  }

  if (minute >= 10 && after_minute >= 10) {
    lcd.print("  ");
  }

  if (minute < 10 && after_minute < 10) {
    lcd.print("    ");
  }

  lcd.print("Save");
}

void FUNCTION_SET_TIMER_MINUTE() {
  lcd.setCursor(0,0);
  lcd.print("Set Timer (24 Hour)");
  lcd.setCursor(0,1);
  lcd.print(" Day   : every day");
  lcd.setCursor(0,2);
  
  lcd.print(" Hour  : ");
  sprintf(display_h, "%d", hour);
  lcd.print(display_h);
  lcd.print("-");
  sprintf(display_after_h, "%d", after_hour);
  lcd.print(display_after_h);

  lcd.setCursor(0,3);
  lcd.write(4);
  lcd.print("Minute: ");
  sprintf(display_m, "%d", minute);
  lcd.print(display_m);
  lcd.print("-");
  sprintf(display_after_m, "%d", after_minute);
  lcd.print(display_after_m);
  
  if (minute >= 10 && after_minute < 10) {
    lcd.print("   ");
  }

  if (minute >= 10 && after_minute >= 10) {
    lcd.print("  ");
  }

  if (minute < 10 && after_minute < 10) {
    lcd.print("    ");
  }

  lcd.print("Save");
}

void FUNCTION_SET_TIMER_SAVE() {
  lcd.setCursor(0,0);
  lcd.print("Set Timer (24 Hour)");
  lcd.setCursor(0,1);
  lcd.print(" Day   : every day");
  lcd.setCursor(0,2);

  lcd.print(" Hour  : ");
  sprintf(display_h, "%d", hour);
  lcd.print(display_h);
  lcd.print("-");
  sprintf(display_after_h, "%d", after_hour);
  lcd.print(display_after_h);

  lcd.setCursor(0,3);
  lcd.print(" Minute: ");
  sprintf(display_m, "%d", minute);
  lcd.print(display_m);
  lcd.print("-");
  sprintf(display_after_m, "%d", after_minute);
  lcd.print(display_after_m);
  
  if (minute >= 10 && after_minute < 10) {
    lcd.print("   ");
  }

  if (minute >= 10 && after_minute >= 10) {
    lcd.print(" ");
  }

  if (minute < 10 && after_minute < 10) {
    lcd.print("   ");
  }

  lcd.write(4);
  lcd.print("Save");
}

void FUNCTION_SET_TIMER() {
  directory = "root/set_timer";
  
  if (reboot_time < 6) {
    FUNCTION_WRITE_EEPROM();
  } else {

    if (select_current == "Select hour" || select_current == "Set hour is active.") {
      FUNCTION_SET_TIMER_HOUR();
    }

    if (select_current == "Set after hour is active.") {
      FUNCTION_SET_TIMER_HOUR();
    }

    if (select_current == "Select minute" || select_current == "Set minute is active.") {
      FUNCTION_SET_TIMER_MINUTE();
    }

    if (select_current == "Set after minute is active.") {
      FUNCTION_SET_TIMER_MINUTE();
    }

    if (select_current == "Select save") {
      FUNCTION_SET_TIMER_SAVE();
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

        if (select_current == "Set after hour is active.") {
          after_hour--;
          if (after_hour < 0) {
              after_hour = 23;
          }
        }

        if (select_current == "Set minute is active.") {
          minute--;
          if (minute < 0) {
              minute = 59;
          }
        }

        if (select_current == "Set after minute is active.") {
          after_minute--;
          if (after_minute < 0) {
              after_minute = 59;
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
          if (select_current != "Set after hour is active." && select_current != "Set after minute is active.") {
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
        }

        if (select_current == "Set hour is active.") {
          hour++;
          if (hour > 23) {
              hour = 0;
          }
        }

        if (select_current == "Set after hour is active.") {
          after_hour++;
          if (after_hour > 23) {
              after_hour = 0;
          }
        }

        if (select_current == "Set minute is active.") {
          minute++;
          if (minute > 59) {
              minute = 0;
          }
        }

        if (select_current == "Set after minute is active.") {
          after_minute++;
          if (after_minute > 59) {
              after_minute = 0;
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

        if (select_current == "Set hour is active.") {
          select_current = "Set after hour is active.";
        }

        if (select_current == "Set minute is active.") {
          select_current = "Set after minute is active.";
        }

        if (select_current == "Select hour") {
          select_current = "Set hour is active.";
        }

        if (select_current == "Select minute") {
          select_current = "Set minute is active.";
        }

        if (select_current == "Select save") {

          if (after_hour < hour) {
            lcd.clear();
            move_right = 0;
            select_current = "Select hour";
            FUNCTION_SET_TIMER_HOUR();
          } else {
            if (after_minute < minute) {
              lcd.clear();
              move_right = 1;
              select_current = "Select minute";
              FUNCTION_SET_TIMER_MINUTE();
            }
          }

          if (after_hour >= hour && after_minute >= minute) {
            select_current = "Save data to eeprom.";
            Serial.println("Write");
            EEPROM.write(eeprom_addr_h, hour);
            EEPROM.write(eeprom_addr_after_h, after_hour);
            EEPROM.write(eeprom_addr_m, minute);
            EEPROM.write(eeprom_addr_after_m, after_minute);
            EEPROM.commit();
            FUNCTION_WRITE_EEPROM();
          }

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

        if (select_current == "Set after hour is active.") {
          select_current = "Select hour";
          lcd.clear();
          FUNCTION_SET_TIMER();
        }

        if (select_current == "Set minute is active.") {
          select_current = "Select minute";
          lcd.clear();
          FUNCTION_SET_TIMER();
        }

        if (select_current == "Set after minute is active.") {
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

    if (boot_mode == "setting wifi") {

      server.handleClient();

    } else {
      // get_heap();

      FUNCTION_NORMAL();
    }

  }

  

  delay(100);
}
