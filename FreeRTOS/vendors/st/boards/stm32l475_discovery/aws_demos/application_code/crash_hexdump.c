#include <CrashCatcher.h>
#include "FreeRTOSConfig.h"

void vSTM32L475getc( void * pv,
                     char * ch );
void vSTM32L475putc( void * pv,
                     char ch );

const CrashCatcherMemoryRegion* CrashCatcher_GetMemoryRegions(void)
{
    static const CrashCatcherMemoryRegion regions[] = {
        {0x10000000, 0x10008000, CRASH_CATCHER_BYTE},
        {0xFFFFFFFF, 0xFFFFFFFF, CRASH_CATCHER_BYTE}
    };
    return regions;
}

void CrashCatcher_putc(int c)
{
    vSTM32L475putc( NULL, (char)c);
}

int CrashCatcher_getc(void)
{
    int ch = 0;
    vSTM32L475getc( NULL, (char*)ch);
    return ch;
}

static void printString(const char* pString)
{
    while (*pString)
        CrashCatcher_putc(*pString++);
}

CrashCatcherReturnCodes CrashCatcher_DumpEnd(void)
{
    printString("\r\nEnd of dump\r\n");

    while (1);
    return CRASH_CATCHER_EXIT;
}
