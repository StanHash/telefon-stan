#pragma once

#include "types.h"
#include "fwd.h"

#include "unit.h"

enum
{
    BATTLE_FLAG_REAL = (1 << 0),
    BATTLE_FLAG_SIMULATE = (1 << 1),
    BATTLE_FLAG_BIT2 = (1 << 2),
    BATTLE_FLAG_BALLISTA = (1 << 3),
    BATTLE_FLAG_BIT4 = (1 << 4),
    BATTLE_FLAG_ARENA = (1 << 5),
    BATTLE_FLAG_REFRESH = (1 << 6),
    BATTLE_FLAG_MAPANIMS = (1 << 7),
    BATTLE_FLAG_PROMOTION = (1 << 8),
    BATTLE_FLAG_DANCERING = (1 << 9),
};

enum
{
    BATTLE_HIT_ATTR_CRIT      = (1 << 0),
    BATTLE_HIT_ATTR_MISS      = (1 << 1),
    BATTLE_HIT_ATTR_FOLLOWUP  = (1 << 2),
    BATTLE_HIT_ATTR_RETALIATE = (1 << 3),
    BATTLE_HIT_ATTR_BRAVE     = (1 << 4),
    BATTLE_HIT_ATTR_5         = (1 << 5), // unused?
    BATTLE_HIT_ATTR_POISON    = (1 << 6),
    BATTLE_HIT_ATTR_DEVIL     = (1 << 7),
    BATTLE_HIT_ATTR_HPSTEAL   = (1 << 8),
    BATTLE_HIT_ATTR_HPHALVE   = (1 << 9),
    BATTLE_HIT_ATTR_TATTACK   = (1 << 10), // triangle attack!
    BATTLE_HIT_ATTR_SILENCER  = (1 << 11),
    BATTLE_HIT_ATTR_12        = (1 << 12), // unused?
    BATTLE_HIT_ATTR_PETRIFY   = (1 << 13),
    BATTLE_HIT_ATTR_SURESHOT  = (1 << 14),
    BATTLE_HIT_ATTR_GREATSHLD = (1 << 15),
    BATTLE_HIT_ATTR_PIERCE    = (1 << 16),
    BATTLE_HIT_ATTR_17        = (1 << 17), // unused?
    BATTLE_HIT_ATTR_18        = (1 << 18), // unused?
};

enum
{
    BATTLE_HIT_INFO_BEGIN        = (1 << 0),
    BATTLE_HIT_INFO_FINISHES     = (1 << 1),
    BATTLE_HIT_INFO_KILLS_TARGET = (1 << 2),
    BATTLE_HIT_INFO_RETALIATION  = (1 << 3),
    BATTLE_HIT_INFO_END          = (1 << 4),
};

struct BattleSt
{
    /* 00 */ u16 flags;
    /* 02 */ u8 range;
    /* 03 */ u8 pad_03[1];
    /* 04 */ short damage;
    /* 06 */ short attack;
    /* 08 */ short defense;
    /* 0A */ short hit;
    /* 0C */ short crit;
    /* 0E */ short silencer_rate;
    /* 10 */ struct Unit * ta_unit_a;
    /* 14 */ struct Unit * ta_unit_b;
};

struct BattleUnit
{
    /* 00 */ struct Unit unit;

    /* 48 */ u16 weapon;
    /* 4A */ u16 weapon_before;
    /* 4C */ u32 weapon_attributes;
    /* 50 */ u8 weapon_kind;
    /* 51 */ u8 weapon_inventory_slot;
    /* 52 */ bool has_inventory_weapon;

    /* 53 */ i8 advantage_bonus_hit;
    /* 54 */ i8 advantage_bonus_damage;

    /* 55 */ u8 terrain;
    /* 56 */ i8 terrain_defense;
    /* 57 */ i8 terrain_avoid;
    /* 58 */ i8 terrain_resistance;
    /* 59 */ u8 pad_59[1];

    /* 5A */ short battle_attack;
    /* 5C */ short battle_defense;
    /* 5E */ short battle_speed;
    /* 60 */ short battle_hit;
    /* 62 */ short battle_avoid;
    /* 64 */ short battle_effective_hit;
    /* 66 */ short battle_crit;
    /* 68 */ short battle_dodge;
    /* 6A */ short battle_effective_crit;
    /* 6C */ short battle_silencer_rate;

    /* 6E */ i8 exp_gain;
    /* 6F */ i8 output_status;
    /* 70 */ u8 previous_level;
    /* 71 */ i8 previous_exp;
    /* 72 */ i8 previous_hp;

    /* 73 */ i8 change_hp;
    /* 74 */ i8 change_pow;
    /* 75 */ i8 change_skl;
    /* 76 */ i8 change_spd;
    /* 77 */ i8 change_def;
    /* 78 */ i8 change_res;
    /* 79 */ i8 change_lck;
    /* 7A */ i8 change_con;

    /* 7B */ i8 wexp_gain;
    /* 7C */ bool dealt_damage;
    /* 7D */ bool weapon_broke;

    /* 7E */ bool has_item_effect_target;
    /* 7F */ /* pad */
};


struct BattleHit
{
    /* 00:18 */ unsigned attributes    : 19;
    /* 19:23 */ unsigned info          : 5;
    /* 24:31 */ signed   hp_difference : 8;
};
