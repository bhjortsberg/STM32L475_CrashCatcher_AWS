#include <CrashCatcher.h>
#include "FreeRTOSConfig.h"
#include <string.h>

#include "flash.h"
#include "stm32l4xx_hal.h"

void vSTM32L475getc( void * pv,
                     char * ch );
void vSTM32L475putc( void * pv,
                     char ch );

#define MQTT_MAX_PAYLOAD_SIZE 8192
static uint8_t ucDataBuffer[MQTT_MAX_PAYLOAD_SIZE] __attribute__ ((aligned (8)));
static uint8_t* ucBufferPos;
uint8_t CRASH_DATA[2048] __attribute__( ( section( ".crash_data" ), aligned (8) ) ) = { 0 };
//uint8_t CRASH_DATA[2048] __attribute__( ( section( ".crash_data" ), aligned (8) ) );

static void dumpHalfWords(const uint16_t* pMemory, size_t elementCount);
static void dumpWords(const uint32_t* pMemory, size_t elementCount);

void CrashCatcher_putc(int c)
{
    vSTM32L475putc( NULL, (char)c);
}

static void printString(const char* pString)
{
    while (*pString)
        CrashCatcher_putc(*pString++);
}

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

typedef struct {
    uint32_t size;
    uint32_t d;
} data_t;
data_t data __attribute__( (aligned (8) ) ) = { sizeof(data_t), 0xdeadbeef };

static void store_flash(uint32_t size)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    if (FLASH_unlock_erase((uint32_t)&CRASH_DATA, sizeof(CRASH_DATA)) == 0)
    {
        printString("store erased the data\r\n");
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
        if (FLASH_write_at((uint32_t)&CRASH_DATA, &size, sizeof(uint32_t)) != 0)
        {
            printString("Failed to write size to FLASH\r\n");
            return;
        }
    }
    /* Above will write 8 bytes */
    printString("Will write the data\r\n");
    if (FLASH_write_at((uint32_t)&CRASH_DATA + 8, ucDataBuffer, size) != 0)
    {
        printString("Failed to write flash\r\n");
        return;
    }
    printString("Wrote to flash\r\n");
}

const char* read_flash(uint32_t* size)
{
    *size = *(uint32_t*)&CRASH_DATA[0];
    return &CRASH_DATA[8];
}

const CrashCatcherMemoryRegion* CrashCatcher_GetMemoryRegions(void)
{
    return NULL;
}

void CrashCatcher_DumpStart(const CrashCatcherInfo* pInfo)
{
    (void)pInfo;
    printString("DumpStart\r\n");
    ucBufferPos = ucDataBuffer;
}

void CrashCatcher_DumpMemory(const void* pvMemory, CrashCatcherElementSizes elementSize, size_t elementCount)
{
    printString("DumpMemory\r\n");
    switch (elementSize)
    {
    case CRASH_CATCHER_BYTE:
        memcpy((void*)ucBufferPos, pvMemory, elementCount);
        ucBufferPos += elementCount;
        break;
    case CRASH_CATCHER_HALFWORD:
        dumpHalfWords(pvMemory, elementCount);
        break;
    case CRASH_CATCHER_WORD:
        dumpWords(pvMemory, elementCount);
        break;
    }
    printString("Leaving DumpMemory\r\n");
}

static void dumpHalfWords(const uint16_t* pMemory, size_t elementCount)
{
    size_t i;
    for (i = 0 ; i < elementCount ; i++)
    {
        uint16_t val = *pMemory++;
        memcpy((void*)ucBufferPos, &val, sizeof(val));
        ucBufferPos += sizeof(val);
    }
}

static void dumpWords(const uint32_t* pMemory, size_t elementCount)
{
    size_t i;
    for (i = 0 ; i < elementCount ; i++)
    {
        uint32_t val = *pMemory++;
        memcpy((void*)ucBufferPos, &val, sizeof(val));
        ucBufferPos += sizeof(val);
    }
}

CrashCatcherReturnCodes CrashCatcher_DumpEnd(void)
{
    printString("Dump End\r\n");
    store_flash((uint32_t)ucBufferPos - (uint32_t)ucDataBuffer);
    printString("Endless loop\r\n");
    while (1);
    return CRASH_CATCHER_EXIT;
}
