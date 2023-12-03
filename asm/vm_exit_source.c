#include "vm_exit.h"

BOOLEAN handle_vm_exit(struct guest_context* const ctx) {

	Log("vm exit handler called");
	__debugbreak();
	
}