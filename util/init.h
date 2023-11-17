#pragma once
#include "../includes.h"
#include "../struct_defs/vcpu.h"
#include "../struct_defs/vmcs.h"
#include "../struct_defs/vmm_context.h"
int check_vmx_support();
	 
int enable_vmx_operation(void);

void init_logical_processor();

int vmm_init(void);