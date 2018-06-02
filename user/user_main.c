#include <user_interface.h>
#include <osapi.h>
#include <c_types.h>
#include <mem.h>
#include <os_type.h>
#include <gpio.h>
#include "user_config.h"
#include "mqtt.h"
#include "wifi.h"
#include "info.h"

static ETSTimer state_timer;
static uint8_t relayState = OFF;
static uint8_t ledState = OFF;
static uint8_t blink = BLINK_LONG;
static uint8_t lastbutton = OFF;
static uint8_t lasttouch = OFF;
static uint8_t loop = 0;
static MQTT_Client mqttClient;

static void ICACHE_FLASH_ATTR switchRelay() {
	if (relayState == OFF) {
		INFO("Relay OFF\r\n");
		blink = BLINK_3;
		GPIO_OUTPUT_SET(REL_PIN, 0);
	} else if (relayState == ON) {
		INFO("Relay ON\r\n");
		GPIO_OUTPUT_SET(REL_PIN, 1);
		blink = BLINK_3;
	} else {
		INFO("Ignore switch relay state %d\r\n", relayState);
	}
}

static void ICACHE_FLASH_ATTR switchLed() {
	if (ledState == OFF) {
		GPIO_OUTPUT_SET(LED_PIN, 1);
	} else if (ledState == ON) {
		GPIO_OUTPUT_SET(LED_PIN, 0);
	} else {
		INFO("Ignore switch LED state %d\r\n", ledState);
	}
}

static void ICACHE_FLASH_ATTR wifiConnectCb(uint8_t status) {
	if (status == STATION_GOT_IP) {
		INFO("Got WiFi\r\n");
		MQTT_Connect(&mqttClient);
		blink = OFF;
		ledState = ON;
		switchLed();
	} else if (status == STATION_IDLE || status == STATION_CONNECTING) {
		INFO("Wifi waiting: %d\r\n", status);
		ledState = OFF;
		blink = BLINK_LONG;
	} else if (status == STATION_WRONG_PASSWORD || status == STATION_NO_AP_FOUND || status == STATION_CONNECT_FAIL) {
		INFO("Wifi failed: %d\r\n", status);
		MQTT_Disconnect(&mqttClient);
		ledState = OFF;
		blink = OFF;
		switchLed();
	}
}

static void ICACHE_FLASH_ATTR pubRelayState(MQTT_Client* client) {
	char *topicBuf = (char*) os_zalloc(64);
	char *dataBuf;
	//Tell switch status
	os_sprintf(topicBuf, "%s/%08X/%s", MQTT_TOPIC_BASE, system_get_chip_id(), MQTT_SWITCH);
	if (relayState == OFF) {
		dataBuf = "{\"state\":\""MQTT_SWITCH_OFF"\"}";
	} else if (relayState == ON) {
		dataBuf = "{\"state\":\""MQTT_SWITCH_ON"\"}";
	} else {
		dataBuf = "{\"state\":\""MQTT_SWITCH_ERR"\"}";
	}
	int len = os_strlen(dataBuf);
	MQTT_Publish(client, topicBuf, dataBuf, len, 0, 0);
	os_free(topicBuf);
}

static void ICACHE_FLASH_ATTR subscribe(MQTT_Client* client) {
	char *topic = (char*) os_zalloc(1024);

	os_sprintf(topic, "%s/%08X/%s/%s", MQTT_TOPIC_BASE, system_get_chip_id(), MQTT_SWITCH, MQTT_SWITCH_ON);
	if (MQTT_Subscribe(client, topic, 0)) {
		INFO("Subscribe from: %s\r\n", topic);
	} else {
		ERROR("Failed to subscribe from %s\r\n", topic);
	}

	os_sprintf(topic, "%s/%08X/%s/%s", MQTT_TOPIC_BASE, system_get_chip_id(), MQTT_SWITCH, MQTT_SWITCH_OFF);
	if (MQTT_Subscribe(client, topic, 0)) {
		INFO("Subscribe from: %s\r\n", topic);
	} else {
		ERROR("Failed to subscribe from %s\r\n", topic);
	}

	//participate in discovery requests
	if (MQTT_Subscribe(client, MQTT_DISCOVER, 0)) {
		INFO("Subscribe from: %s\r\n", MQTT_DISCOVER);
	} else {
		ERROR("Failed to subscribe from %s\r\n", MQTT_DISCOVER);
	}

	os_free(topic);
}

static void ICACHE_FLASH_ATTR sendDeviceInfo(MQTT_Client* client) {
	char *topicBuf = (char*) os_zalloc(128);
	char *dataBuf = (char*) os_zalloc(1024);
	//Tell Online status
	os_sprintf(topicBuf, "%s/%08X/info", MQTT_TOPIC_BASE, system_get_chip_id());

	int len = 0;
	len += os_sprintf(dataBuf + len, "{\"name\":\"switch\"");
	len += os_sprintf(dataBuf + len, ",\"app\":\"%s\"", APP_NAME);
	len += os_sprintf(dataBuf + len, ",\"version\":\"%d.%d.%d\"", APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);

	uint8 mac_addr[6];
	if (wifi_get_macaddr(0, mac_addr)) {
		len += os_sprintf(dataBuf + len, ",\"mac\":\"%02x:%02x:%02x:%02x:%02x:%02x\"", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	}
	struct ip_info info;
	if (wifi_get_ip_info(0, &info)) {
		len += os_sprintf(dataBuf + len, ",\"ip\":\"%d.%d.%d.%d\"", IP2STR(&info.ip.addr));
	}
	len += os_sprintf(dataBuf + len, ",\"type\":\"%s\"", MQTT_CLIENT_TYPE);

	if (relayState == OFF) {
		len += os_sprintf(dataBuf + len, ",\"%s\": \"%s\"", MQTT_SWITCH, MQTT_SWITCH_OFF);
	} else if (relayState == ON) {
		len += os_sprintf(dataBuf + len, ",\"%s\": \"%s\"", MQTT_SWITCH, MQTT_SWITCH_ON);
	} else {
		len += os_sprintf(dataBuf + len, ",\"%s\": \"%s\"", MQTT_SWITCH, MQTT_SWITCH_ERR);
	}

	len += os_sprintf(dataBuf + len, ",\"base\":\"%s/%08X/\"}", MQTT_TOPIC_BASE, system_get_chip_id());
	MQTT_Publish(client, topicBuf, dataBuf, len, 0, 0);

	os_free(topicBuf);
	os_free(dataBuf);
}

static void ICACHE_FLASH_ATTR mqttConnectedCb(uint32_t *args) {
	INFO("MQTT: Connected\r\n");
	mqttClient = *(MQTT_Client*) args;

	subscribe(&mqttClient);


	INFO("Sending Device Info.\r\n");
	sendDeviceInfo(&mqttClient);

	INFO("Publishing state.\r\n");
	pubRelayState(&mqttClient);
}

static void ICACHE_FLASH_ATTR state_cb(void) {
	DEBUG("Loop (%d/%d) start\r\n", loop, STATES);

	if (loop >= STATES) {
		loop = 0;
	} else {
		loop++;
	}

	if (loop == 0) {
		if (blink != OFF) {
			ledState ^= 1;
			switchLed();
			blink--;
		}
	}

	uint8_t button = 1 - GPIO_INPUT_GET(KEY_PIN);
	uint8_t touch = GPIO_INPUT_GET(TOC_PIN);

	DEBUG("button: %d, touch: %d\r\n", button, touch);

	if (lastbutton == OFF && button == ON) {
		relayState ^= 1;
		switchRelay();
		blink = BLINK_3;
		pubRelayState(&mqttClient);
	} else if (lasttouch == OFF && touch == ON) {
		relayState ^= 1;
		switchRelay();
		blink = BLINK_3;
		pubRelayState(&mqttClient);
	}
	lastbutton = button;
	lasttouch = touch;
	DEBUG("Loop end\r\n");
}

static void ICACHE_FLASH_ATTR stopLoop() {
	os_timer_disarm(&state_timer);
	os_timer_setfn(&state_timer, (os_timer_func_t *) state_cb, NULL);
}

static void ICACHE_FLASH_ATTR startLoop() {
	os_timer_disarm(&state_timer);
	os_timer_setfn(&state_timer, (os_timer_func_t *) state_cb, NULL);
	os_timer_arm(&state_timer, 1000 / STATES, 1);
}

static void ICACHE_FLASH_ATTR mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len) {
	//Stop Timer that checks inputs
	stopLoop();
	MQTT_Client* client = (MQTT_Client*) args;
	char *topicBuf = (char*) os_zalloc(topic_len + 1);
	os_memcpy(topicBuf, topic, topic_len);
	topicBuf[topic_len] = 0;

	INFO("Receive topic: %s, \r\n", topicBuf);
	if (topic_len >= os_strlen(MQTT_ABOUT) && strcmp(topicBuf + topic_len - os_strlen(MQTT_ABOUT), MQTT_ABOUT) == 0) {
		sendDeviceInfo(client);
	} else if (os_strcmp(topicBuf, MQTT_DISCOVER) == 0) {
		sendDeviceInfo(client);
	} else if (topic_len >= os_strlen(MQTT_SWITCH_ON) && strcmp(topicBuf + topic_len - os_strlen(MQTT_SWITCH_ON), MQTT_SWITCH_ON) == 0) {
		relayState = ON;
		switchRelay();
		pubRelayState(client);
	} else if (topic_len >= os_strlen(MQTT_SWITCH_OFF) && strcmp(topicBuf + topic_len - os_strlen(MQTT_SWITCH_OFF), MQTT_SWITCH_OFF) == 0) {
		relayState = OFF;
		switchRelay();
		pubRelayState(client);
	} else {
		INFO("Ignore data\r\n");
	}
	os_free(topicBuf);

	//start timer
	startLoop();
}

static void ICACHE_FLASH_ATTR mqtt_init(void) {
	DEBUG("INIT MQTT\r\n");
	//If WIFI is connected, MQTT gets connected (see wifiConnectCb)
	MQTT_InitConnection(&mqttClient, MQTT_HOST, MQTT_PORT, SEC_NONSSL);

	char *clientId = (char*) os_zalloc(64);
	os_sprintf(clientId, "%s%08X", MQTT_CLIENT_ID, system_get_chip_id());

	char *id = (char*) os_zalloc(32);
	os_sprintf(id, "%08X", system_get_chip_id());

	//id will be copied by MQTT_InitLWT
	if (!MQTT_InitClient(&mqttClient, clientId, id, id, MQTT_KEEPALIVE, MQTT_CLEAN_SESSION)) {
		ERROR("Could not initialize MQTT client");
	}
	os_free(id);
	os_free(clientId);

	MQTT_OnConnected(&mqttClient, mqttConnectedCb);
	MQTT_OnData(&mqttClient, mqttDataCb);
}

static void ICACHE_FLASH_ATTR app_init(void) {
	print_info();

	switchLed();
	mqtt_init();
	WIFI_Connect(STA_SSID, STA_PASS, wifiConnectCb);
	startLoop();
}

void user_init(void) {
	PIN_FUNC_SELECT(REL_MUX, REL_FUNC);

	//Relay State OFF on start up.
	relayState = OFF;
	switchRelay();

	PIN_FUNC_SELECT(LED_MUX, LED_FUNC);
	ledState = OFF;
	blink = BLINK_LONG;

	PIN_FUNC_SELECT(KEY_MUX, KEY_FUNC);
	PIN_PULLUP_EN(KEY_MUX);            // Enable pull-up
	GPIO_DIS_OUTPUT(KEY_PIN);          // Set KEY_PIN pin as an input

	PIN_FUNC_SELECT(TOC_MUX, TOC_FUNC);
	PIN_PULLUP_DIS(TOC_MUX);            // Disable pull-up
	GPIO_DIS_OUTPUT(TOC_PIN);          // Set TOC_PIN pin as an input

	system_init_done_cb(app_init);
}
