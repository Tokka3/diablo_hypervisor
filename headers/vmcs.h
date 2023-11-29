
#ifndef VMCS_2_H
#define VMCS_2_H

#pragma once


struct __vmcs_t
{
    union
    {
        unsigned int all;
        struct
        {
            unsigned int revision_identifier : 31;
            unsigned int shadow_vmcs_indicator : 1;
        } bits;
    } header;
    unsigned int abort_indicator;
    char data[0x1000 - 2 * sizeof(unsigned)];
};

int init_vmcs(struct __vcpu_t* vcpu, void* guest_rsp, /*void (*guest_rip)(), */ int is_pt_allowed);
#endif // ! VMCS_H
