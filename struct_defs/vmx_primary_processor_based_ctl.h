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

union __exception_bitmap_t
{
    unsigned __int32 all;
    struct
    {
        unsigned __int32 divide_error : 1;
        unsigned __int32 debug : 1;
        unsigned __int32 nmi_interrupt : 1;
        unsigned __int32 breakpoint : 1;
        unsigned __int32 overflow : 1;
        unsigned __int32 bound : 1;
        unsigned __int32 invalid_opcode : 1;
        unsigned __int32 device_not_available : 1;
        unsigned __int32 double_fault : 1;
        unsigned __int32 coprocessor_segment_overrun : 1;
        unsigned __int32 invalid_tss : 1;
        unsigned __int32 segment_not_present : 1;
        unsigned __int32 stack_segment_fault : 1;
        unsigned __int32 general_protection : 1;
        unsigned __int32 page_fault : 1;
        unsigned __int32 x87_floating_point_error : 1;
        unsigned __int32 alignment_check : 1;
        unsigned __int32 machine_check : 1;
        unsigned __int32 simd_floating_point_error : 1;
        unsigned __int32 virtualization_exception : 1;
    }bits;
};