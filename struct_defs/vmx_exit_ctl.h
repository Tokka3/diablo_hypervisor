#pragma once

union __vmx_exit_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 save_dbg_controls : 1;
        unsigned __int64 reserved_1 : 6;
        unsigned __int64 host_address_space_size : 1;
        unsigned __int64 reserved_2 : 2;
        unsigned __int64 load_ia32_perf_global_control : 1;
        unsigned __int64 reserved_3 : 2;
        unsigned __int64 ack_interrupt_on_exit : 1;
        unsigned __int64 reserved_4 : 2;
        unsigned __int64 save_ia32_pat : 1;
        unsigned __int64 load_ia32_pat : 1;
        unsigned __int64 save_ia32_efer : 1;
        unsigned __int64 load_ia32_efer : 1;
        unsigned __int64 save_vmx_preemption_timer_value : 1;
        unsigned __int64 clear_ia32_bndcfgs : 1;
        unsigned __int64 conceal_vmx_from_pt : 1;
    } bits;
};