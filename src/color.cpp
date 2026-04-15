#include "color.hpp"

void Brass::ColorData::print(TokenColor color, string str)
{
    switch (mode)
    {
    case ColorMode::NOCOLOR: {
        std::cout << str;
        break;
    }
    case ColorMode::COLOR16: {
        std::cout << "\e[1;" << colorTable.at(color).code16 << "m" << str << "\e[0m";
        break;
    }
    case ColorMode::COLOR256: {
        std::cout << "\e[1;38;5;" << colorTable.at(color).code256 << "m" << str << "\e[0m";
        break;
    }
    }
}

#ifdef _WIN32
#include <windows.h>
Brass::ColorData::ColorData(bool quiet)
{
    if (quiet) {
        mode = ColorMode::NOCOLOR;
    }
    else {
        const char* vscodeTerm = getenv("TERM_PROGRAM");
        bool isVSCode = vscodeTerm && std::string(vscodeTerm) == "vscode";

        if (isVSCode) {
            mode = ColorMode::COLOR256;
        }
        else {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            bool vtEnabled = false;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                vtEnabled = SetConsoleMode(hOut, dwMode);
            }
            mode = vtEnabled ? ColorMode::COLOR256 : ColorMode::COLOR16;
        }
    }
}
#else
Brass::ColorData::ColorData(bool quiet)
{
    if (quiet) {
        mode = ColorMode::NOCOLOR;
    }
    else {
        const char* vscodeTerm = getenv("TERM_PROGRAM");
        bool isVSCode = vscodeTerm && std::string(vscodeTerm) == "vscode";

        if (isVSCode) {
            mode = ColorMode::COLOR256;
        }
        else {
            const char* term = getenv("TERM");
            bool supports256 = term && std::string(term).find("256color") != std::string::npos;
            if (supports256) {
                mode = ColorMode::COLOR256;
            }
            else {
                mode = ColorMode::COLOR16;
            }
        }
    }
}
#endif