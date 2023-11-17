#pragma once

#define VMM_STACK_SIZE (PAGE_SIZE * 8)
#define VMM_TAG 0x123


struct __vmm_context_t
{
   
    void* stack;
    unsigned long processor_count;
    __declspec(align(4096)) struct __vcpu_t** vcpu_table;
    __declspec(align(4096)) void* msr_bitmap;
   
};

struct __vmm_stack_t
{

    unsigned char limit[VMM_STACK_SIZE - sizeof(struct __vmm_context_t)];
    struct __vmm_context_t vmm_context;
   
};
