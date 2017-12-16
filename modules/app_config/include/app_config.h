#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_

typedef struct{
  uint32_t cfg_holder; //2
  char sta_ssid[32];   //31
  char sta_pwd[32];    //31
  char mqtt_host[32];  //32
  char name[32];       //31
  uint8_t power;       //1

} SYSCFG;
typedef struct {
    uint8 flag;
    uint8 pad[3];
} SAVE_FLAG;

void ICACHE_FLASH_ATTR CFG_Save();
void ICACHE_FLASH_ATTR CFG_Load();
void ICACHE_FLASH_ATTR CFG_Default();

extern SYSCFG sysCfg;

#endif /* USER_CONFIG_H_ */
