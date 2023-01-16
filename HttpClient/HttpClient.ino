/*
  This example connects to an unencrypted Wifi network.
  Then it prints the  MAC address of the Wifi shield,
  the IP address obtained, and other network details.

  Circuit:
  * Filogic 130 HDK

  created 13 July 2010
  by dlf (Metodo2 srl)
  modified 31 May 2012
  by Tom Igoe
  modified 23 May 2017
  by MediaTek Labs
*/

// Youtube 影片: Develop WiFi interface on Filogic130A (Arduino IDE), 
// 教學 HTTP data to cloud 數據上雲端。
// https://youtu.be/OTzjujqmcdo

#include <LWiFi.h>
#include "httpclient.h"

#define BUF_SIZE    (512)
#define URL_BUF_LEN (256)

char ssid[] = "ssid";     //  your network SSID (name)
char pass[] = "1234567890";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

httpclient_t iotClient = {0};
httpclient_data_t iotData = {0};
char iotRespBuf[BUF_SIZE], iotPostBuf[BUF_SIZE];
char get_url[URL_BUF_LEN];
int tempVal1 = 0, tempVal2 = 0;
 
void setup() {

	//Initialize serial and wait for port to open:
	Serial.begin(115200);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	// attempt to connect to Wifi network:
	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to WPA SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network:
		status = WiFi.begin(ssid, pass);
	}

	// you're connected now, so print out the data:
	Serial.print("You're connected to the network");
	printCurrentNet();
	printWifiData();

  sprintf(get_url, "https://api.thingspeak.com/update");
  printf("httpclient_connect:%d", httpclient_connect(&iotClient, get_url));
}

void loop() {
  uint8_t buf[64];

  memset( &iotData, 0, sizeof(iotData) );
  iotData.response_buf = iotRespBuf;
  iotData.response_buf_len = BUF_SIZE;

  sprintf(iotPostBuf, "api_key=9Y9MTEF4LA44UTOJ&field1=%d&field2=%d", ++tempVal1 % 100, ++tempVal2 % 50);
  iotData.post_buf = iotPostBuf;
  iotData.post_buf_len = strlen(iotPostBuf);
  
  sprintf((char*)buf, "application/x-www-form-urlencoded");
  iotData.post_content_type = (char*)buf;
  
  printf("connecting to ThingSpeak for update...\n");
  if( httpclient_post(&iotClient, get_url, &iotData) == HTTPCLIENT_OK)
  {
    printf("data_len=%d %s\n", iotData.response_content_len - iotData.retrieve_len, iotData.response_buf);
  }

  delay(60000);
}

void printWifiData() {
	// print your WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
	Serial.println(ip);

	// print your MAC address:
	byte mac[6];
	WiFi.macAddress(mac);
	Serial.print("MAC address: ");
	Serial.print(mac[5], HEX);
	Serial.print(":");
	Serial.print(mac[4], HEX);
	Serial.print(":");
	Serial.print(mac[3], HEX);
	Serial.print(":");
	Serial.print(mac[2], HEX);
	Serial.print(":");
	Serial.print(mac[1], HEX);
	Serial.print(":");
	Serial.println(mac[0], HEX);

}

void printCurrentNet() {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print the MAC address of the router you're attached to:
	byte bssid[6];
	WiFi.BSSID(bssid);
	Serial.print("BSSID: ");
	Serial.print(bssid[5], HEX);
	Serial.print(":");
	Serial.print(bssid[4], HEX);
	Serial.print(":");
	Serial.print(bssid[3], HEX);
	Serial.print(":");
	Serial.print(bssid[2], HEX);
	Serial.print(":");
	Serial.print(bssid[1], HEX);
	Serial.print(":");
	Serial.println(bssid[0], HEX);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
        Serial.print(rssi);
        Serial.println(" dBm");

	// print the encryption type:
	byte encryption = WiFi.encryptionType();
	Serial.print("Encryption Type:");
	Serial.println(encryption, HEX);
	printEncryptionType(encryption);
	Serial.println();
}
void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;
    default :
      Serial.println("WPA-PSK/WPA2-PSK");
      break;
  }
}
