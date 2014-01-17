#ifndef __TIBLIB__INCLUDED
#define __TIBLIB__INCLUDED

// Offsets were pulled from my version of winnt.h,
// msdn, wikipedia and dlls on my system
// If this does not work for you then let me know
// and I'll see what I can do to make it work

void *getStackBase();
void *getStackLimit();
void *getTib();

int getProcessID();
int getThreadID();

void *getPeb();
bool beingDebugged();

void *getLdr();
void *getModList();

void printModList();
void *getModBase(const wchar_t *name);
void *getModBase2(const char *name);

#endif//__TIBLIB__INCLUDED