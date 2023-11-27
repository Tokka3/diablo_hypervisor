
#ifndef MSR
#define MSR

#pragma warning (disable: 4201)

union __vmx_true_control_settings_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int32 allowed_0_settings;
        unsigned __int32 allowed_1_settings;
    };
};

#endif // !MSR