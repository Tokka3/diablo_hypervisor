#include "headers/includes.h"
#include "headers/init.h"
#include "headers/logging.h"
#include "struct_defs/vmm_context.h"


NTSTATUS DriverUnload(PDRIVER_OBJECT driver_obj) {

	UNREFERENCED_PARAMETER(driver_obj);

	Log("[+] Driver Exit Called\n");
	return STATUS_SUCCESS;

}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING reg_path) {

	UNREFERENCED_PARAMETER(reg_path);
	UNREFERENCED_PARAMETER(driver_obj);

	Log("[+] Driver Entry Called\n");

	if (check_vmx_support()) {
		Log("[+] VMX Supported\n");
		vmm_init();		
	}
	else {
		Log("[+] VMX Not Supported\n");

		return STATUS_UNSUCCESSFUL;
	}


	return STATUS_SUCCESS;
}