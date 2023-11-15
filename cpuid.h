#pragma once
#pragma warning(disable : 4201)
typedef union __cpuid_t {
    int cpu_info[4]; // eax, ebx, ecx, edx
    struct {
        unsigned int eax;
        unsigned int ebx;
        struct {
            unsigned int stepping_id : 4;
            unsigned int model : 4;
            unsigned int family_id : 4;
            unsigned int processor_type : 2;
            unsigned int reserved1 : 2;
            unsigned int extended_model_id : 4;
            unsigned int extended_family_id : 8;
            unsigned int reserved2 : 4;
        } version_info;
        struct {
            unsigned int : 5; // Skip the first 5 bits
            unsigned int virtual_machine_extensions : 1; // VMX Bit
            // Define other bits here as needed...
        } feature_ecx;
        unsigned int edx;
    };
} __cpuid_t;