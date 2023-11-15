#pragma once

union __vmx_basic_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 vmcs_revision_identifier : 31;
        unsigned __int64 always_0 : 1;
        unsigned __int64 vmxon_region_size : 13;
        unsigned __int64 reserved_1 : 3;
        unsigned __int64 vmxon_physical_address_width : 1;
        unsigned __int64 dual_monitor_smi : 1;
        unsigned __int64 memory_type : 4;
        unsigned __int64 io_instruction_reporting : 1;
        unsigned __int64 true_controls : 1;
    } bits;
};