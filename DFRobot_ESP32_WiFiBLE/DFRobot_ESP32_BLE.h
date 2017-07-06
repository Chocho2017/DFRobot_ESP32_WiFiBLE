/*!
 * @file DFRobot_ESP32_BLE.h
 * @brief DFRobot's Connect ESP32 to Bluetooth
 * @n This example is to achieve through the ESP32 to connect multiple Bluetooth devices and achieve communication
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2017
 * @copyright	GNU Lesser General Public License
 * @author [Wuxiao](xiao.wu@dfrobot.com)
 * @version  V0.1
 * @date  2017-07-03
 * @https://github.com/DFRobot/DFRobot_ESP32_WiFiBLE
 */

#ifndef _DFROBOT_ESP32_BLE_H_
#define _DFROBOT_ESP32_BLE_H_

#include "esp32-hal-log.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bt.h"

#include "Arduino.h"


class DFRobot_ESP32_BLE{
    public:

        DFRobot_ESP32_BLE(void);
        ~DFRobot_ESP32_BLE(void);
		String readdata(char *buf);
		void setService(uint16_t ser);
		void setCharacteristic(uint16_t cha);
		void setconnummax(uint8_t max);
		void setconnectname0(String str);
		void setconnectname1(String str);
		void setconnectname2(String str);
		void setconnectname3(String str);
		void writedata(const char *name,char data);
		void writedata(const char *name,char *buf);
		void writedata(const char *name,int data);
		void writedata(const char *name,String str);
		void writedata(const char *name,const char *buf);
		void writedata(const char *name,String str,unsigned int len);
		void writedata(const char *name,uint8_t *buf,uint16_t len);
		//void writedata(const char *name,String str);
        void begin(void);
		void init(void);
        void end(void);
    private:
        //String connet_name;
    private:

};

#endif
