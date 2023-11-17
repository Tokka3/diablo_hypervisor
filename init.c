
#pragma warning (disable: 4996)
#pragma warning(disable: 4100)
#pragma warning(disable: 6273)
#pragma warning(disable: 4242)
#pragma warning(disable: 4244)
#pragma warning(disable: 28182)
#include "util/init.h"
#include "struct_defs/cpuid.h"
#include "struct_defs/feature_control.h"
#include "struct_defs/control_registers.h"
#include "struct_defs/vmx_basic.h"
#include "struct_defs/vcpu.h"
#include "struct_defs/vmx_misc.h"
#include "util/debug.h"

void init_logical_processor(struct __vmm_context_t* context, void* guest_rsp);

int check_vmx_support() {

	__cpuid_t cpuid = { 0 };
	__cpuid(cpuid.cpu_info, 1);
	return cpuid.feature_ecx.virtual_machine_extensions;
};


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

struct __vmm_context_t* allocate_vmm_context(void)
{
    struct __vmm_context_t* vmm_context = NULL;
    vmm_context = (struct __vmm_context_t*)ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vmm_context_t), VMM_TAG);
    if (vmm_context == NULL) {
       // log_error("Oops! vmm_context could not be allocated.\n");
        return NULL;
    }
    vmm_context->processor_count = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    vmm_context->vcpu_table = ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vcpu_t*) * vmm_context->processor_count, VMM_TAG);
    //
    // Allocate stack for vm-exit handlers and fill it with garbage
    // data.
    //
    vmm_context->stack = ExAllocatePool(NonPagedPool, VMM_STACK_SIZE);
    memset(vmm_context->stack, 0xCC, VMM_STACK_SIZE);
	/*log_success("vmm_context allocated at %llX\n", vmm_context);
	log_success("vcpu_table allocated at %llX\n", vmm_context->vcpu_table);
	log_debug("vmm stack allocated at %llX\n", vmm_context->stack);*/
    return vmm_context;
};
int init_vmcs(struct __vcpu_t* vcpu, void* guest_rsp, void (*guest_rip)(), int is_pt_allowed)
{

    struct __vmcs_t* vmcs;
    union __vmx_basic_msr_t vmx_basic = { 0 };
    PHYSICAL_ADDRESS physical_max;
    vmx_basic.control = __readmsr(IA32_VMX_BASIC_MSR);
    physical_max.QuadPart = ~0ULL;
    vcpu->vmcs = MmAllocateContiguousMemory(PAGE_SIZE, physical_max);
    vcpu->vmcs_physical = MmGetPhysicalAddress(vcpu->vmcs).QuadPart;
    RtlSecureZeroMemory(vcpu->vmcs, PAGE_SIZE);
    vmcs = vcpu->vmcs;
    vmcs->header.all = vmx_basic.bits.vmcs_revision_identifier;
    vmcs->header.bits.shadow_vmcs_indicator = 0;
    return TRUE;
}
struct __vcpu_t* init_vcpu(void)
{
    struct __vcpu_t* vcpu = NULL;
    vcpu = ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vcpu_t), VMM_TAG);
    if (!vcpu) {
       // log_error("Oops! vcpu could not be allocated.\n");
        return NULL;
    }
    RtlSecureZeroMemory(vcpu, sizeof(struct __vcpu_t));
    return vcpu;
}
void adjust_control_registers(void)
{
    union __cr4_t cr4 = { 0 };
    union __cr0_t cr0 = { 0 };
    union __cr_fixed_t cr_fixed;
    cr_fixed.all = __readmsr(IA32_VMX_CR0_FIXED0);
    cr0.control = __readcr0();
    cr0.control |= cr_fixed.split.low;
    cr_fixed.all = __readmsr(IA32_VMX_CR0_FIXED1);
    cr0.control &= cr_fixed.split.low;
    __writecr0(cr0.control);
    cr_fixed.all = __readmsr(IA32_VMX_CR4_FIXED0);
    cr4.control = __readcr4();
    cr4.control |= cr_fixed.split.low;
    cr_fixed.all = __readmsr(IA32_VMX_CR4_FIXED1);
    cr4.control &= cr_fixed.split.low;
    __writecr4(cr4.control);
}
int vmm_init(void)
{
    struct __vmm_context_t* vmm_context;
    PROCESSOR_NUMBER processor_number;
    GROUP_AFFINITY affinity, old_affinity;
   // KIRQL old_irql;
    vmm_context = allocate_vmm_context();
    for (unsigned iter = 0; iter < vmm_context->processor_count; iter++) {
        vmm_context->vcpu_table[iter] = init_vcpu();
        vmm_context->vcpu_table[iter]->vmm_context = vmm_context;
    }
    for (unsigned iter = 0; iter < vmm_context->processor_count; iter++) {
        //
        // Convert from an index to a processor number.
        //
        KeGetProcessorNumberFromIndex(iter, &processor_number);
        RtlSecureZeroMemory(&affinity, sizeof(GROUP_AFFINITY));
        affinity.Group = processor_number.Group;
        affinity.Mask = (KAFFINITY)1 << processor_number.Number;
        KeSetSystemGroupAffinityThread(&affinity, &old_affinity);
        init_logical_processor(vmm_context, 0);
        KeRevertToUserGroupAffinityThread(&old_affinity);
    }
    return TRUE;
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
        DbgPrintEx(0, 0, "VMXON for vcpu %d initialized:\n\t-> VA: %llX\n\t-> PA: %llX\n\t-> REV: %X\n",
        KeGetCurrentProcessorNumber(),
        vcpu->vmxon,
        vcpu->vmxon_physical,
        vcpu->vmxon->header.all);
    return TRUE;
}

void init_logical_processor(struct __vmm_context_t* context, void* guest_rsp)
{
    struct __vmm_context_t* vmm_context;
    struct __vcpu_t* vcpu;
    //union __vmx_misc_msr_t vmx_misc;
    unsigned long processor_number;
    processor_number = KeGetCurrentProcessorNumber();
    vmm_context = (struct __vmm_context_t*)context;
    vcpu = vmm_context->vcpu_table[processor_number];
    log_debug("vcpu %d guest_rsp = %llX\n", processor_number, guest_rsp);
    adjust_control_registers();
    if (!check_vmx_support()) {
        DbgPrintEx(0, 0, "VMX operation is not supported on this processor.\n");
        //free_vmm_context(vmm_context);
        return;
    }
    if (!init_vmxon(vcpu)) {
        DbgPrintEx(0, 0, "VMXON failed to initialize for vcpu %d.\n", processor_number);
       // free_vcpu(vcpu);
       // disable_vmx();
        return;
    }
    if (__vmx_on(&vcpu->vmxon_physical) != 0) {
        DbgPrintEx(0, 0, "Failed to put vcpu %d into VMX operation.\n", KeGetCurrentProcessorNumber());
      //  free_vcpu(vcpu);
      //  disable_vmx();
       // free_vmm_context(vmm_context);
        return;
    }
    DbgPrintEx(0, 0, "vcpu %d is now in VMX operation.\n", KeGetCurrentProcessorNumber());
}