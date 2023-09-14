#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>


#include <ESP8266WiFi.h>
#include <HCSR04.h>
#define THINGSBOARD_ENABLE_PROGMEM 0
#include <ThingsBoard.h>
constexpr uint32_t MAX_MESSAGE_SIZE = 128U;

WiFiClient wifiClient;
HCSR04 hc(16, new int[2] { 5, 4 }, 2);
ThingsBoard tb(wifiClient, MAX_MESSAGE_SIZE);

#define WIFI_SSID "BT-KXF6Z9"
#define WIFI_PASSWORD "q4dV4FaMHgCQYR"

//#if THINGSBOARD_ENABLE_PROGMEM
//constexpr char TOKEN[] PROGMEM = "8rOqr4ojS7aAamRztHse";
//#else
constexpr char THINGSBOARD_TOKEN[] = "q8hLYOEitMLDXcQGDaoC";
//#endif

constexpr char THINGSBOARD_SERVER[] = "demo.thingsboard.io";
constexpr uint16_t THINGSBOARD_PORT = 80U;

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup() {
	pinMode(2, OUTPUT);
	Serial.begin(115200);
	delay(10);
	InitWiFi();
	lastSend = 0;
}

int lastSlot1 = -1, lastSlot2 = -1;
void loop() {
	if (!tb.connected()) {
		reconnect();
	}

	if (millis() - lastSend > 5000) { // Update and send only after 5 seconds
		digitalWrite(2, LOW);
		float distance1 = hc.dist(0);
		delay(2000);
		float distance2 = hc.dist(1);
		Serial.print("Distance 1: ");
		Serial.println(distance1);
		Serial.print("Distance 2: ");
		Serial.println(distance2);
		int slot1 = distance1 < 60.0 ? 1 : 0;
		int slot2 = distance2 < 60.0 ? 1 : 0;
		if (distance1 > 0 && slot1 != lastSlot1){			
			tb.sendTelemetryString("SLOT1", slot1 ? "Occupied" : "Free");
			lastSlot1 = slot1;
		}
		if (distance2 > 0 && slot2 != lastSlot2){
			tb.sendTelemetryString("SLOT2", slot2 ? "Occupied" : "Free");
			lastSlot2 = slot2;
		}
		if (distance1>0 || distance2 >0)
			lastSend = millis();
		digitalWrite(2, HIGH);
  
		Serial.print("SLOT 1: ");
		Serial.println(slot1 ? "Occupied" : "Unoccupied");
		Serial.print("SLOT 2: ");
		Serial.println(slot2 ? "Occupied" : "Unoccupied");
	}

	tb.loop();
}

void reconnect() {
	// Loop until we're reconnected
	while (!tb.connected()) {
		status = WiFi.status();
		if (status != WL_CONNECTED) {
			WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
			while (WiFi.status() != WL_CONNECTED) {
				Serial.print(".");
				digitalWrite(2, LOW);
				delay(100);
				digitalWrite(2, HIGH);
				delay(100);
			}
			Serial.println("Connected to AP");
		}
		digitalWrite(2, LOW);
		delay(300);
		digitalWrite(2, HIGH);
		delay(300);
		Serial.print("Connecting to ThingsBoard...");
		if (tb.connect(THINGSBOARD_SERVER, THINGSBOARD_TOKEN)) {
			Serial.println("[DONE]");
		} else {
			Serial.print("[FAILED]");
			Serial.println(" : retrying in 5 seconds]");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

/// @brief Initalizes WiFi connection,
// will endlessly delay until a connection has been successfully established
void InitWiFi() {
	Serial.println("Connecting to AP ...");
	WiFi.mode(WIFI_STA);
	// Attempting to establish a connection to the given WiFi network
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED) {
		// Delay 500ms until a connection has been succesfully established
		Serial.print(".");
		digitalWrite(2, LOW);
		delay(100);
		digitalWrite(2, HIGH);
		delay(100);
	}
	Serial.println("Connected to AP");
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}