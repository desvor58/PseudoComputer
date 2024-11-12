#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "types.hpp"

std::vector<std::string> split(std::string str, const char splitter);

bool sisd(std::string s);

std::string signore(std::string s, char cign);

std::vector<std::string> sins(std::vector<std::string> v1, std::vector<std::string> v2, int index);

std::vector<std::string> get_file_lines(std::string file_name);

std::map<std::string, unsigned int> lables {};

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cerr << "Error: Expected command of type: deasm <input file name>.[in file extentions] <output file name>.[out file extentions]" << std::endl;
        std::cerr << "[in file extention] .asm, .tbc (old format from PseudoProcessor), .deasm" << std::endl;
        std::cerr << "[out file extention] .pexe and .exe (executeble file for PC), .pvd (load sector pseudo disk)" << std::endl;
        return 1;
    }

    u32 res_size = 0;

    if (argc > 3) {
        res_size = std::stoi(argv[3]);
    }

    std::vector<std::string> strs = get_file_lines(argv[1]);

    char comm[4] {0, 0, 0, 0};
    int comm_index = 0;

    std::vector<char> bytes {};

    int declaring_size = 1;  // in bytes
    
    unsigned int nocompile_lines = 0;

incls:
    int incl_counter = 0;

    // include loop
    for (int i = 0; i < strs.size(); i++) {
        std::string str = strs[i];
        std::vector<std::string> sstr = split(str, ' ');
        
        for (int j = 0; j < sstr.size(); j++) {
            std::string part = sstr[j];

            if (part == "#incl") {
                incl_counter++;
                std::vector<std::string> p1 {};
                std::vector<std::string> p2 {};
                std::vector<std::string> incl_body {};

                for (int k = 0; k < i; k++)
                    p1.push_back(strs[k]);
                for (int k = i + 1; k < strs.size(); k++)
                    p2.push_back(strs[k]);

                incl_body = get_file_lines(sstr[j + 1]);

                strs.clear();

                for (std::string p : p1) strs.push_back(p);
                for (std::string p : incl_body) strs.push_back(p);
                for (std::string p : p2) strs.push_back(p);
            }
            if (part == "#incle") {
                incl_counter++;
                std::vector<std::string> p1 {};
                std::vector<std::string> p2 {};
                std::vector<std::string> incl_body {};

                for (int k = 0; k < i; k++)
                    p1.push_back(strs[k]);
                for (int k = i + 1; k < strs.size(); k++)
                    p2.push_back(strs[k]);

                incl_body = get_file_lines(sstr[j + 1]);

                strs.clear();

                for (std::string p : p1) strs.push_back(p);
                for (std::string p : p2) strs.push_back(p);
                for (std::string p : incl_body) strs.push_back(p);
            }
        }
    }

    if (incl_counter > 0) {
        goto incls;
    }

    // labels loop
    for (int i = 0; i < strs.size(); i++) {
        std::string str = strs[i];
        std::vector<std::string> sstr = split(str, ' ');


        for (int j = 0; j < sstr.size(); j++) {
            std::string part = sstr[j];

            if (part[part.length() - 1] == ':') {
                lables[signore(part, ':')] = i - nocompile_lines;
                nocompile_lines++;
                continue;
            }
            if (part[0] == '~') {
                nocompile_lines++;
                continue;
            }
        }
    }

    // parse loop (convert labels names to numbers)
    for (int i = 0; i < strs.size(); i++) {
        std::string str = strs[i];
        std::vector<std::string> sstr = split(str, ' ');

        for (int j = 0; j < sstr.size(); j++) {
            sstr[j] = signore(sstr[j], ',');
            std::string part = sstr[j];

            if (lables.count(part) > 0) {
                sstr[j] = std::to_string(lables.at(part) * 4);
                continue;
            }
        }

        std::string res_str = "";

        for (std::string p : sstr) {
            res_str += p;
            res_str += ' ';
        }

        strs[i] = res_str;
    }

    for (std::string str : strs) {
        //std::cout << str << std::endl;
    }
    
    // compile loop
    for (int i = 0; i < strs.size(); i++) {
        comm[0] = 0;
        comm[1] = 0;
        comm[2] = 0;
        comm[3] = 0;

        comm_index = 0;

        std::string str = strs[i];
        std::vector<std::string> sstr = split(str, ' ');
        
        declaring_size = 2;

        for (int j = 0; j < sstr.size(); j++) {
            std::string part = sstr[j];
            //std::cout << part << std::endl;

            if (part[0] == ';') {
                goto coment_end;
            }

            if (declaring_size_derectives.count(part) > 0) {
                declaring_size = declaring_size_derectives.at(part);
                continue;
            }
            if (sisd(part)) {
                char np1;
                char np2;

                if (declaring_size == 1) {
                    comm[comm_index] = std::stoi(part);
                    comm_index++;
                    continue;
                }
                if (declaring_size == 2) {
                    int offset = std::stoi(part) / 256;
                    np2 = offset;
                    np1 = std::stoi(part) - offset * 256;

                    comm[comm_index] = np2;
                    comm[++comm_index] = np1;
                    continue;
                }
            }
            if (part[0] == 'h') {
                std::string buf = "";

                for (int k = 1; k < part.size(); k++) {
                    buf += part[k];
                }

                comm[comm_index]   = std::stoi(buf, 0, 16) >> 8;
                comm[++comm_index] = std::stoi(buf, 0, 16);
                continue;
            }
            if (regs.count(part) > 0) {
                comm[comm_index] = regs.at(part);
                comm_index++;
                continue;
            }
            if (opcodes.count(part) > 0) {
                if (j + 2 < sstr.size() || j + 1 < sstr.size()) {
                    if (sisd(sstr[j + 1]) || sisd(sstr[j + 2])
                     || sstr[j + 1][0] == 'h' || sstr[j + 2][0] == 'h') {
                        if (part != "call") {
                            comm[comm_index] = opcodes.at(part) + 128;
                            goto opcodeWRT;
                        }
                    }
                }
                comm[comm_index] = opcodes.at(part);
opcodeWRT:      comm_index++;
                continue;
            }
            else {
                goto end;
            }
        }
coment_end:
        for (char c : comm) {
            bytes.push_back(c);
        }
end:
    }

    if (res_size != 0) {
        for (int i = bytes.size(); i < res_size; i++) {
            bytes.push_back(0x00);
        }
    }

    std::ofstream ofile(argv[2], std::ios::binary);

    ofile.write(&bytes[0], bytes.size());
    ofile.close();

    return 0;
}

std::vector<std::string> split(std::string str, const char splitter)
{
    std::vector<std::string> ss;
    std::string buf;

    for (const char c : str) {
        if (c == splitter) {
            if (buf != "") {
                ss.push_back(buf);
                buf = "";
            }
            continue;
        }
        buf += c;
    }
    ss.push_back(buf);

    std::vector<std::string> res {};

    for (std::string s : ss) {
        if (s.empty()) {
            continue;
        }
        res.push_back(s);
    }

    return res;
}

bool sisd(std::string s)
{
    if (s.empty()) return false;

    for (char c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

std::string signore(std::string s, char cign) {
    std::string res;
    for (char c : s) {
        if (c == cign) continue;
        res += c;
    }

    return res;
}

std::vector<std::string> get_file_lines(std::string file_name)
{
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error: File not be open" << std::endl;
        exit(EXIT_FAILURE);
    }

    char c;
    std::string s;
    std::vector<std::string> strs1 {};

    while (file.get(c)) {
        if (c == '\n') {
            strs1.push_back(s);
            s = "";
            continue;
        }
        s += c;
    }
    strs1.push_back(s);
    file.close();

    std::vector<std::string> strs2 {};

    for (std::string str : strs1) {
        if (str == "") {
            continue;
        }
        strs2.push_back(str);
    }

    return strs2;
}