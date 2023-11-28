#include "segments.h"
#include "../headers/includes.h"
#include "../asm/vmm_intrin.h"

#pragma warning (disable : 4333)
#define SEGMENT_DESCRIPTOR_TYPE_TSS_AVAILABLE 0x9

#define SEGMENT_DESCRIPTOR_TYPE_TSS_BUSY 0xB
 unsigned __int32 read_segment_access_rights(unsigned __int16 segment_selector)
{
    union __segment_selector_t selector;
    union __segment_access_rights_t vmx_access_rights;

    selector.flags = segment_selector;

    //
    // Check for null selector use, if found set access right to unusable
    // and return. Otherwise, get access rights, modify format, return the
    // segment access rights.
    //
    if (selector.table == 0
        && selector.index == 0)
    {
        vmx_access_rights.flags = 0;
        vmx_access_rights.unusable = TRUE;
        return vmx_access_rights.flags;
    }

    //
    // Use our custom intrinsic to store our access rights, and
    // remember that the first byte of the access rights returned
    // are not used in VMX access right format.
    //
    vmx_access_rights.flags = (__load_ar(segment_selector) >> 8);
    vmx_access_rights.unusable = 0;
    vmx_access_rights.reserved0 = 0;
    vmx_access_rights.reserved1 = 0;

    return vmx_access_rights.flags;
}

 unsigned __int64 get_segment_base(unsigned __int64 gdt_base, unsigned __int16 segment_selector)
{
    unsigned __int64 segment_base;
    union __segment_selector_t selector;
    struct __segment_descriptor_32_t* descriptor;
    struct __segment_descriptor_32_t* descriptor_table;

    selector.flags = segment_selector;

    if (selector.table == 0
        && selector.index == 0)
    {
        segment_base = 0;
        return segment_base;
    }

    descriptor_table = (struct __segment_descriptor_32_t*)gdt_base;
    descriptor = &descriptor_table[selector.index];

    //
    // All of this bit masking and shifting is just a shortcut instead
    // of allocating some local variables to hold the low, mid, and high base
    // values. 
    //
    // If we did it with local variables it would look similar to this:
    // base_high = descriptor->base_high << 24;
    // base_mid = descriptor->base_middle << 16;
    // base_low = descriptor->base_low;
    // segment_base = (base_high | base_mid | base_low) & 0xFFFFFFFF;
    //
    // But for the purposes of doing it all in one fell-swoop we did the shifting
    // and masking inline.
    //
    segment_base = (unsigned __int64)((descriptor->base_high & 0xFF000000) |
        ((descriptor->base_middle << 16) & 0x00FF0000) |
        ((descriptor->base_low >> 16) & 0x0000FFFF));

    //
    // As mentioned in the discussion in the article, some system descriptors are expanded
    // to 16 bytes on Intel 64 architecture. We only need to pay attention to the TSS descriptors
    // and we'll use our expanded descriptor structure to adjust the segment base.
    //
    if ((descriptor->system == 0) &&
        ((descriptor->type == SEGMENT_DESCRIPTOR_TYPE_TSS_AVAILABLE) ||
            (descriptor->type == SEGMENT_DESCRIPTOR_TYPE_TSS_BUSY)))
    {
        struct __segment_descriptor_64_t* expanded_descriptor;
        expanded_descriptor = (struct __segment_descriptor_64_t*)descriptor;
        segment_base |= ((unsigned __int64)expanded_descriptor->base_upper << 32);
    }

    return segment_base;
}