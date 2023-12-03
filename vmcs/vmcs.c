#include "../headers/includes.h"
#include "../headers/ia32_defs.h"
#include "../asm/vm_exit.h"


#define IA32_DEBUGCTL_MSR 0x1D9
#define IA32_SYSENTER_ESP_MSR 0x175
#define IA32_SYSENTER_EIP_MSR 0x176
#define IA32_SYSENTER_CS_MSR 0x174
#define IA32_FS_BASE_MSR 0xC0000100
#define IA32_GS_BASE_MSR 0xC0000101
union __msr
{
    unsigned __int64 all;
    struct
    {
        unsigned __int32 low;
        unsigned __int32 high;
    };
};

unsigned __int32 ajdust_controls(unsigned __int32 ctl, unsigned __int32 msr)
{
    union __msr msr_value = { 0 };
    msr_value.all = __readmsr(msr);
    ctl &= msr_value.high;
    ctl |= msr_value.low;
    return ctl;
}


int init_vmcs(struct __vcpu_t* vcpu, void* guest_rsp, /* void (*guest_rip)() ,*/ int is_pt_allowed)
{
      segment_selector host_cs_selector = { 0, 0, 1 };
      segment_selector host_tr_selector = { 0, 0, 2 };

      struct __pseudo_descriptor64 gdtr = { 0 };
      struct __pseudo_descriptor64 idtr = { 0 };
      union __exception_bitmap_t exception_bitmap = { 0 };
      union  __vmx_basic_msr_t vmx_basic = { 0 };
      union __vmx_entry_control_t entry_controls = { 0 };
      union __vmx_exit_control_t exit_controls = { 0 };
      union __vmx_pinbased_control_msr_t pinbased_controls = { 0 };
      union  __vmx_primary_processor_based_control_t primary_controls = { 0 };
      union  __vmx_secondary_processor_based_control_t secondary_controls = { 0 };

      Log("vmcs init called \n");
      __debugbreak();
      set_entry_control(&entry_controls);

      set_exit_control(&exit_controls);

      set_primary_controls(&primary_controls);

      set_secondary_controls(&secondary_controls);

      set_exception_bitmap(&exception_bitmap);

      set_pinbased_control_msr(&pinbased_controls);

    //Obtain System CR3
    PEPROCESS System = NULL;
    PsLookupProcessByProcessId((HANDLE)4, &System);
    unsigned long long system_cr3 = *(unsigned long long*)((char*)System + 0x28 /* EPROCESS -> DirectoryTableBase */ );
    ObfDereferenceObject(System);

    struct __vmcs_t* vmcs;

    PHYSICAL_ADDRESS physical_max;
    vmx_basic.all = __readmsr(IA32_VMX_BASIC_MSR);


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

    // set up VMCS guest state with the proper data 

    __vmx_vmwrite(VMCS_GUEST_CR0, __readcr0());
    __vmx_vmwrite(VMCS_GUEST_CR3, system_cr3);
    __vmx_vmwrite(VMCS_GUEST_CR4, __readcr4());
    __vmx_vmwrite(VMCS_GUEST_DR7, __readdr(7));
    __vmx_vmwrite(VMCS_GUEST_RSP, vcpu->guest_rsp);
    __vmx_vmwrite(VMCS_GUEST_RIP, vcpu->guest_rip);
    __vmx_vmwrite(VMCS_GUEST_RFLAGS, __readeflags());
    __vmx_vmwrite(VMCS_GUEST_DEBUGCTL, __readmsr(IA32_DEBUGCTL_MSR));
    __vmx_vmwrite(VMCS_GUEST_SYSENTER_ESP, __readmsr(IA32_SYSENTER_ESP_MSR));
    __vmx_vmwrite(VMCS_GUEST_SYSENTER_EIP, __readmsr(IA32_SYSENTER_EIP_MSR));
    __vmx_vmwrite(VMCS_GUEST_SYSENTER_CS, __readmsr(IA32_SYSENTER_CS_MSR));
    __vmx_vmwrite(VMCS_GUEST_VMCS_LINK_POINTER, MAXUINT64);
    __vmx_vmwrite(VMCS_GUEST_FS_BASE, __readmsr(IA32_FS_BASE_MSR));
    __vmx_vmwrite(VMCS_GUEST_GS_BASE, __readmsr(IA32_GS_BASE_MSR));


  

    __vmx_vmwrite(VMCS_CTRL_PIN_BASED_VM_EXECUTION_CONTROLS, ajdust_controls(pinbased_controls.control, vmx_basic.bits.true_controls ? IA32_VMX_TRUE_PINBASED_CTLS : IA32_VMX_PINBASED_CTLS));
    __vmx_vmwrite(VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, ajdust_controls(primary_controls.control, vmx_basic.bits.true_controls ? IA32_VMX_TRUE_PROCBASED_CTLS : IA32_VMX_PROCBASED_CTLS));
    __vmx_vmwrite(VMCS_CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, ajdust_controls(secondary_controls.all, IA32_VMX_PROCBASED_CTLS2));
    __vmx_vmwrite(VMCS_CTRL_PRIMARY_VMEXIT_CONTROLS, ajdust_controls(exit_controls.all, vmx_basic.all ? IA32_VMX_TRUE_EXIT_CTLS : IA32_VMX_EXIT_CTLS));
    __vmx_vmwrite(VMCS_CTRL_VMENTRY_CONTROLS, ajdust_controls(entry_controls.all, vmx_basic.all ? IA32_VMX_TRUE_ENTRY_CTLS : IA32_VMX_ENTRY_CTLS));
    __vmx_vmwrite(VMCS_CTRL_EXCEPTION_BITMAP, exception_bitmap.all);

    __vmx_vmwrite(VMCS_CTRL_CR4_GUEST_HOST_MASK, 0x2000);
    __vmx_vmwrite(VMCS_CTRL_CR3_TARGET_COUNT, 0);
    __vmx_vmwrite(VMCS_CTRL_CR0_READ_SHADOW, __readcr0());
    __vmx_vmwrite(VMCS_CTRL_CR4_READ_SHADOW, __readcr4() & -0x2000);
    __vmx_vmwrite(VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD, 0);
    __vmx_vmwrite(VMCS_CTRL_VMENTRY_EXCEPTION_ERROR_CODE, 0);
    __vmx_vmwrite(VMCS_CTRL_VMENTRY_INSTRUCTION_LENGTH, 0);
  //  __vmx_vmwrite(VMCS_CTRL_VIRTUAL_PROCESSOR_IDENTIFIER, KeGetCurrentProcessorNumberEx(NULL) + 1);

    __vmx_vmwrite(VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD, 0);
    __vmx_vmwrite(VMCS_CTRL_VMENTRY_EXCEPTION_ERROR_CODE, 0);
    __vmx_vmwrite(VMCS_CTRL_VMENTRY_INSTRUCTION_LENGTH, 0);

    // 3.24.7.2
    __vmx_vmwrite(VMCS_CTRL_VMEXIT_MSR_LOAD_COUNT, 0);
    __vmx_vmwrite(VMCS_CTRL_VMEXIT_MSR_LOAD_ADDRESS, 0);

    // vm-exit on every CR0/CR4 modification
    __vmx_vmwrite(VMCS_CTRL_CR0_GUEST_HOST_MASK, 0xFFFFFFFF'FFFFFFFF);
    __vmx_vmwrite(VMCS_CTRL_CR4_GUEST_HOST_MASK, 0xFFFFFFFF'FFFFFFFF);

    __vmx_vmwrite(VMCS_GUEST_CS_SELECTOR, __read_cs());
    __vmx_vmwrite(VMCS_GUEST_SS_SELECTOR, __read_ss());
    __vmx_vmwrite(VMCS_GUEST_DS_SELECTOR, __read_ds());
    __vmx_vmwrite(VMCS_GUEST_ES_SELECTOR, __read_es());
    __vmx_vmwrite(VMCS_GUEST_FS_SELECTOR, __read_fs());
    __vmx_vmwrite(VMCS_GUEST_GS_SELECTOR, __read_gs());
    __vmx_vmwrite(VMCS_GUEST_LDTR_SELECTOR, __read_ldtr());
    __vmx_vmwrite(VMCS_GUEST_TR_SELECTOR, __read_tr());


    _sgdt(&gdtr);
    __sidt(&idtr);

    __vmx_vmwrite(VMCS_GUEST_GDTR_LIMIT, gdtr.limit);
    __vmx_vmwrite(VMCS_GUEST_IDTR_LIMIT, idtr.limit);

    __vmx_vmwrite(VMCS_GUEST_GDTR_BASE, gdtr.base_address);
    __vmx_vmwrite(VMCS_GUEST_IDTR_BASE, idtr.base_address);

    __vmx_vmwrite(VMCS_GUEST_CS_ACCESS_RIGHTS, read_segment_access_rights(__read_cs()));
    __vmx_vmwrite(VMCS_GUEST_SS_ACCESS_RIGHTS, read_segment_access_rights(__read_ss()));
    __vmx_vmwrite(VMCS_GUEST_DS_ACCESS_RIGHTS, read_segment_access_rights(__read_ds()));
    __vmx_vmwrite(VMCS_GUEST_ES_ACCESS_RIGHTS, read_segment_access_rights(__read_es()));
    __vmx_vmwrite(VMCS_GUEST_FS_ACCESS_RIGHTS, read_segment_access_rights(__read_fs()));
    __vmx_vmwrite(VMCS_GUEST_GS_ACCESS_RIGHTS, read_segment_access_rights(__read_gs()));
    __vmx_vmwrite(VMCS_GUEST_LDTR_ACCESS_RIGHTS, read_segment_access_rights(__read_ldtr()));
    __vmx_vmwrite(VMCS_GUEST_TR_ACCESS_RIGHTS, read_segment_access_rights(__read_tr()));

    __vmx_vmwrite(VMCS_GUEST_LDTR_BASE, get_segment_base(gdtr.base_address, __read_ldtr()));
    __vmx_vmwrite(VMCS_GUEST_TR_BASE, get_segment_base(gdtr.base_address, __read_tr()));


    __vmx_vmwrite(VMCS_HOST_CS_SELECTOR, __read_cs() & 0xFFF8);
    __vmx_vmwrite(VMCS_HOST_SS_SELECTOR, __read_ss() & 0xFFF8);
    __vmx_vmwrite(VMCS_HOST_DS_SELECTOR, __read_ds() & 0xFFF8);
    __vmx_vmwrite(VMCS_HOST_ES_SELECTOR, __read_es() & 0xFFF8);
    __vmx_vmwrite(VMCS_HOST_FS_SELECTOR, __read_fs() & 0xFFF8);
    __vmx_vmwrite(VMCS_HOST_GS_SELECTOR, __read_gs() & 0xFFF8);
    __vmx_vmwrite(VMCS_HOST_TR_SELECTOR, __read_tr() & 0xFFF8);


 // __vmx_vmwrite(VMCS_HOST_CS_SELECTOR, host_cs_selector.flags);
 //__vmx_vmwrite(VMCS_HOST_SS_SELECTOR, 0x00);
 // __vmx_vmwrite(VMCS_HOST_DS_SELECTOR, 0x00);
 // __vmx_vmwrite(VMCS_HOST_ES_SELECTOR, 0x00);
 // __vmx_vmwrite(VMCS_HOST_FS_SELECTOR, 0x00);
 // __vmx_vmwrite(VMCS_HOST_GS_SELECTOR, 0x00);
 //__vmx_vmwrite(VMCS_HOST_TR_SELECTOR, host_tr_selector.flags);




    __vmx_vmwrite(VMCS_HOST_TR_BASE, get_segment_base(gdtr.base_address, __read_tr()));
    __vmx_vmwrite(VMCS_HOST_GS_BASE, 0);
    __vmx_vmwrite(VMCS_HOST_FS_BASE, __readmsr(IA32_FS_BASE_MSR));
    __vmx_vmwrite(VMCS_HOST_GDTR_BASE, gdtr.base_address);
    __vmx_vmwrite(VMCS_HOST_IDTR_BASE, idtr.base_address);

    __vmx_vmwrite(VMCS_HOST_RSP, ((ULONG64)vmm_stack + VMM_STACK_SIZE));
    __vmx_vmwrite(VMCS_HOST_RIP, (ULONG64)(entrypoint));

  
    

    cr3 host_cr3;
    host_cr3.flags = 0;
    host_cr3.page_level_cache_disable = 0;
    host_cr3.page_level_write_through = 0;
  
    __vmx_vmwrite(VMCS_HOST_CR3, host_cr3.flags);
    cr4 host_cr4;
    host_cr4.flags = __readcr4();

    // these are flags that may or may not be set by Windows
    host_cr4.fsgsbase_enable = 1;
    host_cr4.os_xsave = 1;
    host_cr4.smap_enable = 0;
    host_cr4.smep_enable = 0;

    __vmx_vmwrite(VMCS_HOST_CR0, __readcr0());
    __vmx_vmwrite(VMCS_HOST_CR4, host_cr4.flags);

    __vmx_vmwrite(VMCS_HOST_SYSENTER_CS, __readmsr(IA32_SYSENTER_CS_MSR));
    __vmx_vmwrite(VMCS_HOST_SYSENTER_ESP, __readmsr(IA32_SYSENTER_ESP_MSR));
    __vmx_vmwrite(VMCS_HOST_SYSENTER_EIP,__readmsr(IA32_SYSENTER_EIP_MSR));
    __vmx_vmwrite(VMCS_HOST_EFER, __readmsr(IA32_EFER));
    __vmx_vmwrite(VMCS_HOST_PERF_GLOBAL_CTRL, 0);

    ia32_pat_register host_pat;
    host_pat.flags = 0;
    host_pat.pa0 = MEMORY_TYPE_WRITE_BACK;
    host_pat.pa1 = MEMORY_TYPE_WRITE_THROUGH;
    host_pat.pa2 = MEMORY_TYPE_UNCACHEABLE_MINUS;
    host_pat.pa3 = MEMORY_TYPE_UNCACHEABLE;
    host_pat.pa4 = MEMORY_TYPE_WRITE_BACK;
    host_pat.pa5 = MEMORY_TYPE_WRITE_THROUGH;
    host_pat.pa6 = MEMORY_TYPE_UNCACHEABLE_MINUS;
    host_pat.pa7 = MEMORY_TYPE_UNCACHEABLE;
    __vmx_vmwrite(VMCS_HOST_PAT, host_pat.flags);

    Log("vmcs initiated \n");

    return TRUE;
}