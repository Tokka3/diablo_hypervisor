#include "../headers/includes.h"
#include "../headers/vmx.h"
#define IA32_VMX_BASIC 0x480
#define IA32_VMX_TRUE_ENTRY_CTLS 0x490
#define IA32_VMX_ENTRY_CTLS 0x484
#define IA32_VMX_TRUE_EXIT_CTLS 0x48F
#define IA32_VMX_EXIT_CTLS 0x483
 uintptr_t vmx_adjust_cv(unsigned int capability_msr, unsigned int value)
{
    union __vmx_true_control_settings_t cap;
    unsigned int actual;

    cap.control = __readmsr(capability_msr);
    actual = value;

    actual |= cap.allowed_0_settings;
    actual &= cap.allowed_1_settings;

    return actual;

}
 void vmx_adjust_entry_controls(union __vmx_entry_control_t* entry_controls)
{
    unsigned int capability_msr;
    union __vmx_basic_msr_t basic;

    basic.control = __readmsr(IA32_VMX_BASIC);
    capability_msr = (basic.bits.true_controls != FALSE) ? IA32_VMX_TRUE_ENTRY_CTLS : IA32_VMX_ENTRY_CTLS;

    entry_controls->control = (uintptr_t)vmx_adjust_cv(capability_msr, entry_controls->control);
}

 void vmx_adjust_exit_controls(union __vmx_exit_control_t* exit_controls)
{
    unsigned int capability_msr;
    union __vmx_basic_msr_t basic;
    basic.control = __readmsr(IA32_VMX_BASIC);
    capability_msr = (basic.bits.true_controls != FALSE) ? IA32_VMX_TRUE_EXIT_CTLS : IA32_VMX_EXIT_CTLS;
    exit_controls->control = (uintptr_t)vmx_adjust_cv(capability_msr, exit_controls->control);
}

 void vmx_adjust_pinbased_controls(union __vmx_pinbased_control_msr_t* pinbased_controls)
{
    unsigned int capability_msr;
    union __vmx_basic_msr_t basic;
    basic.control = __readmsr(IA32_VMX_BASIC);
    capability_msr = (basic.bits.true_controls != FALSE) ? IA32_VMX_TRUE_EXIT_CTLS : IA32_VMX_EXIT_CTLS;
    pinbased_controls->control = (uintptr_t)vmx_adjust_cv(capability_msr, pinbased_controls->control);
}

 void vmx_adjust_processor_based_controls(union __vmx_primary_processor_based_control_t* processor_based_controls) {

    unsigned int capability_msr;
    union __vmx_basic_msr_t basic;
    basic.control = __readmsr(IA32_VMX_BASIC);
    capability_msr = (basic.bits.true_controls != FALSE) ? IA32_VMX_TRUE_EXIT_CTLS : IA32_VMX_EXIT_CTLS;
    processor_based_controls->control = (uintptr_t)vmx_adjust_cv(capability_msr, processor_based_controls->control);
}

 void vmx_adjust_secondary_controls(union __vmx_secondary_processor_based_control_t* secondary_controls) {
    unsigned int capability_msr;
    union __vmx_basic_msr_t basic;
    basic.control = __readmsr(IA32_VMX_BASIC);
    capability_msr = (basic.bits.true_controls != FALSE) ? IA32_VMX_TRUE_EXIT_CTLS : IA32_VMX_EXIT_CTLS;
    secondary_controls->control = (uintptr_t)vmx_adjust_cv(capability_msr, secondary_controls->control);
}