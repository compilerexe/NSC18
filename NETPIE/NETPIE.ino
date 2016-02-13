#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <MicroGear.h>
#include <Wire.h>

// #define DEBUG_CODE
#define DEBUG_PRINTER Serial

#ifdef DEBUG_CODE

    #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
    #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }

#else

    #define DEBUG_PRINT(...) {}
    #define DEBUG_PRINTLN(...) {}

#endif

#define APPID                   "NSC18"
#define KEY                     "jWnMSWIG9IbEq9y"
#define SECRET                  "mPY8ZMkliWoFp1jlDHm7M4euZSucwb"
#define ALIAS                   "NODEMCU"
#define SCOPE                   "r:/NSC18/STATE,w:/NSC18/STATE,name:NODEMCU-CONTROL,chat:wwwNSC18"

const char* ssid                = "WIFI-MAXNET";
const char* password            = "macrol3ios";

// const char* ssid                = "CMMC-MEMBER";
// const char* password            = "devicenetwork";

String on_msg = "";

String memory_rx                 = "";

WiFiClient client;
AuthClient *authclient;

int timer                       = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {

    DEBUG_PRINT("Incoming message --> ");
    msg[msglen] = '\0';
    DEBUG_PRINTLN((char *)msg);
    
    for (int n = 0; n <= 1; n++) {
      switch(msg[n]) {
        case 48 : on_msg += "0"; break;  
        case 49 : on_msg += "1"; break;  
        case 50 : on_msg += "2"; break;  
        case 51 : on_msg += "3"; break;        
      }
    }
    
    if (on_msg == "1") { // Intiligent on

        Serial.print("1");
      // digitalWrite(D1, 0);
      // Serial.println("Found command : Intiligent on");
      
    }

    if (on_msg == "0") { // Intiligent off
    
        Serial.print("0");
      // digitalWrite(D1, 1);
      // Serial.println("Found command : Intiligent off");
      
    }

    on_msg = "";

}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    DEBUG_PRINT("Found new member --> ");
    for (int i=0; i<msglen; i++)
        DEBUG_PRINT((char)msg[i]);
    DEBUG_PRINTLN();  
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    DEBUG_PRINT("Lost member --> ");
    for (int i=0; i<msglen; i++)
        DEBUG_PRINT((char)msg[i]);
    DEBUG_PRINTLN();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    DEBUG_PRINTLN("Connected to NETPIE...");
    microgear.setName("mygear");
}

void setup() {
    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(PRESENT,onFoundgear);
    microgear.on(ABSENT,onLostgear);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    

    DEBUG_PRINTLN("Starting...");

    if (WiFi.begin(ssid, password)) {

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            DEBUG_PRINT(".");
        }

        DEBUG_PRINTLN("WiFi connected");  
        DEBUG_PRINTLN("IP address: ");
        DEBUG_PRINTLN(WiFi.localIP());

        //uncomment the line below if you want to reset token -->
        //microgear.resetToken();
        microgear.init(KEY,SECRET,ALIAS,SCOPE);
        microgear.connect(APPID);

    }
}

void loop() {
    if (microgear.connected()) {
        microgear.loop();
        if (timer >= 1000) {
            // Serial.println("Publish...");
            // microgear.chat("mygear","Hello");


            

            timer = 0;
        } 
        else timer += 100;
    }
    else {
        // Serial.println("connection lost, reconnect...");
        if (timer >= 5000) {
            microgear.connect(APPID);
            timer = 0;
        }
        else timer += 100;
    }

    delay(100);
}