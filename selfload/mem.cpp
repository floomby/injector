#include "mem.h"

void copy(void *dest, void *src, size_t bytes)
{
    for(int i = 0; i < bytes; i++){
        ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
    }
}