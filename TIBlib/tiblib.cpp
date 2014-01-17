#include "tiblib.h"

#include <cstdio>

//on x86-64 gs stores the TIB rather than fs like on x86

void *getTib()
{
    void *tib;
    __asm__ __volatile__(
            "movq %%gs:0x30, %0"
            : "=r" (tib)
            :
            :
    );
    return tib;
}

void *getStackBase()
{
    void *base;
    __asm__ __volatile__(
            "movq %%gs:0x08, %0"
            : "=r" (base)
            :
            :
    );
    return base;
}

void *getStackLimit()
{
    void *limit;
    __asm__ __volatile__(
            "movq %%gs:0x10, %0"
            : "=r" (limit)
            :
            :
    );
    return limit;
}

int getProcessID()
{
    int pid;
    __asm__ __volatile__(
            "movl %%gs:0x40, %0"
            : "=r" (pid)
            :
            :
    );
    return pid;
}

int getThreadID()
{
    int tid;
    __asm__ __volatile__(
            "movl %%gs:0x48, %0"
            : "=r" (tid)
            :
            :
    );
    return tid;
}

void *getPeb()
{
    void *peb;
    __asm__ __volatile__(
            "movq %%gs:0x60, %0"
            : "=r" (peb)
            :
            :
    );
    return peb;
}

bool beingDebugged()
{
    unsigned char bugged;
    __asm__ __volatile__(
            "movq %%gs:0x60, %%rax;"
            "movb 0x02(%%rax), %0;"
            : "=r" (bugged)
            :
            : "rax"
    );
    return (bool) bugged;
}

void *getLdr()
{
    void *ldr;
    __asm__ __volatile__(
            "movq %%gs:0x60, %%rax;"
            "movq 0x18(%%rax), %0;"
            : "=r" (ldr)
            :
            : "rax"
    );
    return ldr;
}

void *getModList()
{
    void *modList;
    __asm__ __volatile__(
            "movq %%gs:0x60, %%rax;"
            "movq 0x18(%%rax), %%rbx;"
            "movq 0x18(%%rbx), %0;"
            : "=r" (modList)
            :
            : "rax", "rbx"
    );
    return modList;
}

// struct definitions curtesy of msdn

typedef struct _uni_string {
    unsigned short length;
    unsigned short max_length;
    wchar_t *buf;
} uni_string;

typedef struct _entry {
    struct _entry *next;
    struct _entry *prev;
} entry;

typedef struct _data {
    void *res1[2];
    entry links;
    void *res2[2];
    void *base;
    void *entry_point;
    void *res3;
    uni_string name;
    char res4[8];
    void *res5[3];
    union {
        unsigned long checksum;
        void *res6;
    };
    unsigned long timestamp;
} data;

bool matchUniString(uni_string *str, const wchar_t *str2)
{
    wchar_t buf[str->length+1];
    for(int i = 0; i < str->length; i++){
        buf[i] = str->buf[i];
    }
    buf[str->length] = L'\0';
    
    wchar_t *ita = buf;
    wchar_t *itb = (wchar_t *)str2;
    while((*ita != L'\0') && (*itb != L'\0')){
        if(*ita != *itb)
            return false;
        ita++; itb++;
    }
    
    return true;
}

void *getModBase(const wchar_t *name)
{
    data *the_data = (data *)getModList();
    data *stop = the_data;
    while(stop != (data *)(the_data->links.prev - 1)){
        if(matchUniString(&the_data->name, name)){
            return the_data->base;
        }
        the_data = (data *)(the_data->links.prev - 1);
    }
    return NULL;
}

void printUniString(uni_string *str)
{
    wchar_t buf[str->length+1];
    for(int i = 0; i < str->length; i++){
        buf[i] = str->buf[i];
    }
    buf[str->length] = L'\0';
    
    wprintf(L"module name: %ls", buf);
}

void printModList()
{
    data *the_data = (data *)getModList();
    data *stop = the_data;
    while(stop != (data *)(the_data->links.prev - 1)){
        printUniString(&the_data->name);
        puts("");
        the_data = (data *)(the_data->links.prev - 1);
    }
}