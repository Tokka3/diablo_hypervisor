#ifndef SEGMENT_DEF
#define SEGMENT_DEF

#pragma warning (disable : 4201)
#pragma pack(push, 1)


struct __pseudo_descriptor_64_t
{
	unsigned __int16 limit;
	unsigned __int64 base_address;
};
#pragma pack(pop)


struct __segment_descriptor_64_t
{
    unsigned __int16 segment_limit_low;
    unsigned __int16 base_low;
    union
    {
        struct
        {
            unsigned __int32 base_middle : 8;
            unsigned __int32 type : 4;
            unsigned __int32 descriptor_type : 1;
            unsigned __int32 dpl : 2;
            unsigned __int32 present : 1;
            unsigned __int32 segment_limit_high : 4;
            unsigned __int32 system : 1;
            unsigned __int32 long_mode : 1;
            unsigned __int32 default_big : 1;
            unsigned __int32 granularity : 1;
            unsigned __int32 base_high : 8;
        };
        unsigned __int32 flags;
    };
    unsigned __int32 base_upper;
    unsigned __int32 reserved;
};

struct __segment_descriptor_32_t
{
  unsigned __int16 segment_limit_low;
  unsigned __int16 base_low;
  union
  {
    struct
    {
      unsigned __int32 base_middle : 8;
      unsigned __int32 type : 4;
      unsigned __int32 descriptor_type : 1;
      unsigned __int32 dpl : 2;
      unsigned __int32 present : 1;
      unsigned __int32 segment_limit_high : 4;
      unsigned __int32 system : 1;
      unsigned __int32 long_mode : 1;
      unsigned __int32 default_big : 1;
      unsigned __int32 granularity : 1;
      unsigned __int32 base_high : 8;
    };
    unsigned __int32 flags;
  };
};

union __segment_selector_t
{
    struct
    {
        unsigned __int16 rpl : 2;
        unsigned __int16 table : 1;
        unsigned __int16 index : 13;
    };
    unsigned __int16 flags;
};


union __segment_access_rights_t
{
    struct
    {
        unsigned __int32 type : 4;
        unsigned __int32 descriptor_type : 1;
        unsigned __int32 dpl : 2;
        unsigned __int32 present : 1;
        unsigned __int32 reserved0 : 4;
        unsigned __int32 available : 1;
        unsigned __int32 long_mode : 1;
        unsigned __int32 default_big : 1;
        unsigned __int32 granularity : 1;
        unsigned __int32 unusable : 1;
        unsigned __int32 reserved1 : 15;
    };
    unsigned __int32 flags;
};
#endif

static unsigned __int32 read_segment_access_rights(unsigned __int16 segment_selector);

static unsigned __int64 get_segment_base(unsigned __int64 gdt_base, unsigned __int16 segment_selector);