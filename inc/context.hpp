#pragma once
#include "common.hpp"

namespace Brass {
    struct BrassFile
    {
        string path;
        string source;
    };


    class BrassContext {
        string ReadFile(string path);
        bool ParseArgument(string arg);

        bool nerdOut = false;
        string printTokens = "";

        public:
        vector<BrassFile> sources;

        BrassContext(int argc, char** argv);
    };
}