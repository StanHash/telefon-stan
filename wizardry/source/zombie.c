#include "proc.h"
#include "unit.h"
#include "bwl.h"
#include "battle.h"
#include "mgba.h"

struct SelectTarget
{
    /* 00 */ i8 x, y;
    /* 02 */ i8 uid;
    /* 03 */ u8 extra;

    /* 04 */ struct SelectTarget * next;
    /* 08 */ struct SelectTarget * prev;
};

extern int GetItemIid(int item);
extern struct BwlEnt * BWL_GetEntry(int pid);
extern void BWL_SetDeathStats(int pid, int opponent_pid, int death_method);
extern int GetUnitCurrentHp(struct Unit * unit);
extern struct Unit * GetUnit(int id);
extern void KillUnit(struct Unit * unit);
extern void InitTargets(int x, int y);
extern void AddTarget(int x, int y, int id, int extra);
extern int GetTargetListSize(void);
extern void Proc_Goto(ProcPtr proc, int label);
extern void EnsureCameraOntoPosition(ProcPtr parent, int x, int y);
extern void RefreshUnitSprites(void);
extern void FindUnitClosestValidPosition(struct Unit * unit, int x, int y, struct Vec2 * out);
extern void New6C_SummonGfx(ProcPtr parent, int x, int y);
extern void InitBattleUnit(struct BattleUnit * bu, struct Unit * unit);
extern void CheckBattleUnitLevelUp(struct BattleUnit * bu);
extern void UpdateUnitFromBattle(struct Unit * unit, struct BattleUnit * bu);
extern struct SelectTarget * GetTarget(int id);
extern void NewGotItemPopup(struct Unit * unit, int item, ProcPtr parent);
extern int CreateItem(int item);
extern struct BattleSt gBattleStats;
extern struct BattleUnit gBattleUnitA;
extern struct BattleUnit gBattleUnitB;
extern struct ProcScr const gUnknown_0859BDA0[];

enum { PID_EIRIKA = 1, PID_EPHRAIM = 15 };
enum { ITEM_KIND_LIGHT = 6 };

enum
{
    IID_IRONSWORD        = 0x01,
    IID_SILVERSWORD      = 0x04,
    IID_IRONLANCE        = 0x14,
    IID_SILVERLANCE      = 0x17,
    IID_IRONAXE          = 0x1F,
    IID_SILVERAXE        = 0x21,
    IID_IRONBOW          = 0x2D,
    IID_SILVERBOW        = 0x2F,
    IID_SHADOWKILLER     = 0x81,
    IID_BRIGHTLANCE      = 0x82,
    IID_FIENDCLEAVER     = 0x83,
    IID_BEACONBOW        = 0x84,
    IID_SIEGLINDE        = 0x85,
    IID_SIEGMUND         = 0x92,
};

struct RaiseProc
{
    PROC_HEADER;

    int idx;
    struct Unit * raised;
};

struct ItemGotProc
{
    /* 00 */ PROC_HEADER;
    /* 29 */ u8 pad[0x54 - 0x29];
    /* 54 */ struct Unit * unit;
    /* 58 */ u32 item;
};

int IsZombie(int pid)
{
    struct BwlEnt * bwl = BWL_GetEntry(pid);
    return bwl != NULL && bwl->loss_count != 0;
}

int IsZombieMode(void)
{
    return IsZombie(PID_EIRIKA) || IsZombie(PID_EPHRAIM);
}

// replace
void NuBattleUpdateBattleStats(struct BattleUnit * attacker, struct BattleUnit * defender)
{
    gBattleStats.attack = attacker->battle_attack;
    gBattleStats.defense = defender->battle_defense;
    gBattleStats.hit = attacker->battle_effective_hit;
    gBattleStats.crit = attacker->battle_effective_crit;
    gBattleStats.silencer_rate = attacker->battle_silencer_rate;

    if (IsZombie(defender->unit.pinfo->id) || IsZombieMode())
    {
        if (gBattleStats.hit != 0)
            gBattleStats.hit += (100 - gBattleStats.hit) / 2;
        gBattleStats.crit *= 3;
    }
}

int IsHolyWeapon(int item)
{
    u8 const list[] =
    {
        IID_SILVERSWORD,
        IID_SILVERLANCE,
        IID_SILVERAXE,
        IID_SILVERBOW,
        IID_SHADOWKILLER,
        IID_BRIGHTLANCE,
        IID_FIENDCLEAVER,
        IID_BEACONBOW,
        IID_SIEGLINDE,
        IID_SIEGMUND,
    };

    int iid = GetItemIid(item);

    for (u32 i = 0; i < (sizeof(list) / sizeof(*list)); i++)
        if (list[i] == iid)
            return TRUE;

    return FALSE;
}

struct BattleUnit * FindBattleUnit(int pid)
{
    if (gBattleUnitA.unit.pinfo != NULL && gBattleUnitA.unit.pinfo->id == pid)
        return &gBattleUnitA;

    if (gBattleUnitB.unit.pinfo != NULL && gBattleUnitB.unit.pinfo->id == pid)
        return &gBattleUnitB;

    return NULL;
}

int IsCrusader(struct Unit * opponent)
{
    struct BattleUnit * bu = FindBattleUnit(opponent->pinfo->id);

    if (bu == NULL)
        return FALSE;

    if (bu->weapon_kind == ITEM_KIND_LIGHT)
        return TRUE;

    if (IsHolyWeapon(bu->weapon_before))
        return TRUE;

    return FALSE;
}

// replace
void NuKillUnitOnCombatDeath(struct Unit * unit, struct Unit * opponent)
{
    if (GetUnitCurrentHp(unit) == 0)
    {
        BWL_SetDeathStats(unit->pinfo->id, opponent->pinfo->id, 2);

        if (!(UNIT_ATTRIBUTES(unit) & UNIT_ATTR_MONSTER) || IsCrusader(opponent))
        {
            KillUnit(unit);
        }
        else
        {
            unit->flags |= UNIT_FLAG_HIDDEN | UNIT_FLAG_DEAD;
        }
    }
}

void MakeRaiseTargets(void)
{
    InitTargets(0, 0);

    for (int i = 0x81; i < 0xC0; i++)
    {
        struct Unit * unit = GetUnit(i);

        if (unit == NULL || unit->pinfo == NULL)
            continue;

        if (unit->flags & UNIT_FLAG_DEAD)
            AddTarget(unit->x, unit->y, unit->id, 0);
    }
}

void RaiseInit(struct RaiseProc * proc)
{
    MakeRaiseTargets();
    proc->idx = 0;
    proc->raised = NULL;
}

void RaiseNext(struct RaiseProc * proc)
{
    if (proc->idx >= GetTargetListSize())
        Proc_Goto(proc, 1);

    proc->raised = GetUnit(GetTarget(proc->idx)->uid);
    EnsureCameraOntoPosition(proc, proc->raised->x, proc->raised->y);

    proc->idx++;
}

void RaiseAnim(struct RaiseProc * proc)
{
    struct Unit * unit = proc->raised;

    unit->hp = unit->max_hp;
    unit->flags &= ~(UNIT_FLAG_HIDDEN | UNIT_FLAG_DEAD);
    unit->level = 1;

    struct Vec2 pos;
    FindUnitClosestValidPosition(unit, unit->x, unit->y, &pos);
    unit->x = pos.x;
    unit->y = pos.y;

    New6C_SummonGfx(proc, unit->x, unit->y);
}

void RaiseTidy(struct RaiseProc * proc)
{
    RefreshUnitSprites();
}

// replace
struct ProcScr const ProcScr_RaiseLoop[] =
{
    PROC_CALL(RaiseInit),
    PROC_SLEEP(0),

PROC_LABEL(0),
    PROC_CALL(RaiseNext),
    PROC_SLEEP(0),

    PROC_CALL(RaiseAnim),
    PROC_SLEEP(0),

    PROC_CALL(RaiseTidy),
    PROC_SLEEP(0),

    PROC_GOTO(0),

PROC_LABEL(1),
    PROC_GOTO_SCR(gUnknown_0859BDA0), // replaced proc
};

void NuItemGot_DisplayLePopup(struct ItemGotProc * proc)
{
    int iid = GetItemIid(proc->item);

    switch (iid)
    {
        case IID_IRONSWORD: proc->item = CreateItem(IID_SHADOWKILLER); break;
        case IID_IRONLANCE: proc->item = CreateItem(IID_BRIGHTLANCE); break;
        case IID_IRONAXE: proc->item = CreateItem(IID_FIENDCLEAVER); break;
        case IID_IRONBOW: proc->item = CreateItem(IID_BEACONBOW); break;
    }

    NewGotItemPopup(proc->unit, proc->item, proc);
}
