#include "../headers/includes.h"
#include "../asm/vmm_intrin.h"
#include "../headers/vmexit.h"
#include "../headers/logging.h"


#pragma warning (disable : 4115)
#pragma warning (disable : 4245)
BOOLEAN vmexit_handler(struct __vmexit_guest_registers_t* guest_registers)
{

  
    Log("vm exit handler called");
    __debugbreak();
  //  DbgBreakPointWithStatus(STATUS_BREAKPOINT);


    
 
    //DbgBreakPointWithStatus(STATUS_BREAKPOINT);

    return 0;
}