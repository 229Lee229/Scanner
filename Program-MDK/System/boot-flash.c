#include "stm32f10x_flash.h"
#include "boot-flash.h"



FLASH_Status Erase_Flag_Sector(void)
{
    FLASH_Status status = FLASH_COMPLETE;
    
    /* ??Flash */
    FLASH_Unlock();
    
    /* ???????? */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    
    /* ????? */
    status = FLASH_ErasePage(APP_VERSION_ADDRESS);
    
    /* ????Flash */
    FLASH_Lock();
    
    return status;
}

void Write_App_Version(void) 
{
    FLASH_Status status = FLASH_COMPLETE;
    uint16_t data;
    uint32_t address = APP_VERSION_ADDRESS;
    const uint8_t app_version[] = "APP-V0.1-Lee";
    uint32_t page_num = (APP_VERSION_ADDRESS - FLASH_BASE) / FLASH_PAGE_SIZE;

    // ??????
    __disable_irq();

    // ??Flash
    FLASH_Unlock();

    // ????????
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    // ?????
    status = FLASH_ErasePage(FLASH_BASE + (page_num * FLASH_PAGE_SIZE));
    if(status != FLASH_COMPLETE)
    {
        FLASH_Lock();
        __enable_irq();
        return;
    }

    // ????
    for(int i = 0; i < sizeof(app_version); i += 2)
    {
        // ?????????
        data = (uint16_t)(app_version[i+1] << 8) | app_version[i];
        
        status = FLASH_ProgramHalfWord(address + i, data);
        if(status != FLASH_COMPLETE)
        {
            break;
        }
    }

    // ??Flash
    FLASH_Lock();
    
    // ????
    __enable_irq();
}
