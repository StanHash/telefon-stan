#pragma once

#include <string.h>

#define REG_DEBUG_ENABLE ((volatile unsigned short *) 0x4FFF780)
#define REG_DEBUG_FLAGS  ((volatile unsigned short *) 0x4FFF700)
#define REG_DEBUG_STRING ((char *)                    0x4FFF600)

static inline void mgba_puts(int level, char const * ptr)
{
    level &= 0x7;
    strcpy(REG_DEBUG_STRING, ptr);
    *REG_DEBUG_FLAGS = level | 0x100;
}

static inline int mgba_open(void)
{
    *REG_DEBUG_ENABLE = 0xC0DE;
    return *REG_DEBUG_ENABLE == 0x1DEA;
}

static inline void mgba_close(void)
{
    *REG_DEBUG_ENABLE = 0;
}
