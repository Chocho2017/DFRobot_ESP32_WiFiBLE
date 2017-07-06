/*!
 * @file DFRobot_queue.cpp
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
#ifndef DFROBOT_QUEUE_H_
#define DFROBOT_QUEUE_H_
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
struct blemsgBuf{
        struct blemsgBuf *next;
        uint16_t handle;
        uint16_t len;
		uint16_t conn_id;
        uint8_t data[0];
};
struct wifimsgBuf{
        struct wifimsgBuf *next;
        uint16_t handle;
        uint16_t len;
		uint16_t conn_id;
        uint8_t data[0];
};
struct ble_name_mac{
		struct ble_name_mac *next;
		uint16_t conn_id;
		uint8_t mac[6];
		uint8_t name[0];
};
extern int mys;
void send_wifi_data(int s);
void recevice_data(uint8_t * buf, uint16_t len);
void ble_name_idEequeue(uint8_t *new_mac,uint16_t new_connid);
char *getname_Dequeue(uint16_t conn_id);
uint16_t getid_Dequeue(const char *name);
extern void cuappEnqueue(uint8_t *pbuf,uint16_t len,uint16_t conn_id);
extern struct blemsgBuf *cuappDequeue( void );
extern void wificuappEnqueue(uint8_t *pbuf,uint16_t len,uint16_t conn_id);
extern struct wifimsgBuf *wificuappDequeue( void );
extern void ble_name_Enqueue(uint8_t *mac,uint16_t mac_len ,char *name, uint16_t name_len);
extern struct ble_name_mac *ble_namecuappDequeue( void );
extern bool wifi_true;
#endif
