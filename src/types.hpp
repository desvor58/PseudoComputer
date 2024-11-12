#include <iostream>
#include <map>

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

static const std::map<std::string, int> declaring_size_derectives {
    {"db", 1},
    {"dw", 2},
    {"dt", 3},
};

static const std::map<std::string, unsigned char> regs {
    {"ax",  0x00},
    {"al",  0x01},
    {"ah",  0x02},

    {"bx",  0x10},
    {"bl",  0x11},
    {"bh",  0x12},

    {"cx",  0x20},
    {"cl",  0x21},
    {"ch",  0x22},

    {"dx",  0x30},
    {"dl",  0x31},
    {"dh",  0x32},

    {"cs",  0x40},
    {"ds",  0x50},
    {"ss",  0x60},

    {"ip",  0x70},
    {"rsp", 0x80},
};

static const std::map<std::string, unsigned char> opcodes {
    {"mov",  0x00},
    {"add",  0x01},
    {"sub",  0x02},
    {"and",  0x03},
    {"or",   0x04},
    {"xor",  0x05},
    {"not",  0x06},
    {"shl",  0x07},
    {"shr",  0x08},

    {"jmp",  0x09},
    {"jez",  0x0A},
    {"jgz",  0x0B},
    {"jlz",  0x0C},

    {"push",  0x0D},
    {"pushb", 0x4D},
    {"pop",   0x0E},
    {"popb",  0x4E},
};