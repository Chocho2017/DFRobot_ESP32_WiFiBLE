
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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "controller.h"



#include "bt.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "btm_api.h"
#include "bta_api.h"
#include "bta_gatt_api.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "DFRobot_queue.h"
#include "esp_types.h"
#include "freertos/FreeRTOS.h"

#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_err.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_err.h"
struct blemsgBuf *cumsgBufHead=NULL;
struct blemsgBuf *cumsgBufTail=NULL;
struct wifimsgBuf *wificumsgBufHead=NULL;
struct wifimsgBuf *wificumsgBufTail=NULL;
struct ble_name_mac *ble_namecumsgBufHead=NULL;
struct ble_name_mac *ble_namecumsgBufTail=NULL;
//#define dbg
void cuappEnqueue(uint8_t *pbuf, uint16_t len, uint16_t conn_id){
        struct blemsgBuf *p;
                p = (struct blemsgBuf*)malloc(sizeof(struct blemsgBuf)+len);
                if(p == NULL){
                        free(p);
                        return;
                }
                p->next = NULL;

                if(cumsgBufHead==NULL){
                        cumsgBufHead=p;
                        cumsgBufTail=p;
                }else{
                        cumsgBufTail->next = p;
                        cumsgBufTail = p;
                }
                p->len = len;
                p->handle = 0;
				p->conn_id = conn_id;
                memcpy(p->data, pbuf, len);
}
struct blemsgBuf *cuappDequeue(void){
        struct blemsgBuf *p;
        p = cumsgBufHead;
        if(cumsgBufHead != NULL){
                cumsgBufHead = p->next;
        }
        return p;
}

void wificuappEnqueue(uint8_t *pbuf, uint16_t len, uint16_t conn_id){
        struct wifimsgBuf *p;
                p = (struct wifimsgBuf*)malloc(sizeof(struct wifimsgBuf)+len);
                if(p == NULL){
                        free(p);
                        return;
                }
                p->next = NULL;
                if(wificumsgBufHead==NULL){
                        wificumsgBufHead=p;
                        wificumsgBufTail=p;
                }else{
                        wificumsgBufTail->next = p;
                        wificumsgBufTail = p;
                }
                p->len = len;
                p->handle = 0;
				p->conn_id = conn_id;
                memcpy(p->data, pbuf, len);
}
struct wifimsgBuf *wificuappDequeue(void){
        struct wifimsgBuf *p;
        p = wificumsgBufHead;
        if(wificumsgBufHead != NULL){
                wificumsgBufHead = p->next;
        }
        return p;
}

void ble_name_Enqueue(uint8_t *mac,uint16_t mac_len ,char *name, uint16_t name_len)
{
	#ifdef dbg
	ESP_LOGE("ble_name_Enqueue", "bd_addr:%08x%04x",(mac[0] << 24) + (mac[1] << 16) + (mac[2] << 8) + mac[3],
                 (mac[4] << 8) + mac[5]);
	ESP_LOGE("ble_name_Enqueue","ble_name_Enqueue\n");
	#endif
	struct ble_name_mac *p;
	p = (struct ble_name_mac*)malloc(sizeof(struct ble_name_mac)+name_len);
	if(p == NULL){
		#ifdef dbg
		ESP_LOGE("ble_name_Enqueue","p == NULL\n");
		#endif
        free(p);
        return;
	}
	
    p->next = NULL;
    if(ble_namecumsgBufHead==NULL){
		#ifdef dbg
		ESP_LOGE("ble_name_Enqueue","ble_namecumsgBufHead==NULL\n");
		#endif
        ble_namecumsgBufHead=p;
        ble_namecumsgBufTail=p;
    }else{
		#ifdef dbg
		ESP_LOGE("ble_name_Enqueue","ble_namecumsgBufHead==NULL______else\n");
		#endif
        ble_namecumsgBufTail->next = p;
        ble_namecumsgBufTail = p;
    }
	memcpy(p->mac, mac, mac_len);
	//ESP_LOGE("ble_name_Enqueue", "memcpybd1_addr:%08x%04x",(p->mac[0] << 24) + (p->mac[1] << 16) + (p->mac[2] << 8) + p->mac[3],
    //             (p->mac[4] << 8) + p->mac[5]);	
	memcpy(p->name, name, name_len);
	#ifdef dbg
	ESP_LOGE("ble_name_Enqueue", "memcpybd2_addr:%08x%04x",(p->mac[0] << 24) + (p->mac[1] << 16) + (p->mac[2] << 8) + p->mac[3],
                 (p->mac[4] << 8) + p->mac[5]);	
	ESP_LOGE("ble_name_Enqueue", "adv_name== %s\n", p->name);	
	#endif	
}
struct ble_name_mac *ble_namecuappDequeue(void){
        struct ble_name_mac *p;
        p = ble_namecumsgBufHead;
        if(ble_namecumsgBufHead != NULL){
			#ifdef dbg
			ESP_LOGE("ble_namecuappDequeue","ble_namecumsgBufHead!NULL\n");
			#endif
                //ble_namecumsgBufHead = p->next;
        }
        return p;
}
bool eqmac(uint8_t *buf,uint8_t *newbuf)
{
	for(int i = 0;i<6;i++){
		if(buf[i] != newbuf[i]){
			return false;
		}
	}
	return true;
}
void ble_name_idEequeue(uint8_t *new_mac,uint16_t new_connid)
{
		static struct ble_name_mac *p=NULL;
        p = ble_namecuappDequeue();
		#ifdef dbg
		ESP_LOGE("ble_name_idEequeue","new_connid=%d\n",new_connid);
		ESP_LOGE("ble_name_idEequeue", "bd_addr:%08x%04x",(new_mac[0] << 24) + (new_mac[1] << 16) + (new_mac[2] << 8) + new_mac[3],
                 (new_mac[4] << 8) + new_mac[5]);
		#endif
		while(p != NULL){
			#ifdef dbg
			ESP_LOGE("while(p != NULL)","while(p != NULL)");
			ESP_LOGE("ble_name_idEequeue", "bd_addr:%08x%04x",(p->mac[0] << 24) + (p->mac[1] << 16) + (p->mac[2] << 8) + p->mac[3],
                 (p->mac[4] << 8) + p->mac[5]);
			#endif
			if(eqmac(p->mac,new_mac)){
				p->conn_id = new_connid;
				return;
			}
			p = p->next;
		}
}
uint16_t getid_Dequeue(const char *name)
{
	uint16_t id = 255;
	static struct ble_name_mac *p=NULL;
    p = ble_namecuappDequeue();
	#ifdef dbg
	ESP_LOGE("getid_Dequeue","getid_Dequeue");
	#endif
	while(p != NULL){
		#ifdef dbg
		ESP_LOGE("getid_Dequeue","p != NULL");
		ESP_LOGE("getid_Dequeue","name=%s\n",name);
		ESP_LOGE("getid_Dequeue","p->name=%s\n",p->name);
		ESP_LOGE("getid_Dequeue","name=%d\n",strlen(name));
		#endif
		if(strncmp(name,(const char *)p->name,strlen(name)) == 0){
			id = p->conn_id;
			#ifdef dbg
			ESP_LOGE("getid_Dequeue","id = p->conn_id=%d\n",id);
			#endif
		}
		p = p->next;
	}
		return id;
}
char *getname_Dequeue(uint16_t conn_id)
{
	//char *name;
	static struct ble_name_mac *p=NULL;
	p = ble_namecuappDequeue();
	while(p != NULL){
		if(conn_id == p->conn_id){
			#ifdef dbg
			ESP_LOGE("getname_Dequeue","conn_id == p->conn_id");
			ESP_LOGE("getname_Dequeue","p->name=%s\n",p->name);
			#endif
			return (char*)p->name;
		}
		p = p->next;
	}
	
}



