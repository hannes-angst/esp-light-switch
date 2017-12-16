#ifndef __USER_CONFIG_LOCAL_H__
#define __USER_CONFIG_LOCAL__

#define ERROR_LEVEL
#define WARN_LEVEL
#define INFO_LEVEL
//#define DEBUG_LEVEL

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

#define STATES        10             // loops per second
#define ON        	  1
#define OFF        	  0
#define BLINK_5	  	  10
#define BLINK_3	  	  6
#define BLINK_LONG	  100

#endif // __MQTT_CONFIG_H__
