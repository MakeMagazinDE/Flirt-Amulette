// SR 2024

/* Komplettes Programm mit WS2812fx library, Farb- und Moduswechsel in Abhängigkeit von Peers  */

/* Define your LED and switch configuration:
 * ESP32-C3 Super Mini has a built-in LED on GPIO8  connected to 3V3 and boot switch on GPIO9
 * ESP32-S2 Mini       has a built-in LED on GPIO15 connected to GND and boot switch on GPIO0
 * ESP-12x (e.g. D1)   has a built-in LED on GPIO2  connected to 3V3 and no built-in switch, use GPIO0
 * ESP-M3              has a built-in LED on GPIO16 connected to 3V3 and no built-in switch, use GPIO0
 */

#define STATUS_LED 2 // 8
#define LEDON LOW     // HIGH if LED is connected to GND, LOW if connected to 3V3
#define SWITCH 0 // 9

#if LEDON == HIGH
 #define LEDOFF LOW
#else
 #define LEDOFF HIGH
#endif

// WiFi library depends on ESP series
#if defined(ARDUINO_ARCH_ESP8266)
 #include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
 #include <WiFi.h>
#endif

#include <WifiEspNowBroadcast.h>
#define WIFI_CHANNEL 1
#define MAX_PEERS 20
void processRx(const uint8_t mac[WIFIESPNOW_ALEN], const uint8_t* buf, size_t count, void* arg);
char message[]="Button pressed!";
#define LEDonTime 3000               // switch on LED for 3sec when button is pressed
long lastReceivedMillis=-LEDonTime;  // do not start believing we are connected

#define WS2812_PIN   4  // 10 works fine on ESP32-C3, 4 (=D2) is good on D1
#define NUM_LEDS    30  // no problem if we have less than this
const int modes[3][3]={{ 12, 50000,  10},   // inactive: rainbow (12), slow (50000), dim (10/255)
                       {  2,     1, 200},   // peer found: breathe (2), bright (speed ignored)
                       { 51,   100, 250}};  // button pressed: circus, very fast
int newmode,mymode=0;  // keep track of LED modes
int showpeer=0;        // rotate through the colours of peers
#include <WS2812FX.h>
WS2812FX ws2812fx = WS2812FX(NUM_LEDS, WS2812_PIN, NEO_GRB); 


void setup() {
 pinMode(STATUS_LED, OUTPUT);
 pinMode(SWITCH, INPUT_PULLUP);
 Serial.begin(115200); Serial.println();   // C3 needs USB CDC On boot "Enabled"!
 Serial.printf("Starting ESPnow demo, LED on GPIO%i\n", STATUS_LED);
 WiFi.persistent(false);
 bool wifiok = WifiEspNowBroadcast.begin("ESPNOW", 3);
 if (!wifiok) { Serial.println("WifiEspNowBroadcast.begin() failed"); ESP.restart(); }
 WifiEspNowBroadcast.onReceive(processRx, nullptr);  // register the callback
 Serial.print("MAC address of this node is ");
 Serial.println(WiFi.softAPmacAddress());

 ws2812fx.init();
 ws2812fx.setMode(modes[0][0]);
 ws2812fx.setSpeed(modes[0][1]);
 ws2812fx.setBrightness(modes[0][2]);
 ws2812fx.setColor(0xFF0000);
 ws2812fx.start(); 
}

void loop() {
  ws2812fx.service(); 
  WifiEspNowBroadcast.loop();             // EspNowBroadcast worker
  WifiEspNowPeerInfo peers[MAX_PEERS];    // Declare array peers[] for peer info
  int nPeers = WifiEspNow.listPeers(peers, MAX_PEERS);  // store peer info in peers[]
  Serial.printf("Number of peers: %i ",nPeers);
  for (int i = 0; i < nPeers; ++i) {
    Serial.printf(" %02X:%02X:%02X:%02X:%02X:%02X ", peers[i].mac[0], peers[i].mac[1],
                       peers[i].mac[2], peers[i].mac[3], peers[i].mac[4], peers[i].mac[5]); }
  Serial.println();
  
  // If button is pressed: Send message, strlen()+1 to include end delimiter \0
  if(digitalRead(SWITCH)==LOW) WifiEspNowBroadcast.send(reinterpret_cast<const uint8_t*>(message), strlen(message)+1);

  digitalWrite(STATUS_LED, LEDON);
  if( nPeers>0 ) {
    showpeer++; showpeer%=nPeers;  // cycle through the peers
    // unique colour for each peer: get RGB value from lower value bytes of MAC address
    ws2812fx.setColor(peers[showpeer].mac[3]<<16|peers[showpeer].mac[4]<<8|peers[showpeer].mac[5]);
    newmode = 1;
    mydelay(150); }
  else {
    newmode = 0;
    mydelay(50); }
   
  // if button pressed on other device: make a circus!
  if( millis()-lastReceivedMillis < LEDonTime )
    newmode = 2;
  else
    digitalWrite(STATUS_LED, LEDOFF);  // button not pressed: status LED off!

  if (newmode != mymode) {
    ws2812fx.setMode(modes[newmode][0]);
    ws2812fx.setSpeed(modes[newmode][1]);
    ws2812fx.setBrightness(modes[newmode][2]);
    mymode=newmode; }
    
  mydelay(950);  // wait ~1s but keep updating the LED(s)
}

void processRx(const uint8_t mac[WIFIESPNOW_ALEN], const uint8_t* buffer, size_t count, void* arg){
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X: ", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (size_t i = 0; i < count; ++i) Serial.printf(" %i",buffer[i]); Serial.println();
  if(buffer[count-1]==0) Serial.printf("Message is a string: '%s'\n", (char*)buffer);
  if(strcmp(message, (char*)buffer) ==0 ) lastReceivedMillis = millis();  // correct message received!
}

void mydelay(unsigned int waitms){  // keep on updating the leds while waiting
  unsigned long nowms=millis();
  while (millis()<nowms+waitms){ yield(); ws2812fx.service(); 
  delay(10);}
}
