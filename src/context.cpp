#include "lexer.hpp"

#include <fstream>
#include <thread>
#include "context.hpp"
#include "threadpool.hpp"
#include "magic_enum/magic_enum.hpp"

namespace Brass {
    void BrassContext::InitArguments()
    {
        MakeArgument("--single-thread", "Make compiler run in single thread mode.", ArgUsage::NONE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->multiThread = false; });
        MakeArgument("--quiet", "Print only essential errors.", ArgUsage::NONE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->quiet = true; });
        MakeArgument("-n", "--nerd", "Print nerd stats.", ArgUsage::NONE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->nerdOut = true; });
        MakeArgument("-t", "--emit-tokens", "Print tokens to a file.", ArgUsage::FILE, 
            [](BrassConfiguration* ctx, const std::string& param){ ctx->printTokens = param; });
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
                        message += "<file>";
                    }
                    else if (pair.second.usage == ArgUsage::FILES) {
                        message += "<file(s)>";
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
                << "(" << sources[token.file].source.substr(token.start, token.length) << ")\n";
        }

        ofs.close();
    }

    LexerResult BrassContext::runlexer(string file)
    {
        lexer lex = lexer(this, file);
        return lex.lexFile();
    }

    void BrassContext::run(bool* returnTo) {
        ThreadPool pool = ThreadPool();
        std::vector<std::future<LexerResult>> lexFutures;
        std::vector<vector<Token>> lexResults;

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
    }

    BrassContext::BrassContext(int argc, char **argv, bool* returnTo)
    {
        vector<string> files = {};
        cfg = BrassConfiguration();
        cfg.argList = reinterpret_cast<void*>(&argumentList);
        InitArguments();

        for (int i = 1; i < argc; i++) {
            if (argumentList.contains(argv[i])) {
                Arg argument = argumentList[argv[i]];

                if (argument.usage == ArgUsage::NONE) {
                    argument.code(&this->cfg, "");
                } else if (argument.usage == ArgUsage::FILE) {
                    if (i + 1 >= argc) {
                        std::cout << "\e[1;31mNo file given for argument '" << argv[i] << ".\n\e[0m";
                        *returnTo = true;
                        break;
                    }
                    argument.code(&this->cfg, argv[i + 1]);
                    i++;
                } else {
                    if (i + 1 >= argc) {
                        std::cout << "\e[1;31mNo file given for argument '" << argv[i] << ".\n\e[0m";
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