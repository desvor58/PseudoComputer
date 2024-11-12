#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "types.hpp"
#include <string>

void buf_analis(std::string buf, vectok *tokens)
{
    if (comm_opcodes.count(buf) > 0) {
        tokens->push_back(Token(TokenType::_comm, buf));
        return;
    }
    if (reg_opcodes.count(buf) > 0) {
        tokens->push_back(Token(TokenType::_reg, buf));
        return;
    }
}

vectok tokenization(std::string text)
{
    std::string buf {};
    vectok tokens {};

    char c;
    char cn = ' ';

    bool hex_digit = false;

    for (int i = 0; i < text.size(); i++) {
        c = text[i];
        if (i < text.size() - 1) cn = text[i + 1];

        // if (c == ':') {
        //     tokens.push_back(Token(TokenType::_label, buf));
        //     continue;
        // }
        if (c == '\n') {
            tokens.push_back(Token(TokenType::_NL));
            continue;
        }

        if (c == 'h') {
            for (i++; isdigit(text[i]) || ((text[i] >= 'A' && text[i] <= 'F') || (text[i] >= 'a' && text[i] <= 'f')); i++) {
                buf += text[i];
            }
            tokens.push_back(Token(TokenType::_num_hex, buf));
            buf = "";
            continue;
        }

        if (isalpha(c)) {
            buf += c;
            if (!isalpha(cn)) {
                if (isdigit(cn)) {
                    for (++i; isdigit(text[i]) || isalpha(text[i]); i++) {
                        buf += text[i];
                    }
                    buf_analis(buf, &tokens);
                    buf = "";
                    continue;
                }
                if (cn == ':') {
                    tokens.push_back(Token(TokenType::_label, buf));
                    buf = "";
                    continue;
                }
                buf_analis(buf, &tokens);
                buf = "";
                continue;
            }
        }
        if (isdigit(c)) {
            buf += c;
            if (!isdigit(cn) || cn == '\n' || i == text.size() - 1) {
                tokens.push_back(Token(TokenType::_num_dec, buf));
                buf = "";
                continue;
            }
        }
    }

    return tokens;
}

vectok parse(vectok pre_tokens)
{
    vectok tokens {};

    u32 line = 1;

    u32 bytes = 0;

    for (int i = 0; i < pre_tokens.size(); i++) {
        Token token = pre_tokens[i];

        if (token.type == TokenType::_NL) {
            line++;
            continue;
        }

        if (token.type == TokenType::_label) {
            labels[token.val] = bytes;
        }

        if (token.type == TokenType::_num_hex
         || token.type == TokenType::_num_dec
         || token.type == TokenType::_derive) {
            tokens.push_back(token);
            continue;
        }

        if (token.type == TokenType::_reg) {
            tokens.push_back(token);
            bytes += 1;
            continue;
        }

        if (token.type == TokenType::_num_dec
         || token.type == TokenType::_num_hex) {
            tokens.push_back(token);
            bytes += 2;
            continue;
        }

        if (token.type == TokenType::_comm) {
            bytes += 4;

            tokens.push_back(token);

        //     stdcomm(&i, &tokens, line, token, pre_tokens[i + 1], pre_tokens[i + 2], "mov");
        //     stdcomm(&i, &tokens, line, token, pre_tokens[i + 1], pre_tokens[i + 2], "add");
        //     stdcomm(&i, &tokens, line, token, pre_tokens[i + 1], pre_tokens[i + 2], "sub");
        //     stdcomm(&i, &tokens, line, token, pre_tokens[i + 1], pre_tokens[i + 2], "and");
        //     stdcomm(&i, &tokens, line, token, pre_tokens[i + 1], pre_tokens[i + 2], "or");
        //     stdcomm(&i, &tokens, line, token, pre_tokens[i + 1], pre_tokens[i + 2], "xor");
        //     stdcomm(&i, &tokens, line, token, pre_tokens[i + 1], pre_tokens[i + 2], "not");
        //     stdcomm(&i, &tokens, line, token, pre_tokens[i + 1], pre_tokens[i + 2], "shl");
        //     stdcomm(&i, &tokens, line, token, pre_tokens[i + 1], pre_tokens[i + 2], "shr");
        }
    }

    return tokens;
}

std::vector<u8> byte_generator(vectok tokens)
{
    std::vector<u8> bytes {};
    std::string buf = "";

    int comm_fill_counter = 0;

    for (int i = 0; i < tokens.size(); i++) {
        Token token = tokens[i];

        switch (token.type)
        {
        case TokenType::_comm:
            if (comm_fill_counter > 0) {
                for (; comm_fill_counter > 0; comm_fill_counter--) {
                    bytes.push_back(0x00);
                }
            }

            comm_fill_counter = 4;

            if (token.val[0] == 'c') {
                for (int j = 1; j < token.val.size(); j++) {
                    buf += token.val[j];
                }
                bytes.push_back(comm_opcodes[buf] + 0x80);
                buf += "";
                break;
            }
            bytes.push_back(comm_opcodes[buf]);
            break;
        case TokenType::_label_use:
            bytes.push_back(std::stoi(token.val) >> 8);
            bytes.push_back(std::stoi(token.val));
            break;
        case TokenType::_reg:
            bytes.push_back(reg_opcodes[token.val]);
            break;
        case TokenType::_num_dec:
            bytes.push_back(std::stoi(token.val) >> 8);
            bytes.push_back(std::stoi(token.val));
            comm_fill_counter--;
            break;
        case TokenType::_num_hex:
            bytes.push_back(std::stoi(token.val, 0, 16) >> 8);
            bytes.push_back(std::stoi(token.val, 0, 16));
            comm_fill_counter--;
            break;
        
        default: continue;
        }
        if (comm_fill_counter > 0) {
            comm_fill_counter--;
        }
    }
    return bytes;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cerr << "Error: Expected command of type: deasm <input file name>.[in file extentions] <output file name>.[out file extentions]" << std::endl;
        std::cerr << "[in file extention] .asm, .tbc (old format from PseudoProcessor), .deasm" << std::endl;
        std::cerr << "[out file extention] .pexe and .exe (executeble file for PC), .pvd (load sector pseudo disk)" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    char c;
    std::string text;
    while (file.get(c)) {
        text += c;
    }

    vectok tokens = tokenization(text);

    for (Token tok : tokens) {
        std::cout << tok << std::endl;
    }

    tokens = parse(tokens);

    std::cout << std::endl;
    for (Token tok : tokens) {
        std::cout << tok << std::endl;
    }

    for (auto label : labels) {
        std::cout << label.first << '\t' << label.second << std::endl;
    }

    std::vector<u8> bytes = byte_generator(tokens);
    std::cout << "BG END" << std::endl;

    for (u8 byte : bytes) {
        std::cout << +byte << ' ';
    }
}