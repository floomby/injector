#ifndef STR_H_INCLUDED
#define STR_H_INCLUDED

#include <cstdlib>
#include <cstdint>

struct unicode_string {
    unsigned short length;
    unsigned short max_length;
    wchar_t *buf;
};

bool dllMatch(struct unicode_string *str, const char *str2);

#endif//STR_H_INCULDED