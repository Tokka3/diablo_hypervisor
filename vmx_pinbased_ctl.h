#pragma once

union __vmx_pinbased_control_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 external_interrupt_exiting : 1;
        unsigned __int64 reserved_0 : 2;
        unsigned __int64 nmi_exiting : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 virtual_nmis : 1;
        unsigned __int64 vmx_preemption_timer : 1;
        unsigned __int64 process_posted_interrupts : 1;
    } bits;
};