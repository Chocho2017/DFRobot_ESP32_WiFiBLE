/*!
 * @file SimpleEsp32_BLEWiFi.ino
 * @brief DFRobot's Connect ESP32 to Bluetooth
 * @n This example is through the ESP32 to connect to Bluetooth and WIFI through TCP connection and achieve Bluetooth and TCP SERVER communication
 *
 * @copyright	[DFRobot](https://www.dfrobot.com/product-1590.html), 2017
 * @copyright	GNU Lesser General Public License
 * @author [Wuxiao](xiao.wu@dfrobot.com)
 * @version  V0.1
 * @date  2017-07-03
 * @https://github.com/DFRobot/DFRobot_ESP32_WiFiBLE
 */

#include <WiFi.h>
#include <WiFiMulti.h>
#include "DFRobot_ESP32_BLE.h"
DFRobot_ESP32_BLE ble;
char dataName[15]={'\0'};
uint16_t ServiceUuid = 0xdfb0;
uint16_t CharacteristicUuid = 0xdfb1;
void setup()
{
    Serial.begin(115200);
    delay(10);
	  ble.setService(ServiceUuid);
	  ble.setCharacteristic(CharacteristicUuid);
    ble.setconnummax(1);
	  ble.setconnectname0("Bluno0");
  	ble.init();
    delay(50);
    ble.begin();
	
    WiFi.begin("hitest", "12345678");
    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}


void loop()
{
    const uint16_t port = 80;
    const char * host = "192.168.2.17"; // ip or dns
    Serial.print("connecting to ");
    Serial.println(host);

    WiFiClient client;
    if (!client.connect(host, port)) {
        Serial.println("connection failed");
        Serial.println("wait 5 sec...");
        delay(5000);
        return;
    }
	while(1){
			String bledata = ble.readdata(dataName);
			String line = client.readStringUntil('\r');
			if(bledata.length()>0){
				if(strncmp(dataName,"Bluno0",strlen("Bluno0")) == 0){
					Serial.print("recevice from Bluno0 data:");
					Serial.println(bledata);
				}
				//ble.writedata("Bluno2",bledata);
				client.print(bledata);
			}
			if(line.length()>0){
				Serial.print("send data to Bluno2:");
				Serial.print(line);
				ble.writedata("Bluno0",line);
			}
    }
}

