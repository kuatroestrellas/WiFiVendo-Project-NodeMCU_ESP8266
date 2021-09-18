#ifndef GLOBAL_H
#define GLOBAL_H


// ESP8266WebServer server(80);									 // The Webserver
// AsyncWebServer server(80);                    // Create AsyncWebServer object on port 80
boolean firstStart = true;										// On firststart = true, NTP will try to get a valid time
unsigned int StartUpTimeOut=0;  // Warning Buzzer for Counter for Disabling the AdminMode
int AdminTimeOutCounter = 0;									// Counter for Disabling the AdminMode
strDateTime DateTime;											// Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;											// NTP Client
unsigned long UnixTimestamp = 0;								// GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;											// Counter for Updating the time via NTP
Ticker tkSecond;												// Second - Timer for Updating Datetime Structure
Ticker tkTimeOut;
Ticker tkNetCheckTimer;         // Network Timer Scheduler 
boolean AdminEnabled = true;		// Enable Admin Mode for a given Time
byte Minute_Old = 100;				// Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)
int BootSetup = 0;           // BootSetup initial 0
int tempadd;
int xtmp_days;
int xtmp_hours;
int xtmp_mins;
int xtmp_magkano;
/*
#define varPH // base rate per min
#define varPH1 // 5=1H
#define varPK1 // 60min
#define varPH2 // 10=3H
#define varPK2 // 60*3
#define varPH3 // 20=10H
#define varPK3 // 60*10
#define varPH4 // 30=1D
#define varPK4 // 60*24
#define varPH5 // 50=3D
#define varPK5 // 60*24*3
#define varPH6 // 100=7D
#define varPK6 // 60*24*7
#define varPH7 // 300=30D++
#define varPK7 // 60*24*30
*/

struct strConfig {
	String ssid;
	String password;
	byte  IP[4];
	byte  Netmask[4];
	byte  Gateway[4];
	boolean dhcp;
	String ntpServerName;
	long Update_Time_Via_NTP_Every;
	long timezone;
	boolean daylight;
	String DeviceName;
int vPH;       // base rate per min
int vPH1;       // 5=1H
int vPK1;      // 60min
int vPH2;      // 10=3H
int vPK2;     // 60*3
int vPH3;      // 20=10H
int vPK3;     // 60*10
int vPH4;      // 30=1D
int vPK4;    // 60*24
int vPH5;      // 50=3D
int vPK5;    // 60*24*3
int vPH6;     // 100=7D
int vPK6;   // 60*24*7
int vPH7;     // 300=30D++
int vPK7;   // 60*24*30
int vHP1; // Hotspot Profile
int vHP2; // Hotspot Profile
int vHP3; // Hotspot Profile
int vHP4; // Hotspot Profile
int vHP5; // Hotspot Profile
int vHP6; // Hotspot Profile
int vHP7; // Hotspot Profile
int vSales;  // Sales Counter
int vLockSetup; // Admin Setup Protection 1=Lock 0=Unlocked
String vTelnetUsr;  //Telnet Account
String vTelnetPwd; //Telnet Password


///*	boolean AutoTurnOff;
///*	boolean AutoTurnOn;
///*	byte TurnOffHour;
///*	byte TurnOffMinute;
///*	byte TurnOnHour;
///*	byte TurnOnMinute;
///*	byte LED_R;
///*	byte LED_G;
///*	byte LED_B;
}   config;


/*
**
** CONFIGURATION HANDLING
**
*/
void ConfigureWifi()
{
  Serial.println(F("Configuring Wifi"));
  WiFi.disconnect();
  Serial.println(F("Connecting as wifi client..."));
	WiFi.begin (config.ssid.c_str(), config.password.c_str());
  int connRes = WiFi.waitForConnectResult();
  Serial.print(F("connRes: "));
  Serial.println(connRes);
	if (!config.dhcp)
	{
		WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
  }
}

void WriteConfig()
{

	Serial.println("Writing Config");
	EEPROM.write(0,'C');
	EEPROM.write(1,'F');
	EEPROM.write(2,'G');

	EEPROM.write(16,config.dhcp);
	EEPROM.write(17,config.daylight);
	
	EEPROMWritelong(18,config.Update_Time_Via_NTP_Every); // 4 Byte

	EEPROMWritelong(22,config.timezone);  // 4 Byte


///*	EEPROM.write(26,config.LED_R);
///*	EEPROM.write(27,config.LED_G);
///*	EEPROM.write(28,config.LED_B);

	EEPROM.write(32,config.IP[0]);
	EEPROM.write(33,config.IP[1]);
	EEPROM.write(34,config.IP[2]);
	EEPROM.write(35,config.IP[3]);

	EEPROM.write(36,config.Netmask[0]);
	EEPROM.write(37,config.Netmask[1]);
	EEPROM.write(38,config.Netmask[2]);
	EEPROM.write(39,config.Netmask[3]);

	EEPROM.write(40,config.Gateway[0]);
	EEPROM.write(41,config.Gateway[1]);
	EEPROM.write(42,config.Gateway[2]);
	EEPROM.write(43,config.Gateway[3]);
  EEPROM.write(44,config.vHP1); // Hotspot Profile
  EEPROM.write(45,config.vHP2); // Hotspot Profile
  EEPROM.write(46,config.vHP3); // Hotspot Profile
  EEPROM.write(47,config.vHP4); // Hotspot Profile
  EEPROM.write(48,config.vHP5); // Hotspot Profile
  EEPROM.write(49,config.vHP6); // Hotspot Profile
  EEPROM.write(50,config.vHP7); // Hotspot Profile


	WriteStringToEEPROM(64,config.ssid);
	WriteStringToEEPROM(96,config.password);
	WriteStringToEEPROM(128,config.ntpServerName);
  WriteStringToEEPROM(160,config.vTelnetUsr); //Telnet Account
  WriteStringToEEPROM(192,config.vTelnetPwd); //Telnet Password
  WriteStringToEEPROM(300,config.DeviceName);	
	EEPROM.commit();
  
tempadd=270;
EEPROM.put(tempadd,config.vLockSetup); // WebConfig Protection
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vLockSetup);
EEPROM.commit();
  
tempadd=310;
EEPROM.put(tempadd,config.vPH); // base rate per min
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPH);
EEPROM.commit();

tempadd=320;
EEPROM.put(tempadd,config.vPH1); // Rate 1
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPH1);
EEPROM.commit();

tempadd=330;
EEPROM.put(tempadd,config.vPK1); // Min 1
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPK1);
EEPROM.commit();

tempadd=340;
EEPROM.put(tempadd,config.vPH2); // Rate 2
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPH2);
EEPROM.commit();

tempadd=350;
EEPROM.put(tempadd,config.vPK2); // Min 2
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPK2);
EEPROM.commit();

tempadd=360;
EEPROM.put(tempadd,config.vPH3); // Rate 3
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPH3);
EEPROM.commit();

tempadd=370;
EEPROM.put(tempadd,config.vPK3); // Min 3
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPK3);
EEPROM.commit();

tempadd=380;
EEPROM.put(tempadd,config.vPH4); // Rate 4
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPH4);
EEPROM.commit();

tempadd=390;
EEPROM.put(tempadd,config.vPK4); // Min 4
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPK4);
EEPROM.commit();

tempadd=400;
EEPROM.put(tempadd,config.vPH5); // Rate 5
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPH5);
EEPROM.commit();

tempadd=410;
EEPROM.put(tempadd,config.vPK5); // Min 5
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPK5);
EEPROM.commit();

tempadd=420;
EEPROM.put(tempadd,config.vPH6); // Rate 6
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPH6);
EEPROM.commit();

tempadd=430;
EEPROM.put(tempadd,config.vPK6); // Min 6
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPK6);
EEPROM.commit();

tempadd=440;
EEPROM.put(tempadd,config.vPH7); // Rate 7
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPH7);
EEPROM.commit();

tempadd=450;
EEPROM.put(tempadd,config.vPK7); // Min 7
tempadd += sizeof(int);
EEPROM.put(tempadd,config.vPK7);
EEPROM.commit();  

varPH = EEPROM.get(310,config.vPH); // base rate per min
varPH1 = EEPROM.get(320,config.vPH1); // 5=1H
varPK1 = EEPROM.get(330,config.vPK1); // 60min
varPH2 = EEPROM.get(340,config.vPH2); // 10=3H
varPK2 = EEPROM.get(350,config.vPK2); // 60*3
varPH3 = EEPROM.get(360,config.vPH3); // 20=10H
varPK3 = EEPROM.get(370,config.vPK3); // 60*10
varPH4 = EEPROM.get(380,config.vPH4); // 30=1D
varPK4 = EEPROM.get(390,config.vPK4); // 60*24
varPH5 = EEPROM.get(400,config.vPH5); // 50=3D
varPK5 = EEPROM.get(410,config.vPK5); // 60*24*3
varPH6 = EEPROM.get(420,config.vPH6); // 100=7D
varPK6 = EEPROM.get(430,config.vPK6); // 60*24*7
varPH7 = EEPROM.get(440,config.vPH7); // 300=30D++
varPK7 = EEPROM.get(450,config.vPK7); // 60*24*30
varSales = EEPROM.get(280,config.vSales);
varLockSetup = EEPROM.get(270,config.vLockSetup);
varTelnetUsr = ReadStringFromEEPROM(160); //Telnet Account
varTelnetPwd = ReadStringFromEEPROM(192); //Telnet Password
    varHP1 = EEPROM.read(44); // Hotspot Profile
    varHP2 = EEPROM.read(45); // Hotspot Profile
    varHP3 = EEPROM.read(46); // Hotspot Profile
    varHP4 = EEPROM.read(47); // Hotspot Profile
    varHP5 = EEPROM.read(48); // Hotspot Profile
    varHP6 = EEPROM.read(49); // Hotspot Profile
    varHP7 = EEPROM.read(50); // Hotspot Profile
}

boolean ReadConfig()
{

	Serial.println("Reading Configuration");
	if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
	{
		Serial.println("Configurarion Found!");
		config.dhcp = 	EEPROM.read(16);

		config.daylight = EEPROM.read(17);

		config.Update_Time_Via_NTP_Every = EEPROMReadlong(18); // 4 Byte

		config.timezone = EEPROMReadlong(22); // 4 Byte

///*		config.LED_R = EEPROM.read(26);
///*		config.LED_G = EEPROM.read(27);
///*		config.LED_B = EEPROM.read(28);

		config.IP[0] = EEPROM.read(32);
		config.IP[1] = EEPROM.read(33);
		config.IP[2] = EEPROM.read(34);
		config.IP[3] = EEPROM.read(35);
		config.Netmask[0] = EEPROM.read(36);
		config.Netmask[1] = EEPROM.read(37);
		config.Netmask[2] = EEPROM.read(38);
		config.Netmask[3] = EEPROM.read(39);
		config.Gateway[0] = EEPROM.read(40);
		config.Gateway[1] = EEPROM.read(41);
		config.Gateway[2] = EEPROM.read(42);
		config.Gateway[3] = EEPROM.read(43);
    config.vHP1 = EEPROM.read(44); // Hotspot Profile
    config.vHP2 = EEPROM.read(45); // Hotspot Profile
    config.vHP3 = EEPROM.read(46); // Hotspot Profile
    config.vHP4 = EEPROM.read(47); // Hotspot Profile
    config.vHP5 = EEPROM.read(48); // Hotspot Profile
    config.vHP6 = EEPROM.read(49); // Hotspot Profile
    config.vHP7 = EEPROM.read(50); // Hotspot Profile    
		config.ssid = ReadStringFromEEPROM(64);
		config.password = ReadStringFromEEPROM(96);
		config.ntpServerName = ReadStringFromEEPROM(128);
    config.vTelnetUsr = ReadStringFromEEPROM(160); //Telnet Account
    config.vTelnetPwd = ReadStringFromEEPROM(192); //Telnet Password

varWifiSSID = config.ssid;	
config.DeviceName= ReadStringFromEEPROM(300);
config.vPH = EEPROM.get(310,config.vPH); // base rate per min
config.vPH1 = EEPROM.get(320,config.vPH1); // 5=1H
config.vPK1 = EEPROM.get(330,config.vPK1); // 60min
config.vPH2 = EEPROM.get(340,config.vPH2); // 10=3H
config.vPK2 = EEPROM.get(350,config.vPK2); // 60*3
config.vPH3 = EEPROM.get(360,config.vPH3); // 20=10H
config.vPK3 = EEPROM.get(370,config.vPK3); // 60*10
config.vPH4 = EEPROM.get(380,config.vPH4); // 30=1D
config.vPK4 = EEPROM.get(390,config.vPK4); // 60*24
config.vPH5 = EEPROM.get(400,config.vPH5); // 50=3D
config.vPK5 = EEPROM.get(410,config.vPK5); // 60*24*3
config.vPH6 = EEPROM.get(420,config.vPH6); // 100=7D
config.vPK6 = EEPROM.get(430,config.vPK6); // 60*24*7
config.vPH7 = EEPROM.get(440,config.vPH7); // 300=30D++
config.vPK7 = EEPROM.get(450,config.vPK7); // 60*24*30
config.vSales = EEPROM.get(280,config.vSales);  
varPH = EEPROM.get(310,config.vPH); // base rate per min
varPH1 = EEPROM.get(320,config.vPH1); // 5=1H
varPK1 = EEPROM.get(330,config.vPK1); // 60min
varPH2 = EEPROM.get(340,config.vPH2); // 10=3H
varPK2 = EEPROM.get(350,config.vPK2); // 60*3
varPH3 = EEPROM.get(360,config.vPH3); // 20=10H
varPK3 = EEPROM.get(370,config.vPK3); // 60*10
varPH4 = EEPROM.get(380,config.vPH4); // 30=1D
varPK4 = EEPROM.get(390,config.vPK4); // 60*24
varPH5 = EEPROM.get(400,config.vPH5); // 50=3D
varPK5 = EEPROM.get(410,config.vPK5); // 60*24*3
varPH6 = EEPROM.get(420,config.vPH6); // 100=7D
varPK6 = EEPROM.get(430,config.vPK6); // 60*24*7
varPH7 = EEPROM.get(440,config.vPH7); // 300=30D++
varPK7 = EEPROM.get(450,config.vPK7); // 60*24*30 
varSales = EEPROM.get(280,config.vSales);
varLockSetup = EEPROM.get(270,config.vLockSetup);
varTelnetUsr = ReadStringFromEEPROM(160); //Telnet Account
varTelnetPwd = ReadStringFromEEPROM(192); //Telnet Password
    varHP1 = EEPROM.read(44); // Hotspot Profile
    varHP2 = EEPROM.read(45); // Hotspot Profile
    varHP3 = EEPROM.read(46); // Hotspot Profile
    varHP4 = EEPROM.read(47); // Hotspot Profile
    varHP5 = EEPROM.read(48); // Hotspot Profile
    varHP6 = EEPROM.read(49); // Hotspot Profile
    varHP7 = EEPROM.read(50); // Hotspot Profile

BootSetup = 1;
return true;
		
	}
	else
	{
		Serial.println("Configurarion NOT FOUND!!!!");
    BootSetup = 0;
		return false;
	}
}

/*
**  NTP Section 
*/

const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; 
void NTPRefresh()
{

	


	if (WiFi.status() == WL_CONNECTED)
	{
		IPAddress timeServerIP; 
		WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP); 
		//sendNTPpacket(timeServerIP); // send an NTP packet to a time server


		Serial.println("sending NTP packet...");
		memset(packetBuffer, 0, NTP_PACKET_SIZE);
		packetBuffer[0] = 0b11100011;   // LI, Version, Mode
		packetBuffer[1] = 0;     // Stratum, or type of clock
		packetBuffer[2] = 6;     // Polling Interval
		packetBuffer[3] = 0xEC;  // Peer Clock Precision
		packetBuffer[12]  = 49;
		packetBuffer[13]  = 0x4E;
		packetBuffer[14]  = 49;
		packetBuffer[15]  = 52;
		UDPNTPClient.beginPacket(timeServerIP, 123); 
		UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
		UDPNTPClient.endPacket();


		delay(1000);
  
		int cb = UDPNTPClient.parsePacket();
		if (!cb) {
			Serial.println("NTP no packet yet");
		}
		else 
		{
			Serial.print("NTP packet received, length=");
			Serial.println(cb);
			UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
			unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
			unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
			unsigned long secsSince1900 = highWord << 16 | lowWord;
			const unsigned long seventyYears = 2208988800UL;
			unsigned long epoch = secsSince1900 - seventyYears;
			UnixTimestamp = epoch;
		}
	}
}

void Second_Tick()
{
	strDateTime tempDateTime;
  StartUpTimeOut--;
  NetCheckTimer++;  
	AdminTimeOutCounter++;
	cNTP_Update++;
	UnixTimestamp++;
	ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) , &tempDateTime);
	if (config.daylight) // Sommerzeit beachten
		if (summertime(tempDateTime.year,tempDateTime.month,tempDateTime.day,tempDateTime.hour,0))
		{
			ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) + 3600, &DateTime);
		}
		else
		{
			DateTime = tempDateTime;
		}
	else
	{
			DateTime = tempDateTime;
	}
	Refresh = true;
}

void TickTimeOut() {
  varTimeSecs--;
  if (varTimeSecs == 0) {
     tkTimeOut.detach();  
  }
}


void ReadVendoRate() {
Serial.print("BaseRate Per Min: ");
Serial.println(varPH);
Serial.print("PromoRate 1: ");
Serial.println(varPH1);
Serial.println(varPK1);
Serial.print("PromoRate 2: ");
Serial.println(varPH2);
Serial.println(varPK2);
Serial.print("PromoRate 3: ");
Serial.println(varPH3);
Serial.println(varPK3);
Serial.print("PromoRate 4: ");
Serial.println(varPH4);
Serial.println(varPK4);
Serial.print("PromoRate 5: ");
Serial.println(varPH5);
Serial.println(varPK5);
Serial.print("PromoRate 6: ");
Serial.println(varPH6);
Serial.println(varPK6);
Serial.print("PromoRate 7: ");
Serial.println(varPH7);
Serial.println(varPK7); 
}

void espSysReboot () {
     Serial.println("ESP System Restarted..! :("); 
     ESP.restart();
}

/////////////////////////////////////

#endif
