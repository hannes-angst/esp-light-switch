#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include <c_types.h>
#define USE_OPTIMIZE_PRINTF

#define MQTT_HOST     			"192.168.13.100"
#define MQTT_PORT     			1883
#define MQTT_KEEPALIVE    		30  /*second*/
#define MQTT_RECONNECT_TIMEOUT  10  /*second*/
#define MQTT_CLEAN_SESSION 		1
#define MQTT_BUF_SIZE   		1024
#define MQTT_CLIENT_ID    		"ESP"

#define PROTOCOL_NAMEv311


#define MQTT_TOPIC_BASE		"/angst/devices"
#define MQTT_DISCOVER		MQTT_TOPIC_BASE"/discovery/"
#define MQTT_SET_NAME		"name"
#define MQTT_OTA			"ota"
#define MQTT_ABOUT			"about"
#define MQTT_STATUS    		"status"
#define MQTT_STATUS_ONLINE  "online"
#define MQTT_STATUS_OFFLINE "offline"
#define MQTT_STATUS_OTA_UP  "updating"
#define MQTT_CLIENT_TYPE    "switch"


//SSL disabled
//#define MQTT_SSL_ENABLE

//Wlan info
#define STA_SSID "olympus"
#define STA_PASS "3854687302232046"

//1015808 F8000

#define CFG_HOLDER      0x20164711   // Change this value to load default configurations
#define CFG_LOCATION    0x1FE		// Please don't change or if you know what you doing
//#define CFG_LOCATION  0xF8         // Please don't change or if you know what you doing
//#define CFG_LOCATION	0x79	/* Please don't change or if you know what you doing */
//#define CFG_LOCATION	0x3C	/* Please don't change or if you know what you doing */

#define LOCAL_CONFIG_AVAILABLE

#ifndef LOCAL_CONFIG_AVAILABLE
#error Please copy user_config.sample.h to user_config.local.h and modify your configurations
#else
#include "user_config.local.h"
#endif

#ifdef ERROR_LEVEL
#define ERROR( format, ... ) os_printf( "[ERROR] " format, ## __VA_ARGS__ )
#else
#define ERROR( format, ... )
#endif

#ifdef WARN_LEVEL
#define WARN( format, ... ) os_printf( "[WARN] " format, ## __VA_ARGS__ )
#else
#define WARN( format, ... )
#endif

#ifdef INFO_LEVEL
#define MQTT_DEBUG_ON
#define INFO( format, ... ) os_printf( "[INFO] " format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif

#ifdef DEBUG_LEVEL
#define DEBUG( format, ... ) os_printf( "[DEBUG] " format, ## __VA_ARGS__ )
#else
#define DEBUG( format, ... )

#endif
#endif

