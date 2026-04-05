#pragma once
#include "common.hpp"
#include "color.hpp"
#include <map>

namespace Brass {
    struct BrassFile
    {
        string path;
        string source;
        vector<size_t> lineStarts;
    };

    class BrassPrinter {
        std::map<string, BrassFile>& sourceData;
        ColorData colorData;
        bool quiet;
        bool color;

        void print(string msg);
        void print(char msg);
        void print_white(string msg);
        void print_white_bold(string msg);
        void print_code(string code);

    public:
        string getCode(string file, int line);
        void error(string file, string msg, int line, int column);
        void warning(string file, string msg, int line, int column);
        void note(string file, string msg, int line, int column);

        BrassPrinter(std::map<string, BrassFile>& sources, bool quiet, bool color);
    };
}

// Bold white file: bold yellow warning: yellow message (bold blue keywords, bold magenta types, bold green identifiers)