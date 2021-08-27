// freedom2000's ESP32 disco formation flying project (july 2021)
// **************************************************************

// - based on Parachute mod (for the evtest command)
// V0.00 : 210720 : first alpha release
// V0.01 : 210826 : first alpha public release
//***************************************************************

#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_now.h>
#include <esp_wifi.h>

#define WROOM32 //uncomment this line for WROOM32 board

//#define DISCO_IS_MASTER 	//uncomment if this Disco is the master (followMe)
							//comment if this Disco is a slave (will follow the Master)

//#define TEST_NOSLAVE  //test mode without slave disco (router  + MASTER + ESPNOW) (comment also DISCO_IS_MASTER)

//#define DEBUG_SPYSC2
//#define DEBUG_PUD2ESP
//#define DEBUG_ESPNOW

#define WATCHDOG_VALUE 6000000  //watchdog protection on SC2 sticks reception expressed in ms

#ifdef DISCO_IS_MASTER
const char* ssid     = "DISCO-JP";
const char* password = "";
#else
#ifdef TEST_NOSLAVE
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
#else
const char* ssid     = "DISCO-JP2";
const char* password = "";
#endif
#endif

IPAddress local_IP(192, 168, 42, 25);
IPAddress gateway(192, 168, 42, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

const uint16_t port = 23;            //telnet port
const char * host = "192.168.42.1"; // ip of chuck
// Use WiFiClient class to create TCP connections
WiFiClient client;
WiFiClient client2;

unsigned int localPort = 8888;      // local port to listen on for spySC2
#define UDP_TX_PACKET_MAX_SIZE 256
uint8_t packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
int packetSize;
WiFiUDP Udp;

unsigned int localPort2 = 9999;      // local port to listen on for pud2ESP files
uint8_t packetBuffer2[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
int packetSize2;
WiFiUDP Udp2;

long Xtime;
int Xcount = 0;
long watchdog ;

int WiFiChannel;

int connectionStatus = 0;
int maxloops = 0;
int keyCode;
int keyValue;
String line2 = "";

uint16_t channels[6];
int sticks[5];
int trims[5];
boolean trimming = false;
enum sticksName {ROLL, PITCH, THROTTLE, LOITER, WHEEL };
int channelIndex = 0;
int buttons[8];
boolean RCmode = false;
boolean prevRCmode = false;
enum buttonsName {A, B, SET, HOME, PINKIE_RIGHT, PINKIE_LEFT, WHEELR_RIGHT, WHEELR_LEFT};


//servos
#define TIMER_WIDTH 16
#include "esp32-hal-ledc.h"



#ifdef WROOM32
#define  LED_PIN 2
#else
// define PINS for servo 1..8 for ESP lolin32 lite
#define  LED_PIN 22
#endif

//SBUS
#include "SBUS.h"
#ifdef WROOM32
#define TX_PIN 13
#else
#define TX_PIN 23
#endif
#define RX_PIN 22 //useless

// a SBUS object, which is on hardware
// serial port 1
SBUS x8r(Serial1);
long pulse;

#include <Preferences.h> //ESP32 to store preferences
Preferences preferences;

// Structure to keep the data from a Disco master
typedef struct telemetry {
  int time;
  double longitude ;
  double latitude;
  float roll;
  float pitch;
  float yaw;
  float Vx;
  float Vy;
  float VZ;
  int altitude;
  int stickRoll;
  int stickPitch;
  int stickThrottle;
};
// Create a struct_message called myData
telemetry tmDisco;
telemetry tempDisco;
telemetry tmDiscoMaster;

// Set the SLAVE MAC Address
//uint8_t slaveAddress[] = {0x24, 0x0A, 0xC4, 0x5A, 0x6E, 0x58};  //send to single slave
uint8_t slaveAddress[] = {255, 255, 255, 255, 255, 255};  //broadcast to all slaves

#ifndef DISCO_IS_MASTER
// ESP-Now callback function executed when data is received
void OnRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&tmDiscoMaster, incomingData, sizeof(tmDiscoMaster));            //get telemetry from Master
#ifdef DEBUG_ESPNOW
  Serial.print("Master \t");
  Serial.print(tmDiscoMaster.time);
  Serial.print("\t");
  Serial.print(tmDiscoMaster.longitude);
  Serial.print("\t");
  Serial.print(tmDiscoMaster.latitude);
  Serial.print("\t");
  Serial.print(tmDiscoMaster.roll);
  Serial.print("\t");
  Serial.print(tmDiscoMaster.pitch);
  Serial.print("\t");
  Serial.print(tmDiscoMaster.yaw);
  Serial.print("\t");
  Serial.print(tmDiscoMaster.altitude);
  Serial.print("\t");
  Serial.print(tmDiscoMaster.stickRoll);
  Serial.print("\t");
  Serial.print(tmDiscoMaster.stickPitch);
  Serial.print("\t");
  Serial.println(tmDiscoMaster.stickThrottle);
#endif
}
#endif

#ifdef DISCO_IS_MASTER
// Callback to have a track of sent messages
void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) Serial.println("ESP-Now Failed");
  //else Serial.println("ESP-Now send ok");
}
#endif

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i = 0; i < n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}


void setup()
{
  Serial.begin(115200);

  //  //test double
  //   union f_tag
  //  {
  //    byte b[8];
  //    double dval;
  //  } f;
  //  double test = 1.49707816666667;
  //  f.dval = test;
  //  Serial.println("double test ");
  //  for (int i = 0; i < 8; i++)
  //  {
  //    Serial.print(f.b[i], HEX);
  //    Serial.print("\t");
  //  }
  //  Serial.println("\t");
  //  delay(300);

  // Open Preferences with my-app namespace. Each application module, library, etc
  // has to use a namespace name to prevent key name collisions. We will open storage in
  // RW-mode (second parameter has to be false).
  // Note: Namespace name is limited to 15 chars.
  preferences.begin(ssid, false);

  //unsigned int counter = preferences.getUInt("counter", 0); // Get the counter value, if the key does not exist, return a default value of 0 / Note: Key name is limited to 15 chars.
  trims[0] =  preferences.getInt("trimRoll", 0);
  trims[1] =  preferences.getInt("trimPitch", 0);
  trims[2] =  preferences.getInt("trimThrottle", 0);

  WiFi.mode(WIFI_AP_STA);
#ifndef TEST_NOSLAVE
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
#endif

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to ");
  Serial.println(ssid);
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP Mac Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());

  WiFiChannel = getWiFiChannel(ssid);
  Serial.print("channel : ");
  Serial.println(WiFiChannel);
  //#ifndef DISCO_IS_MASTER
  //  esp_wifi_set_channel(WiFiChannel, WIFI_SECOND_CHAN_NONE); //not need for the master which is already connected to Wifi STA
  //#endif
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("There was an error initializing ESP-NOW");
    return;
  }

#ifdef DISCO_IS_MASTER
  // We will register the callback function to respond to the event
  esp_now_register_send_cb(OnSent);

  // Register the slave
  esp_now_peer_info_t slaveInfo;
  memcpy(slaveInfo.peer_addr, slaveAddress, 6);
  slaveInfo.channel = WiFiChannel;
  slaveInfo.encrypt = false;

  // Add slave
  if (esp_now_add_peer(&slaveInfo) != ESP_OK)
  {
    Serial.println("There was an error registering the slave");
    return;
  }
#else    //it's a slave
  // Once the ESP-Now protocol is initialized, we will register the callback function
  // to be able to react when a package arrives in near to real time without pooling every loop.
  esp_now_register_recv_cb(OnRecv);
#endif

  pinMode(LED_PIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  for (int i = 0; i < 6; i++) channels[i] = 992;  //init the channels for SBUS between 172 and 1811 (992 middle)


  //led blink
  ledcSetup(1, 1, TIMER_WIDTH);   // channel 1, 1 Hz, 16-bit width
  ledcAttachPin(LED_PIN, 1);      // LED assigned to channel 1
  ledcWrite(1,  65535); // set blue LED ON while not connected to chuck

  //SBUS
  // begin the SBUS communication
  //x8r.begin();
  x8r.begin(RX_PIN, TX_PIN, true); // optional parameters for ESP32: RX pin, TX pin, inverse mode


  //UDP (spySC2)
  if (Udp.begin(localPort)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
  }

  //UDP2 (pud2ESP)
  if (Udp2.begin(localPort2)) {
    Serial.print("UDP2 Listening on IP: ");
    Serial.println(WiFi.localIP());
  }

#ifndef TEST_NOSLAVE
  connectionStatus = 0;
#else
  connectionStatus = 6; //skip all connection process
#endif
}

void loop()
{
  switch (connectionStatus)
  {
    case 0:           //not connected
      Serial.print("Connecting to ");
      Serial.println(host);
      if (!client.connect(host, port))    //launch first telnet session (for spysc2.sh)
      {
        Serial.println("Connection failed.");
        return;
      }
      if (!client2.connect(host, port)) //launch second telnet session (for spysc2.sh)
      {
        Serial.println("Connection2 failed.");
        return;
      }
      connectionStatus++;
      break;

    case 1:           // connected to telnet
      //wait for the server's reply to become available
      while (!client.available() && maxloops < 1000) //only check telnet1 session !
      {
        maxloops++;
        delay(1); //delay 1 msec
      }
      maxloops = 0;
      while ((client.available() > 0) && maxloops < 1000 )
      {
        //read back one line from the server
        String line = client.readStringUntil('\r');
        Serial.println(line);
        maxloops++;
      }
      if (maxloops < 1000)
      {
        client.print("./logsc2.sh\r");
        client2.print("./pud2ESP.sh\r"); //connect to Chuck to get pud files :https://github.com/AutonomyLab/bebop_hacking/blob/master/README.md
        //client.print("help\r"); //for help
        maxloops = 0;
        connectionStatus++;
        Serial.print("./pud2ESP.sh\r : connection Status");
        Serial.println(connectionStatus);
      }
      else
      {
        Serial.println("client.available() timed out ");
        Serial.println("Closing connection.");
        client.stop();
        client2.stop();
        connectionStatus = 0;
      }
      break;

    case 2:           // pud2ESP.sh and logsc2.sh launched
      //wait for the server's reply to become available
      while (!client.available() && maxloops < 1000)
      {
        maxloops++;
        delay(1); //delay 1 msec
      }
      maxloops = 0;
      while ((client.available() > 0) && maxloops < 1000 )
      {
        //read back one line from the server
        String line = client.readStringUntil('\r');
        Serial.println(line);
        maxloops++;
      }
      if (maxloops < 1000)
      {
        client.print("./logsc2.sh\r"); //connect to SkyController2
        maxloops = 0;
        connectionStatus++;
        Serial.print("./logsc2.sh\r : connection Status");
        Serial.println(connectionStatus);
      }
      else
      {
        Serial.println("client.available() timed out ");
        Serial.println("Closing connection.");
        client.stop();
        client2.stop();
        connectionStatus = 0;
      }
      break;

    case 3:           // connected to SC2
      maxloops = 0;
      while (!client.available() && maxloops < 1000)
      {
        maxloops++;
        delay(1); //delay 1 msec
      }
      maxloops = 0;
      while ((client.available() > 0) && maxloops < 1000 )
      {
        String line = client.readStringUntil('\r');
        Serial.println(line);
        maxloops++;
      }
      if (maxloops < 1000)
      {
        client.print("./data/lib/ftp/uavpal/bin/spysc2.sh\r"); //launch event logger (but does not work every time...)
        maxloops = 0;
        connectionStatus++;
        Serial.print("spysc2.sh : connection Status");
        Serial.println(connectionStatus);
      }
      else
      {
        Serial.println("connect sc2 timed out ");
        Serial.println("Closing connection.");
        client.stop();
        client2.stop();
        connectionStatus = 0;
      }
      break;

    case 4:           // launch the event logger
      while ((client.available() > 0) && maxloops < 10000 )
      {
        String line = client.readStringUntil('\r');
        Serial.println(line);
        maxloops++;
      }
      client.print("./data/lib/ftp/uavpal/bin/spysc2.sh\r"); //relaunch event logger
      connectionStatus++;
      Serial.println(connectionStatus);
      watchdog = millis();
      break;

    case 5:           // running
      //while ( packetSize = Udp.parsePacket()) // while there's data available, read a packet from spySC2.sh
      packetSize = Udp.parsePacket();
      if (packetSize > 0)
      {
        Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        //                Serial.print("Udp: ");
        //                Serial.println((char *)packetBuffer);

        //read back one line from the server
        line2 = (char *)packetBuffer;
        //        Serial.print("Udp read line : ");
        //        Serial.println(line2);

        int index = line2.indexOf("g") ;  // search for "gxxx yyyLF" where xxx is the keycode yyy is the keyvalue
        if (index > -1)
        {
          //          Serial.print("code index : ");
          //                Serial.println(index);
          line2 = line2.substring(index + 1);

          index = line2.indexOf(" ");
          keyCode = line2.substring(0, index).toInt();

          index ++;

          line2 = line2.substring(index);
          index = line2.indexOf(char(10));       // search for " yyyLF" where yyy is the keyvalue
          if (index > -1)

          {
            keyValue = line2.substring(0, index).toInt();
#ifdef DEBUG_SPYSC2
            Serial.print("code value : ");
            Serial.print(keyCode);
            Serial.print("\t");
            Serial.println(keyValue);
#endif



            watchdog = millis();
            //now affect values to sticks and buttons
            switch (keyCode)
            {
              case 0:           //LX = LOITER (left = -, right = +)
                sticks[LOITER] = keyValue;
                break;
              case 1:           //LY = PITCH (bottom = -, up = +)
                sticks[PITCH] = -keyValue;
                //              Xcount++;
                //              if (Xcount > 100)
                //              {
                //                Serial.print("100X time (ms) ");
                //                Serial.println(millis() - Xtime);
                //                Xtime = millis();
                //                Xcount = 0;
                //              }
                break;
              case 2:           //RX = ROLL (left = -, right = +)
                sticks[ROLL] = keyValue;
                break;
              case 3:          //RY = THROTTLE (bottom = -, up = +)
                sticks[THROTTLE] = keyValue;
                break;
              case 4:          //WHEEL (left = -, right = +)
                sticks[WHEEL] = keyValue;
                break;
              case 288:          //SET
                buttons[SET] = keyValue;
                break;
              case 289:          //HOME
                buttons[HOME] = keyValue;
                break;
              case 291:          //B
                buttons[B] = keyValue;
                break;
              case 292:          //A
                buttons[A] = keyValue;
                break;
              case 293:          //PINKIE_RIGHT
                buttons[PINKIE_RIGHT] = keyValue;
                break;
              case 294:          //PINKIE_LEFT
                buttons[PINKIE_LEFT] = keyValue;
                break;
              case 298:          //WHEELR_LEFT
                buttons[WHEELR_LEFT] = keyValue;
                break;
              case 299:          //WHEELR_RIGHT
                buttons[WHEELR_RIGHT] = keyValue;
                break;
              default:
                break;
            }
          }
        }
      }
      //while ( packetSize = Udp2.parsePacket()) // while there's data available, read a packet from pud2ESP.sh
      packetSize2 = Udp2.parsePacket();
      if (packetSize2 > 0)
      {
        Udp2.read(packetBuffer2, UDP_TX_PACKET_MAX_SIZE);
        //                        Serial.print("Udp2: ");
        //                        Serial.println((char *)packetBuffer2);

        int8_t wifiLevel;
        wifiLevel = packetBuffer2[26];
        //                Serial.print("wifi : ");
        //                Serial.println(wifiLevel);


        //decom of the telemetry from the binary packet of the .pud file (flight recording)
        tempDisco.time = decomInt(0);
        tempDisco.longitude = decomDouble(29);
        tempDisco.latitude = decomDouble(37);
        tempDisco.roll = decomFloat(66) * 180. / 3.14159;
        tempDisco.pitch = decomFloat(70) * 180. / 3.14159;
        tempDisco.yaw = decomFloat(74) * 180. / 3.14159;
        tempDisco.altitude = decomInt(78);

        //tm quality checks
        if (abs(tempDisco.time - tmDisco.time) > 3600000)
        {
          //skip the frame
        }
        else if (tempDisco.time == tmDisco.time)
        {
          //skip the frame
        }
        else if (abs(tempDisco.longitude) > 180)
        {
          //skip the frame
        }
        else if (abs(tempDisco.latitude) > 90)
        {
          //skip the frame
        }
        else if ( (abs(tempDisco.roll) > 90) || (abs(tempDisco.pitch) > 90))
        {
          //skip the frame
        }
        else  //it's a good frame
        {
          tmDisco.time =  tempDisco.time ;
          tmDisco.longitude = tempDisco.longitude ;
          tmDisco.latitude = tempDisco.latitude ;
          tmDisco.roll = tempDisco.roll;
          tmDisco.pitch = tempDisco.pitch ;
          tmDisco.yaw = tempDisco.yaw ;
          tmDisco.altitude = tempDisco.altitude;
          //now add the sticks
          tmDisco.stickRoll = sticks[ROLL];
          tmDisco.stickPitch = sticks[PITCH];
          tmDisco.stickThrottle = sticks[THROTTLE];

#ifdef DEBUG_PUD2ESP
          Serial.print(tmDisco.time);
          Serial.print("\t");
          Serial.print(tmDisco.longitude);
          Serial.print("\t");
          Serial.print(tmDisco.latitude);
          Serial.print("\t");
          Serial.print(tmDisco.roll);
          Serial.print("\t");
          Serial.print(tmDisco.pitch);
          Serial.print("\t");
          Serial.print(tmDisco.yaw);
          Serial.print("\t");
          Serial.print(tmDisco.altitude);
          Serial.print("\t");
          Serial.print(tmDisco.stickRoll);
          Serial.print("\t");
          Serial.print(tmDisco.stickPitch);
          Serial.print("\t");
          Serial.println(tmDisco.stickThrottle);
#endif
#ifdef DISCO_IS_MASTER
          // Is time to send the messsage via ESP-NOW to all the slaves
          esp_now_send(slaveAddress, (uint8_t *) &tmDisco, sizeof(tmDisco));
#endif
        }
      }
      break;
    default:
      break;
  }

  //detect RCmode
  if (buttons[PINKIE_RIGHT] == 1)      //RCmode toggle on with both pinkie and A
  {
    if (buttons[A] == 1) RCmode = true;
    else
    {
      if (buttons[PINKIE_LEFT] == 0)  //don't quit RCmode while trimming !
      {
        RCmode = false;          //RCmode toggle off with pinkie only
        watchdog = millis();
      }
    }
    if (prevRCmode != RCmode)
    {
      Serial.print("RCmode ");
      Serial.println(RCmode);
      prevRCmode = RCmode;

    }

    if (buttons[PINKIE_LEFT] == 1)
    {
      trims[0] = sticks[0];
      trims[1] = sticks[1];
      trims[2] = sticks[2];
      trimming = true;
    }
    else
    {
      if (trimming)
      {
        trimming = false;
        preferences.putInt("trimRoll", trims[0]);
        preferences.putInt("trimPitch", trims[1]);
        preferences.putInt("trimThrottle", trims[2]);
      }
    }
  }

  // fail safe if connection lost
  if (RCmode && (((millis() - watchdog) > WATCHDOG_VALUE) || (abs(sticks[LOITER]) > 95)))
  {
    RCmode = false;
    //    client.stop();
    //    connectionStatus = 0;
    Serial.print("RC timeout");
    watchdog = millis();
  }

  //channels mixing




#ifdef DISCO_IS_MASTER
  for (int i = 0; i < 4; i++) channels[i] = 1024 + 10 * sticks[i] ; //SBUS between 0 and 2047 (1024 middle)
  //if (trimming) for (int i = 0; i < 4; i++) channels[i] = 1024 + 10 * (sticks[i]) ; //don't apply trims values
  // else for (int i = 0; i < 4; i++) channels[i] = 1024 + 10 * (sticks[i] + trims[i]); //SBUS between 0 and 2048 (1024 middle)
#else
  int tmpSticks[4];
  tmpSticks[ROLL] = sticks[ROLL] + tmDiscoMaster.stickRoll;
  tmpSticks[PITCH] = sticks[PITCH] + tmDiscoMaster.stickPitch;
  tmpSticks[THROTTLE] = sticks[THROTTLE] + tmDiscoMaster.stickThrottle;
  for (int i = 0; i < 4; i++)
  {
    tmpSticks[i] = min (tmpSticks[i], 100);
    tmpSticks[i] = max (tmpSticks[i], -100);
    channels[i] = 1024 + 10 * tmpSticks[i] ; //SBUS between 0 and 2047 (1024 middle)
  }
#endif

  //  if (RCmode) channels[4] = 1811;  //force channel 5 to max PWM value to fly with RC
  //  else channels[4] = 172;         //force channel 5 to min PWM value to fly with SkyController2
  if (RCmode) channels[4] = 2047;  //force channel 5 to max PWM value to fly with RC
  else channels[4] = 0;         //force channel 5 to min PWM value to fly with SkyController2

  //servos pulse generation

  if ((millis() - pulse) > 10) //every 10ms
  {
    pulse = millis();
    x8r.write(&channels[0]); // write the SBUS packet to an SBUS compatible servo
  }



  if ((RCmode) && (connectionStatus > 4)) // when connected to CHUCK blue LED will turn off, then flash in RCmode
  {
    ledcWrite(1,  65535 / 2); //just to flash the blue led
  }
  else
  {
    ledcWrite(1,  0); //just to flash the blue led
  }
}

float decomFloat(int index)
{
  union f_tag {
    byte b[4];
    float fval;
  } f;
  for (int i = 0; i < 4; i++)
  {
    f.b[i] = packetBuffer2[index + i];
  }
  return f.fval;
}
int decomInt(int index)
{
  union f_tag {
    byte b[4];
    int ival;
  } f;

  for (int i = 0; i < 4; i++)
  {
    f.b[i] = packetBuffer2[index + i];
  }
  return f.ival;
}
double decomDouble(int index)
{
  union f_tag
  {
    byte b[8];
    double dval;
  } f;
  for (int i = 0; i < 8; i++)
  {
    f.b[i] = packetBuffer2[index + i];
  }
  return f.dval;
}
