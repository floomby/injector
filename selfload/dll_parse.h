#ifndef DLL_PARSE_H_INCLUDED
#define DLL_PARSE_H_INCLUDED

#include "pe_structs.h"
#include "str.h"

void *getExportByName(void *base, const char *name);
void *getExportByOrdinal(void *base, uint16_t ordinal);

#endif//DLL_PARSE_H_INCLUDED