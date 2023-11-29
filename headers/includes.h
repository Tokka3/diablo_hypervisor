#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <wdm.h>
#include <intrin.h>


#include "../headers/logging.h"
#include "../headers/includes.h"


#include "../headers/init.h"
#include "../struct_defs/cpuid.h"
#include "../struct_defs/feature_control.h"
#include "../struct_defs/control_registers.h"

#include "../struct_defs/vcpu.h"

#include "../struct_defs/vmx_entry_ctl.h"

#include "../encodings/guest_state/guest_state_encodings.h"
#include "../encodings/host_state/host_state_encodings.h"
#include "../encodings/vmexit/vm_exit_encodings.h"
#include "../encodings/control/control_encodings.h"

#include "../headers/vmx.h"
#include "../asm/vmm_intrin.h"

#include "../segments/segments.h"
#define VMX_OK 0x0
#define selector_mask 7

#pragma warning (disable: 4996)
#pragma warning(disable: 4100)
#pragma warning(disable: 6273)
#pragma warning(disable: 4242)
#pragma warning(disable: 4244)
#pragma warning(disable: 28182)

