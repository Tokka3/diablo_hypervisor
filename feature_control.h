#pragma once

union __ia32_feature_control_msr_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 lock : 1;
        unsigned __int64 vmxon_inside_smx : 1;
        unsigned __int64 vmxon_outside_smx : 1;
        unsigned __int64 reserved_0 : 5;
        unsigned __int64 senter_local : 6;
        unsigned __int64 senter_global : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 sgx_launch_control_enable : 1;
        unsigned __int64 sgx_global_enable : 1;
        unsigned __int64 reserved_2 : 1;
        unsigned __int64 lmce : 1;
        unsigned __int64 system_reserved : 42;
    } bits;
};