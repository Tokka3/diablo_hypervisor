#include "../headers/includes.h"


#define IA32_DEBUGCTL_MSR 0x1D9
#define IA32_SYSENTER_ESP_MSR 0x175
#define IA32_SYSENTER_EIP_MSR 0x176
#define IA32_SYSENTER_CS_MSR 0x174
#define IA32_FS_BASE_MSR 0xC0000100
#define IA32_GS_BASE_MSR 0xC0000101
int init_vmcs(struct __vcpu_t* vcpu, void* guest_rsp, /* void (*guest_rip)() ,*/ int is_pt_allowed)
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
  

    if (__vmx_vmclear(&vcpu->vmcs_physical) != VMX_OK || __vmx_vmptrld(&vcpu->vmcs_physical) != VMX_OK) {
        Log("Failed to clear or lead VMCS Physical Address\n");

    }

    unsigned __int64 vmm_stack = (unsigned __int64)&vcpu->vmm_stack.vmm_context;
    //  interval.QuadPart = -(10 * 1000 * 1000) * 2;

  

    // set up VMCS guest state with the proper data 

    if (__vmx_vmwrite(GUEST_CR0, __readcr0()) != 0) {

        Log("failed to write cr0");

    }
  
    if (__vmx_vmwrite(GUEST_CR3, __readcr3()) != 0) {
        Log("failed to write cr3");

    }
 
    if (__vmx_vmwrite(GUEST_CR4, __readcr4()) != 0) {
        Log("failed to write cr4");

    }

    if (__vmx_vmwrite(GUEST_DR7, __readdr(7)) != 0) {
        Log("failed to write dr7");

    }
 
    if (__vmx_vmwrite(GUEST_RSP, vcpu->guest_rsp) != 0) {
        Log("failed to write guest_rsp");

    }

 
    if (__vmx_vmwrite(GUEST_RIP, vcpu->guest_rip) != 0) {
        Log("failed to write guest_rip");

    }

    if (__vmx_vmwrite(GUEST_RFLAGS, __readeflags()) != 0) {
        Log("failed to write guest_rflags");

    }   
    if (__vmx_vmwrite(GUEST_DEBUG_CONTROL, __readmsr(IA32_DEBUGCTL_MSR)) != 0) {
        Log("failed to write guest_debug_control");

    }


    if (__vmx_vmwrite(GUEST_SYSENTER_ESP, __readmsr(IA32_SYSENTER_ESP_MSR)) != 0) {
        Log("failed to write guest_sysenter_esp");

    }
  

    if (__vmx_vmwrite(GUEST_SYSENTER_EIP, __readmsr(IA32_SYSENTER_EIP_MSR)) != 0) {
        Log("failed to write guest_sysenter EIP");

    }

    if (__vmx_vmwrite(GUEST_SYSENTER_CS, __readmsr(IA32_SYSENTER_CS_MSR)) != 0) {
        Log("failed to write guest_sysenter_cs");

    }


    if (__vmx_vmwrite(GUEST_VMCS_LINK_POINTER, MAXUINT64) != 0) {
        Log("failed to write VMCS_LINK_POINTER");

    }

    if (__vmx_vmwrite(GUEST_FS_BASE, __readmsr(IA32_FS_BASE_MSR)) != 0) {
        Log("failed to write guest_debug_control");

    }


    if (__vmx_vmwrite(GUEST_GS_BASE, __readmsr(IA32_GS_BASE_MSR)) != 0) {
        Log("failed to write guest_debug_control");

    }


    Log("setting read shadows");

    __vmx_vmwrite(CTRL_CR0_READ_SHADOW, __readcr0());
    __vmx_vmwrite(CTRL_CR4_READ_SHADOW, __readcr4());



    __vmx_vmwrite(HOST_RSP, vmm_stack);

    Log("setting up stack \n");

    union __vmx_entry_control_t entry_controls;

    entry_controls.control = 0;
    entry_controls.bits.ia32e_mode_guest = TRUE;

    union __vmx_exit_control_t exit_controls;
    //
    // Zero the control value, set address space size, save and load IA32_EFER.
    //
    exit_controls.control = 0;
    exit_controls.bits.host_address_space_size = TRUE;


    vmx_adjust_exit_controls(&exit_controls);

    vmx_adjust_entry_controls(&entry_controls);

    union __vmx_pinbased_control_msr_t pinbased_controls;
    pinbased_controls.control = 0;
    vmx_adjust_pinbased_controls(&pinbased_controls);

    union __vmx_primary_processor_based_control_t primary_controls;
    primary_controls.control = 0;
    primary_controls.bits.use_msr_bitmaps = TRUE;
    primary_controls.bits.active_secondary_controls = TRUE;
    vmx_adjust_processor_based_controls(&primary_controls);

    union __vmx_secondary_processor_based_control_t secondary_controls;
    secondary_controls.control = 0;
    secondary_controls.bits.enable_rdtscp = TRUE;
    secondary_controls.bits.enable_xsave_xrstor = TRUE;
    secondary_controls.bits.enable_invpcid = TRUE;

    vmx_adjust_secondary_controls(&secondary_controls);

    if (__vmx_vmwrite(CTRL_PIN_BASED_VM_EXECUTION_CONTROLS, pinbased_controls.control) != 0)
    {
        Log("failed to write pin_based \n");

    }

    if (__vmx_vmwrite(CTRL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, primary_controls.control) != 0) {
        Log("failed to write primary_processor_based \n");

    }

    if (__vmx_vmwrite(CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, secondary_controls.control) != 0)
    {
        Log("failed to write secondary processor based \n");
    }

    if (__vmx_vmwrite(CTRL_PRIMARY_VM_EXIT_CONTROLS, exit_controls.control) != 0)
    {
        Log("failed to write primary vm exit ctrls \n");

    }

    if (__vmx_vmwrite(CTRL_VM_ENTRY_CONTROLS, entry_controls.control) != 0)
    {
        Log("failed to write vm entry ctrls \n");

    }
    if (__vmx_vmwrite(HOST_RIP, (UINT64)vmm_entrypoint) != 0)
    {
        Log("failed to write vmm entry point \n");

    }



    __vmx_vmwrite(GUEST_CS_SELECTOR, __read_cs());
    __vmx_vmwrite(GUEST_SS_SELECTOR, __read_ss());
    __vmx_vmwrite(GUEST_DS_SELECTOR, __read_ds());
    __vmx_vmwrite(GUEST_ES_SELECTOR, __read_es());
    __vmx_vmwrite(GUEST_FS_SELECTOR, __read_fs());
    __vmx_vmwrite(GUEST_GS_SELECTOR, __read_gs());
    __vmx_vmwrite(GUEST_LDTR_SELECTOR, __read_ldtr());
    __vmx_vmwrite(GUEST_TR_SELECTOR, __read_tr());


    struct __pseudo_descriptor_64_t gdtr;
    struct __pseudo_descriptor_64_t idtr;

    _sgdt(&gdtr);
    __sidt(&idtr);

    __vmx_vmwrite(GUEST_GDTR_LIMIT, gdtr.limit);
    __vmx_vmwrite(GUEST_IDTR_LIMIT, idtr.limit);

    __vmx_vmwrite(GUEST_GDTR_BASE, gdtr.base_address);
    __vmx_vmwrite(GUEST_IDTR_BASE, idtr.base_address);


    __vmx_vmwrite(GUEST_CS_ACCESS_RIGHTS, read_segment_access_rights(__read_cs()));
    __vmx_vmwrite(GUEST_SS_ACCESS_RIGHTS, read_segment_access_rights(__read_ss()));
    __vmx_vmwrite(GUEST_DS_ACCESS_RIGHTS, read_segment_access_rights(__read_ds()));
    __vmx_vmwrite(GUEST_ES_ACCESS_RIGHTS, read_segment_access_rights(__read_es()));
    __vmx_vmwrite(GUEST_FS_ACCESS_RIGHTS, read_segment_access_rights(__read_fs()));
    __vmx_vmwrite(GUEST_GS_ACCESS_RIGHTS, read_segment_access_rights(__read_gs()));
    __vmx_vmwrite(GUEST_LDTR_ACCESS_RIGHTS, read_segment_access_rights(__read_ldtr()));
    __vmx_vmwrite(GUEST_TR_ACCESS_RIGHTS, read_segment_access_rights(__read_tr()));

    __vmx_vmwrite(GUEST_LDTR_BASE, get_segment_base(gdtr.base_address, __read_ldtr()));
    __vmx_vmwrite(GUEST_TR_BASE, get_segment_base(gdtr.base_address, __read_tr()));


    __vmx_vmwrite(HOST_CS_SELECTOR, __read_cs() & ~selector_mask);
    __vmx_vmwrite(HOST_SS_SELECTOR, __read_ss() & ~selector_mask);
    __vmx_vmwrite(HOST_DS_SELECTOR, __read_ds() & ~selector_mask);
    __vmx_vmwrite(HOST_ES_SELECTOR, __read_es() & ~selector_mask);
    __vmx_vmwrite(HOST_FS_SELECTOR, __read_fs() & ~selector_mask);
    __vmx_vmwrite(HOST_GS_SELECTOR, __read_gs() & ~selector_mask);
    __vmx_vmwrite(HOST_TR_SELECTOR, __read_tr() & ~selector_mask);


    __vmx_vmwrite(HOST_TR_BASE, get_segment_base(gdtr.base_address, __read_tr()));
    __vmx_vmwrite(HOST_GS_BASE, get_segment_base(gdtr.base_address, __read_gs()));
    __vmx_vmwrite(HOST_FS_BASE, get_segment_base(gdtr.base_address, __read_fs()));
    __vmx_vmwrite(HOST_GDTR_BASE, gdtr.base_address);
    __vmx_vmwrite(HOST_IDTR_BASE, idtr.base_address);


    Log("vmcs initiated \n");




    return TRUE;
}