#include "context.hpp"

#include <fstream>

namespace Brass {
    bool BrassContext::ParseArgument(string arg) {
        if (arg == "-n" or "--nerd") {
            nerdOut = true;
            return true;
        } else if (arg == "-t" or "--tokens") {
            printTokens = "...";
            return true;
        }
    }

    BrassContext::BrassContext(int argc, char** argv) {
        vector<string> args = {};

        for (int i = 0; i < argc; i++) {
            if (printTokens == "...") {
                printTokens = argv[i];
                continue;
            }

            if (!ParseArgument(argv[i]))
                args.push_back(argv[i]);
        }

        for (auto& arg : args) {
            BrassFile file;
            file.path = arg;
            file.source = ReadFile(arg);
        }
    }
    string BrassContext::ReadFile(string path)
    {
        std::ifstream ifs = std::ifstream(path, std::ios::ate);
        if (!ifs) {
            std::cout << "Failure opening file '" << path << "'.\n";
            ifs.close();
            exit(EXIT_FAILURE);
        }

        std::streamsize size = ifs.tellg();
        ifs.seekg(ifs.beg);

        char* data = new char[size + 1];
        ifs.read(data, size);
        data[size] = '\0';

        string result = data;
        delete[] data;

        return result;
    }
}