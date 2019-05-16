#pragma once

#include "mwseinstruction.h"

struct mwseUIShow : public mwseInstruction {
    mwseUIShow(TES3MACHINE& mach) : mwseInstruction(mach) {
        vptr = &vtable;
    }
    static __thiscall bool execute(mwseInstruction* _this);
private:
    static vtable_t vtable;
};

struct mwseUIHide : public mwseInstruction {
    mwseUIHide(TES3MACHINE& mach) : mwseInstruction(mach) {
        vptr = &vtable;
    }
    static __thiscall bool execute(mwseInstruction* _this);
private:
    static vtable_t vtable;
};
