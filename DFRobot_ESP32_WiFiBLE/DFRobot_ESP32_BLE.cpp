/*!
 * @file DFRobot_ESP32_BLE.cpp
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


#include "esp32-hal-log.h"
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
#include "DFRobot_ESP32_BLE.h"

#define GATTC_TAG "GATTC_DEMO"


bool wifi_true = false;





///Declare static functions
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_a_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_b_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static esp_gatt_if_t mygattc_if;
static uint16_t myser_uuid16 = 0xdfb0;
static uint16_t mydes_uuid16 = 0xdfb1;
static esp_gatt_srvc_id_t alert_service_id = {
    .id = {
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid = {.uuid16 = myser_uuid16,},
        },
        .inst_id = 0,
    },
    .is_primary = true,
};

static esp_gatt_id_t notify_descr_id = {
    .uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid = {.uuid16 = mydes_uuid16,},
    },
    .inst_id = 0,
};
#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

static bool connect = false;
static String connectname0="";
static String connectname1="";
static String connectname2="";
static String connectname3="";
static uint8_t connummax = 4;
static uint8_t conn_num = 0;
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};


#define PROFILE_NUM 2
#define PROFILE_A_APP_ID 0
#define PROFILE_B_APP_ID 1

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    esp_bd_addr_t remote_bda;
};

/* One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_a_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    #if 0
    [PROFILE_B_APP_ID] = {
        .gattc_cb = gattc_profile_b_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    #endif
};
extern int mys;
extern void send_wifi_data(int s);
extern void cuappEnqueue(uint8_t *pbuf,uint16_t len,uint16_t conn_id);
extern void wificuappEnqueue(uint8_t *pbuf,uint16_t len,uint16_t conn_id);
extern void ble_name_Enqueue(uint8_t *mac,uint16_t mac_len ,char *name, uint16_t name_len);
extern void ble_name_idEequeue(uint8_t *new_mac,uint16_t new_connid);
static void wifi_data_enqueue(uint8_t *pbuf, uint16_t len, uint16_t conn_id)
{
	wificuappEnqueue(pbuf,len,conn_id);
}
void recevice_data(uint8_t * buf, uint16_t len, uint16_t conn_id)
{
	cuappEnqueue(buf,len,conn_id);
};
static void send_ble_data(uint8_t *buf,uint16_t len,uint16_t conn_id)
{	
/*
	uint8_t conn_id = 0;
	if(conn_num == 1 && connummax == 1){
		conn_id = 0;
	}else{
		conn_id = buf[0];
		buf[0] += 0x01;
	}
	*/
    esp_ble_gattc_write_char(
		mygattc_if,
		conn_id,
		&alert_service_id,
		&notify_descr_id,
		len,
		buf,
		ESP_GATT_WRITE_TYPE_NO_RSP,
		ESP_GATT_AUTH_REQ_NONE);
	
};
/*
static void data_out_queue(void)
{
	static struct blemsgBuf *p=NULL;
	p = cuappDequeue();
	if(p != NULL){
		send_ble_data((uint8_t *)p->data,p->len);
		free(p);		
	}else{
		return;
	}
}
*/
static void df_test_ble_name(void)
{
	static struct ble_name_mac *p=NULL;
	#ifdef dbg
	ESP_LOGE(GATTC_TAG, "df_test_ble_name\n");
	#endif
	p = ble_namecuappDequeue();
	if(p != NULL){
		#ifdef dbg
		ESP_LOGE(GATTC_TAG, "conn_id= %d\n", p->conn_id);
		ESP_LOGE(GATTC_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:%08x%04x",(p->mac[0] << 24) + (p->mac[1] << 16) + (p->mac[2] << 8) + p->mac[3],
                 (p->mac[4] << 8) + p->mac[5]);
		ESP_LOGE(GATTC_TAG, "adv_name== %s\n", p->name);
		#endif
		//free(p);
	}else{
		#ifdef dbg
		ESP_LOGE(GATTC_TAG,"else######");
		#endif
		return;
	}
	
}
static void send_date( esp_gatt_if_t gattc_if,
                       uint16_t conn_id,
                       esp_gatt_srvc_id_t *srvc_id,
                       esp_gatt_id_t *char_id,
                       uint16_t value_len,
                       uint8_t *value,
                       esp_gatt_write_type_t write_type,
                       esp_gatt_auth_req_t auth_req)
{
		recevice_data(value,value_len,conn_id);
}

static void gattc_profile_a_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    uint16_t conn_id = 0;
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        esp_ble_gap_set_scan_params(&ble_scan_params);
        break;
    case ESP_GATTC_OPEN_EVT:
        conn_id = p_data->open.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->open.remote_bda, sizeof(esp_bd_addr_t));
        esp_ble_gattc_search_service(gattc_if, conn_id, NULL);
		#ifdef dbg
		ESP_LOGE(GATTC_TAG, "ESP_GATTC_OPEN_EVT_id:%d\n",conn_id);
		#endif
		ble_name_idEequeue(p_data->open.remote_bda,conn_id);
		//连接id入队
		mygattc_if = gattc_if;
		if(connummax > conn_num){
			esp_ble_gap_start_scanning(30);
		}
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        esp_gatt_srvc_id_t *srvc_id = &p_data->search_res.srvc_id;
        conn_id = p_data->search_res.conn_id;
        if (srvc_id->id.uuid.len == ESP_UUID_LEN_16) {
			//log_e("UUID16: ");
            ESP_LOGI(GATTC_TAG, "UUID16: %x", srvc_id->id.uuid.uuid.uuid16);
        } else if (srvc_id->id.uuid.len == ESP_UUID_LEN_32) {
			//log_e("UUID32: ");
            ESP_LOGI(GATTC_TAG, "UUID32:");
        } else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128) {
            ESP_LOGI(GATTC_TAG, "UUID128:");
           // esp_log_buffer_hex(GATTC_TAG, (char *)srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128);
        } else {
			//log_e("UNKNOWN LEN ");
            ESP_LOGE(GATTC_TAG, "UNKNOWN LEN %d", srvc_id->id.uuid.len);
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        conn_id = p_data->search_cmpl.conn_id;
        esp_ble_gattc_get_characteristic(gattc_if, conn_id, &alert_service_id, NULL);
        break;
    case ESP_GATTC_GET_CHAR_EVT:
        if (p_data->get_char.status != ESP_GATT_OK) {
            break;
        }
        if (p_data->get_char.char_id.uuid.uuid.uuid16 == mydes_uuid16) {
            esp_ble_gattc_register_for_notify(gattc_if, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, &alert_service_id, &p_data->get_char.char_id);
        }
        esp_ble_gattc_get_characteristic(gattc_if, conn_id, &alert_service_id, &p_data->get_char.char_id);
        break;
		
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        uint16_t notify_en = 1;
        esp_ble_gattc_write_char_descr(
                gattc_if,
                conn_id,
                &alert_service_id,
                &p_data->reg_for_notify.char_id,
                &notify_descr_id,
                sizeof(notify_en),
                (uint8_t *)&notify_en,
                ESP_GATT_WRITE_TYPE_NO_RSP,
                ESP_GATT_AUTH_REQ_NONE);
    break;
    }
    case ESP_GATTC_NOTIFY_EVT:{
		#ifdef dbg
		ESP_LOGE(GATTC_TAG, "p_data->search_cmpl.conn_id %d", p_data->congest.conn_id);
		#endif
		recevice_data((uint8_t *)p_data->notify.value, p_data->notify.value_len, p_data->congest.conn_id);
		df_test_ble_name();
    break;
  } 
 
    case ESP_GATTC_WRITE_DESCR_EVT:
		//log.e("ESP_GATTC_WRITE_DESCR_EVT:");
		ESP_LOGI(GATTC_TAG, "WRITE: status %d", p_data->write.status);
      break;
  
    case ESP_GATTC_SRVC_CHG_EVT: {
        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:%08x%04x",(bda[0] << 24) + (bda[1] << 16) + (bda[2] << 8) + bda[3],
                 (bda[4] << 8) + bda[5]);
        break;
    }
	case ESP_GATTC_CLOSE_EVT:{
		if(conn_num > 0){
			conn_num--;
		}
		if(connummax > conn_num){
			esp_ble_gap_start_scanning(30);
		}
		break;
	}
    default:
        break;
    }
}
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 6;
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        //the unit of the duration is second
        uint32_t duration = 30;
        esp_ble_gap_start_scanning(duration);


        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "Scan start failed");
        }


        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);

            if (adv_name != NULL) {
                //if (strlen(device_name) == adv_name_len && strncmp((char *)adv_name, device_name, adv_name_len) ==0) {
				if (strncmp((char *)adv_name, connectname0.c_str(), connectname0.length()) == 0){
					#ifdef dbg
					ESP_LOGE(GATTC_TAG, "adv_namedone");
					#endif
					ble_name_Enqueue(scan_result->scan_rst.bda, 6, (char*)connectname0.c_str(),connectname0.length());
                    if (connect == false) {
						conn_num++;
                        connect = true;
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                    }else{
						conn_num++;
						esp_ble_gap_stop_scanning();
						esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
					}
                }else if(strncmp((char *)adv_name, connectname1.c_str(), connectname1.length()) == 0){
					#ifdef dbg
					ESP_LOGE(GATTC_TAG, "connectname1\n");
					#endif
					ble_name_Enqueue(scan_result->scan_rst.bda, 6, (char*)connectname1.c_str(),connectname1.length());
                    if (connect == false) {
						conn_num++;
                        connect = true;
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                    }else{
						conn_num++;
						esp_ble_gap_stop_scanning();
						esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
					}
				}else if(strncmp((char *)adv_name, connectname2.c_str(), connectname2.length()) == 0){
					#ifdef dbg
					ESP_LOGE(GATTC_TAG, "connectname2\n");
					#endif
					ble_name_Enqueue(scan_result->scan_rst.bda, 6, (char*)connectname2.c_str(),connectname2.length());
                    if (connect == false) {
						conn_num++;
                        connect = true;
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                    }else{
						conn_num++;
						esp_ble_gap_stop_scanning();
						esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
					}
				}else if(strncmp((char *)adv_name, connectname3.c_str(), connectname3.length()) == 0){
					#ifdef dbg
					ESP_LOGE(GATTC_TAG, "connectname3\n");
					#endif
					ble_name_Enqueue(scan_result->scan_rst.bda, 6, (char*)connectname3.c_str(),connectname3.length());
                    if (connect == false) {
						conn_num++;
                        connect = true;
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                    }else{
						conn_num++;
						esp_ble_gap_stop_scanning();
						esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
					}
				}
            }
            break;
        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            break;
        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "Scan stop failed");
        }
        else {
            ESP_LOGI(GATTC_TAG, "Stop scan successfully");
        }
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "Adv stop failed");
        }
        else {
            ESP_LOGI(GATTC_TAG, "Stop adv successfully");
        }
        break;

    default:
        break;
    }
}
static uint8_t esp_gattc_cb_num=0;
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
	esp_gattc_cb_num++;
	ESP_LOGI(GATTC_TAG, "esp_gattc_cb_num=%d", esp_gattc_cb_num);
    ESP_LOGI(GATTC_TAG, "EVT %d, gattc if %d", event, gattc_if);

    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(GATTC_TAG, "Reg app failed, app_id %04x, status %d",
                    param->reg.app_id, 
                    param->reg.status);
            return;
        }
    }

    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gattc_if == gl_profile_tab[idx].gattc_if) {
                if (gl_profile_tab[idx].gattc_cb) {
                    gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}


void ble_client_appRegister(void)
{
    esp_err_t status;

    ESP_LOGI(GATTC_TAG, "register callback");

    //register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
		#ifdef dbg
        ESP_LOGE(GATTC_TAG, "gap register error, error code = %x", status);
		#endif
        return;
    }

    //register the callback function to the gattc module
    if ((status = esp_ble_gattc_register_callback(esp_gattc_cb)) != ESP_OK) {
		#ifdef dbg
        ESP_LOGE(GATTC_TAG, "gattc register error, error code = %x", status);
		#endif
        return;
    }
    esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    //esp_ble_gattc_app_register(PROFILE_B_APP_ID);
}

static void gattc_client_test(void)
{
    esp_bluedroid_init();
    esp_bluedroid_enable();
    ble_client_appRegister();
}
static void wifi_data_ble_dequeue(void){
	static struct wifimsgBuf *p=NULL;
	p = wificuappDequeue();
	if(p != NULL){
		send_ble_data((uint8_t *)p->data, p->len, p->conn_id);
		free(p);		
	}else{
		return;
	}
}
esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
void bluno2_ble_evt_task(void *arg)
{
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BTDM);
	gattc_client_test(); 
	while(1){
		wifi_data_ble_dequeue();
		vTaskDelay(10);
        /*Read timer value from task*/
		//ESP_LOGI(GATTC_TAG, "bluno2_ble_evt_task");
	}
	
};
DFRobot_ESP32_BLE::DFRobot_ESP32_BLE(void)
{
	connectname0 = "Bluno0";
	connectname1 = "Bluno1";
	connectname2 = "Bluno2";
	connectname3 = "Bluno3";
} 

DFRobot_ESP32_BLE::~DFRobot_ESP32_BLE(void)
{
	
} 
void DFRobot_ESP32_BLE::setService(uint16_t ser)
{
	myser_uuid16 = ser;
	alert_service_id.id.uuid.uuid.uuid16 = ser;
}
void DFRobot_ESP32_BLE::setCharacteristic(uint16_t cha)
{
	mydes_uuid16 = cha;
	notify_descr_id.uuid.uuid.uuid16 = cha;
}
void DFRobot_ESP32_BLE::setconnummax(uint8_t max)
{
	connummax = max;
}

void DFRobot_ESP32_BLE::setconnectname0(String str)
{
	connectname0 = str;
}

void DFRobot_ESP32_BLE::setconnectname1(String str)
{
	connectname1 = str;
}

void DFRobot_ESP32_BLE::setconnectname2(String str)
{
	connectname2 = str;
}

void DFRobot_ESP32_BLE::setconnectname3(String str)
{
	connectname3 = str;
}

void DFRobot_ESP32_BLE::init()
{
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BTDM);
}

void DFRobot_ESP32_BLE::begin()
{
	xTaskCreate(bluno2_ble_evt_task, "ble_evt_task", 2048, NULL, 5, NULL);
}
static uint16_t getid(const char *name)
{
	uint16_t id =0;
	id = getid_Dequeue(name);
	#ifdef dbg
	ESP_LOGE("readdata","getid_Dequeue----id=%d\n",id);
	#endif
	return id;
}
static char* getname(uint16_t conn_id)
{
	return getname_Dequeue(conn_id);
}

String DFRobot_ESP32_BLE::readdata(char *name)
{	
	String ret = "";
	static struct blemsgBuf *p=NULL;
	p = cuappDequeue();
	if(p != NULL){
		memcpy(name,getname(p->conn_id),strlen(getname(p->conn_id)));
		#ifdef dbg
		ESP_LOGE("readdata","getname=%s\n",name);
		#endif
		for(int i = 0; i< (p->len); i++){
			ret += (char) (p->data[i]);
		}	
		free(p);
	}
    return ret;
}


void DFRobot_ESP32_BLE::writedata(const char *name,char data)
{
	uint8_t buf[2]={0,0};
	buf[0] = (uint8_t)data;
	writedata(name,buf,1);
}
void DFRobot_ESP32_BLE::writedata(const char *name,char *buf)
{
	uint8_t *bufu8 = NULL;
	memcpy(bufu8,buf,strlen(buf));
	writedata(name,bufu8,strlen(buf));
}
void DFRobot_ESP32_BLE::writedata(const char *name,String str)
{
	unsigned int len = 0;
	len = str.length();
	writedata(name,str,len);
}
void DFRobot_ESP32_BLE::writedata(const char *name, const char *buf)
{
	unsigned int len = 0;
	uint16_t cur_len;
	uint16_t n = 0;
	len = strlen(buf);
	uint8_t u8_buf[len];
	uint16_t conn_id = 0;
	conn_id = getid(name);
	for(int i = 0; i < len; i++){
		u8_buf[i] = (*buf++);
	}
	do{
		cur_len = len > 20 ? 20 : len;
		wifi_data_enqueue(u8_buf + n,cur_len, conn_id);
		n += cur_len;
		len -= cur_len;
	}while(len != 0);
}

void DFRobot_ESP32_BLE::writedata(const char *name,int data)
{
	uint8_t str[10];
	uint8_t str1[10];
	uint8_t c = 0;
	uint8_t len = 0;
	while(1) {
		str[c] = (data % 10) + '0';
        c++;
        data = data / 10;
        if (data == 0) {
            break;
        }
    }
	len = c;
	for(int i = 0; i < c; i++){
		str1[i] = str[c -i-1];
	}
	writedata(name,str1,len);
}

void DFRobot_ESP32_BLE::writedata(const char *name,uint8_t *buf,uint16_t len)
{
	uint16_t conn_id = 0;
	conn_id = getid(name);
	wifi_data_enqueue(buf,len, conn_id);
}
void DFRobot_ESP32_BLE::writedata(const char *name,String str,unsigned int len)
{
	const char *buf;
    buf=str.c_str();
	writedata(name,buf);

}
/*
void Bluno2BLE::writedata(const char *name,String str)
{
	
}
*/





