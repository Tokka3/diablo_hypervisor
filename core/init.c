
#include "../headers/includes.h"
#define CR4_VMX_ENABLE_BIT                                           13
void init_logical_processor(struct __vmm_context_t* context, void* guest_rsp);

int check_vmx_support() {

    __cpuid_t cpuid = { 0 };
    __cpuid(cpuid.cpu_info, 1);
    return cpuid.feature_ecx.virtual_machine_extensions;
};

struct __vmm_context_t* allocate_vmm_context(void)
{

    struct __vmm_context_t* vmm_context = NULL;
    vmm_context = (struct __vmm_context_t*)ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vmm_context_t), VMM_TAG);
    if (vmm_context == NULL) {
        Log("Oops! vmm_context could not be allocated.\n");
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

struct __vcpu_t* init_vcpu(void)
{
    struct __vcpu_t* vcpu = NULL;
    vcpu = ExAllocatePoolWithTag(NonPagedPool, sizeof(struct __vcpu_t), VMM_TAG);
    if (!vcpu) {
        Log("Oops! vcpu could not be allocated.\n");
        return NULL;
    }
    RtlSecureZeroMemory(vcpu, sizeof(struct __vcpu_t));
    //
    // Zero out msr bitmap so that no traps occur on MSR accesses
    // when in guest operation.
    //
    vcpu->msr_bitmap = ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, VMM_TAG);
    RtlSecureZeroMemory(vcpu->msr_bitmap, PAGE_SIZE);
    vcpu->msr_bitmap_physical = MmGetPhysicalAddress(vcpu->msr_bitmap).QuadPart;
    Log( "vcpu entry allocated successfully at %llX\n", vcpu);
    return vcpu;
}
void enable_vmx(void)
{
    unsigned long long cr4 = __readcr4();

    Log("vmxenable: %u\n", (cr4 >> CR4_VMX_ENABLE_BIT) & 0x01);

    cr4 |= (1 << CR4_VMX_ENABLE_BIT);

    __writecr4(cr4);
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
    Log("adjust_control_registers\n");
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
    Log("all vcpu's initiated \n ");
    for (unsigned iter = 0; iter < vmm_context->processor_count; iter++) {
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



void guest_entry_stub() {

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
    
    //Log("vcpu %d guest_rsp = %llX\n", processor_number, guest_rsp);

    enable_vmx(); // should have been doing this the whole time!!!!!

    adjust_control_registers();
    
    if (!check_vmx_support()) {
        Log("VMX operation is not supported on this processor.\n");
        //free_vmm_context(vmm_context);
        return;
    }
    if (!init_vmxon(vcpu, processor_number)) {
        Log("VMXON failed to initialize for vcpu %d.\n", processor_number);
        // free_vcpu(vcpu);
        // disable_vmx();
        return;
    }
    if (__vmx_on(&vcpu->vmxon_physical) != 0) {
        Log("Failed to put vcpu %d into VMX operation.\n", processor_number);
          //free_vcpu(vcpu);
        //  disable_vmx();
         // free_vmm_context(vmm_context);
        return;
    }
    Log("vcpu %d is now in VMX operation.\n", processor_number);
    
   init_vmcs(vcpu, guest_rsp, 0);
   
   
   unsigned char status = __vmx_vmlaunch();
    if (status != 0)
    {
        UINT64 vmx_error; 
        __vmx_vmread(VM_EXIT_VM_INSTRUCTION_ERROR, &vmx_error);
        Log("vmlaunch failed: %u \n", vmx_error);

        __vmx_off();
        // Some clean-up procedure
    }  
}
