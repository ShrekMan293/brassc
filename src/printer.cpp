#include "printer.hpp"
#include "token.hpp"
#include "magic_enum/magic_enum.hpp"
#include <algorithm>
#include <cctype>

namespace Brass {
    void BrassPrinter::print(string msg) {
        std::cout << msg;
    }
    void BrassPrinter::print(char msg)
    {
        std::cout << msg;
    }

    void BrassPrinter::print_white(string msg)
    {
        if (!color) print(msg);
        std::cout << "\e[0;37m" << msg << "\e[0m";
    }

    void BrassPrinter::print_white_bold(string msg)
    {
        if (!color) print(msg);
        std::cout << "\e[1;37m" << msg << "\e[0m";
    }

    bool ishex(char c) {
        c = std::tolower(c);

        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    void BrassPrinter::print_code(string code) {
        string buffer = "";

        for (size_t i = 0; i < code.length(); i++) {
            if (isalpha(code[i])) {
                while (isalnum(code[i]) || code[i] == '_')
                    buffer += code[i++];

                string copy = string(buffer);
                std::transform(copy.begin(), copy.end(), copy.begin(), [](unsigned char c){ return std::toupper(c);});
                auto type = magic_enum::enum_cast<TokenType>(copy);

                if (type.has_value()) {
                    if (type.value() >= TokenType::AND && type.value() <= TokenType::THROWS) {
                        if (type.value() >= TokenType::U8 && type.value() <= TokenType::F64) {
                            colorData.print(TokenColor::Type, buffer);
                            buffer = "";
                            i--;
                            continue;
                        }
                        else {
                            colorData.print(TokenColor::Keyword, buffer);
                            buffer = "";
                            i--;
                            continue;
                        }
                    }
                }

                colorData.print(TokenColor::Identifier, buffer);
                buffer = "";
                i--;
                continue;
            }
            else {
                if (code[i] == '"' || code.substr(i, 2) == "$\"") {
                    size_t length = code[i] == '$' ? 2 : 1;
                    while (code[i + length] != '"' && i + length < code.length())
                        length++;
                    length++;
                    buffer = code.substr(i, length);
                    i += length - 1;
                    colorData.print(TokenColor::Literal, buffer);
                    buffer = "";
                    continue;
                }
                else if (code[i] == '\'') {
                    size_t length = 1;
                    while (code[i + length] != '\'' && i + length < code.length())
                        length++;
                    length++;
                    buffer = code.substr(i, length);
                    i += length - 1;
                    colorData.print(TokenColor::Literal, buffer);
                    buffer = "";
                    continue;
                }
                else if (isdigit(code[i])) {
                    if (code[i + 1] == 'b') {
                        size_t length = 0;
                        while ((code[i + length] == '0' || code[i + length] == '0') && i + length < code.length())
                            length++;
                        buffer = code.substr(i, length);
                        i += length - 1;
                        colorData.print(TokenColor::Literal, buffer);
                        buffer = "";
                        continue;
                    }
                    else if (code[i + 1] == 'x') {
                        size_t length = 0;
                        while (ishex(code[i + length]) && i + length < code.length())
                            length++;
                        buffer = code.substr(i, length);
                        i += length - 1;
                        colorData.print(TokenColor::Literal, buffer);
                        buffer = "";
                        continue;
                    }
                    else {
                        bool hasDot = false;
                        size_t length = 0;
                        while ((isdigit(code[i + length]) || (code[i + length] == '.' && !hasDot)) && i + length < code.length()) {
                            if (code[i] == '.') hasDot = true;
                            length++;
                        }
                        buffer = code.substr(i, length);
                        i += length - 1;
                        colorData.print(TokenColor::Literal, buffer);
                        buffer = "";
                        continue;
                    }
                }
                string data = "";
                data += code[i];
                colorData.print(TokenColor::Normal, data);
            }
        }
    }

    string BrassPrinter::getCode(string file, int line)
    {
        BrassFile& bFile = sourceData.at(file);
        
        size_t lineStart = bFile.lineStarts[line - 1];
        int length = 0;
        for (; bFile.source[lineStart + length] != '\n'; length++);

        return bFile.source.substr(lineStart, length);
    }

    void BrassPrinter::error(string file, string msg, int line, int column)
    {
        string code = getCode(file, line);

        int length = 0;
        if (isalpha(code[column - 1])) {
            while (isalnum(code[(column - 1) + length]) || code[(column - 1) + length] == '_') length++;
        }
        else {
            while (code[(column - 1) + length] != ' ' && 
                !(isalnum(code[(column - 1) + length]) || code[(column - 1) + length] == '_')) length++;
        }

        print_white_bold(file);
        print_white_bold(":");
        print_white_bold(std::to_string(line));
        print_white_bold(":");
        print_white_bold(std::to_string(column));
        print_white_bold(": ");
        colorData.print(TokenColor::Error, "ERROR: ");
        colorData.print(TokenColor::Error, msg);

        print("\n   " + std::to_string(line) + " | ");
        print_code(code);
        print("\n     | ");
        for (int i = 0; i < column - 1; i++) print(" ");
        for (int i = 0; i < length; i++) 
            colorData.print(TokenColor::Error, "^");
        print("\n");
    }

    void BrassPrinter::warning(string file, string msg, int line, int column)
    {
        string code = getCode(file, line);

        int length = 0;
        if (isalpha(code[column - 1])) {
            while (isalnum(code[(column - 1) + length]) || code[(column - 1) + length] == '_') length++;
        }
        else {
            while (code[(column - 1) + length] != ' ') length++;
        }

        print_white_bold(file);
        print_white_bold(":");
        print_white_bold(std::to_string(line));
        print_white_bold(":");
        print_white_bold(std::to_string(column));
        print_white_bold(": ");
        colorData.print(TokenColor::Warning, "WARNING: ");
        colorData.print(TokenColor::Warning, msg);

        print("\n   " + std::to_string(line) + " | ");
        print_code(code);
        print("\n     | ");
        for (int i = 0; i < column - 1; i++) print(" ");
        for (int i = 0; i < length; i++) 
            colorData.print(TokenColor::Warning, "^");
        print("\n");
    }

    void BrassPrinter::note(string file, string msg, int line, int column)
    {
        if (quiet) return;

        string code = getCode(file, line);

        int length = 0;
        if (isalpha(code[column - 1])) {
            while (isalnum(code[(column - 1) + length]) || code[(column - 1) + length] == '_') length++;
        }
        else {
            while (code[(column - 1) + length] != ' ') length++;
        }

        print_white_bold(file);
        print_white_bold(":");
        print_white_bold(std::to_string(line));
        print_white_bold(":");
        print_white_bold(std::to_string(column));
        print_white_bold(": ");
        colorData.print(TokenColor::Info, "NOTE: ");
        colorData.print(TokenColor::Info, msg);

        print("\n   " + std::to_string(line) + " | ");
        print_code(code);
        print("\n     | ");
        for (int i = 0; i < column - 1; i++) print(" ");
        for (int i = 0; i < length; i++) 
            colorData.print(TokenColor::Info, "^");
        print("\n");
    }
    BrassPrinter::BrassPrinter(std::map<string, BrassFile>& sources, bool quiet, bool color) : sourceData(sources), colorData(ColorData(quiet)) {
        this->quiet = quiet;
        this->color = color;
    }
}