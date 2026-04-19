#pragma once
#include "common.hpp"

namespace Brass {
    struct Type {
        Type* baseType;
        Type* innerType;

        string_view name;
        
    };
}