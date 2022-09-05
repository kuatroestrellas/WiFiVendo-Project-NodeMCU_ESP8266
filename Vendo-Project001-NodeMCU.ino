/*    
 * Working Filename : Vendo-Project001S04E03-NodeMCU
 * Timeline         : March 2020
 * Language         : C++ / JAVA / JSon / AJAX 
 * Project          : HOTSPOT WiFi Vendo
 * Hardware         : Mikrotik / NodeMCU 4MB / Coin Acceptor / 16x4 LCD / 12v-5v Switching Power / Push Botton Trigerring  
 * Credit Sources   : GitHub / Youtube / Forums / Facebook  
 *                  : https://www.john-lassen.de/en/projects/esp-8266-arduino-ide-webconfig 
 * License          : Under GPL / OpenSource
 * ******************************************************************************
    Version  Modified By     Date        Comment / Update ++ / Developement --
   -------   -----------  ---------- --------------------------------------------
    S04-E01   Rommel      06/26/2020 Initiate to Mix with LPB Module (Orange PI)
                                     Change Mikrotik IP 10.10.10.X / Remove Hours from VoucherCode 
                                     RE-STRUCTURE COIN DROP TO BUY CODE SEQUENCE && NETWORK ERROR and RECOVERY DETECTION
                                     MAJOR RECODE on LOOP and TRIGGERING METHODS
                                     FINAL RELEASE FOR MIKROTIK MODULE                                                    
    S04-E02   Rommel      06/26/2020 LPB Module (Orange PI) && MIKROTIK MODULE MIXED SETUP 
                                     - TBA (Mixed Sharing Module OneCoinSlot Separate Sequences)
    S04-E03   Rommel      06/28/2020 Fixed Bug on Coins Slot Routine  (FINAL RELEASE FOR MIKROTIK MODULE)
                                     
*/                                                    
 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//===>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//===>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
// #include <Bounce2.h>
// #include <EasyBuzzer.h>


//#include <ESP8266mDNS.h>
//#include <ESPAsyncTCP.h>
//#include <ESPAsyncWebServer.h>
// #include <ArduinoOTA.h> // HTTP Advanced Authentication Component
#include <Ticker.h>
#include <WiFiUdp.h>
//===>
#include <ESP8266TelnetClient.h>
#include <ESP8266Ping.h>
// #include <ESP8266HTTPClient.h>  // use to HTTP command
//===>
//For ESP32, To use ESP32 Dev Module, QIO, Flash 4MB/80MHz, Upload 921600
//Ported to ESP32
#ifdef ESP32
#include <esp_wifi.h>
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
#else
#define ESP_getChipId()   (ESP.getChipId())
#endif

const int varDEMO=1; // DEMOFLAG Settings 0=DEMO 1=OEM
int varPH; // base rate per min
int varPH1; // 5=1H
int varPK1; // 60min
int varPH2; // 10=3H
int varPK2; // 60*3
int varPH3; // 20=10H
int varPK3; // 60*10
int varPH4; // 30=1D
int varPK4; // 60*24
int varPH5; // 50=3D
int varPK5; // 60*24*3
int varPH6; // 100=7D
int varPK6; // 60*24*7
int varPH7; // 300=30D++
int varPK7; // 60*24*30
int varHP1; // Hotspot Profile
int varHP2; // Hotspot Profile
int varHP3; // Hotspot Profile
int varHP4; // Hotspot Profile
int varHP5; // Hotspot Profile
int varHP6; // Hotspot Profile
int varHP7; // Hotspot Profile
int varSales; // SalesCounter
int varLockSetup;  // WebConfig Protection
String varWifiSSID;
String varTelnetUsr;
String varTelnetPwd;
String FWStatus;
String SysDate;  // System Date Display
unsigned int TimeSecs = 0;        // CountDown Timers
unsigned int varTimeSecs = 0;     // CountDown Timers
int NetCheckTimer=0;   //  Network Connectivity Scheduler
unsigned int varBeepTone = 0;     // BuyCode BeepTone
unsigned int StarTime=0;
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);                   // The Webserver
#include "helpers.h"
#include "global.h"


/*
Include the HTML, STYLE and Script "Pages"
*/
#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
//#include "Page_md5.js.h"
#include "Page_Style.css.h"
//#include "Page_Style1.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
//#include "Page_CoinInfo.h"
#include "Page_SysReboot.h"
#include "example.h"
//#include "Page_WebCoin.h"
//#include "mario.h"



// <================================>

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;
// LiquidCrystal_I2C lcd(0x27, 16, 2);  // 2LinerLCD
// LiquidCrystal_I2C lcd(0x27, 20, 4);     // 4LinerLCD
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
// HTTPClient http;
// String APssid = "WiFiVendoCFG-" + String(ESP_getChipId(), HEX);
String APssid = String(ESP_getChipId(), HEX) + "_BARYA";
String APpass = "#" + String(ESP_getChipId(), HEX) + "!";
String DEVhostname = "S02E03rev1" + String(ESP_getChipId(), HEX);
String SSIDname;
String SSIDpwd;
String MainDisplay;
/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */

void TrialSSID();

// for OEM
// String SSIDname = APssid;
// String SSIDpwd =  APpass;

// for DEMO
// String SSIDname = "DemoWiFiVendo";
// String SSIDpwd =  "";

#define ACCESS_POINT_NAME SSIDname          
#define ACCESS_POINT_PASSWORD SSIDpwd

/// #define ACCESS_POINT_NAME "DemoWiFiVendo"          
/// #define ACCESS_POINT_PASSWORD ""

// #define AdminTimeOut 180  //180=3mins Defines the Time in Seconds, when the Admin-Mode will be diabled 
unsigned int AdminTimeOut = 0;


// <================ Variables HERE ================>
//put here your ip address, and login details
WiFiClient client;
ESP8266telnetClient tc(client); 
IPAddress mikrotikRouterIp (192, 168, 88, 1);
const IPAddress remote_ip(8, 8, 8, 8); // Google DNS
const IPAddress nodemcu_ip(10, 10, 10, 254); 
IPAddress routerOS_ip(10, 10, 10, 1);
// IPAddress IProuterOS;
String telnetcmd; //Mikrotik Telnet command
String aCodetcmd; //Mikrotik Telnet command
String bCodetcmd; //Mikrotik Telnet command
String cCodetcmd; //Mikrotik Telnet command
char letters[34] = {'a', 'b', 'c', 'd', 'e', 'h', 'k', 'n', 'r', 's', 't', 'x', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'M', 'N', 'P', 'R', 'S', 'T', 'U', 'W', 'X', 'Y', 'Z'};
// char letters[36] = {'z', 'c', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
// String telnetSnd; // Send Telnet command

// https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
// Change *int to byte as per arduino samples
const byte SensorD5Coin = 14;     // I/O for Coin Counter
const byte SensorD1CoinLck = 5;   // I/O for Coin Lock Switch Pin 
const byte SensorD6Button = 12;   // I/O for Push Button Pin
const byte SensorD8Buzzer = 15;   // I/O for Buzzer Pin
/*
const int SensorD5Coin = 14;     // I/O for Coin Counter
const int SensorD1CoinLck = 5;   // I/O for Coin Lock Switch Pin 
const int SensorD6Button = 12;   // I/O for Push Button Pin
const int SensorD8Buzzer = 15;   // I/O for Buzzer Pin
*/

// Timer: Auxiliary variables
#define timeSeconds 25
#define codecantwait 45
int CoinLckState = 0;     // variable for reading the pushbutton status 
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean RunOnce = false;  // to check DNS routine using LOOP Section
boolean startTimer = false;
// boolean timecheck = false;  //CheckTime
boolean PushButtonKey = false;  //CheckTime
boolean pindot;  //Push Button Buying validation
boolean WifiRecheck = false;  //Wifi Reconnection
boolean PingResult = false;  //Network Ping Results
boolean wwwReload = false;  //Webserver Reconnection

//Scrolling Message VAR statement
String BmessageLine1 = " COINS ACCEPTED ";
String BmessageLine2 = "P1SO-L5MA-SAMP10";
String BmessageLine3 = "* * VENDOTRONICS * *";
String messageToScroll = "LIKE @fb.com/VENDOTRONICS |HP# 0933 861 1145 :)";
String messageToScroll01 = "Logon http://10.0.0.1";
String messageToScroll02 = "System Restarting!";
String messagePressBtn00 = "W I F I  L O G I N";
String messagePressBtn01 = "http://10.10.10.1";
String messagePressBtn02 = "==> ToContinue <==";
String messagePressBtn03 = "Insert Coins NOW:)";
String messageLine1 = " PisoWiFiVendo ";
//String messageLine2 = "PushBtn&InsertCOINS";
String messageLine2 = "PUSH BUTTON TO START";
String messageLine31 = "Insert More Coins";
// String messageLine3 = "Connect 2 HotSpot ";
String messageLine3 = "SSID:" + (varWifiSSID);
// String messageLine4 = "Insert Coin/Barya";
String messageLine4 = " Happy Browsing:) ";
String messageLine41 = "Press BTN for CODE";
String messageLine42 = "Thank You. Enjoy:)";   //20 lines
// String messageLine42 = "Thank You..";   //12 lines
int magkano_na=0; // CoinCount
int BuyCode=0;
int Hour=0;
int Min=0;
int Sec=0;
int Day=0;
int xmins=0;
int promorate=0; // for promo rates
unsigned int HSprof=0;  //Hotspot Profile
String HSemail;  //Hotspot User email
unsigned int loopbreaker=0;  // loop breaker
unsigned int netcheck=0;  // 0=DOWN 1=UP Internet & Network Connectivity Validation
unsigned int neterror=0;  // Network ERROR  0=GOOD 1=WIFI 2=ROUTER 3=ISP
unsigned int wifiTimer=0;  // Wifi Reconnecting Counter after Disconnected
int coinlockpin=5;
int pasaload = 0;
int VendoWifi;
int VendoMode;
int AdminMode;
int xtemperature=0;   //MIN
int xhumidity=0;      //HOUR
int xpressure=0;      //DAY
String strTOname;
String strFMname;
unsigned int newmins = 0;
unsigned int newhours = 0;
String strcode;

/*
int vPH = 10; // base rate per min
int vPH1 = 5; // 5=1H
int vPK1 = 60; // 60min
int vPH2 = 10; // 10=3H
int vPK2 = 180; // 60*3
int vPH3 = 20; // 20=10H
int vPK3 = 600; // 60*10
int vPH4 = 30; // 30=1D
int vPK4 = 1440; // 60*24
int vPH5 = 50; // 50=3D
int vPK5 = 4320; // 60*24*3
int vPH6 = 100; // 100=7D
int vPK6 = 10080; // 60*24*7
int vPH7 = 300; // 300=30D++
int vPK7 = 43200; // 60*24*30
*/
//<==================>//


// end of VARIABLE
// <================================>
//  <================================> <<FUNCTIONS HERE>> <================================>
ICACHE_RAM_ATTR void PushButton() {   
digitalWrite(LED_BUILTIN,LOW);
if (PushButtonKey) {  
CoinLckState = digitalRead(SensorD1CoinLck);
       if(magkano_na>0) {
            if (CoinLckState == HIGH) {
                Serial.println(F("PRESS PushButton()Function TO Initiate BUYCODE ")); //STEP-MARKER
                BuyCode = 1;
                pindot=true;   // if TRUE=BUYCODE FALSE=To OPEN COINSLOT IF Network is Good
                NetCheckTimer =0;  // Reset Network Ping Scheduler
                delay(50);              
            }
            if (CoinLckState == LOW) {
                  Serial.println(F("PRESS PushButton()Function TO Terminate BUYCODE CoinLckState == LOW")); //STEP-MARKER                                  
               if (TimeSecs > 0) loopbreaker = 0;
            }
         startTimer = false;   
       } else {
            if(PingResult) {
               Serial.println(F("PRESS PushButton()Function TO Initiate Coins Slot ")); //STEP-MARKER 
                  if (!pindot) {
                      CHECK_ISP();
                  }
            }                           
       }
} else {
Serial.println(F("PushButton()Function TEMPORARY NOT AVAILABLE")); //STEP-MARKER   
} // end (PushButtonKey)

} // end of PushButton()

ICACHE_RAM_ATTR void CoinAccept() {
     digitalWrite(LED_BUILTIN,LOW);
    magkano_na++;
    if (StarTime == 1) {  // for Coins Counter LCD Display      
        DisplayCoinCounter();  // realtime coin counter
    }
    if (StarTime > 1) {  // for Coins Counter LCD Display
      PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES
      ComputeCoins();        // realtime coin counter
    }
} // end of CoinAccept()

void ICACHE_RAM_ATTR espREBOOT () {
     digitalWrite(5, LOW); //coinslot LOCKED triggered
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("0 9 3 5 701 22  56"); 
     lcd.setCursor(2, 1);
     lcd.print("SYSTEM RESTARTING.");
     lcd.setCursor(2, 2);
     lcd.print("FOR MORE INFO CALL");
     lcd.setCursor(0, 3);
     lcd.print("fb.com/Vendotronics"); 
     lcd.display();
     delay(8000);   
     Serial.println("System Reboot!"); 
     ESP.restart();
  }
// DelayTimeScrollType
void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    lcd.display();
        if(magkano_na>0) {
           //delayTime = 0;
           delay(20); 
        } else {
          delay(delayTime);
        }          
  }
}

void scrollTxtNormal(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message; 
  } 
     message = message + " "; 
     for (int pos = 0; pos < message.length(); pos++) {
          lcd.setCursor(0, row);
          lcd.print(message.substring(pos, pos + lcdColumns));
          lcd.display();
          delay(delayTime);
     }
          
}

void scrTimeDelay(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
        if(magkano_na>0) {
           delay(20); 
        } else {
          delay(delayTime);
        }          
  }
}

void pasatimeForm() {
  Serial.println(F("Accessing PASATIME...")); //STEP-MARKER  
   if (WiFi.status() == WL_CONNECTED) {
     if(Ping.ping(routerOS_ip, 3)) {
           Serial.println(F("Validating Accounts...")); //STEP-MARKER 
          String Page;
          Page += F(
            "<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"
            "<meta http-equiv='refresh' content='10; url=http://10.10.10.1/logout''>"
            "<meta http-equiv='pragma' content='no-cache'>"
            "<meta http-equiv='expires' content='-1'>"
            "<title>Redirect</title></head><body>"
            "<h1>Validating Accounts !!</h1>");
         Page += F(
               "<p>You may check your <a href='http://10.0.0.1/status'>wifi connection and account status</a>.</p>"
               "<p>Validating Account Status</p>"
               "<p>To Verify the Transaction</p>"
               "<p>==========LOGOUT=========</p>"
               "<p>==========LOG IN=========</p>"
               "<p>Required to APPLY Procedure</p>"
               "</body></html>");
          server.send(200, "text/html", Page);            
           String firstName = server.arg("firstname"); 
           String lastName = server.arg("lastname"); 
           Serial.print(F("Current User : "));
           Serial.println(firstName);
           Serial.print(F("TimeCodeDonor : "));
           Serial.println(lastName);
           strTOname = firstName;   // papasahan
           strFMname = lastName;    // nagpasa
           strFMname.trim();
           pasaload = 1;          
           // String s = "<a href=http://10.10.10.1/status.html> Finished </a>";
           // server.send(200, "text/html", s); //Send web page          
        } else {
           Serial.println(F("NODEMCU is BUSY...")); //STEP-MARKER            
          String Page;
          Page += F(
            "<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"
            "<meta http-equiv='refresh' content='3; url=http://10.10.10.1/''>"
            "<meta http-equiv='pragma' content='no-cache'>"
            "<meta http-equiv='expires' content='-1'>"
            "<title>Redirect</title></head><body>"
            "<h1>NODE Server is BUSY !!</h1>");
         Page += F(
               "<p>You may check your <a href='http://10.10.10.1/status'>wifi connection and account status</a>.</p>"
               "<p>HOST is UNREACABLE</p>"
               "</body></html>");
          server.send(200, "text/html", Page);          
        }
   } else {
          String Page;
          Page += F(
            "<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"
            "<meta http-equiv='refresh' content='3; url=http://10.0.0.1/''>"
            "<meta http-equiv='pragma' content='no-cache'>"
            "<meta http-equiv='expires' content='-1'>"
            "<title>Redirect</title></head><body>"
            "<h1>WIFI Server is BUSY !!</h1>");
         Page += F(
               "<p>You may check your <a href='http://10.0.0.1/status'>wifi connection and account status</a>.</p>"
               "<p>Cannot Connect to WIFI</p>"               
               "</body></html>");
          server.send(200, "text/html", Page);
   }  
// server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
// server.client().stop(); // Stop is needed because we sent no content length
}
void LockUnLocked() {
       tempadd=270;  // eeprom address location
       config.vLockSetup = 0;
       EEPROM.put(tempadd,config.vLockSetup); // WebConfig Protection
       tempadd += sizeof(int);
       EEPROM.put(tempadd,config.vLockSetup);
       EEPROM.commit();
       Serial.print(F("Vendo ADMIN LOCK Status: "));
       Serial.println(EEPROM.get(tempadd,config.vLockSetup));
       String s = "<a href=/sysreboot.html>UNlockedSuccessful-Reboot </a>)";

 server.send(200, "text/html", s); //Send web page
}

// <================================> FUNCTIONS END <================================>

//  <================================> <SETUP TOP> <================================>
void setup ( void ) {
//  <================================> <SETUP TOP> <================================>  
Wire.begin(2, 0);
EEPROM.begin(512);
  Serial.begin(115200);
  delay(500);
  WiFi.persistent(true);  //refer: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html
  TrialSSID ();
if (varDEMO > 0) {
  FWStatus = "FW: 01F-OEM-S04E03";
} else {
  FWStatus = "Ver: Trial200coins";
}  
  lcd.begin();
  //lcd.init();
  lcd.backlight();
  lcd.home(); 
  lcd.clear();
  lcd.noAutoscroll();
  lcd.setCursor(1, 2);
  lcd.print("                   ");
  lcd.setCursor(1, 2);
  lcd.print("   W E L C O M E   ");
  lcd.display();
  Serial.println(" ");
  Serial.println(F("=======================ECQ-MMXX========================"));
  Serial.println(F("For More Details Contact Me: charmedonesFJO@me.com     "));
  Serial.println(F("======================================================="));
  Serial.println(F("FREE Firmware and Use at Your Own Risk                 "));
  Serial.println(F("This NodeMCU Firmware is Under GPL Source License      "));
  Serial.println(F("An OPEN SOURCE Project Written using arduino.cc/credits"));
  Serial.println(F("Copyright and Trademarks All Rights Reserved - MMXX    "));
  Serial.println(F("                 myip.local.host/ssoe                  "));
  Serial.println(F("=======================ECQ-MMXX========================"));
  Serial.println(F(""));
  Serial.println(F("Starting WiFiVendoNodeMCU-FW.Base.S04E00"));
  Serial.println(F(" "));
  Serial.print(F("Firmware Type #>>>"));
  Serial.println(FWStatus);
  Serial.println(F("   "));  
  Serial.print(F("Setup SSID : "));
//  Serial.println(SSIDname);
  Serial.println(ACCESS_POINT_NAME);
  Serial.println(F(" "));
  Serial.print(F("SSID PWD : "));
  Serial.println(F("#macadd!"));
  Serial.println(F(" "));
  // vendo section     
     // pinMode(LED_BUILTIN,OUTPUT);
     pinMode (SensorD8Buzzer,OUTPUT) ;  // Buzzer 
     pinMode(SensorD5Coin, INPUT);
//     Serial.println(F("PinMode Coin Sensor INPUT"));
     pinMode(SensorD1CoinLck, OUTPUT); //D1 coinslot trigger COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED
//     Serial.println(F("Coins Slot Sensor Activate"));
     digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED
     InitTone();
//     lcd.clear();
//     DISPLAYbMSG();
     PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES 
     
      
  if (!ReadConfig())
  {
      lcd.setCursor(1, 3);
      lcd.print(FWStatus);
      lcd.display();
    // DEFAULT CONFIG
    config.ssid = "Vendo_WiFiNode";
    // config.password = "<1qaz2wsx>";
    config.password = "";
    config.dhcp = true;
    config.IP[0] = 10;config.IP[1] = 0;config.IP[2] = 0;config.IP[3] = 254;
    config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
    config.Gateway[0] = 10;config.Gateway[1] = 0;config.Gateway[2] = 0;config.Gateway[3] = 1;
    config.ntpServerName = "sg.pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  10;
    config.timezone = 00;
    config.daylight = false;
    config.DeviceName = DEVhostname;
    // Vendo Variable Rates //
// "Rates 5=1H 10=3H 20=10H 30=1D 50=3D 100=7D 300=30D++"
    config.vPH = 10;       // base rate per min
    config.vPH1 = 5;       // 5=1H
    config.vPK1 = 60;      // 60min
    config.vPH2 = 10;      // 10=3H
    config.vPK2 = 180;     // 60*3
    config.vPH3 = 20;      // 20=10H
    config.vPK3 = 600;       //600;     // 60*10
    config.vPH4 = 30;      // 30=1D
    config.vPK4 = 1440;       //1440;    // 60*24
    config.vPH5 = 50;      // 50=3D
    config.vPK5 = 4320;       //4320;    // 60*24*3
    config.vPH6 = 100;     // 100=7D
    config.vPK6 = 10080;         //10080;   // 60*24*7
    config.vPH7 = 300;     // 300=30D++
    config.vPK7 = 43200;         //43200;   // 60*24*30
    config.vHP1 = 10; // HotSpot Profile
    config.vHP2 = 20; // HotSpot Profile
    config.vHP3 = 30; // HotSpot Profile
    config.vHP4 = 40; // HotSpot Profile
    config.vHP5 = 50; // HotSpot Profile
    config.vHP6 = 60; // HotSpot Profile
    config.vHP7 = 70; // HotSpot Profile
    config.vSales = 0;  //Sales Counter
    config.vLockSetup = 0;  //Sales Counter
      if (varDEMO > 0) {
    config.vTelnetUsr = "password";  //Telnet Account
    config.vTelnetPwd = "username"; //Telnet Password        
      } else {
    config.vTelnetUsr = "demoMMXX";
    config.vTelnetPwd = "MMXXdemo";        
      }
    WriteConfig();
    Serial.println(F("General config applied"));
      
  }  
    
  if (AdminEnabled)
  {
    if ((EEPROM.get(270,config.vLockSetup)) == 0) {
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_AP);
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
      delay(500); // Without delay I've seen the IP address blank
      Serial.print(F("AP IP address: "));
      Serial.println(WiFi.softAPIP());
      dnsServer.start(DNS_PORT, "*", apIP);
      // dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
      Serial.println(F( "AP and DNS STATION SETUP MODE" ));    
    } else {
      WiFi.mode(WIFI_AP_STA); // Admin Mode SETUP Section
      WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
      delay(800); // Without delay I've seen the IP address blank
      Serial.println(F( "STATION AP Mode SETUP Section" ));  
    }
    VendoMode = 0;
  }
  else
  {
    Serial.println(F( "CLIENT AP Mode SETUP Section" ));
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    WiFi.mode(WIFI_STA); // Client Mode SETUP Section
    delay(800); // Without delay I've seen the IP address blank
    VendoMode = 1;
  }
   ConfigureWifi();
  // == <Administration Pages TOP> ==   
  if ((EEPROM.get(270,config.vLockSetup)) == 0) {
  server.on ( "/", handleRoot);
  server.on ( "/admin/filldynamicdata", filldynamicdata );
  
  server.on ( "/favicon.ico",   []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );


  server.on ( "/admin.html", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/info.html", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ( "/ntp.html", send_NTP_configuration_html  );
  server.on ( "/general.html", send_general_html  );
  server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
  server.on ( "/admin/values", send_network_configuration_values_html );
  server.on ( "/admin/connectionstate", send_connection_state_values_html );
  server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
  server.on ( "/admin/generalvalues", send_general_configuration_values_html);
  server.on ( "/admin/devicename",     send_devicename_value_html);
  // == <Administration Pages END> ==
  server.on ( "/action_page", pasatimeForm); // For PASATIME Transaction
  server.on ( "/ssoe", LockUnLocked); // re-enable WEBCONFIG
  server.on ( "/sysreboot.html", send_reboot_html  ); // view reboot and sales page
  server.on ( "/admin/rebootvalues", send_reboot_configuration_values_html); // send reboot command
  server.onNotFound ( []() { Serial.println(F("Page Not Found...!")); server.send ( 400, "text/html", "Page not Found" );   }  );  
  } else {    // NON-Admin WebPage //
  server.on ( "/", []() { Serial.println(F("info.html")); server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ( "/admin/filldynamicdata", filldynamicdata ); 

  server.on ( "/favicon.ico",   []() { Serial.println(F("favicon.ico")); server.send ( 200, "text/html", "" );   }  );


  server.on ( "/info.html", []() { Serial.println(F("info.html")); server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ( "/style.css", []() { Serial.println(F("style.css")); server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { Serial.println(F("microajax.js")); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
  server.on ( "/action_page", pasatimeForm); // For PASATIME Transaction
  server.on ( "/ssoe", LockUnLocked); // re-enable WEBCONFIG
  server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/sysreboot.html", send_reboot_html  ); // view reboot and sales page
  server.on ( "/admin/rebootvalues", send_reboot_configuration_values_html); // send reboot command
  server.onNotFound ( []() { Serial.println(F("Page Not Found...!")); server.send ( 400, "text/html", "Page not Found" );   }  );
  }

  server.begin();
 
  Serial.println(F( "HTTP server started" ));
  Serial.println(F("   "));
  Serial.print(F("Vendo ADMIN LOCK Status: "));
  Serial.println(EEPROM.get(270,config.vLockSetup));
  Serial.println(F("   "));
  Serial.println(F("Boot Status : 0= Admin 1= Vendo"));
  Serial.print(F("BootSetup Status : "));
  Serial.println(BootSetup);
  Serial.println(F("   "));
  lcd.clear();
  DISPLAYbMSG();  //Display ADS

    if (BootSetup > 0) {
      if ((EEPROM.get(270,config.vLockSetup)) == 0) {
           AdminTimeOut = 210;  //180=3mins Defines the Time in Seconds, when the Admin-Mode will be diabled 
      } else {
           AdminTimeOut = 30;  //180=3mins Defines the Time in Seconds, when the Admin-Mode will be diabled       
      }
    Serial.println(F("   "));
    Serial.println(F("Client Mode"));
    Serial.println(F("Vendo is Operational"));
    Serial.println(F("   "));
    StartUpTimeOut = AdminTimeOut; 
    VendoMode = 1;
    ReadVendoRate();  // Vendo Rates Values
       tempadd=280;  // eeprom address location
       Serial.print(F("UpToDate Vendo Sales : "));
       Serial.println(EEPROM.get(tempadd,config.vSales));
   lcd.setCursor(0, 3);
   lcd.print("                    ");  
   lcd.setCursor(0, 3);
   lcd.print("WiFiUpdate:PHP ");   
   lcd.print(EEPROM.get(tempadd,config.vSales));
   lcd.display();
   delay(800);    
   } else {
    ScreenAdminMode();
    VendoMode = 0;
    AdminMode = 0;
          if ((EEPROM.get(270,config.vLockSetup)) == 0) {
              AdminTimeOut = 600;  //180=3mins Defines the Time in Seconds, when the Admin-Mode will be diabled 
              // varBeepTone = 0;  // BuyCode BeepTone
          } else {
              AdminTimeOut = 30;  //180=3mins Defines the Time in Seconds, when the Admin-Mode will be diabled
              // varBeepTone = 1;  // BuyCode BeepTone       
          }
    Serial.println(F("   "));
    Serial.println(F("Admin Mode"));
    Serial.println(F("Please Setup Your Vendo"));
    Serial.println(F("   "));
    CheckVendoRate();
    tempadd=280;  // eeprom address location
    config.vSales = 0;    // Resetting Vendo Sales Counter
    EEPROM.put(tempadd,config.vSales); // Sales Counter
    tempadd += sizeof(int);
    EEPROM.put(tempadd,config.vSales);
    EEPROM.commit();
   }
  tkSecond.attach(1,Second_Tick);
  //tkSecond.attach(1,Second_Tick);
  UDPNTPClient.begin(2390);  // Port for NTP receive
 String IP_gateway = (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3];
 messageToScroll01 = "Logon http://" + IP_gateway;
 IPAddress routerOS_ip (WiFi.gatewayIP()[0], WiFi.gatewayIP()[1], WiFi.gatewayIP()[2], WiFi.gatewayIP()[3]);
 IPAddress mikrotikRouterIp (WiFi.gatewayIP()[0], WiFi.gatewayIP()[1], WiFi.gatewayIP()[2], WiFi.gatewayIP()[3]);
 Serial.print(F("Checking Gateway IP : "));
 Serial.println(IP_gateway);
 Serial.println(F("   "));  
 Serial.print(F("Checking RouterOS : "));
 Serial.println(mikrotikRouterIp);
 Serial.println(F("   "));
 // IPAddress IProuterOS (WiFi.gatewayIP()[0], WiFi.gatewayIP()[1], WiFi.gatewayIP()[2], WiFi.gatewayIP()[3]);
  
 
 
//==> VENDO CODE
////  start of insert //////////////////////////////////////////////////////////////////////  
   if (BootSetup > 0) {
      pinMode(SensorD5Coin, INPUT_PULLUP);  // add-on S03-E15
      attachInterrupt(digitalPinToInterrupt(SensorD5Coin), CoinAccept, RISING); // Coin Counter
      // attachInterrupt(SensorD5Coin, CoinAccept, RISING); // Coin Counter
      
      pinMode(SensorD6Button, INPUT_PULLUP);  // add-on S03-E15
      attachInterrupt(digitalPinToInterrupt(SensorD6Button), PushButton, RISING); // Push Button PinMode
      // attachInterrupt(SensorD6Button, PushButton, RISING); // Push Button PinMode
        if(Ping.ping(routerOS_ip, 3)) {
           VendoMode = 1;
           Serial.print(F("SUCCESS...! "));
           Serial.println(F("NodeMCU Connected to RouterOS"));
           lcd.clear();
           lcd.setCursor(0, 0);
            if (AdminMode > 0) {
               // DIPSLAYSTATIC();
              }
              else {
               ScreenVendoMode(); 
              }
                 
           NTPRefresh();
           cNTP_Update =0;
        } else {
          VendoMode = 0;
          Serial.print(F("ERROR :"));
          Serial.println(F("Connection Failed to RouterOS"));
          digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED
                if (WiFi.status() != WL_CONNECTED) {
                      WiFi.disconnect();
                      Serial.println(F("Connecting as wifi client..."));
                      WiFi.begin (config.ssid.c_str(), config.password.c_str());
                           lcd.clear();
                           lcd.setCursor(2, 1);
                           lcd.print("Checking Wireless");
                           lcd.setCursor(2, 2);
                           lcd.print("Please Wait......");
                           lcd.setCursor(2, 3);
                           lcd.print(".....reconnecting");
                           lcd.display(); 
                         WifiTimeOutDisplay();
                         PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES                      
                         while (WiFi.status() != WL_CONNECTED) {
                           delay(500);
                           WifiTimeOut();
                           Serial.print(":");
                                if (wifiTimer > 555) {
                                   Serial.println(F(""));
                                   Serial.print(F("WiFi Connection TimeOut: ReStarting..."));
                                   delay(1000);
                                    // ESP.restart();
                                    PingResult = false; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
                                    break;                      
                                 } 
                          }
                      int connRes = WiFi.waitForConnectResult();
                        Serial.print(F("connRes: "));
                        Serial.println(connRes);
                        if (connRes > 0) PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES  
                } else {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.setCursor(2, 0);
                    lcd.print(messageLine1);
                    lcd.setCursor(2, 1);
                    lcd.print("Setup Completed");
                    lcd.setCursor(2, 2);
                    lcd.print("Please Wait...!");
                    lcd.setCursor(0, 3);
                    lcd.print(messageToScroll02);
                    lcd.display();
                   delay(15000);   
                   Serial.println(F("System Rebooting.!")); 
                     ESP.restart();
                }           

          }    
    }
////  end of insert //////////////////////////////////////////////////////////////////////
// <================================> <SETUP END> <================================>      
delay(50);
StartUpTone();   
} 
// <================================> <SETUP END> <================================>

// ######### VOID FUNCTIONS TOP ######### //
  void ReloadWWW() {
  Serial.println(F("Reloading WWW Non-Admin Pages..."));   
  server.on ( "/", []() { Serial.println(F("info.html")); server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ( "/admin/filldynamicdata", filldynamicdata ); 
  server.on ( "/favicon.ico",   []() { Serial.println(F("favicon.ico")); server.send ( 200, "text/html", "" );   }  );
  server.on ( "/info.html", []() { Serial.println(F("info.html")); server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ( "/style.css", []() { Serial.println(F("style.css")); server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { Serial.println(F("microajax.js")); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
  server.on ( "/action_page", pasatimeForm); // For PASATIME Transaction
  server.on ( "/ssoe", LockUnLocked); // re-enable WEBCONFIG
  server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/sengkang.html", send_reboot_html  ); // view reboot and sales page
  server.on ( "/admin/rebootvalues", send_reboot_configuration_values_html); // send reboot command
  server.onNotFound ( []() { Serial.println(F("Page Not Found...!")); server.send ( 400, "text/html", "Page not Found" );   }  );    
  }
void WifiReconnect()
{
  PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES
  /* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
// Wifi SSID
    String str_ssid = ReadStringFromEEPROM(64);
    int ssid_len = str_ssid.length() + 1;
    char c_ssid[ssid_len];
    str_ssid.toCharArray(c_ssid, ssid_len);    

// Wifi Password
    String str_wpa = ReadStringFromEEPROM(96);
    int wpa_len = str_wpa.length() + 1;
    char c_wpa[wpa_len];
    str_wpa.toCharArray(c_wpa, wpa_len);    
  Serial.println(F("RE-Configuring Wireless"));
                           lcd.clear();
                           lcd.setCursor(2, 1);
                           lcd.print("Checking Wireless");
                           lcd.setCursor(2, 2);
                           lcd.print("Please Wait......");
                           lcd.setCursor(2, 3);
                           lcd.print(".....reconnecting");
                           lcd.display();
                           WifiTimeOutDisplay();                      
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);   // Prevents reconnection issue (taking too long to connect)
  WiFi.mode(WIFI_STA);   // Client Mode SETUP Section
  delay(500); // Without delay I've seen the IP address blank
  Serial.println(F("Connecting as WirelessClient..."));
  WiFi.begin (c_ssid, c_wpa);
         while (WiFi.status() != WL_CONNECTED) {
               delay(500);
               WifiTimeOut();
               Serial.print(F("^"));
                  if (wifiTimer > 555) {
                     Serial.println(F(""));
                     Serial.print(F("WiFi Connection TimeOut: ReStarting..."));
                     delay(1000);
                     // ESP.restart();
                     PingResult = false; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
                     break;                      
                  }

         }
  Serial.println(F(""));
  Serial.print(F("Connected to :"));
  Serial.println(c_ssid);
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  int connRes = WiFi.waitForConnectResult();
  Serial.print(F("connRes: "));
  Serial.println(connRes);
    if (connRes>0) {
        PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES
        PingResult = true; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
        WifiRecheck = true;  //Wifi Reconnection
                           lcd.clear();
                           lcd.setCursor(2, 1);
                           lcd.print("WIFI Reconnected");
                           lcd.setCursor(2, 2);
                           lcd.print("Connected to....");
                           lcd.setCursor(2, 3);
                           lcd.print(c_ssid); 
                           lcd.display();                             
        // netcheck = 3;
        // neterror=0;  // Network ERROR  0=GOOD 1=WIFI 2=ROUTER 3=ISP
    }
  if (!config.dhcp)
  {
    WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
  }
}

void InitTone() {
       varBeepTone = 7;
          while (varBeepTone >= 0) {
           tone(SensorD8Buzzer, HIGH, 1000);
           delay(1000);
           tone(SensorD8Buzzer, LOW, 500) ; //FA note ...
           delay (500);
           tone(SensorD8Buzzer, HIGH, 1000);
           delay(1000);
             varBeepTone--;
             if (varBeepTone == 0) {  // BuyCode BeepTone
              noTone(SensorD8Buzzer) ; //Turn off the pin attached to the tone()
              break;
             }
        }   
}


void StartUpTone() {
tone(SensorD8Buzzer, HIGH, 1000) ; //DO note 523 Hz
delay (1000); 
tone(SensorD8Buzzer, 2000, 500) ; //RE note ...
delay (1000); 
tone(SensorD8Buzzer, HIGH, 1000) ; //MI note ...
delay (1000); 
tone(SensorD8Buzzer, 2000, 500) ; //FA note ...
delay (1000); 
tone(SensorD8Buzzer, HIGH, 1000) ; //FA note ...
delay (1000); 
noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()
}
// ######### VOID FUNCTIONS END ######### //

// <================================> <<LOOP STARTS>> <================================>
void loop(void){ 
// <================================> <<LOOP STARTS>> <================================>  
  
// EasyBuzzer.update();   // /* Always call this function in the loop for EasyBuzzer to work. */
 
if (((EEPROM.get(270,config.vLockSetup)) == 0) && (AdminEnabled)) {
    dnsServer.processNextRequest();
}
if (WifiRecheck) {
   WifiRecheck = false;  //Wifi Reconnection
    // Serial.print(F("RouterOS IP: "));
    // Serial.println(IProuterOS);
   server.close();
   delay(50);
   Serial.println(F("WebServer STOP...")); 
   ReloadWWW();
   delay(50);   
   server.begin();
   delay(50);     
   Serial.println(F("re-started WebServer..."));
   server.handleClient();  // WEBSERVER
   Serial.println(F("re-stablish Server Client..."));
   Serial.println(F("All Systems GO..!"));
   Serial.println(F("Machine is Now Activated...= Insert Coins = ..."));
   pindot=false;  // PUSH BUTTON SERVICE 
   // startTimer = false; 
   PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES 
}
     
server.handleClient();  // WEBSERVER

/* BEGIN OF ADMIN SETUP LOOP SECTION */
  if (AdminEnabled)
  {
    VendoMode = 0; // Vendo Not Ready
    // MDNS.update();    
   digitalWrite(5, LOW); //coinslot high=UNLOCKED low=LOCKED triggered
    // Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
    // String SysDate = (String(DateTime.month) + "-" + String(DateTime.day) + "-" + String(DateTime.year) + "|" + String(DateTime.hour) + ":" + String(DateTime.minute) + ":" + String(DateTime.second));
    if (StartUpTimeOut == 11 ) {
      SysDate = (String(DateTime.month) + "-" + String(DateTime.day) + "-" + (String(DateTime.year)).substring(0,2) + "|" + String(DateTime.hour) + ":" + String(DateTime.minute) + ":" + String(DateTime.second));      
      //Serial.println(SysDate);
      lcd.clear(); 
      lcd.setCursor(2, 0);
      lcd.print(SysDate);
      lcd.display(); 
      delay(50);     
    }
    if (StartUpTimeOut < 11) {            
      lcd.setCursor(2, 2);
      lcd.print(F("Starting in : "));
      lcd.setCursor(15, 2);  
      (StartUpTimeOut < 10) ? lcd.print("0") : NULL;
      lcd.print(StartUpTimeOut);
      lcd.display();
      delay(50);
      tone(SensorD8Buzzer, HIGH, 1000);    
      delay(1000);
    }
    if (AdminTimeOutCounter > AdminTimeOut)
    {
       AdminEnabled = false;
           lcd.clear();
           lcd.setCursor(1, 0);
           lcd.print(F(" WiFiVendo Project"));
           lcd.setCursor(2, 1);
           lcd.print(F("By: CharmedOnes "));
           lcd.setCursor(2, 2);
           lcd.print(F("09 3 5701 2 2 56"));
           lcd.setCursor(1, 3);
           lcd.print(FWStatus);
           lcd.display();
           delay(1000);       
         noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()       
          if (AdminMode == 0  && BootSetup == 0) { // Reboot At AdminSetup Mode
                if (WiFi.status() != WL_CONNECTED) {
                  PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES
                  WifiReconnect();
                } else {
                   lcd.clear();
                   lcd.setCursor(2, 1);
                   lcd.print(F("Finalizing Setup"));
                   lcd.setCursor(2, 2);
                   lcd.print(F("Please Wait....."));
                   lcd.setCursor(2, 3);
                   lcd.print(F("System Restarting"));
                   lcd.display(); 
                     delay(15000);   
                     Serial.println(F("System Rebooting...!")); 
                        ESP.restart();                  
                }      
          } 
       AdminMode = 1;  // Admin Setup Disable
       VendoMode = 1;  // Vendo is Ready
       // varBeepTone = 2;  // BuyCode BeepTone
          Serial.println(F("Admin Mode disabled!"));
             WiFi.mode(WIFI_OFF);   // Prevents reconnection issue (taking too long to connect)
             WiFi.mode(WIFI_STA);   // Client Mode SETUP Section
             ConfigureWifi();       // re-stablish WiFi reconnect
             Serial.println(F("Wireless connection Re-connect"));
             NTPRefresh();
             RunPing();
                cNTP_Update =0;
                loopbreaker = 1;
                WifiRecheck = true;  //Wifi Reconnection
                tone(SensorD8Buzzer, HIGH, 2000);
                delay(1000);
                tone(SensorD8Buzzer, LOW, 500);
                noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()
                DIPSLAYSTATIC();  //LCDMain Screen
                Serial.println(F("PisoWIFI Vendo Project"));
                Serial.println(F("By: charmedonesfjo@me.com 0 9 3 5 701 22 5 6"));
                PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES
    } else {
      if (AdminTimeOutCounter > 180) {
        if ((AdminMode == 0) && (VendoMode == 0)) {
          int timeleft = AdminTimeOut - AdminTimeOutCounter;
          lcd.setCursor(0, 0);
          lcd.print("                 ");
          lcd.setCursor(17, 0);  
          lcd.print(timeleft);
          lcd.display();         
        }
      }  
    }
  }
/* END OF ADMIN SETUP LOOP SECTION */ 
//////////////////////////////////////// NO MORE SETUP CONFIG BEYOND THIS LINE ///////////////////////////////////

//// START HERE THE LOOP ROUTINES //////
/**************************************/
  if (config.Update_Time_Via_NTP_Every  > 0 )
  {
    if (cNTP_Update > 5 && firstStart)
    {
      NTPRefresh();
      cNTP_Update =0;
      firstStart = false;
    }
    else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
    {
      // SchedulerRoutine
      NTPRefresh();
      cNTP_Update =0;
      NetCheckTimer =0;  // Reset Network Ping Scheduler
        Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
    }
  }   
/***********************************************/
/************    Your Code here  ***************/
/***********************************************/
// <================================> <<VENDO LOOP BEGIN>> <================================>
if (varDEMO == 0) { //DemoOnly 
   if(varSales > 120) {  // Demo Coins Counter
      digitalWrite(5, LOW); //coinslot TEMPORARY LOCKED
      TrialPeriod ();
   }
} //END DemoOnly

if ((AdminMode == 1) && (VendoMode == 1)) {  // MainRoutine BeginsHere..!

/////////////////////////////////////
// STAND BY MAIN ROUTINE CODE TOP //       
////////////////////////////////////  
  if(magkano_na == 0) {
    if (RunOnce) { // to check DNS routine using LOOP Section
      NetCheckTimer =0;  // Reset Network Ping Scheduler
            delay(50);
            tone(SensorD8Buzzer, HIGH, 2000);
            delay(1000);
            tone(SensorD8Buzzer, HIGH, 1000);
            tone(SensorD8Buzzer, LOW, 1000);
            delay(50);
            noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()      
      CHECK_DNS();
    }
     if(NetCheckTimer > 201) { //3.3mins
       RunPing(); 
       if(!PingResult) {
            Serial.println(F("SYSTEM DOWN...! NETWORK ERROR RE-CHECK CONNECTION ):  "));
            digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED                
       }                   
       if(PingResult) {
           Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);      
            if (!startTimer) {
             tone(SensorD8Buzzer, HIGH, 2000);
             delay(1000);
             tone(SensorD8Buzzer, LOW, 500);
             noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()              
            }
       } 
       Serial.println(F("PingScheduler Routine Timer DONE."));                      
       NetCheckTimer =0;  // Reset Network Ping Scheduler             
    } //end NetCheckTimer
    if (startTimer && varTimeSecs <= 5) {
        digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED 
              lcd.clear();
              lcd.noBlink();
              lcd.setCursor(4, 0);
              lcd.print(F("N O  C O I N S "));
              lcd.setCursor(3, 1);
              lcd.print(F("D E T E C T E D"));
              SysDate=""; 
              SysDate = (String(DateTime.month) + "-" + String(DateTime.day) + "-" + (String(DateTime.year)).substring(0,2) + " " + String(DateTime.hour) + ":" + String(DateTime.minute));      
                  lcd.setCursor(0, 2);
                  lcd.print(F("                  ")); //20lines
                  lcd.setCursor(3, 2);
                  lcd.print(SysDate);
              lcd.setCursor(3, 3);
              lcd.print(F("Time Out :(| "));
                lcd.setCursor(16, 3);  
                (varTimeSecs < 10) ? lcd.print("0") : NULL;
                lcd.print(varTimeSecs);
              lcd.display();
            delay(50);
            tone(SensorD8Buzzer, HIGH, 2000);
            delay(1000);
            tone(SensorD8Buzzer, LOW, 1000);
            delay(50);
            noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()      
    }
    if (startTimer && varTimeSecs == 0) {
            digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED 
            delay(50);
            noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()      
            pindot=false;   // if TRUE=BUYCODE FALSE=To OPEN COINSLOT IF Network is Good
            startTimer = false;     // For Push Button Counter during Initiate DropCoin Sequence
            StarTime = 0;  // 0=To Lock Button During BuyCode Above 0 Enable
            PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES 
            DIPSLAYSTATIC();  //LCDMain Screen
            //delay(50);
            //tone(SensorD8Buzzer, HIGH, 2000);
            //delay(1000);
            //tone(SensorD8Buzzer, HIGH, 1000);
            //tone(SensorD8Buzzer, LOW, 1000);
            //delay(50);
            //noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()             
            Serial.print(F("No COINS Detected..."));
            Serial.println(F("Coins Slot is LOCK..."));
          varTimeSecs = 0;
          tkTimeOut.detach();  
    } //end varTimeSecs == 0
  } //end magkano_na = 0
    
  if(magkano_na > 0) {
     if (varTimeSecs > 0) {
          startTimer = false;     // For Push Button Counter during Initiate DropCoin Sequence      
          varTimeSecs = 0;
          tkTimeOut.detach();        
          noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()                    
     } //end (varTimeSecs > 0)
     if(NetCheckTimer > 199) {
        NetCheckTimer=0;  
     } //end (NetCheckTimer > 200)
  } //end magkano_na > 0
/////////////////////////////////////
// STAND BY MAIN ROUTINE CODE END //
////////////////////////////////////  
  
///////////////////////////////
// BUYCODE Main Routine TOP //       
/////////////////////////////
  if(PingResult) { // Main Loop For BuyCode
      if (pindot) {
          if (BuyCode>0) {
             digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED
               StarTime = 0;  // 0=To Lock Button During BuyCode Above 0 Enable 
               PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES      
               cNTP_Update =0;
               NetCheckTimer =0;
               varBeepTone = 3;
               delay(50);
                   DisplayCode(); // Then Buzz Alert                                  
                      while (varBeepTone >= 0) {
                      tone(SensorD8Buzzer, HIGH, 1000);
                      delay(1000);
                      tone(SensorD8Buzzer, LOW, 500) ; //FA note ...
                      delay (500);
                       varBeepTone--;
                         if (varBeepTone == 0) {  // BuyCode BeepTone
                           noTone(SensorD8Buzzer) ; //Turn off the pin attached to the tone()
                           break;
                         }
                      }
             TELNETCODE(); // Sending Voucher Code to RouterOS as HotspotUser via TELNET
             SalesCODE(); 
                  delay(50);           
                  tc.disconnect();
                  PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES      
                  TimeSecs = 35;
                  SysDate=""; 
                      SysDate = (String(DateTime.month) + "-" + String(DateTime.day) + "-" + (String(DateTime.year)).substring(0,2) + " " + String(DateTime.hour) + ":" + String(DateTime.minute));      
                      lcd.setCursor(0, 0);
                      lcd.print(F("                  ")); //20lines
                      lcd.setCursor(0, 0);
                      lcd.print(SysDate);
                      lcd.setCursor(15, 0);
                      lcd.print(F("T->"));
                      // lcd.setCursor(0, 3);
                      // lcd.print(F("                  ")); //18lines
                      // lcd.setCursor(0, 3);
                      // lcd.print(F("E X I T  Time OUT:")); 
                      lcd.display();
                      loopbreaker = 1;    // To Terminate BuyCode Sequence
                      SysDate="";
                      SysDate = (String(DateTime.month) + "-" + String(DateTime.day) + "-" + (String(DateTime.year)).substring(0,2) + "|" + String(DateTime.hour) + ":" + String(DateTime.minute) + ":" + String(DateTime.second));      
                          while (TimeSecs >= 0) {
                              lcd.setCursor(18, 0);
                              (TimeSecs < 10) ? lcd.print("0") : NULL;
                              lcd.print(TimeSecs);
                                lcd.display();
                                    if (TimeSecs < 12) {
                                      tone(SensorD8Buzzer, HIGH, 1000);
                                    } 
                             stepDown();
                                delay(1000);
                          if(loopbreaker == 0) {
                                magkano_na = 0;  //reset values
                                BuyCode = 0;     //reset values
                                promorate=0;     //reset values
                                loopbreaker = 0; //reset values                            
                                lcd.clear();
                                lcd.setCursor(0, 1);
                                lcd.print(F(" Please Stand By.! "));
                                lcd.setCursor(0, 2);
                                lcd.print(F("Closing Transaction"));  
                                lcd.setCursor(0, 3);
                                lcd.print(SysDate);                                
                                lcd.display();                    
                                
                              delay(1500);
                           break;
                           }                                  
                           if (TimeSecs == 0) {
                                magkano_na = 0;  //reset values
                                BuyCode = 0;     //reset values
                                promorate=0;     //reset values
                                loopbreaker = 0; //reset values
                            break;
                            }
                        }
                      delay(50);
                         // finished transactions  
                          magkano_na = 0;  //reset values
                          BuyCode = 0;     //reset values
                          promorate=0;     //reset values
                          loopbreaker = 0; //reset values
                          TimeSecs = 0;    //reset values  
                 lcd.clear();
                 DIPSLAYTY();
                 delay(3500);
                 pindot = false;
                 strcode = "";
                 DIPSLAYSTATIC();
          Serial.println(F(" Voucher Code Created "));
          Serial.print(F(" Transaction Finished "));
          Serial.print(F("    READY TO SERVE    ")); 
          Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);                                                                                                              
        } //end BuyCode             
      } //end (pindot)               
  } //end (PingResult) 
}  //end ((AdminMode == 1) && (VendoMode == 1)) MainRoutine ENDHere..!
// <================================> <<VENDO LOOP   END>> <================================>
/***********************************************/
/************    END Code here   ***************/
/***********************************************/
// <================================> <<LOOP END>> <================================>  
    if (Refresh)  {
       Refresh = false;
       // Serial.println("Refreshing...");
       // Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
    }
} 
// <================================> <<LOOP END>> <================================>
  
void handleRoot() {
  server.send(200, "text/html", PAGE_AdminMainPage);
}

void TrialSSID () {
  if (varDEMO == 0) { //DemoOnly
  // for DEMO
     SSIDname = "DemoPisoWiFi";
     SSIDpwd =  "";    
//     tcUser = "demoMMXX";
//     tcCode = "MMXXdemo";        

  } else {
   // for OEM
   SSIDname = APssid;
   SSIDpwd =  APpass;    
//   tcUser = "TelnetUser";  //Telnet Account
//   tcCode = "TelnetCode"; //Telnet Password        
  } 
}

void TrialPeriod () {
     digitalWrite(5, LOW); //coinslot LOCKED triggered
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print(F("09 3 5 70 1  22 5 6")); 
     lcd.setCursor(0, 1);
     lcd.print(F("Purchase OEM COPY"));
     lcd.setCursor(0, 2);
     lcd.print(F("My Contact Details"));
     lcd.setCursor(0, 3);
     lcd.print(F("fb.com/Vendotronics"));
     lcd.display(); 
     delay(8000);   
     Serial.println(F("Trial Period Timeout...!")); 
     ESP.restart();
  } 


void NETERROR() {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print(messageLine1);
    lcd.setCursor(2, 1);
    lcd.print(F("ISP Disconnected"));
    lcd.setCursor(2, 2);
    lcd.print(F(" Internet Error "));
    lcd.setCursor(2, 3);
    lcd.print(F("Unable to Connect"));
    lcd.display();
  } 

void ScreenAdminMode() {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print(F("PisoWiFiVendo"));
    lcd.setCursor(1, 1);
    lcd.print(F("SETUP CONFIGURATION"));
    lcd.setCursor(0, 2);
    lcd.print(F("AP SSID:"));
    lcd.print(SSIDname.substring(0,12));
    lcd.setCursor(1, 3);
    lcd.print(F("http://192.168.4.1"));
    lcd.display();

  }
void ScreenVendoMode() {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print(F("Please Wait....."));
    lcd.setCursor(2, 1);
    lcd.print(F("Validating Setup"));
    lcd.setCursor(2, 2);
    lcd.print(F("Checking Services"));
    lcd.display();
  }
  
void DIPSLAYSTATIC() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print(messageLine1);
  lcd.setCursor(0, 1);
  lcd.print(messageLine2);
  lcd.setCursor(1, 2);
  // lcd.print("Connect to HOTSPOT ");
  lcd.print(F("  Connect to WIFI  "));
  lcd.setCursor(0, 3);
  lcd.print(varWifiSSID.substring(0,19));
  lcd.display();
  }
  
void DIPSLAYTY() {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print(messageLine1);
    lcd.setCursor(1, 1);
    lcd.print(F("   Salamat  Po.!   "));
    lcd.setCursor(0, 2);
    lcd.print(F("GO http://"));
    lcd.setCursor(10, 2);
    // lcd.print(varWifiSSID.substring(0,19));
    lcd.print(routerOS_ip);    
    lcd.setCursor(1, 3);
    lcd.print(messageLine4);
    lcd.display();

  }    
void DISPLAYbMSG() { //bootup message 
   // set cursor to first column, first row
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(BmessageLine1);
  lcd.setCursor(1, 1);
  lcd.print(BmessageLine2);
  lcd.setCursor(0, 2);
  lcd.print(BmessageLine3);
  lcd.display();
  scrollText(3, messageToScroll, 250, lcdColumns);
  }
void PushBtnDisplay() {
   // set cursor to first column, first row
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(messagePressBtn00);
  lcd.setCursor(1, 1);
  lcd.print(messagePressBtn01);
  lcd.setCursor(1, 2);
  lcd.print(messagePressBtn02);
  lcd.setCursor(1, 3);
  lcd.print(messagePressBtn03);
  lcd.display();
// scrollTxtNormal(3, messagePressBtn02, 250, lcdColumns);
}
  
void DISPLAYMSG() {
   // set cursor to first column, first row
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(messageLine1);
  lcd.setCursor(1, 1);
  lcd.print(messageLine2);
  lcd.setCursor(1, 2);
  lcd.print(messageLine3);
  lcd.display();
  scrollText(3, messageToScroll, 250, lcdColumns);
  }
void DISPLAYLOOPSCR() {
   // set cursor to first column, first row
//  String xSSID = varWifiSSID;
//  String messageLine3 = "SSID:" + xSSID.substring(0,15);   
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print(messageLine1);
  lcd.setCursor(0, 1);
  lcd.print(messageLine2);
  lcd.setCursor(0, 2);
  lcd.print(F("Connect to WIFI "));
  lcd.setCursor(0, 3);
  lcd.print(varWifiSSID.substring(0,19));
  lcd.display();
//    scrollText(3, messageToScroll01, 250, lcdColumns);
  }
  

void CheckVendoRate() {
//varPH = config.vPH; // base rate per min
Serial.print(F("BaseRate Per Min: "));
Serial.println(varPH);
//varPH1 = config.vPH1; // 5=1H
//varPK1 = config.vPK1; // 60min
Serial.print(F("PromoRate 1: "));
Serial.println(varPH1);
Serial.println(varPK1);
//varPH2 = config.vPH2; // 10=3H
//varPK2 = config.vPK2; // 60*3
Serial.print(F("PromoRate 2: "));
Serial.println(varPH2);
Serial.println(varPK2);
//varPH3 = config.vPH3; // 20=10H
//varPK3 = config.vPK3; // 60*10
Serial.print(F("PromoRate 3: "));
Serial.println(varPH3);
Serial.println(varPK3);
//varPH4 = config.vPH4; // 30=1D
//varPK4 = config.vPK4; // 60*24
Serial.print(F("PromoRate 4: "));
Serial.println(varPH4);
Serial.println(varPK4);
//varPH5 = config.vPH5; // 50=3D
//varPK5 = config.vPK5; // 60*24*3
Serial.print(F("PromoRate 5: "));
Serial.println(varPH5);
Serial.println(varPK5);
//varPH6 = config.vPH6; // 100=7D
//varPK6 = config.vPK6; // 60*24*7
Serial.print(F("PromoRate 6: "));
Serial.println(varPH6);
Serial.println(varPK6);
//varPH7 = config.vPH7; // 300=30D++
//varPK7 = config.vPK7; // 60*24*30
Serial.print(F("PromoRate 7: "));
Serial.println(varPH7);
Serial.println(varPK7);
}

// ++++++++++++++ ### DISPLAY COIN COUNTER FUNCTION TOP ### ++++++++++++++
void DisplayCoinCounter(void) {
        varTimeSecs = 0;
        if (varTimeSecs == 0) noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()          
        lcd.clear();
        lcd.noBlink();
        lcd.setCursor(1, 0);
        lcd.print(messageLine1);
        lcd.setCursor(3, 1);
        lcd.print("BAYAD: ");
//        lcd.print(magkano_na);
        lcd.setCursor(15, 1);
        lcd.print("Peso");
        lcd.print("          ");            
        lcd.setCursor(2, 2);
        lcd.print("D->");
//        lcd.print(Day);
        lcd.setCursor(8, 2);
        lcd.print("H->");
//        lcd.print(newhours);
        lcd.setCursor(14, 2);
        lcd.print("M->");
//        lcd.print(newmins);
        lcd.setCursor(2, 3);
        lcd.print(messageLine41);  
        lcd.display();
    ComputeCoins();    
}

void ComputeCoins(void) {
// ADJUST RATES HERE
// Rates 1Piso= 10Mins 5=1H 10=3H 20=10H 30=1D        
// temp=magkano_na*600; // in secs value 1P for 10mins.               
// email=1D@www.com
// insert for promo packages

         int ipiso = magkano_na;
         StarTime += ipiso; // for LCD Display
            // lastTrigger = millis(); // For Push Button Counter during Initiate DropCoin Sequence
            // startTimer = false;     // For Push Button Counter during Initiate DropCoin Sequence
         if (magkano_na < varPH1) {  // Below 5
                Min = magkano_na * varPH; // 10mins  per 1 peso
                HSprof = varHP1; // Hotspot Profile
                HSemail = " email=00@www.com"; // Hotspot Email Profile
         }
         if (magkano_na >= varPH1 && magkano_na < (varPH2-1)) {  // 5Peso 5to9 = 1HR
                if (magkano_na == varPH1) {
                  Min = varPK1;
                }
                else {
                xmins =0;  
                xmins = magkano_na - varPH1;
                Min = ((xmins * varPH) + varPK1);
                HSprof = varHP1; // Hotspot Profile
                HSemail = " email=11D@www.com"; // Hotspot Email Profile 
                }                
         }
         if (magkano_na >= varPH2 && magkano_na < (varPH3-1)) { // 10Peso 10to19 = 3HRS
                if (magkano_na == varPH2) {
                  Min=(varPK2);
                }
                else {
                xmins =0;  
                xmins = magkano_na - varPH2;
                Min = ((xmins * varPH) + (varPK2));
                HSprof = varHP2; // Hotspot Profile
                HSemail = " email=22D@www.com"; // Hotspot Email Profile 
                }                
         }
         if (magkano_na >= varPH3 && magkano_na < (varPH4-1)) { // 20Peso 20to29 = 10HRS
                if (magkano_na == varPH3) {
                  Min=(varPK3);
                }
                else {
                xmins =0;  
                xmins = magkano_na - varPH3;
                Min = ((xmins * varPH) + (varPK3));
                HSprof = varHP3; // Hotspot Profile
                HSemail = " email=33@www.com"; // Hotspot Email Profile 
                }                
         }
         if (magkano_na >= varPH4 && magkano_na < (varPH5-1)) { // 30Peso 30to39 = 24HRS/1D
                if (magkano_na == varPH4) {
                  Min=(varPK4);
                }
                else {
                xmins =0;  
                xmins = magkano_na - varPH4;
                Min = ((xmins * varPH) + (varPK4));
                HSprof = varHP4; // Hotspot Profile
                HSemail = " email=44@www.com"; // Hotspot Email Profile 
                }                
         }
         if (magkano_na >= varPH5 && magkano_na < (varPH6-1)) { // 50Peso 50to99 = 3Days
                if (magkano_na == varPH5) {
                  Min=(varPK5);
                }
                else {
                xmins =0;  
                xmins = magkano_na - varPH5;
                Min = ((xmins * varPH) + (varPK5)); 
                HSprof = varHP5; // Hotspot Profile
                HSemail = " email=55@www.com"; // Hotspot Email Profile
                }                
         }
         if (magkano_na >= varPH6 && magkano_na < (varPH7-1)) { // 100Peso = 7Days
                if (magkano_na == varPH6) {
                  Min=(varPK6);
                }
                else {
                xmins =0;  
                xmins = magkano_na - varPH6;
                Min = ((xmins * varPH) + (varPK6));
                HSprof = varHP6; // Hotspot Profile
                HSemail = " email=66@www.com"; // Hotspot Email Profile 
                }                
         }
         if (magkano_na >= varPH7) { // 300Peso and UP = 30Days
                if (magkano_na == varPH7) {
                  Min=(varPK7);
                }
                else {
                xmins =0;  
                xmins = magkano_na - varPH7;
                Min = ((xmins * varPH) + (varPK7));
                HSprof = varHP7; // Hotspot Profile
                HSemail = " email=77@www.com"; // Hotspot Email Profile 
                }                
         }         
                  
            Hour = floor(Min / 60);
            Day = floor(Hour / 24);
            newhours = Hour - (Day * 24);
            newmins = Min - (Hour * 60);
            xtmp_days = Day;
            xtmp_hours = newhours;
            xtmp_mins = newmins;
            xtmp_magkano = magkano_na;            
            xtemperature = newmins;   //MIN
            xhumidity = newhours;     //HOUR
            xpressure = Day;          //DAY
    CoinCounterLCD();
// insert for promo packages        
}

void CoinCounterLCD(void) {
        lcd.noBlink();
        lcd.setCursor(11, 1);        
    if (magkano_na < 10) lcd.print("0");
        lcd.print(magkano_na);

        lcd.setCursor(5, 2);
    if (Day < 10) lcd.print("0");
        lcd.print(Day);

        lcd.setCursor(11, 2);
    if (newhours < 10) lcd.print("0");
        lcd.print(newhours);

        lcd.setCursor(17, 2);
    if (newmins < 10) lcd.print("0");
        lcd.print(newmins); 
    lcd.display();     
}
// ++++++++++++++ ### DISPLAY COIN COUNTER FUNCTION END ### ++++++++++++++

void SalesCODE () {
       tempadd=280;  // eeprom address location
       varSales = EEPROM.get(tempadd,config.vSales);
       config.vSales = varSales + magkano_na;
       EEPROM.put(tempadd,config.vSales); // Sales Counter
       tempadd += sizeof(int);
       EEPROM.put(tempadd,config.vSales);
       EEPROM.commit();
       Serial.print("Vendo UpTo Date Sales : ");
       Serial.println(EEPROM.get(tempadd,config.vSales));
 }

  void CheckTimeCtr() {
//    Serial.print(daysOfTheWeek[timeClient.getDay()]);
//    Serial.print(", ");
//    Serial.print(timeClient.getHours());
//    Serial.print(DateTime.hour);
//    Serial.print(":");
//    Serial.print(timeClient.getMinutes());
//    Serial.print(":");
//    Serial.println(timeClient.getSeconds());
 char c_letter[3] = {letters[random(34)]};
 int WhaTime = (DateTime.hour);
 long randNumber1;
 long randNumber2; 
 long randNumber3;
 String randomletter = c_letter;
 strcode = "";                
      if (WhaTime <= 12) {
         randNumber1 = random(11,22);
         randNumber2 = random(33,44); 
         randNumber3 = random(randNumber1,randNumber2);                
      } else {
         randNumber1 = random(55,77);
         randNumber2 = random(88,99);                
         randNumber3 = random(randNumber1,randNumber2);
      }
    String randomNum = String(randNumber3);    
    if (WhaTime <= 9) strcode = String(DateTime.hour);
    
//    strcode = String(DateTime.hour);
    strcode = strcode + String(DateTime.minute);
    strcode = strcode + String(DateTime.second);
    strcode = strcode + String(randomletter) + String(magkano_na) + randomNum;
    strcode.trim();
    //strcode = strcode + String("#") + String(magkano_na) + randomNum;
//    strcode = strcode + String("#") + String(magkano_na) + String(c);

}

  void DisplayCode() {
        CheckTimeCtr();  // Code Generator
        delay(50);
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print(messageLine1);
        lcd.setCursor(2, 1);
        lcd.print(F("Your Voucher CODE "));
        // lcd.print(strcode);
        lcd.setCursor(0, 2);
        lcd.print("                   ");
        lcd.setCursor(0, 2);
        lcd.print("==>");        
        lcd.setCursor(17, 2);
        lcd.print("<==");        
        lcd.setCursor(6, 2);
        lcd.print(strcode);
        lcd.setCursor(0, 3);
        lcd.print(messageLine42);
        lcd.display();
        Serial.print(F("Voucher Code : "));
        Serial.println(strcode);
        Serial.print(F("Promo : "));
        Serial.println(promorate);
        Serial.print(F("Temp Computation : "));
        Serial.println(xmins);
        Serial.print(F("Time in a Day : "));
        Serial.println(Day);
        Serial.print(F("Time in a Hrs : "));
        Serial.println(Hour);
        Serial.print(F("Time in a Min : "));
        Serial.println(Min);
        Serial.print(F("Time in a Sec : "));
        Serial.println(Sec);
        // timecheck = true;
        // lastTrigger = millis(); 
}
    
 void TELNETCODE() {  
 // HOTSPOT TEMPLATE 
 //add limit-uptime=1d3h name=wsx password=WSX profile=R1
      telnetcmd = "/ip hotspot user add name=";                  // Router Command
      telnetcmd += strcode;                                      // VoucherCode as UsernamePassword
      telnetcmd += " limit-uptime=";                             // Buy Time Definition
      telnetcmd += Min;                                          // Total Buy Time
//      telnetcmd += "m password=";                              // password     
//      telnetcmd += "m";                                        // mins
      telnetcmd += "m profile=";                                 // TIME:inMinutes plus User Profile 
      telnetcmd += HSprof;                                       // User Profile
      telnetcmd += HSemail;                                      // Hotspot Email Profile
      int telnetcmd_len = telnetcmd.length() + 1;                // init command
      char Sndtelnetcmd[telnetcmd_len];                          // convert command
      telnetcmd.toCharArray(Sndtelnetcmd, telnetcmd_len);        // Command Params
//      tc.login(mikrotikRouterIp, Tuser, Tpass);                // Login using Telnet routerOS_ip
    if (varDEMO > 0) { 
    int user_len = varTelnetUsr.length() + 1;
    char user_char[user_len];
    varTelnetUsr.toCharArray(user_char, user_len);    
    const char* Tuser = user_char;  //Telnet User Account
    
    int pwd_len = varTelnetPwd.length() + 1;
    char pwd_char[pwd_len];
    varTelnetPwd.toCharArray(pwd_char, pwd_len);    
    const char* Tpass = pwd_char;  //Telnet Password   
    tc.login(routerOS_ip, Tuser, Tpass);                // Login using Telnet routerOS_ip
    tc.sendCommand(Sndtelnetcmd);                         // Send Telnet Command
//    Serial.print("Telnet Cmd: ");
//    Serial.println(Sndtelnetcmd);
  } else {
    const char* TCusr = "demoMMXX";
    const char* TCpwd = "MMXXdemo";
    tc.login(routerOS_ip, TCusr, TCpwd);                // Login using Telnet routerOS_ip
    tc.sendCommand(Sndtelnetcmd);                            // Send Telnet Command
//    Serial.println(F("HotSpotUser --> Done Transaction"));

  } 
        
}    

void PASSATIMECODE() {
   // telnet code to add time AKA pasaload
  // /file set [/file find where name=cntusrTO.txt] contents=""
    int user_len = varTelnetUsr.length() + 1;
    char user_char[user_len];
    varTelnetUsr.toCharArray(user_char, user_len);    
    const char* Tuser = user_char;  //Telnet User Account
    
    int pwd_len = varTelnetPwd.length() + 1;
    char pwd_char[pwd_len];
    varTelnetPwd.toCharArray(pwd_char, pwd_len);    
    const char* Tpass = pwd_char;  //Telnet Password   
      
      aCodetcmd = "/file set [/file find where name=cntusrTO.txt]";    //Router Command
      aCodetcmd += " contents=";                                        //add contents
      aCodetcmd += strTOname;                                          //value of contents
      int aCodetcmd_len = aCodetcmd.length() + 1;                      // init command
      char aSndtelnetcmd[aCodetcmd_len];                               // convert command
      aCodetcmd.toCharArray(aSndtelnetcmd, aCodetcmd_len);             // Command Params
// second set
      bCodetcmd = "/file set [/file find where name=cntusrFM.txt]";    //Router Command
      bCodetcmd += " contents=";                                        //add contents
      bCodetcmd += strFMname;                                          //value of contents
      int bCodetcmd_len = bCodetcmd.length() + 1;                      // init command
      char bSndtelnetcmd[bCodetcmd_len];                               // convert command
      bCodetcmd.toCharArray(bSndtelnetcmd, bCodetcmd_len);             // Command Params
// last set
      cCodetcmd = "/system script run pasatime";    //Router Command
      int cCodetcmd_len = cCodetcmd.length() + 1;                      // init command
      char cSndtelnetcmd[cCodetcmd_len];                               // convert command
      cCodetcmd.toCharArray(cSndtelnetcmd, cCodetcmd_len);             // Command Params      
      tc.login(routerOS_ip, Tuser, Tpass);                // Login using Telnet routerOS_ip
      tc.sendCommand(aSndtelnetcmd);                                   // Send Telnet Command
//      Serial.print("TelnetCmd: ");
//      Serial.println(aSndtelnetcmd);
      tc.sendCommand(bSndtelnetcmd);                                   // Send Telnet Command
//      Serial.print("TelnetCmd: ");
//      Serial.println(bSndtelnetcmd);
      tc.sendCommand(cSndtelnetcmd);                                   // Send Telnet Command
//      Serial.print("TelnetCmd: ");
//      Serial.println(cSndtelnetcmd);
    Serial.println(F("PASATIME Service --> Transaction Completed :) "));
    tc.disconnect();
    aCodetcmd=""; //Reset Mikrotik Telnet command
    bCodetcmd=""; //Reset Mikrotik Telnet command
    cCodetcmd=""; //Reset Mikrotik Telnet command    
}

// ^^^^^^^^^^^^^^^^^^ Timer Delay Section Package TOP ^^^^^^^^^^^^^^^^^^^^^^^^^ // 
void trigger() {
// Serial.println(F("Passing thru CountdownTime via trigger...")); 
 while (TimeSecs >= 0) {
 lcd.setCursor(18, 0);
 (TimeSecs < 10) ? lcd.print("0") : NULL;
 lcd.print(TimeSecs);
 lcd.display();
 stepDown();
 delay(1000);
 }
}

void stepDown() {
// Serial.println(F("Passing thru CountdownTime via stepDown..."));
 if (TimeSecs > 0) {
 TimeSecs -= 1;
 } else {
 trigger();
 }
}
/*
void CountDownTimerCode() {
     while (TimeSecs >= 0) {
     lcd.setCursor(17, 3);
     (TimeSecs < 10) ? lcd.print("0") : NULL;
     lcd.print(TimeSecs);
     lcd.display();
     stepDown();
     delay(50);
   } 
}
*/
// ^^^^^^^^^^^^^^^^^^ Timer Delay Section Package END ^^^^^^^^^^^^^^^^^^^^^^^^^ // 
// /=/=/=/=/=/=/=/=/=/=/=/=/======= Check Internet TOP =======/=/=/=/=/=/=/=/=/=/=/=/=/
void CHECK_ISP(){
     Serial.println(F("CHECKING INTERNET CONNECTION"));
     RunOnce = true;  // to check DNS routine using LOOP Section     
             lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print(F("One Moment Please.!"));
             lcd.setCursor(0, 1);
             lcd.print(F("V a l i d a t i n g"));                  
             lcd.setCursor(0, 2);
             lcd.print(F("  I N T E R N E T  "));                  
             lcd.setCursor(0, 3);
             lcd.print(F("C o n n e c t i o n"));
             lcd.display();                       
}
void CHECK_DNS() {
RunOnce = false;  // to check DNS routine using LOOP Section
const char* host_ggl = "www.google.com";
  Serial.print(F("Pinging host "));
  Serial.println(host_ggl);
                           if(Ping.ping(host_ggl, 3)) {
                                       delay(50);
                                         noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()
                                         StarTime =1;                    
                                         startTimer = true; // For Push Button Counter during Initiate DropCoin Sequence
                                         varTimeSecs = timeSeconds;
                                         tkTimeOut.attach(1,TickTimeOut);
                                         PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES 
                                         PushBtnDisplay();  
                                         digitalWrite(SensorD1CoinLck, HIGH); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED                                                                        
                                  Serial.println(F(" Ready to Serve :)"));            
                            } else {
                                  Serial.print("Ping Error :(");
                                   digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED
                                     PingResult = false; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
                                        Serial.println(F("  Google....TTL Time-Out :(")); 
                                        RunPing();     
                            }
}

void RunPing() {
  Serial.println(F("Checking Network Issue"));
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi NO Issue"));
    if(Ping.ping(routerOS_ip, 3)) {
      Serial.println(F("Router NO Issue"));
        PingResult = true; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
        //netcheck = 1;
        //neterror=0;  // Network ERROR  0=GOOD 1=WIFI 2=ROUTER 3=ISP
        if(Ping.ping(remote_ip, 3)) {
          Serial.println(F("ISP NO Issue"));
           PingResult = true; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
          //netcheck = 1;
          //neterror=0;  // Network ERROR  0=GOOD 1=WIFI 2=ROUTER 3=ISP
          Serial.println(F("-- NO ISSUES -- Network Validated :)"));
        } else {
          Serial.println(F("TTL Host Down"));
          PingResult = false; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
          digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED  
          CHECK_ISPERROR();                  
        }
    } else {
       //netcheck = 0;
       //neterror=2;  // Network ERROR  0=GOOD 1=WIFI 2=ROUTER 3=ISP
       PingResult = false; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
       digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED
       CHECK_MIKROTIKERROR();
    }    
  } else {
          //netcheck = 0;
          //neterror=1;  // Network ERROR  0=GOOD 1=WIFI 2=ROUTER 3=ISP
        Serial.println(F("WiFi Problem"));
        PingResult = false; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
        digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED
        CHECKWIFIERROR();     
  }  
}

void CHECKWIFIERROR(){
                       Serial.println(F("WiFi Disconnected NETERROR=1"));
                       PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES
                           lcd.clear();
                           lcd.setCursor(2, 1);
                           lcd.print("Checking Wireless");
                           lcd.setCursor(2, 2);
                           lcd.print("Please Wait......");
                           lcd.setCursor(2, 3);
                           lcd.print(".....reconnecting");
                           lcd.display(); 
                         WifiTimeOutDisplay();
                         PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES                      
                         while (WiFi.status() != WL_CONNECTED) {
                           delay(500);
                           WifiTimeOut();
                           Serial.print(F("1"));
                                if (wifiTimer > 555) {
                                   Serial.println(F(""));
                                   Serial.print(F("WiFi Connection TimeOut: ReStarting..."));
                                   lcd.clear();
                                   lcd.setCursor(3, 2);
                                   lcd.print("WiFi E R R O R");
                                   lcd.display();                                   
                                   delay(1000);
                                   PingResult = false; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
                                   break;
                                   // ESP.restart();                                                          
                                 } 
                          }
                           Serial.println(F(""));
                           Serial.print(F("Connected to :"));
                           Serial.print(F("IP address: "));
                           Serial.println(WiFi.localIP());
                                 int connRes = WiFi.waitForConnectResult();
                            Serial.print(F("connRes: "));
                            Serial.println(connRes);
                             if (connRes>0) {
                                   PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES
                                   PingResult = true; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
                                   lcd.clear();
                                   lcd.setCursor(1, 1);
                                   lcd.print("PRESS START NOW !");
                                   lcd.setCursor(2, 2);
                                   lcd.print("WIFI Reconnected");
                                   lcd.display(); 
                                   delay(1000);
                              }                       
                              delay(50);  
}
void CHECK_MIKROTIKERROR(){
         Serial.println(F("Router IP Unreachable NETERROR=2"));
          bool pingresponse = Ping.ping(routerOS_ip, 5);
          int avg_time_ms = Ping.averageTime();
                            lcd.clear();
                            lcd.setCursor(2, 0);
                            lcd.print(" Invalid  IP  ");
                            lcd.setCursor(2, 1);
                            lcd.print(" Router Error ");
                            lcd.setCursor(2, 2);
                            lcd.print("Connection Lost");
                            lcd.setCursor(0, 3);
                            lcd.print("RouterOS TIMEOUT:");  
                            lcd.display(); 
                               wifiTimer = 0;                            
                               lcd.setCursor(17, 3);
                               lcd.print(wifiTimer);
                               lcd.display();
                         PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES                      
                            while (!pingresponse) {
                                   pingresponse = Ping.ping(routerOS_ip, 5);
                                   avg_time_ms = Ping.averageTime();
                                   wifiTimer++;
                                   delay(500);
                                     lcd.setCursor(17, 3);
                                     lcd.print(wifiTimer);
                                     lcd.display();
                                     Serial.print(F("2"));
                                if (wifiTimer > 54) {
                                   Serial.println(F(""));
                                   Serial.print(F("RouterOS Ping TimeOut: ReStarting..."));
                                   lcd.clear();
                                   lcd.setCursor(3, 2);
                                   lcd.print("Router  ERROR");
                                   lcd.display();
                                   delay(2000);
                                   // ESP.restart();
                                    PingResult = false; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
                                    break;                      
                                 } 
                            } 
                            Serial.print(F("avg time ms: "));
                            Serial.println(avg_time_ms);
                             if (avg_time_ms>0) {
                                   PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES
                                   PingResult = true; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
                                   lcd.clear();
                                   lcd.setCursor(1, 1);
                                   lcd.print("PRESS START NOW !");
                                   lcd.setCursor(0, 2);
                                   lcd.print("RouterOS Reconnected");
                                   lcd.display();
                                   delay(1000); 
                              }                                                             
}

void CHECK_ISPERROR(){
       Serial.println(F("CHECK Your ISP...:( NETERROR=3"));
         NETERROR();
          bool pingresponse = Ping.ping(remote_ip, 5);
          int avg_time_ms = Ping.averageTime();
          wifiTimer = 0;  
            PushButtonKey = false; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES                      
                            while (!pingresponse) {
                                   pingresponse = Ping.ping(remote_ip, 5);
                                   avg_time_ms = Ping.averageTime();
                                   wifiTimer++;
                                   delay(500);
                                     //Serial.print("3");
                                     Serial.print(F("3"));
                                if (wifiTimer > 100) {
                                   Serial.println(F(""));
                                   Serial.print(F("ISP DNS Ping TimeOut: ReStarting..."));
                                   lcd.clear();
                                   lcd.setCursor(3, 2);
                                   lcd.print("I S P  E R R O R");
                                   lcd.display();
                                   delay(2000);
                                   // ESP.restart();
                                    PingResult = false; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
                                    break;                      
                                 } 
                            } 
                            Serial.print(F("avg time ms: "));
                            Serial.println(avg_time_ms);
                             if (avg_time_ms>0) {
                                   PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES
                                   PingResult = true; // PingResult  TRUE=Ping ALIVE FALSE=Ping TimeOUT
                                   lcd.clear();
                                   lcd.setCursor(1, 1);
                                   lcd.print("PRESS START NOW !");
                                   lcd.setCursor(1, 2);
                                   lcd.print(" ISP  Reconnected");
                                   lcd.display();
                                   delay(1000); 
                              }                                                                                               
}

void NetworkReCheck() {
      if(magkano_na == 0) {
         cNTP_Update =0;
          // startTimer = false;
          // lastTrigger = millis(); 
            Serial.println(F("Checking Status Connection")); //STEP-MARKER 
              if (WiFi.status() == WL_CONNECTED) {
                   PushButtonKey = true; //PUSH BUTTON  FALSE=DISABLE FUNCTION ROUTINES TRUE=PROCESS ROUTINES     
                   pindot=true;   // if TRUE=BUYCODE FALSE=To OPEN COINSLOT IF Network is Good
                      if(Ping.ping(routerOS_ip, 3)) { //If Wi-Fi connected successfully
                              delay(1000);
                              netcheck=1;  // Network GOOD
                              Serial.println(F("WiFi connected."));
                                      if(Ping.ping(remote_ip, 3)) {
                                            delay(1000);
                                            netcheck=1;  // Network GOOD
                                            Serial.println(F("TTL is Alive:"));
                                            pindot=false;   // if TRUE=BUYCODE FALSE=CHECK_Connection then OPEN COINSLOT IF Network is Good     
                                       } else {  
                                           delay(50);
                                           netcheck=0;  // Network ERROR                      
                                           digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED
                                           NETERROR();
                                           Serial.println(F("TTL Host Down"));
                                           delay(50);
                                           tone(SensorD8Buzzer, HIGH, 2000);
                                           delay(1000);
                                           tone(SensorD8Buzzer, LOW, 500);
                                           noTone(SensorD8Buzzer); //Turn off the pin attached to the tone()
                                       }    
                     } else {
                           delay(50);
                            digitalWrite(SensorD1CoinLck, LOW); //COINSLOT Activation LOW=LOCKED HIGH=UNLOCKED 
                            netcheck=0;  // Network ERROR
                            lcd.clear();
                            lcd.setCursor(2, 1);
                            lcd.print("  Invalid  WiFi ");
                            lcd.setCursor(2, 2);
                            lcd.print("Connection Error");
                            lcd.setCursor(2, 3);
                            lcd.print("Check Wireless AP");
                            lcd.display();    
                            Serial.println(F("WiFi Disconnected - NetworkReCheck()"));
                            delay(500); 
                    }
             } else {
                netcheck=0;  // Network ERROR
                 delay(500);
                  pindot=false;   // if TRUE=BUYCODE FALSE=CHECK_Connection then OPEN COINSLOT IF Network is Good
             }                     
   } // end if(magkano_na == 0)  
}
// /=/=/=/=/=/=/=/=/=/=/=/=/======= Check Internet END =======/=/=/=/=/=/=/=/=/=/=/=/=/  
void WifiTimeOutDisplay() {
                      wifiTimer = 0;
                      lcd.setCursor(0, 0);
                      lcd.print("                    "); //20lines
                      lcd.setCursor(7, 0);
                      lcd.print("Time OUT: "); 
                      lcd.display();
}
void WifiTimeOut() {
  wifiTimer++;
                      lcd.setCursor(16, 0);
                      lcd.print(wifiTimer);
                      lcd.display();
}
// <================================>
/* END OF LINE */
/*
 *   
  Permission is hereby granted, free of charge, to any person obtaining 
  a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*
*
    S01-E01   Rommel      05/21/2020 Initial coding (Setup Configuration WiFi/Rates)
    S01-E02   Rommel      05/29/2020 Password Generation Rates / Promo Rates Base
    S01-E03   Rommel      05/31/2020 FixBug Promo Rates Base / ++NTP sync.
    S01-E04   Rommel      06/31/2020 FixBug Wifi able to Detect HIDDEN SSID / REMODEL Hotspot CODE
    S01-E05   Rommel      06/01/2020 FixBug REMODEL Promo Rates Base / ++Telnet Integration
    S01-E06   Rommel      06/01/2020 Closing E01S01-E01S06 next phase-> / E02-S01 ++ Start-UP WiFi Portal Configuration (END version)
    S02-E01   Rommel      06/02/2020 --Start-UP WiFi Portal Configuration (postponed)
    S02-E02   Rommel      06/02/2020 -- Add Built-in WWW / -- Add PASATIME Service
    S02-E03   Rommel      06/10/2020 -- Add SSID WEBCONFIG / Add PASATIME Service (TEMP Disable)
    S02-E04   Rommel      06/13/2020 -- same as E02-S03 Add Variable Piso Rates via WebConfig         
    S02-E05   Rommel      06/15/2020 -- Fix LCD to 20x4 / ADD Sales Counter  / ReModel WiFi Name 
    S02-E06   Rommel      06/17/2020 -- remodel WEBCONFIG / Reboot Button Added (END version) 
    
    Version  Modified By     Date        Comment / Update ++ / Developement --    
   -------   -----------  ---------- --------------------------------------------
    S03-E01   Rommel      06/18/2020 ++ Web Coin Counter 
    S03-E02   Rommel      06/19/2020 ++ Add WebConfig Admin LOCK / PASATIME / Web Coin Counter (pending) 
                                     ??? MAC-ADD Captured
    S03-E03   Rommel      06/19/2020 Project Closed // WebConfig / Pasatime / Rate Varaibles / Admin Lock 
    S03-E04   Rommel      06/19/2020 Project Closed ADD WebConfig Telnet Account // OEM and DEMO FLAGS
    S03-E05   Rommel      06/19/2020 Project Closed ...Minor CHANGES >> 
                          fixing --> TIMERS / DELAYS / TEXT SCREEN / LastTwo Strings VoucherCode
    S03-E06   Rommel      07/02/2020 Project Closed ...Minor CHANGES >> ADD CaptivePortal / DNS Service During ADMIN/SETUP Mode 
    S03-E07   Rommel      07/03/2020 Project Closed ...Minor CHANGES >> Revised LOOP Section and Remodel CoinsSlot Behavior
                                     Change on Button sequencing
    S03-E08   Rommel      07/04/2020 Project Closed ...Minor CHANGES >> Revised Time Delay ~ Coin Counter ~ Diplay Coin Sequence  
                                     ADD Diplay Countdown Timer after CODE Generator  / Adjust Captive Portal TBC: tobeFIX Display Coin Counter Next LOOP
    S03-E09   Rommel      07/05/2020 Project Closed ...Minor CHANGES >> FIXED ISSUE on S03-E08
                                     TBC : tobeFIX PushButton and Network Checking Before Coin Drop
    S03-E10   Rommel      07/05/2020 Project Closed ...Minor CHANGES >> FIXED ISSUE on S03-E09
                                     TBC : tobeFIX Reboot while Checking Network Services 
    S03-E11   Rommel      07/06/2020 Project Closed ...Minor CHANGES >> FIXED ISSUE on S03-E10 / ADD Hotspot Profile |SYSTEM FINISHED|
    S03-E12   Rommel      07/15/2020 Project Closed ...Minor CHANGES >> ADD TONES Buzzer |SYSTEM FINISHED|
    S03-E14   Rommel      07/18/2020 Project Closed ...Minor CHANGES >> Fix Bug on VoucherCode Generator |SYSTEM FINISHED|
    S03-E15   Rommel      07/20/2020 Project Closed ...Minor CHANGES >> ADD Debouncing / ReModel PUSH Button structure |SYSTEM FINISHED|
                                                    ...Change RANDOM code generator base on Current TIME / 
                                                    ...reboot command change to HotspotIP/sengkang.html
    S03-E16   Rommel      07/21/2020 SYSTEM FINISHED... Final Enhancement // PUSH BUTTON Routine - Downtime ERROR Capturing and Recovering Routines
                                                        FINAL TESTING...
*/
/* Beyond this line are UNDOCUMENTED */
/*
 * CHEEEEERSSS !
 * 
 */
