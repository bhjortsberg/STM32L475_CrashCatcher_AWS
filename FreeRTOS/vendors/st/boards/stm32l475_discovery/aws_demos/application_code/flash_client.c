#include "flash.h"
#include "stm32l4xx_hal.h"

uint8_t CRASH_DATA[2048] __attribute__( ( section( ".crash_data" ), aligned (8) ) ) = { 0 };

int erase_flash()
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    if (FLASH_unlock_erase((uint32_t)&CRASH_DATA, sizeof(CRASH_DATA)) == 0)
    {
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
        int zero = 0xbbbbaaaa;
        return FLASH_write_at((uint32_t)&CRASH_DATA, &zero, sizeof(zero));
    }
    return -1;
}

int has_data_flash()
{
    uint32_t* pValue = (uint32_t*)&CRASH_DATA[0];
    return (*pValue != 0xbbbbaaaa) ? 1 : 0;
}

int store_flash(uint8_t* buffer, uint32_t size)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    if (FLASH_unlock_erase((uint32_t)&CRASH_DATA, sizeof(CRASH_DATA)) == 0)
    {
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
        if (FLASH_write_at((uint32_t)&CRASH_DATA, &size, sizeof(uint32_t)) != 0)
        {
            return -1;
        }
    }
    /* Above will write 8 bytes */
    if (FLASH_write_at((uint32_t)&CRASH_DATA + 8, buffer, size) != 0)
    {
        return -2;
    }
    return 0;
}

const char* read_flash(uint32_t* size)
{
    *size = *(uint32_t*)&CRASH_DATA[0];
    return &CRASH_DATA[8];
}
