#include "pe_things.h"
#include <cstdint>

#include "tiblib.h"
#include "str.h"

#include <windows.h>

#include <cstdio>

#define realloc_type(x) ((x & 0xF000) >> 12)

#define IMAGE_REL_BASED_ABSOLUTE 0
#define IMAGE_REL_BASED_HIGH 1
#define IMAGE_REL_BASED_LOW 2
#define IMAGE_REL_BASED_HIGHLOW 3
#define IMAGE_REL_BASED_HIGHADJ 4
#define IMAGE_REL_BASED_DIR64 10

#define realloc_loc(x) (x & 0x0FFF)

struct section_header {
    char name[8];
    uint32_t size;
    uint32_t addr;
    uint32_t size_of_raw;
    uint32_t pointer_to_raw;
    uint32_t pointer_to_relocations;
    uint32_t pointer_to_line_numbers;
    uint16_t num_of_relocations;
    uint16_t num_of_line_numbers;
    uint32_t charicteristics;
};

struct import_helper {
    uint32_t orig_thunk;
    uint32_t timestamp;
    uint32_t forwarder_chain;
    uint32_t name;
    uint32_t first_thunk;
};

struct import_entry {
    uint64_t oft;
    uint64_t iat;
    uint16_t hint;
};

struct export_helper {
    uint32_t chariteristics;
    uint32_t timestamp;
    uint16_t major_ver;
    uint16_t minor_ver;
    uint32_t name;
    uint32_t base;
    uint32_t num_funcs;
    uint32_t num_names;
    uint32_t addr_funcs;
    uint32_t addr_names;
    uint32_t addr_ords;
};

static void *base;

#define SEARCH_PATH "C:\\Windows\\System32\\"

#define ordinal(x)          (x & 0x8000000000000000)
#define ordinal_number(x)   ((x & 0x7FFF80000000000) >> 47)
#define hint_rva(x)         ((uint32_t)((x & 0x000007FFFFFFF)))

void load(void *dll)
{
    uint32_t image_size = *(uint32_t *)((uint8_t *)dll + 0xD0);

    //printf("image size is %x\n", image_size);
    
    base = VirtualAlloc(NULL, image_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    
    uint16_t section_count = *(uint16_t *)((uint8_t *)dll + 0x86);
    
    //printf("there are %d sections\n", section_count);
    
    // we know that the section header imediatly follow the file headers
    // this give us an offset of 0x1f8
    
    struct section_header *sections = (struct section_header *)((uint8_t *)dll + 0x188);
    
    for(int i = 0; i < section_count; i++){
        /*printf("name:%c%c%c%c%c%c%c%c size:%x raw size:%x raw pointer:%x virtual address:%x\n",
                    sections[i].name[0],
                    sections[i].name[1],
                    sections[i].name[2],
                    sections[i].name[3],
                    sections[i].name[4],
                    sections[i].name[5],
                    sections[i].name[6],
                    sections[i].name[7],
                    sections[i].size,
                    sections[i].size_of_raw,
                    sections[i].pointer_to_raw,
                    sections[i].addr
                    );*/
        
        fflush(stdout);
        memcpy((uint8_t *)base + sections[i].addr, (uint8_t *)dll + sections[i].pointer_to_raw, sections[i].size_of_raw);   
    }
    
    uint32_t image_base = *(uint32_t *)((uint8_t *)dll + 0xB0);
    
    uint32_t realloc_rva = *(uint32_t *)((uint8_t *)dll + 0x130);
    uint32_t realloc_size = *(uint32_t *)((uint8_t *)dll + 0x134);
    
    uint8_t *it = (uint8_t *)base + realloc_rva;
    
    while(it < (uint8_t *)base + realloc_rva + realloc_size){
        uint32_t page_rva = *(uint32_t *)it;
        uint32_t block_count = *(uint32_t *)(it + 4);
        
        printf("rva %x -- count %x\n", page_rva, (block_count - 8)/2);
        
        for(int i = 0; i < (block_count - 8) / 2; i += 2){
            uint16_t tmp = *(uint16_t *)(it + i + 8);
        
            if(realloc_type(tmp) != 0xa){
                printf("relocation type %hd not supported\n", realloc_type(tmp));
                exit(EXIT_FAILURE);
            }

            uint64_t rva = page_rva + realloc_loc(tmp);
            *(uint64_t *)((uint8_t *)base + rva) += (uint64_t)base - image_base;
        }

        it += block_count;
    }
    
    uint32_t import_rva = *(uint32_t *)((uint8_t *)dll + 0x110);
    uint32_t import_size = *(uint32_t *)((uint8_t *)dll + 0x114);
    
    // printf("the import dir is at %x and is %x big\n", import_rva, import_size);
    
    // NOTE: only loads ascii dlls
    
    int imp_count = 0;
    struct import_helper *imp = (struct import_helper *)((uint8_t *)base + import_rva);
    while(imp->orig_thunk){
        imp++;
        imp_count++;
    }
    
    uint64_t *imp_entry_table = (uint64_t *)((uint8_t *)base + import_rva + sizeof(struct import_helper) * (imp_count + 1));
    
    uint64_t *imp_symbol_location;
    while(*imp_entry_table){
        imp_entry_table++;
    }
    
    imp_symbol_location = imp_entry_table + 1;
    
    imp_entry_table = (uint64_t *)((uint8_t *)base + import_rva + sizeof(struct import_helper) * (imp_count + 1));
    
    imp = (struct import_helper *)((uint8_t *)base + import_rva);
    
    uint64_t *thunk = (uint64_t *)((uint8_t *)base + imp->first_thunk);
    
    while(imp->orig_thunk){
        printf("imports from %s\n", (char *)((uint8_t *)base + imp->name));
        
        void *mod_base = getModBase2((char *)((uint8_t *)base + imp->name));
        // printf("the module base is %p\n", mod_base);
        
        // untested block (pretty sure it will work though)
        if(mod_base == NULL){
            char *name = (char *)((uint8_t *)base + imp->name);
            char buf[strlen(SEARCH_PATH) + strlen(name) + 1];
            strcpy(buf, SEARCH_PATH);
            strcat(buf, name);

            LoadLibraryA(buf);
            mod_base = getModBase2((char *)((uint8_t *)base + imp->name));
        }
        
        uint32_t mod_export_rva = *(uint32_t *)((int8_t *)mod_base + 0x180);
        uint32_t mod_export_size = *(uint32_t *)((int8_t *)mod_base + 0x184);
        
        // printf("the module export rva is % x and size is %x\n", mod_export_rva, mod_export_size);        
        
        struct export_helper *exp  = (struct export_helper *)((int8_t *)mod_base + mod_export_rva);
        
        // printf("the module exports %x functions and %x names\n", exp->num_funcs, exp->num_names);
        // printf("funcs at %x names at %x ords at %x\n", exp->addr_funcs, exp->addr_names, exp->addr_ords);
        
        while(*imp_entry_table){
            uint32_t *names = (uint32_t *)((int8_t *)mod_base + exp->addr_names);
            uint32_t *funcs = (uint32_t *)((int8_t *)mod_base + exp->addr_funcs);
            uint16_t *ords = (uint16_t *)((int8_t *)mod_base + exp->addr_ords);
        
            uint32_t found_rva;
            
            //untested block
            if(ordinal(*imp_entry_table)){
                found_rva = *(funcs + ordinal_number(*imp_entry_table));
                continue;
            }
            
            char *imp_name = (char *)((uint8_t *)base + hint_rva(*imp_entry_table) + 2);
            
            for(int k = 0; k < exp->num_names; k++){
                char *name = (char *)((uint8_t *)mod_base + *names);
                
                if((name != NULL) && !strcmp(name, imp_name)){
                    printf("%s has an rva of %x\n", imp_name, *funcs);
                    found_rva = *funcs;
                    break;
                }

                names++;
                funcs++;
                ords++;
            }
            
            *thunk = (uint64_t)((uint8_t *)mod_base + found_rva);
            printf("this puts thunk at %p\n", (void *)*thunk);
            thunk++;
            
            imp_entry_table++;
        }
        
        imp_entry_table++;
        imp++;
    }
    
    // printf("entry rva %x\n", *(uint32_t *)((uint8_t *)dll + 0xA8));
    fflush(stdout);
    
    BOOL APIENTRY (*entry_fp)(HINSTANCE, DWORD, LPVOID) = (BOOL APIENTRY (*)(HINSTANCE, DWORD, LPVOID))((uint8_t *)base + *(uint32_t *)((uint8_t *)dll + 0xA8));
    entry_fp(NULL, DLL_PROCESS_ATTACH, NULL);
    
    // printf("sleep at %p\n", GetProcAddress(GetModuleHandle("kernel32.dll"), "Sleep"));
}