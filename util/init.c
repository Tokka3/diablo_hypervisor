#include "init.h"
#include "cpuid.h"



int CheckVMXSupport() {

	__cpuid_t cpuid = { 0 };
	__cpuid(cpuid.cpu_info, 1);
	return cpuid.feature_ecx.virtual_machine_extensions;
};

