/*!
   @file SimpleEsp32_BLE.ino
   @brief DFRobot's Connect ESP32 to Bluetooth
   @n This example is to achieve through the ESP32 to connect multiple Bluetooth devices and achieve communication

   @copyright	[DFRobot](http://www.dfrobot.com), 2017
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
  ble.setconnectname0("BLE-Link0");
  ble.setconnectname1("BLE-Link1");
  ble.init();
  delay(100);
  ble.begin();
  Serial.print("ble begin...");
  while(!ble.initdone()){
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  ble.scan();
  Serial.print("ble scan...");
  Serial.println();
  while(1){
    String scanName = ble.scanname();
    if(scanName.length() > 0){
      Serial.print("ScanName:");
      Serial.println(scanName);
      if(scanName == "BLE-Link0"){
        ble.stop_scan();
        delay(10);
        Serial.print("Connect BLE-Link0...");
        while(!ble.connected("BLE-Link0")){
          Serial.print(".");
          delay(100);
          }
          Serial.println("Connect success");
          break;
      }
    }else{
      Serial.print(".");
      delay(100);
    }
  }

  delay(500);
  ble.scan();
  Serial.print("ble scan2...");
  Serial.println();
    while(1){
    String scanName = ble.scanname();
    if(scanName.length() > 0){
      Serial.print("ScanName:");
      Serial.println(scanName);
      if(scanName == "BLE-Link1"){
        ble.stop_scan();
        Serial.print("Connect BLE-Link1...");
        while(!ble.connected("BLE-Link1")){
          Serial.print(".");
          delay(100);
          }
          Serial.println("Connect success");
          break;
      }
    }else{
      Serial.print(".");
      delay(100);
    }
  }

}

void loop() {
    String bledata = ble.readdata(dataName);
    

    if (bledata.length() > 0) {
      if (strncmp(dataName, "BLE-Link0", strlen("BLE-Link0")) == 0) {
        Serial.print("recevice from BLE-Link0 data:");
        Serial.println(bledata);
        ble.writedata("BLE-Link1", bledata);
        Serial.print("send data to BLE-Link1:");
        Serial.println(bledata);
      } else if (strncmp(dataName, "BLE-Link1", strlen("BLE-Link1")) == 0) {
        Serial.print("recevice from BLE-Link1 data:");
        Serial.println(bledata);
        ble.writedata("BLE-Link0", bledata);
        Serial.print("send data to BLE-Link1:");
        Serial.println(bledata);
      }
      delay(100);
    }
}
