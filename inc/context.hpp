#pragma once
#include "common.hpp"
#include "printer.hpp"
#include "token.hpp"
#include "node.hpp"
#include "result.hpp"
#include "symbol.hpp"
#include <functional>
#include <map>

namespace Brass {
    struct BrassConfiguration {
        bool nerdOut = false;
        bool multiThread = true;
        bool quiet = false;
        bool color = true;
        string printTokens = "";
        string printAST = "";
        vector<string> libDirs = {};
        vector<string> libraries = {};
        void* argList;
    };

    enum class ArgUsage {
        NONE, FILE, FILES
    };
    struct Arg {
        ArgUsage usage;
        string description;
        std::function<void(BrassConfiguration*, const std::string&)> code;
    };

    struct LexerResult {
        vector<Token> output;
        vector<Error> errors;
        vector<size_t> lineStarts;
    };
    
    class BrassContext {
        string ReadFile(string path, bool* returnTo);
        void InitArguments();
        void MakeArgument(string name, string description, ArgUsage usage, std::function<void(BrassConfiguration*,  const std::string&)> code);
        void MakeArgument(string shortName, string longName, string description, ArgUsage usage, std::function<void(BrassConfiguration*, const std::string&)> code);

        BrassConfiguration cfg;
        std::map<string, Arg> argumentList;

        void printTokens(vector<Token> tokens);
        void printTree(vector<Node> ast);
        void printNode(std::ofstream& ofs, Node node, int indent=0, vector<int> stops={});
        void outNode(std::ostream& os, Node node);
        void printSymbols(unordered_map<string, vector<std::pair<string, Symbol>>> symbols);
        LexerResult runlexer(string file);
        Result<Node> runParser(vector<Token> tokens);
        std::pair<string, std::vector<std::pair<string, Symbol>>> runCollection(vector<Node> ast);

        public:
        void run(bool* returnTo);
        std::map<string, BrassFile> sources;

        BrassContext(int argc, char** argv, bool* returnTo);
        ~BrassContext();
        BrassPrinter* printer;
    };
}