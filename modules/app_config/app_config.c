/* config.c
*
* Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
* All rights reserved.
*
*/
#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
#include "osapi.h"
#include "user_interface.h"
#include "app_config.h"
#include "user_config.h"

SYSCFG sysCfg;
SYSCFG myCfg;
SAVE_FLAG saveFlag;

void ICACHE_FLASH_ATTR
CFG_Default()
{
  INFO("CONFIG: Use default configuration\r\n");
  os_memset(&sysCfg, 0x00, sizeof(sysCfg));

  sysCfg.cfg_holder = CFG_HOLDER;
  strcpy(sysCfg.sta_ssid, STA_SSID);
  strcpy(sysCfg.sta_pwd, STA_PASS);
  strcpy(sysCfg.mqtt_host, MQTT_HOST);
  os_sprintf(sysCfg.name, "%08X", system_get_chip_id());
  sysCfg.power = 1;
  CFG_Save();
}

void ICACHE_FLASH_ATTR
CFG_Save()
{
  if (os_memcmp(&myCfg, &sysCfg, sizeof(sysCfg))) {
    INFO("CONFIG: Save configuration to flash ...\r\n");
    myCfg = sysCfg;
    spi_flash_read((CFG_LOCATION + 3) * SPI_FLASH_SEC_SIZE, (uint32 *)&saveFlag, sizeof(SAVE_FLAG));
    saveFlag.flag = (saveFlag.flag == 0) ? 1 : 0;
    spi_flash_erase_sector(CFG_LOCATION + saveFlag.flag);
    spi_flash_write((CFG_LOCATION + saveFlag.flag) * SPI_FLASH_SEC_SIZE, (uint32 *)&myCfg, sizeof(SYSCFG));
    spi_flash_erase_sector(CFG_LOCATION + 3);
    spi_flash_write((CFG_LOCATION + 3) * SPI_FLASH_SEC_SIZE, (uint32 *)&saveFlag, sizeof(SAVE_FLAG));
    INFO("done\r\n");
  }
}

void ICACHE_FLASH_ATTR
CFG_Load()
{
  INFO("CONFIG: Load configuration from flash ...\r\n");
  spi_flash_read((CFG_LOCATION + 3) * SPI_FLASH_SEC_SIZE, (uint32 *)&saveFlag, sizeof(SAVE_FLAG));
  spi_flash_read((CFG_LOCATION + saveFlag.flag) * SPI_FLASH_SEC_SIZE, (uint32 *)&myCfg, sizeof(SYSCFG));
  if(myCfg.cfg_holder != CFG_HOLDER) {
	  CFG_Default();
  }
  sysCfg = myCfg;
  INFO("done\r\n");
}
