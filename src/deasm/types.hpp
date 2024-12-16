#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <array>

#define hexSdecS(S) std::to_string( std::stoi( S, 0, 16 ) )
#define LASTEL(X)   X[X.size() - 1]

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

template<typename T>
bool iina(T item, T *arr)
{
    for (int i = 0; i < (sizeof(arr)/sizeof(*arr)); i++) {
        if (arr[i] == item) {
            return true;
        }
    }
    return false;
}

std::string signore(std::string str, char ignorec)
{
    std::string res;
    for (char c : str) {
        if (c == ignorec) {
            continue;
        }
        res += c;
    }
    return res;
}

std::string sslise(std::string str, int start, int end)
{
    std::string res;
    for (int i = start; i < end; i++) {
        res += str[i];
    } 
    return res;
}

bool sisd(std::string str)
{
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool sisdHEXH(std::string str)
{
    std::string S = sslise(str, 0, str.size() - 1);
    for (char c : S) {
        if (!isdigit(c) && !((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
            return false;
        }
    }
    return true;
}

char _compiler_separators[] = {
    ' ',
    ';',
    ',',
    '\n'
};

std::map<std::string, u8> _compiler_opcodes = {
    {"mov",    0x00},
    {"movm",   0x0D},
    {"mova",   0x4D},
    {"movr",   0x0E},
    {"add",    0x01},
    {"sub",    0x02},
    {"and",    0x03},
    {"or",     0x04},
    {"xor",    0x05},
    {"not",    0x06},
    {"shl",    0x07},
    {"shr",    0x08},

    {"jmp",    0x09},
    {"jez",    0x0A},
    {"jgz",    0x0B},
    {"jlz",    0x0C},
    
    {"jmpe",    0x49},
    {"jeze",    0x4A},
    {"jgze",    0x4B},
    {"jlze",    0x4C},

    {"push",    0x0F},
    {"pushb",   0x4F},
    {"pop",     0x10},
    {"popb",    0x50},
};

std::map<std::string, u8> _compiler_regs = {
    {"ax",   0x00},
    {"ah",   0x01},
    {"al",   0x02},
    
    {"bx",   0x10},
    {"bh",   0x11},
    {"bl",   0x12},
    
    {"cx",   0x20},
    {"ch",   0x21},
    {"cl",   0x22},
    
    {"dx",   0x30},
    {"dh",   0x31},
    {"dl",   0x32},
    
    {"cs",   0x40},
    {"ss",   0x50},
    {"ds",   0x60},
    {"es",   0x70},
    
    {"ip",   0x80},
    {"rsp",  0x90},
    {"rbp",  0xA0},
};