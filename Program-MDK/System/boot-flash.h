#ifndef __BOOT_FLASH_H
#define __BOOT_FLASH_H
#define APP_VERSION_ADDRESS 0x08002C00
#define FLASH_PAGE_SIZE     0x400  // 1KB per page

void Write_App_Version(void) ;
FLASH_Status Erase_Flag_Sector(void);

#endif
