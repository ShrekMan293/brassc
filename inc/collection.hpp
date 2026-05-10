#pragma once
#include "common.hpp"
#include "node.hpp"
#include "symbol.hpp"

namespace Brass {
    class collector {
        vector<Node>& ast;
        string currentModule;

        public:
        collector(vector<Node>& ast);
        std::pair<string, std::unordered_map<string, Symbol>> collect();
    };

    string resolveName(Node& node);
    void replaceAll(string& str, const string& from, const string& to);
    vector<SymbolModifier> resolveModifiers(Node& node);
}