#ifndef  VM_EXIT_GUEST_REGISTERS
#define VM_EXIT_GUEST_REGISTERS
struct __vmexit_guest_registers_t
{
    unsigned __int64 rax;
    unsigned __int64 rcx;
    unsigned __int64 rdx;
    unsigned __int64 rbx;
    unsigned __int64 rsp;
    unsigned __int64 rbp;
    unsigned __int64 rsi;
    unsigned __int64 rdi;
    unsigned __int64 r8;
    unsigned __int64 r9;
    unsigned __int64 r10;
    unsigned __int64 r11;
    unsigned __int64 r12;
    unsigned __int64 r13;
    unsigned __int64 r14;        // 70h
    unsigned __int64 r15;
};
#endif // ! VM_EXIT_GUEST_REGISTERS
