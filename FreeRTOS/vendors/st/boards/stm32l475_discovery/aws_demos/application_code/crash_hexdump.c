#include <CrashCatcher.h>
#include "FreeRTOSConfig.h"

void vSTM32L475getc( void * pv,
                     char * ch );
void vSTM32L475putc( void * pv,
                     char ch );

const CrashCatcherMemoryRegion* CrashCatcher_GetMemoryRegions(void)
{
    return NULL;
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

