#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "types.hpp"

static std::map<std::string, u32> labels = {};
static std::map<std::string, u32> segments = {};
static int segments_index = 0;

std::vector<std::string> split(std::string text)
{
    std::vector<std::string> tkns {};
    std::string buf = "";

    for (int i = 0; i < text.size(); i++) {
        if (text[i] == '"') {
            buf = "\"";
            for (int j = ++i; text[j] != '"'; j++, i++) {
                buf += text[j];
            }
            buf += "\"";
            tkns.push_back(buf);
            buf = "";
            continue;
        }
        if (iina(text[i], _compiler_separators) || i == text.size() - 1) {
            if (i == text.size() - 1 && text[i] != '\n') {
                buf += text[i];
            }
            if (!buf.empty()) {
                tkns.push_back(buf);
                buf = "";
            }
        }
        else {
            buf += text[i];
        }
        if (text[i] == '\n' || i == text.size() - 1) {
            tkns.push_back("__NL__");
        }
    }

    return tkns;
}

std::vector<std::string> parse(std::vector<std::string> tkns)
{
    std::vector<std::string> res;
    u32 byte_counter = 0;
    bool fill = false;
    u8 to_bytec = 0;

    for (int i = 0; i < tkns.size(); i++) {
        std::string tkn = tkns[i];

        if (tkn == "ends") {
            byte_counter = 0;
        }

        if (tkn[0] == '"' && LASTEL(tkn) == '"') {
            byte_counter += tkn.size() - 2;
        }
        if ((LASTEL(tkn) == 'h' && sisdHEXH(tkn)) || sisd(tkn) || labels.count(tkns[i]) > 0) {
            byte_counter += 2;
        }
        if (LASTEL(tkn) == 'b' && sisd(sslise(tkn, 0, tkns.size() - 1))) {
            byte_counter += 1;
        }
        if (_compiler_opcodes.count(tkn) > 0) {
            byte_counter += 4;
            for (; tkns[i] != "__NL__"; i++) {}
        }
        if (LASTEL(tkn) == ':') {
            labels[signore(tkn, ':')] = byte_counter;
            continue;
        }
        if (tkns[i] == "segment") {
            segments[tkns[++i]] = segments_index++;
            continue;
        }
    }

    for (int i = 0; i < tkns.size(); i++) {
        if (labels.count(tkns[i]) > 0) {
            tkns[i] = std::to_string(labels[tkns[i]]);
            continue;
        }
        if (segments.count(tkns[i]) > 0) {
            if (tkns[i - 1] != "segment") {
                tkns[i] = std::to_string(segments[tkns[i]]);
                continue;
            }
        }
    }

    for (int i = 0; i < tkns.size(); i++) {
        std::string tkn = tkns[i];

        if (tkn == "ends") {
            res.push_back(tkn);
            continue;
        }

        if (tkn == "segment") {
            res.push_back(tkn);
            res.push_back(tkns[++i]);
        }

        if (tkn == "__NL__") {
            res.push_back("__NL__");
            continue;
        }
        if (tkn[0] == '"' && LASTEL(tkn) == '"') {
            res.push_back(tkn);
            continue;
        }
        if (LASTEL(tkn) == 'h' && sisdHEXH(tkn)) {
            res.push_back(hexSdecS(sslise(tkn, 0, tkn.size() - 1)));
            continue;
        }
        if (LASTEL(tkn) == 'b' && sisd(sslise(tkn, 0, tkn.size() - 1))) {
            res.push_back(tkn);
            continue;
        }
        if (sisd(tkn)) {
            res.push_back(tkn);
            continue;
        }
        if (_compiler_regs.count(tkn) > 0) {
            res.push_back(tkn);
            continue;
        }
        if (_compiler_opcodes.count(tkn) > 0) {
            if (
                (sisd(tkns[i + 1])
                    || sisd(tkns[i + 2])
                    || (LASTEL(tkns[i + 1]) == 'h' && sisdHEXH(tkns[i + 1]))
                    || (LASTEL(tkns[i + 2]) == 'h' && sisdHEXH(tkns[i + 2]))
                    || (LASTEL(tkns[i + 1]) == 'b' && sisd(sslise(tkns[i + 1], 0, tkns[i + 1].size() - 1)))
                    || (LASTEL(tkns[i + 2]) == 'b' && sisd(sslise(tkns[i + 2], 0, tkns[i + 2].size() - 1)))
                )
             && (tkn != "pop" || tkn != "popb")
               ) {
                res.push_back('c' + tkn);
                continue;
            }
            res.push_back(tkn);
            continue;
        }
    }

    return res;
}

std::vector<char> byte_generator(std::vector<std::string> tkns, u32 add_to_size)
{
    std::vector<char> bytes {};

    u16 byte_counter = 0;
    bool fill = false;

    for (int i = 0; i < tkns.size(); i++) {
        if (tkns[i] == "ends") {
            bytes.push_back(0xFF);
            bytes.push_back(0xAA);
            bytes.push_back(0x00);
            bytes.push_back(0x00);
        }

        if (tkns[i] == "__NL__") {
            if (fill) {
                for (;byte_counter < 4; byte_counter++) {
                    bytes.push_back(0x00);
                }
                byte_counter = 0;
                fill = false;
                continue;
            }
        }
        if (LASTEL(tkns[i]) == 'b' && sisd(sslise(tkns[i], 0, tkns[i].size() - 1))) {
            bytes.push_back(std::stoi(sslise(tkns[i], 0, tkns[i].size() - 1)));
            byte_counter++;
            continue;
        }
        if (_compiler_opcodes.count(tkns[i]) > 0) {
            bytes.push_back(_compiler_opcodes[tkns[i]]);
            byte_counter++;
            fill = true;
            continue;
        }
        if (_compiler_opcodes.count(signore(tkns[i], 'c')) > 0) {
            bytes.push_back(_compiler_opcodes[signore(tkns[i], 'c')] + 0x80);
            byte_counter++;
            fill = true;
            continue;
        }
        if (_compiler_regs.count(tkns[i]) > 0) {
            bytes.push_back(_compiler_regs[tkns[i]]);
            byte_counter++;
            continue;
        }
        if (sisd(tkns[i])) {
            bytes.push_back((char)(std::stoi(tkns[i]) >> 8));
            bytes.push_back((char)std::stoi(tkns[i]));
            byte_counter += 2;
            continue;
        }
        if (tkns[i][0] == '"') {
            for (int j = 1; j < tkns[i].size() - 1; j++) {
                bytes.push_back(tkns[i][j]);
            }
            byte_counter += tkns[i].size() - 2;
            continue;
        }
    }
    
    for (int i = bytes.size(); i < add_to_size; i++) {
        bytes.push_back(0x00);
    }

    return bytes;
}

class Args
{
public:
    std::string  infile_name     = "";
    std::string  outfile_name    = "";
    int          addtosize       = 0;
    bool         dbg_tokens_log  = false;
    bool         dbg_labels_log  = false;

    Args(int argc, char** argv)
    {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-i") == 0) {
                infile_name = argv[++i];
            }
            if (strcmp(argv[i], "-o") == 0) {
                outfile_name = argv[++i];
            }
            if (strcmp(argv[i], "-ATS") == 0) {
                addtosize = std::atoi(argv[++i]);
            }
            if (strcmp(argv[i], "-DBGTOKLOG") == 0) {
                dbg_tokens_log = true;
            }
            if (strcmp(argv[i], "-DBGLBLLOG") == 0) {
                dbg_labels_log = true;
            }
        }

        if (infile_name.empty()) {
            std::cerr << "Error: Expected input file name" << std::endl;
            exit(1);
        }
        if (outfile_name.empty()) {
            std::cerr << "Error: Expected output file name" << std::endl;
            exit(1);
        }
    }
};

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cerr << "DEASM (TBC) v4.1.1" << std::endl;
        std::cerr << "deasm <input.asm> <output.pd> [add to size]";
        return 1;
    }

    Args args(argc, argv);

    std::ifstream ifile(args.infile_name);
    if (!ifile.is_open()) {
        std::cerr << "File not de open" << std::endl;
        return 1;
    }
    std::string text = "";
    char c;
    while(ifile.get(c)) {
        text += c;
    }
    ifile.close();

    std::vector<std::string> tkns = split(text);

    
    if (args.dbg_tokens_log) {
        std::cout << "tokens:" << std::endl;
        for (std::string tkn : tkns) {
            std::cout << tkn << std::endl;
        }
    }

    tkns = parse(tkns);

    if (args.dbg_tokens_log) {
        std::cout << std::endl << "parsed tokens:" << std::endl << std::endl;
        for (std::string tkn : tkns) {
            std::cout << tkn << std::endl;
        }
    }

    if (args.dbg_labels_log) {
        std::cout << std::endl << "labels:" << std::endl << std::endl;
        for (auto label : labels) {
            std::cout << label.first << ":\t" << label.second << std::endl;
        }
    }

    std::vector<char> bytes = byte_generator(tkns, args.addtosize);

    std::ofstream ofile(args.outfile_name, std::ios::binary);
    if (!ofile.is_open()) {
        std::cerr << "Output file not be open";
        exit(EXIT_FAILURE);
    }
    
    ofile.write(&bytes[0], bytes.size());
    ofile.close();

    return 0;
}