#pragma once
#include "common.hpp"

namespace Brass {
    class BrassPrinter {
        void print(string msg);
        void print_red(string msg);
        void print_red_bold(string msg);
        void print_green(string msg);
        void print_green_bold(string msg);
        void print_yellow(string msg);
        void print_yellow_bold(string msg);
        void print_blue(string msg);
        void print_blue_bold(string msg);
        void print_magenta(string msg);
        void print_magenta_bold(string msg);
        void print_cyan(string msg);
        void print_cyan_bold(string msg);
        void print_white(string msg);
        void print_white_bold(string msg);

        void print_code(string code);

    public:
        void error(string file, string msg, int line, int column);
        void warning(string file, string msg, int line, int column);
        void note(string file, string msg, int line, int column);
    };
}

// Bold white file: bold yellow warning: yellow message (bold blue keywords, bold magenta types, bold green identifiers)