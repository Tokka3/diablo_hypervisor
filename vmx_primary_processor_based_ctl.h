#pragma once

union __vmx_primary_processor_based_control_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 interrupt_window_exiting : 1;
        unsigned __int64 use_tsc_offsetting : 1;
        unsigned __int64 reserved_1 : 3;
        unsigned __int64 hlt_exiting : 1;
        unsigned __int64 reserved_2 : 1;
        unsigned __int64 invldpg_exiting : 1;
        unsigned __int64 mwait_exiting : 1;
        unsigned __int64 rdpmc_exiting : 1;
        unsigned __int64 rdtsc_exiting : 1;
        unsigned __int64 reserved_3 : 2;
        unsigned __int64 cr3_load_exiting : 1;
        unsigned __int64 cr3_store_exiting : 1;
        unsigned __int64 reserved_4 : 2;
        unsigned __int64 cr8_load_exiting : 1;
        unsigned __int64 cr8_store_exiting : 1;
        unsigned __int64 use_tpr_shadow : 1;
        unsigned __int64 nmi_window_exiting : 1;
        unsigned __int64 mov_dr_exiting : 1;
        unsigned __int64 unconditional_io_exiting : 1;
        unsigned __int64 use_io_bitmaps : 1;
        unsigned __int64 reserved_5 : 1;
        unsigned __int64 monitor_trap_flag : 1;
        unsigned __int64 use_msr_bitmaps : 1;
        unsigned __int64 monitor_exiting : 1;
        unsigned __int64 pause_exiting : 1;
        unsigned __int64 active_secondary_controls : 1;
    } bits;
};