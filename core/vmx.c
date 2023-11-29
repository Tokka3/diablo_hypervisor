#include "../headers/includes.h"
#include "../headers/vmx.h"
#define IA32_VMX_BASIC 0x480
#define IA32_VMX_TRUE_ENTRY_CTLS 0x490
#define IA32_VMX_ENTRY_CTLS 0x484
#define IA32_VMX_TRUE_EXIT_CTLS 0x48F
#define IA32_VMX_EXIT_CTLS 0x483


int enable_vmx_operation(void)
{
    union __cr4_t cr4 = { 0 };
    union __ia32_feature_control_msr_t feature_msr = { 0 };
    cr4.control = __readcr4();
    cr4.bits.vmxe = 1;
    __writecr4(cr4.control);
    feature_msr.control = __readmsr(IA32_FEATURE_CONTROL);

    if (feature_msr.bits.lock == 0) {
        feature_msr.bits.vmxon_outside_smx = 1;
        feature_msr.bits.lock = 1;

        __writemsr(IA32_FEATURE_CONTROL, feature_msr.control);
        return TRUE;
    }
    return FALSE;
}

int init_vmxon(struct __vcpu_t* vcpu)
{
    union __vmx_basic_msr_t vmx_basic = { 0 };
    struct __vmcs_t* vmxon;
    PHYSICAL_ADDRESS physical_max;
    if (!vcpu) {
        //log_error("VMXON region could not be initialized. vcpu was null.\n");
        return FALSE;
    }
    vmx_basic.control = __readmsr(IA32_VMX_BASIC_MSR);
    physical_max.QuadPart = ~0ULL;
    vcpu->vmxon = MmAllocateContiguousMemory(PAGE_SIZE, physical_max);
    vcpu->vmxon_physical = MmGetPhysicalAddress(vcpu->vmxon).QuadPart;
    vmxon = vcpu->vmxon;
    RtlSecureZeroMemory(vmxon, PAGE_SIZE);
    vmxon->header.all = vmx_basic.bits.vmcs_revision_identifier;
    Log("VMXON for vcpu %d initialized:\n\t-> VA: %llX\n\t-> PA: %llX\n\t-> REV: %X\n",
        KeGetCurrentProcessorNumber(),
        vcpu->vmxon,
        vcpu->vmxon_physical,
        vcpu->vmxon->header.all);
    return TRUE;
}

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

