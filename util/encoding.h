#ifndef ENCODING
#define ENCODING


enum __vmcs_access_e
{
    full = 0,
    high = 1
};

enum __vmcs_type_e
{
    control = 0,
    vmexit,
    guest,
    host
};

enum __vmcs_width_e
{
    word = 0,
    quadword,
    doubleword,
    natural
};

#define VMCS_ENCODE_COMPONENT( access, type, width, index )    ( unsigned )( ( unsigned short )( access ) | \
                                                                        ( ( unsigned short )( index ) << 1 ) | \
                                                                        ( ( unsigned short )( type ) << 10 ) | \
                                                                        ( ( unsigned short )( width ) << 13 ) )

#define VMCS_ENCODE_COMPONENT_FULL( type, width, index )    VMCS_ENCODE_COMPONENT( full, type, width, index )

#define VMCS_ENCODE_COMPONENT_HIGH( type, width, index )    VMCS_ENCODE_COMPONENT( high, type, width, index )

#define VMCS_ENCODE_COMPONENT_FULL_16( type, index )        VMCS_ENCODE_COMPONENT_FULL( type, word, index )
#define VMCS_ENCODE_COMPONENT_FULL_32( type, index )        VMCS_ENCODE_COMPONENT_FULL( type, doubleword, index )
#define VMCS_ENCODE_COMPONENT_FULL_64( type, index )        VMCS_ENCODE_COMPONENT_FULL( type, quadword, index )

#define VMCS_ENCODE_COMPONENT_HIGH_16( type, index )        VMCS_ENCODE_COMPONENT_HIGH( type, word, index )
#define VMCS_ENCODE_COMPONENT_HIGH_32( type, index )        VMCS_ENCODE_COMPONENT_HIGH( type, doubleword, index )
#define VMCS_ENCODE_COMPONENT_HIGH_64( type, index )        VMCS_ENCODE_COMPONENT_HIGH( type, quadword, index )

#endif // !ENCODING