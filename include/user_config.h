#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include <c_types.h>
#define USE_OPTIMIZE_PRINTF

#define MQTT_HOST     			"192.168.13.100"
#define MQTT_PORT     			1883
#define MQTT_KEEPALIVE    		30  /*second*/
#define MQTT_RECONNECT_TIMEOUT  	10  /*second*/
#define MQTT_CLEAN_SESSION 		1
#define MQTT_BUF_SIZE   			1024
#define MQTT_CLIENT_ID    		"ESP"

#define PROTOCOL_NAMEv311


#define MQTT_TOPIC_BASE		"/angst/devices"
#define MQTT_DISCOVER		MQTT_TOPIC_BASE"/discovery/"
#define MQTT_ABOUT			"about"
#define MQTT_STATUS    		"status"
#define MQTT_STATUS_ONLINE  "online"
#define MQTT_STATUS_OFFLINE "offline"
#define MQTT_CLIENT_TYPE    "switch"

#define APP_NAME        "Remote Switch"
#define APP_VER_MAJ		1
#define APP_VER_MIN		9
#define APP_VER_REV		7


#define MQTT_SWITCH    		"switch"
#define MQTT_SWITCH_ON  	"on"
#define MQTT_SWITCH_ERR		"err"
#define MQTT_SWITCH_OFF  	"off"


#define APP_POWER			1

#define REL_PIN       12             // GPIO 12 = Red Led and Relay (0 = Off, 1 = On)
#define REL_MUX       PERIPHS_IO_MUX_MTDI_U
#define REL_FUNC      FUNC_GPIO12

#define LED_PIN       13             // GPIO 13 = Green Led (0 = On, 1 = Off)
#define LED_MUX       PERIPHS_IO_MUX_MTCK_U
#define LED_FUNC      FUNC_GPIO13

#define KEY_PIN       0              // GPIO 00 = Button
#define KEY_MUX       PERIPHS_IO_MUX_GPIO0_U
#define KEY_FUNC      FUNC_GPIO0

#define TOC_PIN       14              // GPIO 14 = Touch
#define TOC_MUX       PERIPHS_IO_MUX_MTMS_U
#define TOC_FUNC      FUNC_GPIO14

#define STATES        5             // loops per second
#define ON        	  1
#define OFF        	  0
#define BLINK_5	  	  10
#define BLINK_3	  	  6
#define BLINK_LONG	  100


//SSL disabled
//#define MQTT_SSL_ENABLE

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
#define INFO( format, ... ) os_printf( "[INFO] " format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif

#ifdef DEBUG_LEVEL
#define MQTT_DEBUG_ON
#define DEBUG( format, ... ) os_printf( "[DEBUG] " format, ## __VA_ARGS__ )
#else
#define DEBUG( format, ... )

#endif
#endif

