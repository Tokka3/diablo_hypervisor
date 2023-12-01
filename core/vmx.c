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

int init_vmxon(struct __vcpu_t* vcpu, unsigned long processor_number)
{
    union __vmx_basic_msr_t vmx_basic = { 0 };
    struct __vmcs_t* vmxon;
    PHYSICAL_ADDRESS physical_max;
    if (!vcpu) {
        //log_error("VMXON region could not be initialized. vcpu was null.\n");
        return FALSE;
    }
    vmx_basic.all = __readmsr(IA32_VMX_BASIC_MSR);
    physical_max.QuadPart = ~0ULL;
    vcpu->vmxon = MmAllocateContiguousMemory(PAGE_SIZE, physical_max);
    vcpu->vmxon_physical = MmGetPhysicalAddress(vcpu->vmxon).QuadPart;
    vmxon = vcpu->vmxon;
    RtlSecureZeroMemory(vmxon, PAGE_SIZE);
    vmxon->header.all = vmx_basic.bits.vmcs_revision_identifier;
    Log("VMXON for vcpu %d initialized:\n\t-> VA: %llX\n\t-> PA: %llX\n\t-> REV: %X\n",
        processor_number,
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
 /*void vmx_adjust_entry_controls(union __vmx_entry_control_t* entry_controls)
{
    unsigned int capability_msr;
    union __vmx_basic_msr_t basic;

    basic.control = __readmsr(IA32_VMX_BASIC);
    capability_msr = (basic.bits.true_controls != FALSE) ? IA32_VMX_TRUE_ENTRY_CTLS : IA32_VMX_ENTRY_CTLS;

    entry_controls->all = (uintptr_t)vmx_adjust_cv(capability_msr, entry_controls->all);
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
}*/

#define false 0
#define true 1
 void set_primary_controls(union __vmx_primary_processor_based_control_t* primary_controls)
 {
	 /**
	 * If this control is 1, a VM exit occurs at the beginning of any instruction if RFLAGS.IF = 1 and
	 * there are no other blocking of interrupts (see Section 24.4.2).
	 */
	 primary_controls->bits.interrupt_window_exiting = 0;

	 /**
	 * This control determines whether executions of RDTSC, executions of RDTSCP, and executions
	 * of RDMSR that read from the IA32_TIME_STAMP_COUNTER MSR return a value modified by
	 * the TSC offset field (see Section 24.6.5 and Section 25.3).
	 */
	 primary_controls->bits.use_tsc_offsetting = false;

	 /**
	 * This control determines whether executions of HLT cause VM exits.
	 */
	 primary_controls->bits.hlt_exiting = false;

	 /**
	 * This determines whether executions of INVLPG cause VM exits.
	 */

#ifdef _MINIMAL
	 primary_controls->bits.invldpg_exiting = false;
#else
	 primary_controls->bits.invldpg_exiting = true;
#endif

	 /**
	 * This control determines whether executions of MWAIT cause VM exits.
	 */
	 primary_controls->bits.mwait_exiting = false;

	 /**
	 * This control determines whether executions of RDPMC cause VM exits.
	 */
	 primary_controls->bits.rdpmc_exiting = false;

	 /**
	 * This control determines whether executions of RDTSC and RDTSCP cause VM exits.
	 */
#ifdef _MINIMAL
	 primary_controls->bits.rdtsc_exiting = false;
#else
	 primary_controls->bits.rdtsc_exiting = true;
#endif

	 /**
	 * In conjunction with the CR3-target controls (see Section 24.6.7), this control determines
	 * whether executions of MOV to CR3 cause VM exits. See Section 25.1.3.
	 * The first processors to support the virtual-machine extensions supported only the 1-setting
	 * of this control.
	 */
#ifdef _MINIMAL
	 primary_controls->bits.cr3_load_exiting = false;
#else
	 primary_controls->bits.cr3_load_exiting = true;
#endif

	 /**
	 * This control determines whether executions of MOV from CR3 cause VM exits.
	 * The first processors to support the virtual-machine extensions supported only the 1-setting
	 * of this control.
	 */
#ifdef _MINIMAL
	 primary_controls->bits.cr3_store_exiting = false;
#else
	 primary_controls->bits.cr3_store_exiting = true;
#endif

	 /**
	 * This control determines whether executions of MOV to CR8 cause VM exits.
	 */
	 primary_controls->bits.cr8_load_exiting = false;

	 /**
	 * This control determines whether executions of MOV from CR8 cause VM exits.
	 */
	 primary_controls->bits.cr8_store_exiting = false;

	 /**
	 * Setting this control to 1 enables TPR virtualization and other APIC-virtualization features. See
	 * Chapter 29.
	 */
	 primary_controls->bits.use_tpr_shadow = false;

	 /**
	 * If this control is 1, a VM exit occurs at the beginning of any instruction if there is no virtual-
	 * NMI blocking (see Section 24.4.2).
	 */
	 primary_controls->bits.nmi_window_exiting = false;

	 /**
	 * This control determines whether executions of MOV DR cause VM exits.
	 */
#ifdef _MINIMAL
	 primary_controls->bits.mov_dr_exiting = false;
#else
	 primary_controls->bits.mov_dr_exiting = true;
#endif

	 /**
	 * This control determines whether executions of I/O instructions (IN, INS/INSB/INSW/INSD, OUT,
	 * and OUTS/OUTSB/OUTSW/OUTSD) cause VM exits->bits.
	 */
	 primary_controls->bits.unconditional_io_exiting = false;

	 /**
	 * This control determines whether I/O bitmaps are used to restrict executions of I/O instructions
	 (see Section 24->bits.6->bits.4 and Section 25->bits.1->bits.3)->bits.
	 For this control, “0” means “do not use I/O bitmaps” and “1” means “use I/O bitmaps->bits.” If the I/O
	 bitmaps are used, the setting of the “unconditional I/O exiting” control is ignored
	 */
#ifdef _MINIMAL
	 primary_controls->bits.use_io_bitmaps = false;
#else
	 primary_controls->bits.use_io_bitmaps = false;
#endif

	 /**
	 * If this control is 1, the monitor trap flag debugging feature is enabled->bits. See Section 25->bits.5->bits.2->bits.
	 */
	 primary_controls->bits.monitor_trap_flag = false;

	 /**
	 * This control determines whether MSR bitmaps are used to control execution of the RDMSR
	 * and WRMSR instructions (see Section 24->bits.6->bits.9 and Section 25->bits.1->bits.3)->bits.
	 * For this control, “0” means “do not use MSR bitmaps” and “1” means “use MSR bitmaps->bits.” If the
	 * MSR bitmaps are not used, all executions of the RDMSR and WRMSR instructions cause
	 * VM exits->bits.
	 */
	 primary_controls->bits.use_msr_bitmaps = false;

	 /**
	 * This control determines whether executions of MONITOR cause VM exits->bits.
	 */
	 primary_controls->bits.monitor_exiting = false;

	 /**
	 * This control determines whether executions of PAUSE cause VM exits->bits.
	 */
	 primary_controls->bits.pause_exiting = false;

	 /**
	 * This control determines whether the secondary processor-based VM-execution controls are
	 * used->bits. If this control is 0, the logical processor operates as if all the secondary processor-based
	 * VM-execution controls were also 0->bits.
	 */
	 primary_controls->bits.active_secondary_controls = true;
 }

 /// <summary>
 /// Derived from Intel Manuals Voulme 3 Section 24->bits.6->bits.2 Table 24-7->bits. Definitions of Secondary Processor-Based VM-Execution Controls
 /// </summary>
 /// <param name="secondary_controls"></param>
 void set_secondary_controls(union __vmx_secondary_processor_based_control_t* secondary_controls)
 {
	 /**
	 * If this control is 1, the logical processor treats specially accesses to the page with the APIC-
	 * access address->bits. See Section 29->bits.4->bits.
	 */
	 secondary_controls->bits.virtualize_apic_accesses = false;

	 /**
	 * If this control is 1, extended page tables (EPT) are enabled->bits. See Section 28->bits.2->bits.
	 */
	 secondary_controls->bits.enable_ept = false;

	 /**
	 * This control determines whether executions of LGDT, LIDT, LLDT, LTR, SGDT, SIDT, SLDT, and
	 * STR cause VM exits->bits.
	 */
#ifdef _MINIMAL
	 secondary_controls->bits.descriptor_table_exiting = false;
#else
	 secondary_controls->bits.descriptor_table_exiting = true;
#endif

	 /**
	 * If this control is 0, any execution of RDTSCP causes an invalid-opcode exception (#UD)->bits.
	 */
	 secondary_controls->bits.enable_rdtscp = true;

	 /**
	 * If this control is 1, the logical processor treats specially RDMSR and WRMSR to APIC MSRs (in
	 * the range 800H–8FFH)->bits. See Section 29->bits.5->bits.
	 */
	 secondary_controls->bits.virtualize_x2apic = false;

	 /**
	 * If this control is 1, cached translations of linear addresses are associated with a virtual-
	 * processor identifier (VPID)->bits. See Section 28->bits.1->bits.
	 */
	 secondary_controls->bits.enable_vpid = false;

	 /**
	 * This control determines whether executions of WBINVD cause VM exits->bits.
	 */
#ifdef _MINIMAL
	 secondary_controls->bits.wbinvd_exiting = false;
#else
	 secondary_controls->bits.wbinvd_exiting = true;
#endif

	 /**
	 * This control determines whether guest software may run in unpaged protected mode or in real-
	 * address mode->bits.
	 */
	 secondary_controls->bits.unrestricted_guest = false;

	 /**
	 * If this control is 1, the logical processor virtualizes certain APIC accesses->bits. See Section 29->bits.4 and
	 * Section 29->bits.5->bits.
	 */
	 secondary_controls->bits.apic_register_virtualization = false;

	 /**
	 * This controls enables the evaluation and delivery of pending virtual interrupts as well as the
	 * emulation of writes to the APIC registers that control interrupt prioritization->bits.
	 */
	 secondary_controls->bits.virtual_interrupt_delivery = false;

	 /**
	 * This control determines whether a series of executions of PAUSE can cause a VM exit (see
	 * Section 24->bits.6->bits.13 and Section 25->bits.1->bits.3)->bits.
	 */
	 secondary_controls->bits.pause_loop_exiting = false;

	 /**
	 * This control determines whether executions of RDRAND cause VM exits->bits.
	 */
#ifdef _MINIMAL
	 secondary_controls->bits.rdrand_exiting = false;
#else
	 secondary_controls->bits.rdrand_exiting = true;
#endif

	 /**
	 * If this control is 0, any execution of INVPCID causes a #UD->bits.
	 */
	 secondary_controls->bits.enable_invpcid = true;

	 /**
	 * Setting this control to 1 enables use of the VMFUNC instruction in VMX non-root operation->bits. See
	 * Section 25->bits.5->bits.6->bits.
	 */
	 secondary_controls->bits.enable_vmfunc = false;

	 /**
	 * If this control is 1, executions of VMREAD and VMWRITE in VMX non-root operation may access
	 * a shadow VMCS (instead of causing VM exits)->bits. See Section 24->bits.10 and Section 30->bits.3->bits.
	 */
	 secondary_controls->bits.vmcs_shadowing = false;

	 /**
	 * If this control is 1, executions of ENCLS consult the ENCLS-exiting bitmap to determine whether
	 * the instruction causes a VM exit->bits. See Section 24->bits.6->bits.16 and Section 25->bits.1->bits.3->bits.
	 */
	 secondary_controls->bits.enable_encls_exiting = false;

	 /**
	 * This control determines whether executions of RDSEED cause VM exits->bits.
	 */
#ifdef _MINIMAL
	 secondary_controls->bits.rdseed_exiting = false;
#else
	 secondary_controls->bits.rdseed_exiting = true;
#endif

	 /**
	 * If this control is 1, an access to a guest-physical address that sets an EPT dirty bit first adds an
	 * entry to the page-modification log->bits. See Section 28->bits.2->bits.6->bits.
	 */
	 secondary_controls->bits.enable_pml = false;

	 /**
	 * If this control is 1, EPT violations may cause virtualization exceptions (#VE) instead of VM exits->bits.
	 * See Section 25->bits.5->bits.7->bits.
	 */
	 secondary_controls->bits.use_virtualization_exception = false;

	 /**
	 * If this control is 1, Intel Processor Trace suppresses from PIPs an indication that the processor
	 * was in VMX non-root operation and omits a VMCS packet from any PSB+ produced in VMX non-
	 * root operation (see Chapter 35)->bits.
	 */
	 secondary_controls->bits.conceal_vmx_from_pt = true;

	 /**
	 * If this control is 0, any execution of XSAVES or XRSTORS causes a #UD->bits.
	 */
	 secondary_controls->bits.enable_xsave_xrstor = true;

	 /**
	 * If this control is 1, EPT execute permissions are based on whether the linear address being
	 * accessed is supervisor mode or user mode->bits. See Chapter 28->bits.
	 */
	 secondary_controls->bits.mode_based_execute_control_ept = false;

	 /**
	 * This control determines whether executions of RDTSC, executions of RDTSCP, and executions
	 * of RDMSR that read from the IA32_TIME_STAMP_COUNTER MSR return a value modified by the
	 * TSC multiplier field (see Section 24->bits.6->bits.5 and Section 25->bits.3)->bits.
	 */
	 secondary_controls->bits.sub_page_write_permission_for_ept = false;

	 /**
	 * This control determines whether executions of RDTSC, executions of RDTSCP, and executions
	 * of RDMSR that read from the IA32_TIME_STAMP_COUNTER MSR return a value modified by the
	 * TSC multiplier field (see Section 24->bits.6->bits.5 and Section 25->bits.3)->bits.
	 */
	 secondary_controls->bits.intel_pt_uses_guest_physical_address = false;

	 /**
	 * This control determines whether executions of RDTSC, executions of RDTSCP, and executions
	 * of RDMSR that read from the IA32_TIME_STAMP_COUNTER MSR return a value modified by the
	 * TSC multiplier field (see Section 24->bits.6->bits.5 and Section 25->bits.3)->bits.
	 */
	 secondary_controls->bits.use_tsc_scaling = false;

	 /**
	 * If this control is 0, any execution of TPAUSE, UMONITOR, or UMWAIT causes a #UD->bits.
	 */
	 secondary_controls->bits.enable_user_wait_and_pause = false;

	 /**
	 * If this control is 1, executions of ENCLV consult the ENCLV-exiting bitmap to determine whether
	 * the instruction causes a VM exit->bits. See Section 24->bits.6->bits.17 and Section 25->bits.1->bits.3->bits.
	 */
	 secondary_controls->bits.enable_enclv_exiting = false;
 }

 /// <summary>
 /// Derived from Intel Manuals Voulme 3 Section 24->bits.8->bits.1 Table 24-13->bits. Definitions of VM-Entry Controls
 /// </summary>
 /// <param name="entry_control"></param>
 /// 

 void set_entry_control(union __vmx_entry_control_t* entry_control)
 {
	 /**
	 * This control determines whether DR7 and the IA32_DEBUGCTL MSR are loaded on VM entry->bits.
	 * The first processors to support the virtual-machine extensions supported only the 1-setting of
	 * this control->bits.
	 */
	 entry_control->bits.load_dbg_controls = true;

	 /**
	 * On processors that support Intel 64 architecture, this control determines whether the logical
	 * processor is in IA-32e mode after VM entry->bits. Its value is loaded into IA32_EFER->bits.LMA as part of
	 * VM entry->bits. 1
	 * This control must be 0 on processors that do not support Intel 64 architecture->bits.
	 */
	 entry_control->bits.ia32e_mode_guest = true;

	 /**
	 * This control determines whether the logical processor is in system-management mode (SMM)
	 * after VM entry->bits. This control must be 0 for any VM entry from outside SMM->bits.
	 */
	 entry_control->bits.entry_to_smm = false;

	 /**
	 * If set to 1, the default treatment of SMIs and SMM is in effect after the VM entry (see Section
	 * 34->bits.15->bits.7)->bits. This control must be 0 for any VM entry from outside SMM->bits.
	 */
	 entry_control->bits.deactivate_dual_monitor_treament = false;

	 /**
	 * This control determines whether the IA32_PERF_GLOBAL_CTRL MSR is loaded on VM entry->bits.
	 */
	 entry_control->bits.load_ia32_perf_global_control = false;

	 /**
	 * This control determines whether the IA32_PAT MSR is loaded on VM entry->bits.
	 */
	 entry_control->bits.load_ia32_pat = false;

	 /**
	 * This control determines whether the IA32_EFER MSR is loaded on VM entry->bits.
	 */
	 entry_control->bits.load_ia32_efer = false;

	 /**
	 * This control determines whether the IA32_BNDCFGS MSR is loaded on VM entry->bits.
	 */
	 entry_control->bits.load_ia32_bndcfgs = false;

	 /**
	 * If this control is 1, Intel Processor Trace does not produce a paging information packet (PIP) on
	 * a VM entry or a VMCS packet on a VM entry that returns from SMM (see Chapter 35)->bits.
	 */
	 entry_control->bits.conceal_vmx_from_pt = true;

	 /**
	 * This control determines whether the IA32_RTIT_CTL MSR is loaded on VM entry->bits.
	 */
	 entry_control->bits.load_ia32_rtit_ctl = false;

	 /**
	 * This control determines whether CET-related MSRs and SPP are loaded on VM entry->bits.
	 */
	 entry_control->bits.load_cet_state = false;

	 /**
	 * This control determines whether CET-related MSRs and SPP are loaded on VM entry->bits.
	 */
	 entry_control->bits.load_pkrs = false;
 }

 /// <summary>
 /// Derived from Intel Manuals Voulme 3 Section 24->bits.7->bits.1 Table 24-11->bits. Definitions of VM-Exit Controls
 /// </summary>
 /// <param name="exit_control"></param>

 void set_exit_control(union __vmx_exit_control_t* exit_control)
 {
	 /**
	 * This control determines whether DR7 and the IA32_DEBUGCTL MSR are saved on VM exit->bits.
	 * The first processors to support the virtual-machine extensions supported only the 1-
	 * setting of this control->bits.
	 */
	 exit_control->bits.save_dbg_controls = true;

	 /**
	 * On processors that support Intel 64 architecture, this control determines whether a logical
	 * processor is in 64-bit mode after the next VM exit->bits. Its value is loaded into CS->bits.L,
	 * IA32_EFER->bits.LME, and IA32_EFER->bits.LMA on every VM exit->bits. 1
	 * This control must be 0 on processors that do not support Intel 64 architecture->bits.
	 */
	 exit_control->bits.host_address_space_size = true;

	 /**
	 * This control determines whether the IA32_PERF_GLOBAL_CTRL MSR is loaded on VM exit->bits.
	 */
	 exit_control->bits.load_ia32_perf_global_control = false;

	 /**
	 * This control affects VM exits due to external interrupts:
	 * • If such a VM exit occurs and this control is 1, the logical processor acknowledges the
	 *   interrupt controller, acquiring the interrupt’s vector->bits. The vector is stored in the VM-exit
	 *   interruption-information field, which is marked valid->bits.
	 * • If such a VM exit occurs and this control is 0, the interrupt is not acknowledged and the
	 *   VM-exit interruption-information field is marked invalid->bits.
	 */
	 exit_control->bits.ack_interrupt_on_exit = true;

	 /**
	 * This control determines whether the IA32_PAT MSR is saved on VM exit->bits.
	 */
	 exit_control->bits.save_ia32_pat = false;

	 /**
	 * This control determines whether the IA32_PAT MSR is loaded on VM exit->bits.
	 */
	 exit_control->bits.load_ia32_pat = false;

	 /**
	 * This control determines whether the IA32_EFER MSR is saved on VM exit->bits.
	 */
	 exit_control->bits.save_ia32_efer = false;

	 /**
	 * This control determines whether the IA32_EFER MSR is loaded on VM exit->bits.
	 */
	 exit_control->bits.load_ia32_efer = false;

	 /**
	 * This control determines whether the value of the VMX-preemption timer is saved on
	 * VM exit->bits.
	 */
	 exit_control->bits.save_vmx_preemption_timer_value = false;

	 /**
	 * This control determines whether the IA32_BNDCFGS MSR is cleared on VM exit->bits.
	 */
	 exit_control->bits.clear_ia32_bndcfgs = false;

	 /**
	 * If this control is 1, Intel Processor Trace does not produce a paging information packet (PIP)
	 * on a VM exit or a VMCS packet on an SMM VM exit (see Chapter 35)->bits.
	 */
	 exit_control->bits.conceal_vmx_from_pt = true;

	 /**
	 * This control determines whether the IA32_RTIT_CTL MSR is cleared on VM exit->bits.
	 */
	 exit_control->bits.load_ia32_rtit_ctl = false;

	 /**
	 * This control determines whether CET-related MSRs and SPP are loaded on VM exit->bits.
	 */
	 exit_control->bits.load_cet_state = false;

	 /**
	 * This control determines whether the IA32_PKRS MSR is loaded on VM exit->bits.
	 */
	 exit_control->bits.load_pkrs = false;
 }

 /// <summary>
 /// Derived from Intel Manuals Voulme 3 Section 24->bits.6->bits.1 Table 24-5->bits. Definitions of Pin-Based VM-Execution Controls
 /// </summary>
 /// <param name="pinbased_controls"></param>
 
 void set_pinbased_control_msr(union __vmx_pinbased_control_msr_t* pinbased_controls)
 {
	 /**
	 * If this control is 1, external interrupts cause VM exits->bits. Otherwise, they are delivered normally
	 * through the guest interrupt-descriptor table (IDT)->bits. If this control is 1, the value of RFLAGS->bits.IF
	 * does not affect interrupt blocking->bits.
	 */
	 pinbased_controls->bits.external_interrupt_exiting = false;

	 /**
	 * If this control is 1, non-maskable interrupts (NMIs) cause VM exits->bits. Otherwise, they are
	 * delivered normally using descriptor 2 of the IDT->bits. This control also determines interactions
	 * between IRET and blocking by NMI (see Section 25->bits.3)->bits.
	 */
	 pinbased_controls->bits.nmi_exiting = false;

	 /**
	 * If this control is 1, NMIs are never blocked and the “blocking by NMI” bit (bit 3) in the
	 * interruptibility-state field indicates “virtual-NMI blocking” (see Table 24-3)->bits. This control also
	 * interacts with the “NMI-window exiting” VM-execution control (see Section 24->bits.6->bits.2)->bits.
	 */
	 pinbased_controls->bits.virtual_nmis = false;

	 /**
	 * If this control is 1, the VMX-preemption timer counts down in VMX non-root operation; see
	 * Section 25->bits.5->bits.1->bits. A VM exit occurs when the timer counts down to zero; see Section 25->bits.2->bits.
	 */
	 pinbased_controls->bits.vmx_preemption_timer = false;

	 /**
	 * If this control is 1, the processor treats interrupts with the posted-interrupt notification vector
	 * (see Section 24->bits.6->bits.8) specially, updating the virtual-APIC page with posted-interrupt requests
	 * (see Section 29->bits.6)->bits.
	 */
	 pinbased_controls->bits.process_posted_interrupts = false;
 }

 /// <summary>
 /// Set which exception cause vmexit
 /// </summary>
 /// <param name="exception_bitmap"></param>
 /// 
 
 void set_exception_bitmap(union __exception_bitmap_t* exception_bitmap)
 {
	 exception_bitmap->bits.divide_error = false;

	 exception_bitmap->bits.debug = true;

	 exception_bitmap->bits.nmi_interrupt = false;

	 exception_bitmap->bits.breakpoint = false;

	 exception_bitmap->bits.overflow = false;

	 exception_bitmap->bits.bound = false;

	 exception_bitmap->bits.invalid_opcode = false;

	 exception_bitmap->bits.coprocessor_segment_overrun = false;

	 exception_bitmap->bits.invalid_tss = false;

	 exception_bitmap->bits.segment_not_present = false;

	 exception_bitmap->bits.stack_segment_fault = false;

	 exception_bitmap->bits.general_protection = false;

	 exception_bitmap->bits.page_fault = false;

	 exception_bitmap->bits.x87_floating_point_error = false;

	 exception_bitmap->bits.alignment_check = false;

	 exception_bitmap->bits.machine_check = false;

	 exception_bitmap->bits.simd_floating_point_error = false;

	 exception_bitmap->bits.virtualization_exception = false;
 }
