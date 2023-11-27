#pragma once 

#ifndef CONTROL_REGISTERS
#define CONTROL_REGISTERS

#define IA32_VMX_CR0_FIXED0 0x486
#define IA32_VMX_CR0_FIXED1 0x487

#define IA32_VMX_CR4_FIXED0 0x488
#define IA32_VMX_CR4_FIXED1 0x489
union __cr0_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 protection_enable : 1;
        unsigned __int64 monitor_coprocessor : 1;
        unsigned __int64 emulate_fpu : 1;
        unsigned __int64 task_switched : 1;
        unsigned __int64 extension_type : 1;
        unsigned __int64 numeric_error : 1;
        unsigned __int64 reserved_0 : 10;
        unsigned __int64 write_protection : 1;
        unsigned __int64 reserved_1 : 1;
        unsigned __int64 alignment_mask : 1;
        unsigned __int64 reserved_2 : 10;
        unsigned __int64 not_write_through : 1;
        unsigned __int64 cache_disable : 1;
        unsigned __int64 paging_enable : 1;
    } bits;
};
union __cr3_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 pcid : 12;
        unsigned __int64 page_frame_number : 36;
        unsigned __int64 reserved_0 : 12;
        unsigned __int64 reserved_1 : 3;
        unsigned __int64 pcid_invalidate : 1;
    } bits;
};
union __cr4_t {
    unsigned long long control; // Representing the entire 64-bit CR4 register.
    struct {
        unsigned long long vme : 1;         // Virtual-8086 Mode Extensions
        unsigned long long pvi : 1;         // Protected-Mode Virtual Interrupts
        unsigned long long tsd : 1;         // Time Stamp Disable
        unsigned long long de : 1;         // Debugging Extensions
        unsigned long long pse : 1;         // Page Size Extensions
        unsigned long long pae : 1;         // Physical Address Extension
        unsigned long long mce : 1;         // Machine-Check Exception
        unsigned long long pge : 1;         // Page Global Enable
        unsigned long long pce : 1;         // Performance-Monitoring Counter Enable
        unsigned long long osfxsr : 1;      // OS Support for FXSAVE and FXRSTOR instructions
        unsigned long long osxmmexcpt : 1;  // OS Support for Unmasked SIMD Floating-Point Exceptions
        unsigned long long umip : 1;        // User-Mode Instruction Prevention
        unsigned long long la57 : 1;        // 57-bit Linear Addresses
        unsigned long long vmxe : 1;        // VMX-Enable Bit
        unsigned long long smxe : 1;        // SMX-Enable Bit
        unsigned long long fsgsbase : 1;    // Enable RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE instructions
        unsigned long long pcide : 1;       // PCID Enable
        unsigned long long osxsave : 1;     // XSAVE and Processor Extended States Enable
        unsigned long long reserved1 : 1;   // Reserved, should be zero
        unsigned long long smep : 1;        // Supervisor Mode Execution Protection Enable
        unsigned long long smap : 1;        // Supervisor Mode Access Prevention Enable
        unsigned long long pke : 1;         // Protection Key Enable
        unsigned long long cet : 1;         // Control-flow Enforcement Technology
        unsigned long long pks : 1;         // Protection Keys for Supervisor-mode Pages
        unsigned long long reserved2 : 42;  // Reserved bits, should be zero
    } bits;
};
union __cr8_t
{
    unsigned __int64 control;
    struct
    {
        unsigned __int64 task_priority_level : 4;
        unsigned __int64 reserved : 59;
    } bits;
};

union __cr_fixed_t
{
    struct
    {
        unsigned long low;
        long high;
    } split;
    struct
    {
        unsigned long low;
        long high;
    } u;
    long long all;
};


#endif