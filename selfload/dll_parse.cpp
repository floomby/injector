#include "dll_parse.h"

void *getExportByName(void *base, const char *name)
{
    struct dos_header *DosHeader = (struct dos_header *)base;
    struct nt_header *NtHeader = (struct nt_header *)rva_to_offset(base, DosHeader->e_lfanew);
    
    struct export_directory *exp = (struct export_directory *)rva_to_offset(base, NtHeader->OptionalHeader.DataDirectory.Export.rva);
    
    uint32_t *names = (uint32_t *)rva_to_offset(base, exp->addr_names);
    uint32_t *funcs = (uint32_t *)rva_to_offset(base, exp->addr_funcs);
    
    //TODO replace linear search with bin search
    for(int k = 0; k < exp->num_names; k++){
        if((name != NULL) && match((char *)rva_to_offset(base, *names), name))
            return rva_to_offset(base, *funcs);

        names++;
        funcs++;
    }
    
    return NULL;
}

void *getExportByOrdinal(void *base, uint16_t ordinal)
{
    struct dos_header *DosHeader = (struct dos_header *)base;
    struct nt_header *NtHeader = (struct nt_header *)rva_to_offset(base, DosHeader->e_lfanew);
    
    struct export_directory *exp = (struct export_directory *)rva_to_offset(base, NtHeader->OptionalHeader.DataDirectory.Export.rva);
    
    if(ordinal >= exp->num_funcs){
        return NULL;
    }
    
    uint32_t *funcs = (uint32_t *)rva_to_offset(base, exp->addr_funcs);
    return rva_to_offset(base, funcs[ordinal]);
}

