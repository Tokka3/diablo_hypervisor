#include "includes.h"
#include "util/init.h"


NTSTATUS DriverUnload(PDRIVER_OBJECT driver_obj) {

	UNREFERENCED_PARAMETER(driver_obj);

	DbgPrintEx(0, 0, "[+] Driver Exit Called");
	return STATUS_SUCCESS;

}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING reg_path) {

	UNREFERENCED_PARAMETER(reg_path);
	UNREFERENCED_PARAMETER(driver_obj);
	driver_obj->DriverUnload = DriverUnload;
	DbgPrintEx(0, 0, "[+] Driver Entry Called");
	
	if (CheckVMXSupport()) {
		DbgPrintEx(0, 0, "[+] VMX Supported");
		DbgPrintEx(0, 0, "[+] CPUID: EAX");
	}
	else {
		DbgPrintEx(0, 0, "[+] VMX Not Supported");
	}


	return STATUS_SUCCESS;
}