#pragma once

union __vmx_secondary_processor_based_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 virtualize_apic_accesses : 1;
        unsigned __int64 enable_ept : 1;
        unsigned __int64 descriptor_table_exiting : 1;
        unsigned __int64 enable_rdtscp : 1;
        unsigned __int64 virtualize_x2apic : 1;
        unsigned __int64 enable_vpid : 1;
        unsigned __int64 wbinvd_exiting : 1;
        unsigned __int64 unrestricted_guest : 1;
        unsigned __int64 apic_register_virtualization : 1;
        unsigned __int64 virtual_interrupt_delivery : 1;
        unsigned __int64 pause_loop_exiting : 1;
        unsigned __int64 rdrand_exiting : 1;
        unsigned __int64 enable_invpcid : 1;
        unsigned __int64 enable_vmfunc : 1;
        unsigned __int64 vmcs_shadowing : 1;
        unsigned __int64 enable_encls_exiting : 1;
        unsigned __int64 rdseed_exiting : 1;
        unsigned __int64 enable_pml : 1;
        unsigned __int64 use_virtualization_exception : 1;
        unsigned __int64 conceal_vmx_from_pt : 1;
        unsigned __int64 enable_xsave_xrstor : 1;
        unsigned __int64 reserved_0 : 1;
        unsigned __int64 mode_based_execute_control_ept : 1;
        unsigned __int64 reserved_1 : 2;
        unsigned __int64 use_tsc_scaling : 1;
    } bits;
};