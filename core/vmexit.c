#include "../headers/includes.h"
#include "../asm/vmm_intrin.h"

#pragma warning (disable : 4115)
#pragma warning (disable : 4245)
BOOLEAN vmexit_handler(struct __vmexit_guest_registers_t* guest_registers)
{
    __debugbreak();
    DbgPrintEx(0, 0, "vm exit handler called");
  //  DbgBreakPointWithStatus(STATUS_BREAKPOINT);

    return 0;
}