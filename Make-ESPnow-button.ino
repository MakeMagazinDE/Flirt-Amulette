// SR 2024

/* Etwas komplexeres Beispielprogramm, das die Verarbeitung von messages mit ESPnow zeigt. 
 * Der Blinkrhythmus der on-board LED verändert sich, wenn auf anderen boards Taster gedrückt werden.  */

/* Define your LED and switch configuration:
 * ESP32-C3 Super Mini has a built-in LED on GPIO8  connected to 3V3 and boot switch on GPIO9
 * ESP32-S2 Mini       has a built-in LED on GPIO15 connected to GND and boot switch on GPIO0
 * ESP-12x (e.g. D1)   has a built-in LED on GPIO2  connected to 3V3 and no built-in switch, use GPIO0
 * ESP-M3              has a built-in LED on GPIO16 connected to 3V3 and no built-in switch, use GPIO0
 */

#define STATUS_LED 15 //8
#define LEDON HIGH     // HIGH if LED is connected to GND, LOW if connected to 3V3
#define SWITCH 0// 9

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
unsigned long lastReceivedMillis=0;
#define LEDonTime 3000  // switch on LED for 3sec when button is pressed

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
}

void loop() {
  WifiEspNowBroadcast.loop();             // EspNowBroadcast worker
  WifiEspNowPeerInfo peers[MAX_PEERS];    // Declare array peers[] for peer info
  int nPeers = WifiEspNow.listPeers(peers, MAX_PEERS);  // store peer info in peers[]
  Serial.printf("Number of peers: %i ",nPeers);
  for (int i = 0; i < nPeers; ++i) {
    Serial.printf(" %02X:%02X:%02X:%02X:%02X:%02X ", peers[i].mac[0], peers[i].mac[1],
                       peers[i].mac[2], peers[i].mac[3], peers[i].mac[4], peers[i].mac[5]); }
  Serial.println();
  
  // If button pressed: Send message, strlen()+1 to include end delimiter \0
  if(digitalRead(SWITCH)==LOW) WifiEspNowBroadcast.send(reinterpret_cast<const uint8_t*>(message), strlen(message)+1);

  // if connected: 150ms instead of 50ms, if button pressed: constant on
  digitalWrite(STATUS_LED, LEDON);
  if( nPeers>0 ) delay(150); else delay(50);
   
  // if button pressed on other device: keep LED on for LEDonTime ms
  if( millis()-lastReceivedMillis > LEDonTime ) digitalWrite(STATUS_LED, LEDOFF);
  delay(950);
}

void processRx(const uint8_t mac[WIFIESPNOW_ALEN], const uint8_t* buffer, size_t count, void* arg){
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X: ", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (size_t i = 0; i < count; ++i) Serial.printf(" %i",buffer[i]); Serial.println();
  if(buffer[count-1]==0) Serial.printf("Message is a string: '%s'\n", (char*)buffer);
  if(strcmp(message, (char*)buffer) ==0 ) lastReceivedMillis = millis();  // correct message received!
}
