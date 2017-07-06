/*!
   @file SimpleEsp32_BLE.ino
   @brief DFRobot's Connect ESP32 to Bluetooth
   @n This example is to achieve through the ESP32 to connect multiple Bluetooth devices and achieve communication

   @copyright	[DFRobot](https://www.dfrobot.com/product-1590.html), 2017
   @copyright	GNU Lesser General Public License
   @author [Wuxiao](xiao.wu@dfrobot.com)
   @version  V0.1
   @date  2017-07-03
   @https://github.com/DFRobot/DFRobot_ESP32_WiFiBLE
*/

#include "DFRobot_ESP32_BLE.h"
DFRobot_ESP32_BLE ble;
char dataName[15] = {'\0'};
void setup() {
  Serial.begin(115200);
  ble.setService(0xdfb0);
  ble.setCharacteristic(0xdfb1);
  ble.setconnummax(2);
  ble.setconnectname0("Bluno0");
  ble.setconnectname1("Bluno1");
  ble.init();
  delay(100);
  ble.begin();
}

void loop() {
    String bledata = ble.readdata(dataName);
    if (bledata.length() > 0) {
      if (strncmp(dataName, "Bluno0", strlen("Bluno0")) == 0) {
        Serial.print("recevice from Bluno0 data:");
        Serial.println(bledata);
        ble.writedata("Bluno1", bledata);
        Serial.print("send data to Bluno1:");
        Serial.println(bledata);
      } else if (strncmp(dataName, "Bluno1", strlen("Bluno1")) == 0) {
        Serial.print("recevice from Bluno1 data:");
        Serial.println(bledata);
        ble.writedata("Bluno0", bledata);
        Serial.print("send data to Bluno0:");
        Serial.println(bledata);
      }
      delay(100);
    }
}
