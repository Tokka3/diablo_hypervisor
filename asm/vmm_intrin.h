#pragma once
//
// Segment Selector Intrinsics
//

#ifndef ASM_DEFS
#define ASM_DEFS

void vmm_entrypoint(void);

unsigned short __read_ldtr(void);
unsigned short __read_tr(void);
unsigned short __read_cs(void);
unsigned short __read_ss(void);
unsigned short __read_ds(void);
unsigned short __read_es(void);
unsigned short __read_fs(void);
unsigned short __read_gs(void);
unsigned short __load_ar(unsigned long long rcx);
#endif // !ASM_DEFS