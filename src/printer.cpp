#include "printer.hpp"

namespace Brass {
    void BrassPrinter::print(string msg) {

    }
    void BrassPrinter::print_red(string msg)
    {
        std::cout << "\e[0;31m" << msg << "\e[0m";
    }

    void BrassPrinter::print_red_bold(string msg)
    {
        std::cout << "\e[1;31m" << msg << "\e[0m";
    }

    void BrassPrinter::print_green(string msg)
    {
        std::cout << "\e[0;32m" << msg << "\e[0m";
    }

    void BrassPrinter::print_green_bold(string msg)
    {
        std::cout << "\e[1;32m" << msg << "\e[0m";
    }

    void BrassPrinter::print_yellow(string msg)
    {
        std::cout << "\e[0;33m" << msg << "\e[0m";
    }

    void BrassPrinter::print_yellow_bold(string msg)
    {
        std::cout << "\e[1;31m" << msg << "\e[0m";
    }

    void BrassPrinter::print_blue(string msg)
    {
        std::cout << "\e[0;34m" << msg << "\e[0m";
    }

    void BrassPrinter::print_blue_bold(string msg)
    {
        std::cout << "\e[1;34m" << msg << "\e[0m";
    }

    void BrassPrinter::print_magenta(string msg)
    {
        std::cout << "\e[0;35m" << msg << "\e[0m";
    }

    void BrassPrinter::print_magenta_bold(string msg)
    {
        std::cout << "\e[1;35m" << msg << "\e[0m";
    }

    void BrassPrinter::print_cyan(string msg)
    {
        std::cout << "\e[0;36m" << msg << "\e[0m";
    }

    void BrassPrinter::print_cyan_bold(string msg)
    {
        std::cout << "\e[1;36m" << msg << "\e[0m";
    }

    void BrassPrinter::print_white(string msg)
    {
        std::cout << "\e[0;37m" << msg << "\e[0m";
    }

    void BrassPrinter::print_white_bold(string msg)
    {
        std::cout << "\e[1;37m" << msg << "\e[0m";
    }

    void BrassPrinter::print_code(string code) {

    }

    void BrassPrinter::error(string file, string msg, int line, int column)
    {
        string code = "";
        int length = 0;
        for (; code[length + column] != ' '; length++);

        print_white_bold(file);
        print_white_bold(":");
        print_white_bold(std::to_string(line));
        print_white_bold(":");
        print_white_bold(std::to_string(column));
        print_white_bold(": ");
        print_red_bold("ERROR: ");
        print_red_bold(msg);

        print("\n   " + std::to_string(line) + " | ");
        print_code(code);
        print("\n      | ");
        for (int i = 0; i < column; i++) print(" ");
        for (int i = 0; i < length; i++) print_red_bold("^");
        print("\n");
    }

    void BrassPrinter::warning(string file, string msg, int line, int column)
    {
        string code = "";
        int length = 0;
        for (; code[length + column] != ' '; length++);

        print_white_bold(file);
        print_white_bold(":");
        print_white_bold(std::to_string(line));
        print_white_bold(":");
        print_white_bold(std::to_string(column));
        print_white_bold(": ");
        print_yellow_bold("WARNING: ");
        print_yellow_bold(msg);

        print("\n   " + std::to_string(line) + " | ");
        print_code(code);
        print("\n      | ");
        for (int i = 0; i < column; i++) print(" ");
        for (int i = 0; i < length; i++) print_yellow_bold("^");
        print("\n");
    }

    void BrassPrinter::note(string file, string msg, int line, int column)
    {
        string code = "";
        int length = 0;
        for (; code[length + column] != ' '; length++);

        print_white_bold(file);
        print_white_bold(":");
        print_white_bold(std::to_string(line));
        print_white_bold(":");
        print_white_bold(std::to_string(column));
        print_white_bold(": ");
        print_cyan_bold("NOTE: ");
        print_cyan_bold(msg);

        print("\n   " + std::to_string(line) + " | ");
        print_code(code);
        print("\n      | ");
        for (int i = 0; i < column; i++) print(" ");
        for (int i = 0; i < length; i++) print_cyan_bold("^");
        print("\n");
    }
}