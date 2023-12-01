#include "../headers/msr.h"
#include "../struct_defs/vmx_basic.h"
#include "../struct_defs/vmx_misc.h"
#include "../struct_defs/vmx_entry_ctl.h"
#include "../struct_defs/vmx_exit_ctl.h"
#include "../struct_defs/vmx_pinbased_ctl.h"
#include "../struct_defs/vmx_primary_processor_based_ctl.h"
#include "../struct_defs/vmx_secondary_processor_based_ctl.h"


 //void vmx_adjust_entry_controls(union __vmx_entry_control_t* entry_controls);

 //void vmx_adjust_exit_controls(union __vmx_exit_control_t* exit_controls);

 //void vmx_adjust_secondary_controls(union __vmx_secondary_processor_based_control_t* secondary_controls);

 //void vmx_adjust_processor_based_controls(union __vmx_primary_processor_based_control_t* processor_based_controls);

 //void vmx_adjust_pinbased_controls(union __vmx_pinbased_control_msr_t* pinbased_controls);

void set_primary_controls(union __vmx_primary_processor_based_control_t* primary_controls);

void set_secondary_controls(union __vmx_secondary_processor_based_control_t* secondary_controls);

void set_entry_control(union __vmx_entry_control_t* entry_control);

void set_exit_control(union __vmx_exit_control_t* exit_control);

void set_pinbased_control_msr(union __vmx_pinbased_control_msr_t* pinbased_controls);

void set_exception_bitmap(union __exception_bitmap_t* exception_bitmap);
         
 int enable_vmx_operation(void);

 int init_vmxon(struct __vcpu_t* vcpu, unsigned long processor_number);