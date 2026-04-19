#include "lexer.hpp"
#include "parser.hpp"
#include "symbol.hpp"

#include <fstream>
#include <thread>
#include <filesystem>
#include "context.hpp"
#include "threadpool.hpp"
#include "magic_enum/magic_enum.hpp"

namespace Brass {
    #ifdef _WIN32
    #include <windows.h>
    string getDefaultLibararyPath() {
        char path[MAX_PATH];
        if (!GetModuleFileName(NULL, path, MAX_PATH)) {
            std::cout << "Error: " << GetLastError() << '\n';
        }

        return string(path) + "/";
    }
    #else
    string getDefaultLibararyPath() {
        return "/usr/lib/";
    }
    #endif

    void BrassContext::InitArguments()
    {
        MakeArgument("--single-thread", "Make compiler run in single thread mode.", ArgUsage::NONE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->multiThread = false; });
        MakeArgument("--quiet", "Print only essential errors.", ArgUsage::NONE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->quiet = true; });
        MakeArgument("-n", "--nerd", "Print nerd stats.", ArgUsage::NONE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->nerdOut = true; });
        MakeArgument("-et", "--emit-tokens", "Print tokens to a file.", ArgUsage::FILE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->printTokens = param; });
        MakeArgument("-ea", "--emit-ast", "Print ast to a file.", ArgUsage::FILE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->printAST = param; });
        MakeArgument("-ea", "--emit-ast", "Print ast to a file.", ArgUsage::FILE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->printAST = param; });
        MakeArgument("-L", "--libdir", "Sets directory to search for library.", ArgUsage::FILE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->libDirs.push_back(param); });
        MakeArgument("-l", "--lib", "Adds library at given path.", ArgUsage::FILE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->libraries.push_back(param); });
        MakeArgument("-v", "--version", "Print the Brass Version.", ArgUsage::NONE, 
            [](BrassConfiguration* ctx, const std::string& param){ std::cout << BRASSC << '\n'; });
        MakeArgument("-h", "--help", "Print this help message.", ArgUsage::NONE, 
            [](BrassConfiguration* ctx, const std::string& param)
            { 
                auto argList = reinterpret_cast<std::map<string, Arg>*>(ctx->argList);
                
                for (auto &pair : *argList)
                {
                    string message = pair.first + " ";
                    if (pair.second.usage == ArgUsage::FILE) {
                        message += "<path>";
                    }
                    else if (pair.second.usage == ArgUsage::FILES) {
                        message += "<path(s)>";
                    }
                    int toPrint = 32 - message.length();
                    std::cout << message;
                    for (int i = 0; i < toPrint; i++) {
                        std::cout << ' ';
                    }

                    std::cout << pair.second.description;
                    std::cout << '\n';
                }
            });
    }

    void BrassContext::MakeArgument(string name, string description, ArgUsage usage, std::function<void(BrassConfiguration*, const std::string &)> code)
    {
        Arg arg;
        arg.code = code;
        arg.description = description;
        arg.usage = usage;
        argumentList.emplace(name, arg);
    }

    void BrassContext::MakeArgument(string shortName, string longName, string description, ArgUsage usage, std::function<void(BrassConfiguration*, const std::string &)> code)
    {
        Arg arg;
        arg.code = code;
        arg.description = description;
        arg.usage = usage;
        argumentList.emplace(shortName, arg);
        argumentList.emplace(longName, arg);
    }

    string restOfArgs(char** argv, int argc, int* curArg) {
        string result = "";

        for (; argv[*curArg][0] != '-' && *curArg < argc; *curArg = *curArg + 1) {
            result += argv[*curArg];
            if (*curArg + 1 < argc)
                result += " ";
        }

        return result;
    }
    void BrassContext::printTokens(vector<Token> tokens)
    {
        std::ofstream ofs = std::ofstream(cfg.printTokens);
        if (!ofs) {
            std::cout << "\e[1;31mFailure opening file '" << cfg.printTokens << "'.\n\e[0m";
            ofs.close();
            return;
        }

        for (auto& token : tokens) {
            ofs << token.file << ":" << token.line << ":" << token.column << ": " << magic_enum::enum_name(token.type)
                << "(" << token.value << ")\n";
        }

        ofs.close();
    }

    void BrassContext::outNode(std::ostream& os, Node node) {
        os << magic_enum::enum_name(node.type) << "(" << node.enclosedToken.value << ")";
    }

    void BrassContext::printNode(std::ofstream& ofs, Node node, int indent, vector<int> stops) {
        if (indent == 0) {
            ofs << "|\n|\n|\n|----";
            outNode(ofs, node);
            ofs << '\n';
            stops.push_back(indent + 1);
            for (auto child : node.children)
            {
                printNode(ofs, child, indent + 1, stops);
            }
        } else {
            for (int i = 0; i < 4; i++)
            {
                ofs << "|";
                for (int j = 0; j < indent + 1; j++)
                {
                    ofs << '\t';
                    if (std::ranges::find(stops, j) != stops.end())
                    {
                        ofs << "|";
                    }
                }
                if (i == 3)
                {
                    ofs << "----";
                    outNode(ofs, node);
                    ofs << '\n';
                }
                else
                {
                    ofs << '\n';
                }
            }
            stops.push_back(indent + 1);
            for (auto child : node.children)
            {
                printNode(ofs, child, indent + 1, stops);
            }
        }
    }

    void BrassContext::printTree(vector<Node> ast)
    {
        std::ofstream ofs = std::ofstream(cfg.printAST);
        if (!ofs) {
            std::cout << "\e[1;31mFailure opening file '" << cfg.printAST << "'.\n\e[0m";
            ofs.close();
            return;
        }
        ofs << '\n';

        for (auto node : ast) {
            printNode(ofs, node);
        }
    }

    LexerResult BrassContext::runlexer(string file)
    {
        lexer lex = lexer(this, file);
        return lex.lexFile();
    }

    Result<Node> BrassContext::runParser(vector<Token> tokens) {
        parser parse = parser(&tokens);
        return parse.parseFile();
    }

    vector<string> BrassContext::getModules(bool *returnTo)
    {
        vector<string> result = {};

        for (auto lib : cfg.libraries) {
            bool found = false;
            for (auto dir : cfg.libDirs) {
                string s = dir + lib + ".bmi";
                if (std::filesystem::is_regular_file(s)) {
                    result.push_back(s);
                    found = true;
                    break;
                }
            }

            if (!found) {
                std::cout << "\e[1;31mLibrary '" << lib << "' does not exist.\n\e[0m";
                *returnTo = true;
            }
        }

        return result;
    }

    vector<Symbol> BrassContext::parseModule(string path) {
        std::ifstream ifs(path, std::ios::binary | std::ios::ate);
        size_t length = ifs.tellg();
        ifs.seekg(ifs.beg);
        uint8_t* buffer = new uint8_t[length];

        ifs.read(reinterpret_cast<char*>(buffer), length);

        vector<Symbol> symbols = {};
        for (size_t pos = 0; pos < length; pos++) {
            if (!(buffer[pos] == 'B' && buffer[pos + 1] == 'R' && buffer[pos + 2] == 'C' &&
                buffer[pos + 3] == '0' && buffer[pos + 4] == '0' && buffer[pos + 5] == '1')) 
            {
                std::cout << "\e[1;31mLibrary '" << path << "' has a corrupted module file.\n\e[0m";
                break;
            }
            pos += 6;

            uint16_t modCount;
            memcpy(&modCount, buffer + pos, 2);
            pos += 3;

            for (uint16_t i = 0; i < modCount; i++) {
                uint16_t symCount;
                memcpy(&symCount, buffer + pos, 2);
                pos += 3;

                string name = reinterpret_cast<char*>(buffer + pos);
                pos += name.length() + 1;

                for (uint16_t j = 0; j < symCount; j++) {
                    symbols.push_back(parseSymbol(buffer, pos, name));
                }
            }
        }
    }

    Symbol BrassContext::parseSymbol(uint8_t *buffer, size_t &pos, string_view module)
    {
        Symbol s;
    }

    void BrassContext::run(bool* returnTo) {
        ThreadPool pool = ThreadPool();
        vector<std::future<LexerResult>> lexFutures;
        vector<vector<Token>> lexResults;

        for (auto& pair : sources) {
            const string& file = pair.first;
            lexFutures.push_back(pool.enqueue([this, file]() {
                return this->runlexer(file);
            }));
        }

        for (auto& future : lexFutures) {
            auto result = future.get();
            for (auto& lineStart : result.lineStarts) {
                sources[result.output[0].file].lineStarts.push_back(lineStart);
            }
            for (auto& error : result.errors) {
                printer->error(error.file, error.message, error.line, error.column);
            }

            if (cfg.printTokens != "")
                printTokens(result.output);

            lexResults.push_back(result.output);
        }

        vector<std::future<Result<Node>>> parseFutures;
        vector<vector<Node>> parseResults;

        for (auto& tokens : lexResults) {
            parseFutures.push_back(pool.enqueue([this, tokens]() {
                return this->runParser(tokens);
            }));
        }

        for (auto& future : parseFutures) {
            auto result = future.get();
            for (auto& error : result.errors) {
                printer->error(error.file, error.message, error.line, error.column);
            }

            if (cfg.printAST != "")
                printTree(result.output);

            parseResults.push_back(result.output);
        }
    }

    BrassContext::BrassContext(int argc, char **argv, bool* returnTo)
    {
        vector<string> files = {};
        cfg = BrassConfiguration();
        cfg.argList = reinterpret_cast<void*>(&argumentList);
        cfg.libDirs.push_back(getDefaultLibararyPath());
        InitArguments();

        for (int i = 1; i < argc; i++) {
            if (argumentList.contains(argv[i])) {
                Arg argument = argumentList[argv[i]];

                if (argument.usage == ArgUsage::NONE) {
                    argument.code(&this->cfg, "");
                } else if (argument.usage == ArgUsage::FILE) {
                    if (i + 1 >= argc) {
                        std::cout << "\e[1;31mNo file given for argument '" << argv[i] << "'.\n\e[0m";
                        *returnTo = true;
                        break;
                    }
                    argument.code(&this->cfg, argv[i + 1]);
                    i++;
                } else {
                    if (i + 1 >= argc) {
                        std::cout << "\e[1;31mNo file given for argument '" << argv[i] << "'.\n\e[0m";
                        break;
                    }
                    
                    argument.code(&this->cfg, restOfArgs(argv, argc, &i));
                }
            }
            else {
                files.push_back(argv[i]);
            }
        }

        if (*returnTo) return;

        for (auto& arg : files) {
            BrassFile file;
            file.path = arg;
            file.source = ReadFile(arg, returnTo);

            if (*returnTo) return;

            sources.emplace(file.path, file);
        }

        printer = new BrassPrinter(sources, cfg.quiet, cfg.color);
    }
    BrassContext::~BrassContext()
    {
        delete printer;
    }
    string BrassContext::ReadFile(string path, bool* returnTo)
    {
        std::ifstream ifs = std::ifstream(path, std::ios::ate);
        if (!ifs) {
            std::cout << "\e[1;31mFailure opening file '" << path << "'.\n\e[0m";
            ifs.close();
            *returnTo = true;
            return "";
        }

        std::streamsize size = ifs.tellg();
        ifs.seekg(ifs.beg);

        char* data = new char[size + 1];
        ifs.read(data, size);
        data[size] = '\0';

        string result = data;
        result.push_back('\0');
        delete[] data;
        
        return result;
    }
}