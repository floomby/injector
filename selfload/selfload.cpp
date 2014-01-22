#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <windows.h>

#include "dll_load.h"

void *load_to_mem(const char *name)
{
    FILE *fp = fopen(name, "rb");
    assert(fp);
    
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);
    
    void *ret = malloc(size);
    assert(ret);
    
    fread(ret, 1, size, fp);
    
    fclose(fp);
    
    return ret;
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        fprintf(stderr, "usage: %s <dll_name>\n", argv[0]);
        return 1;
    }
    
    void *buf = load_to_mem(argv[1]);
    
    load2(buf);
    
    free(buf);
    
    return 0;
}