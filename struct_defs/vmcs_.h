#pragma once

#ifndef VMCS_STRUCT
#define VMCS_STRUCT



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
#endif 