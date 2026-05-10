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
        std::pair<string, std::vector<std::pair<string, Symbol>>> collect();
    };
}