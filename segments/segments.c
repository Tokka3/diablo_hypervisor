#include "segments.h"
#include "../headers/includes.h"
#include "../asm/vmm_intrin.h"
#include "../headers/ia32_defs.h"
#pragma warning (disable : 4333)

#define MASK_GET_HIGHER_32BITS(_ARG_)(_ARG_ & 0xffffffff00000000)
#define MASK_GET_LOWER_32BITS(_ARG_)(_ARG_ & 0xffffffff)
#define MASK_GET_LOWER_16BITS(_ARG_)(_ARG_ & 0xffff)
#define MASK_GET_LOWER_8BITS(_ARG_)(_ARG_ & 0xff)
#define MASK_32BITS 0xffffffff

unsigned __int64 get_segment_base(unsigned __int8* gdt_base, unsigned __int16 selector)
{
	struct __segment_descriptor* segment_descriptor;

	segment_descriptor = (struct __segment_descriptor*)(gdt_base + (selector & ~0x7));

	unsigned __int64 segment_base = segment_descriptor->base_low | segment_descriptor->base_middle << 16 | segment_descriptor->base_high << 24;

	if (segment_descriptor->descriptor_type == 0)
		segment_base = (segment_base & MASK_32BITS) | (unsigned __int64)segment_descriptor->base_upper << 32;

	return segment_base;
}

void fill_guest_selector_data(void* gdt_base, unsigned __int32 segment_register, unsigned __int16 selector)
{
	union __segment_access_rights_t segment_access_rights;
	struct __segment_descriptor* segment_descriptor;

	if (selector & 0x4)
		return;

	segment_descriptor = (struct __segment_descriptor*)((unsigned __int8*)gdt_base + (selector & ~0x7));

	unsigned __int64 segment_base = segment_descriptor->base_low | segment_descriptor->base_middle << 16 | segment_descriptor->base_high << 24;

	unsigned __int32 segment_limit = segment_descriptor->limit_low | (segment_descriptor->segment_limit_high << 16);

	//
	// Load ar get access rights of descriptor specified by selector
	// Lower 8 bits are zeroed so we have to bit shift it to right by 8
	//
	segment_access_rights.all = __load_ar(selector) >> 8;
	segment_access_rights.unusable = 0;
	segment_access_rights.reserved0 = 0;
	segment_access_rights.reserved1 = 0;

	// This is a TSS or callgate etc, save the base high part
	if (segment_descriptor->descriptor_type == 0)
		segment_base = (segment_base & MASK_32BITS) | (unsigned __int64)segment_descriptor->base_upper << 32;

	if (segment_descriptor->granularity == 1)
		segment_limit = (segment_limit << 12) + 0xfff;

	if (selector == 0)
		segment_access_rights.all |= 0x10000;

	__vmx_vmwrite(GUEST_ES_SELECTOR + segment_register * 2, selector);
	__vmx_vmwrite(GUEST_ES_LIMIT + segment_register * 2, segment_limit);
	__vmx_vmwrite(GUEST_ES_BASE + segment_register * 2, segment_base);
	__vmx_vmwrite(GUEST_ES_ACCESS_RIGHTS + segment_register * 2, segment_access_rights.all);
}

 unsigned __int32 read_segment_access_rights(unsigned __int16 segment_selector)
{
	union __segment_selector selector;
	union __segment_access_rights_t vmx_access_rights;

	selector.all = segment_selector;

	//
	// Check for null selector use, if found set access right to unusable
	// and return. Otherwise, get access rights, modify format, return the
	// segment access rights.
	//
	if (selector.ti == 0
		&& selector.index == 0)
	{
		vmx_access_rights.all = 0;
		vmx_access_rights.unusable = TRUE;
		return vmx_access_rights.all;
	}

	//
	// Use our custom intrinsic to store our access rights, and
	// remember that the first byte of the access rights returned
	// are not used in VMX access right format.
	//
	vmx_access_rights.all = (__load_ar(segment_selector) >> 8);
	vmx_access_rights.unusable = 0;
	vmx_access_rights.reserved0 = 0;
	vmx_access_rights.reserved1 = 0;

	return vmx_access_rights.all;
}