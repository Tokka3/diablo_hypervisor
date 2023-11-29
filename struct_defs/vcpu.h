#pragma once
#include "vmcs_.h"
#include "vmm_context.h"

#ifndef VCPU_H
#define VCPU_H
struct __vcpu_t
{
    struct __vmm_context_t* vmm_context;

    unsigned __int64 guest_rsp;
    unsigned __int64 guest_rip;

    struct __vmcs_t* vmcs;
    unsigned __int64 vmcs_physical;

    struct __vmcs_t* vmxon;
    unsigned __int64 vmxon_physical;

    void* msr_bitmap;
    unsigned __int64 msr_bitmap_physical;

    __declspec(align(4096)) struct __vmm_stack_t vmm_stack;
};
#endif