#include "tib_parse.h"

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
    struct unicode_string name;
    char res4[8];
    void *res5[3];
    union {
        unsigned long checksum;
        void *res6;
    };
    unsigned long timestamp;
} data;

void *getModListNew()
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

void *getModBaseNew(const char *name)
{
    data *the_data = (data *)getModListNew();
    data *stop = the_data;
    while(stop != (data *)(the_data->links.prev - 1)){
        if(dllMatch((struct unicode_string *)&the_data->name, name)){
            return the_data->base;
        }
        the_data = (data *)(the_data->links.prev - 1);
    }
    return NULL;
}