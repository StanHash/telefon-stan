#pragma once

#include "types.h"
#include "fwd.h"

struct BwlEnt
{
    /* 000 */ unsigned loss_count      : 8;
    /* 008 */ unsigned favval          : 16;
    /* 024 */ unsigned act_count       : 8;
    /* 032 */ unsigned stat_view_count : 8;
    /* 040 */ unsigned death_location  : 6;
    /* 046 */ unsigned death_turn      : 10;
    /* 056 */ unsigned deploy_count    : 6;
    /* 062 */ unsigned move_count      : 10;
    /* 072 */ unsigned death_cause     : 4;
    /* 076 */ unsigned exp_gained      : 12;
    /* 088 */ unsigned win_count       : 10;
    /* 098 */ unsigned battle_count    : 12;
    /* 110 */ unsigned killer_pid      : 9;
    /* 119 */ unsigned death_skirm     : 1;
    /* 120 */ /* 8bit pad */
};
