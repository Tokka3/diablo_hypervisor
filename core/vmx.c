#include "../headers/includes.h"
#include "../headers/vmx.h"
#define IA32_VMX_BASIC 0x480
#define IA32_VMX_TRUE_ENTRY_CTLS 0x490
#define IA32_VMX_ENTRY_CTLS 0x484
static long long vmx_adjust_cv(unsigned int capability_msr, unsigned int value)
{
    union __vmx_true_control_settings_t cap;
    unsigned int actual;

    cap.control = __readmsr(capability_msr);
    actual = value;

    actual |= cap.allowed_0_settings;
    actual &= cap.allowed_1_settings;

    return actual;
}
static void vmx_adjust_entry_controls(union __vmx_entry_control_t* entry_controls)
{
    unsigned int capability_msr;
    union __vmx_basic_msr_t basic;

    basic.control = __readmsr(IA32_VMX_BASIC);
    capability_msr = (basic.bits.true_controls != FALSE) ? IA32_VMX_TRUE_ENTRY_CTLS : IA32_VMX_ENTRY_CTLS;

    entry_controls->control = vmx_adjust_cv(capability_msr, entry_controls->control);
}