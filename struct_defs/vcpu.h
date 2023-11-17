#pragma once
#include "vmcs.h"
#include "vmm_context.h"
struct __vcpu_t
{
    struct __vmcs_t* vmcs;
    unsigned __int64 vmcs_physical;
    struct __vmcs_t* vmxon;
    unsigned __int64 vmxon_physical;
    void* msr_bitmap;
    unsigned __int64 msr_bitmap_physical;
	struct __vmm_context_t* vmm_context;
};