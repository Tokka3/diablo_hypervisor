#pragma once
//
// Segment Selector Intrinsics
//

#ifndef ASM_DEFS
#define ASM_DEFS


unsigned short __read_ldtr(void);
unsigned short __read_tr(void);
unsigned short __read_cs(void);
unsigned short __read_ss(void);
unsigned short __read_ds(void);
unsigned short __read_es(void);
unsigned short __read_fs(void);
unsigned short __read_gs(void);
#endif // !ASM_DEFS