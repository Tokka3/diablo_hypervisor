#pragma once
#ifndef  VM_ENTRY_CTL
#define VM_ENTRY_CTL


union __vmx_entry_control_t
{
    unsigned __int64 all;
    struct
    {
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 load_dbg_controls : 1;
        unsigned __int64 reserved_1 : 6;
        unsigned __int64 ia32e_mode_guest : 1;
        unsigned __int64 entry_to_smm : 1;
        unsigned __int64 deactivate_dual_monitor_treament : 1;
        unsigned __int64 reserved_3 : 1;
        unsigned __int64 load_ia32_perf_global_control : 1;
        unsigned __int64 load_ia32_pat : 1;
        unsigned __int64 load_ia32_efer : 1;
        unsigned __int64 load_ia32_bndcfgs : 1;
        unsigned __int64 conceal_vmx_from_pt : 1;
        unsigned __int64 load_ia32_rtit_ctl : 1;
        unsigned __int64 load_cet_state : 1;
        unsigned __int64 load_pkrs : 1;
    }bits;
};
#endif // ! VM_ENTRY_CTL