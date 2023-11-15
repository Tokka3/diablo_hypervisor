#pragma once

union __vmx_misc_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 vmx_preemption_tsc_rate : 5;
        unsigned __int64 store_lma_in_vmentry_control : 1;
        unsigned __int64 activate_state_bitmap : 3;
        unsigned __int64 reserved_0 : 5;
        unsigned __int64 pt_in_vmx : 1;
        unsigned __int64 rdmsr_in_smm : 1;
        unsigned __int64 cr3_target_value_count : 9;
        unsigned __int64 max_msr_vmexit : 3;
        unsigned __int64 allow_smi_blocking : 1;
        unsigned __int64 vmwrite_to_any : 1;
        unsigned __int64 interrupt_mod : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 mseg_revision_identifier : 32;
    } bits;
};