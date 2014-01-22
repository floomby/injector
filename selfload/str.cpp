#include "str.h"

size_t uniStrLen(struct unicode_string *str)
{
    return str->length + ((*(str->buf + str->length) == L'\0')?(0):(1));
}

void uniToAscii(char *buf, struct unicode_string *str, size_t len)
{
    for(int i = 0; i < (int)len - 1; i++){
        buf[i] = (char)str->buf[i];
    }
    
    buf[len - 1] = '\0';
}

char *lastSlash(char *str)
{
    char *it = str;
    
    while(*str){
        if(*str == '\\')
            it = str;
        str++;
    }
    
    return it;
}

#define is_upper(x) ((0x40 < x) && (x < 0x5B))
#define to_lower(x) (x + 0x20)

bool match(const char *str1, const char *str2)
{
    char *ita = (char *)str1;
    char *itb = (char *)str2;

    while(*ita || *itb){
        if(!(*ita))     return false;
        if(!(*itb))     return false;
    
        char a = *ita;
        char b = *itb;
        
        if(is_upper(a)) a = to_lower(a);
        if(is_upper(b)) b = to_lower(b);
        
        if(!(a == b))   return false;
        
        ita++;
        itb++;
    }
    
    return true;
}

bool dllMatch(struct unicode_string *str, const char *str2)
{
    size_t len = uniStrLen(str);
    char buf[len];

    uniToAscii(buf, str, len);
    
    return match(lastSlash(buf) + 1, str2);
}