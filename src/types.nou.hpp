#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

enum class TokenType
{
    _comm,
    _reg,
    _num_dec,
    _num_hex,
    _derive,
    _label,
    _label_use,
    _NL,
};

class Token
{
public:
    TokenType type;
    std::string val;

    Token(TokenType type, std::string val = "")
    {
        this->type = type;
        this->val = val;
    }
};

typedef std::vector<Token> vectok;

std::ostream &operator<<(std::ostream &stream, const Token &token)
{
    stream << "type: ";
    switch (token.type)
    {
    case TokenType::_comm:
        stream << "comm";
        break;
    case TokenType::_reg:
        stream << "reg";
        break;
    case TokenType::_num_dec:
        stream << "num_dec";
        break;
    case TokenType::_num_hex:
        stream << "num_hex";
        break;
    case TokenType::_derive:
        stream << "derive";
        break;
    case TokenType::_label:
        stream << "label";
        break;
    case TokenType::_label_use:
        stream << "label_use";
        break;
    case TokenType::_NL:
        stream << "NL";
        break;
        
    default: break;
    }
    stream << ";" << '\t';

    stream << "val: " << token.val;
    return stream;
}

static std::map<std::string, u32> labels {};

std::map<std::string, u8> comm_opcodes = {
    {"mov", 0x00},
    {"add", 0x01},
    {"sub", 0x02},
    {"and", 0x03},
    {"or",  0x04},
    {"xor", 0x05},
    {"not", 0x06},
    {"shl", 0x07},
    {"shr", 0x08},
    {"jmp", 0x09},
    {"jez", 0x0A},
    {"jgz", 0x0B},
    {"jlz", 0x0C},
    {"cmov", 0x00},
    {"cadd", 0x01},
    {"csub", 0x02},
    {"cand", 0x03},
    {"cor",  0x04},
    {"cxor", 0x05},
    {"cnot", 0x06},
    {"cshl", 0x07},
    {"cshr", 0x08},
};

std::map<std::string, u8> reg_opcodes = {
    {"ax", 0x00},
    {"al", 0x01},
    {"ah", 0x02},
    
    {"bx", 0x10},
    {"bl", 0x11},
    {"bh", 0x12},
    
    {"cx", 0x20},
    {"cl", 0x21},
    {"ch", 0x22},
    
    {"dx", 0x30},
    {"dl", 0x31},
    {"dh", 0x32},

    {"cs", 0x40},
    {"ds", 0x50},
    {"ss", 0x60},

    {"ip",  0x70},
    {"rsp", 0x80},
};

enum class ErrType
{
    _ExpectedReg,
    _ExpectedNum,
    _ExpectedlabelName,
    _ExpectedNumOrReg,
    _ExpectedNumOrlabelName,
};

std::map<ErrType, std::string> errors = {
    {ErrType::_ExpectedReg,             "Expected register name"},
    {ErrType::_ExpectedNum,             "Expected number"},
    {ErrType::_ExpectedlabelName,       "Expected label name"},
    {ErrType::_ExpectedNumOrReg,        "Expected number or reg name"},
};

void puterr(u32 line, ErrType error_type)
{
    std::cerr << "Error [line: " << line << "]: " << errors[error_type] << std::endl;
    exit(EXIT_FAILURE);
}

void stdcomm(int *i, vectok *tokens, u32 line, Token token, Token token2, Token token3, std::string comm)
{
    if (token.val == comm) {
        if (token2.type == TokenType::_reg) {
            if (token3.type == TokenType::_reg) {
                tokens->push_back(Token(TokenType::_comm, comm));
            }
            else if (token3.type == TokenType::_num_dec
                  || token3.type == TokenType::_num_hex) {
                tokens->push_back(Token(TokenType::_comm, "c" + comm));
            }
            else if (token3.type == TokenType::_label) {
                tokens->push_back(Token(TokenType::_comm, "c" + comm));
                tokens->push_back(Token(TokenType::_reg, token2.val));
                tokens->push_back(Token(TokenType::_label_use, std::to_string(labels[token3.val])));
                *i += 2;
                return;
            }
            else {
                puterr(line, ErrType::_ExpectedNumOrReg);
            }
            tokens->push_back(Token(TokenType::_reg, token2.val));
            tokens->push_back(Token(token3.type, token3.val));

            *i += 2;
            return;
        }
        puterr(line, ErrType::_ExpectedReg);
    }
}