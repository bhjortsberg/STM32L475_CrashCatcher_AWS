#include <CrashCatcher.h>
#include "FreeRTOSConfig.h"
#include <string.h>

void vSTM32L475putc( void * pv,
                     char ch );

extern int store_flash(uint8_t* buffer, uint32_t size);

#define MQTT_MAX_PAYLOAD_SIZE 8192
static uint8_t ucDataBuffer[MQTT_MAX_PAYLOAD_SIZE] __attribute__ ((aligned (8)));
static uint8_t* ucBufferPos;

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

const CrashCatcherMemoryRegion* CrashCatcher_GetMemoryRegions(void)
{
    return NULL;
}

void CrashCatcher_DumpStart(const CrashCatcherInfo* pInfo)
{
    (void)pInfo;
    printString("DumpStart\r\n");
    ucBufferPos = &ucDataBuffer[4];
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
    /* Write size first in the buffer */
    uint32_t size = (uint32_t)ucBufferPos - (uint32_t)&ucDataBuffer[4];
    memcpy(ucDataBuffer, &size, sizeof(uint32_t));
    int ret = store_flash(ucDataBuffer, size + sizeof(uint32_t));
    switch (ret) {
        case -1:
            printString("Failed to write size to FLASH\r\n");
              break;
        case -2:
            printString("Failed to write flash\r\n");
            break;
        case 0:
            printString("Wrote to flash\r\n");
            break;
    }

    printString("Endless loop\r\n");
    while (1);
    return CRASH_CATCHER_EXIT;
}
