#pragma once
#include "common.hpp"
#include "node.hpp"

namespace Brass {
    enum class TypeKind {
        BuiltIn,
        UserDefined,
        Pointer,
        Reference,
    };

    struct Type {
        TypeKind kind;
        Type* baseType;
        Type* innerType;

        string_view name;

        Type(Node& node);
        Type(string name);
        Type();
        ~Type();
    };
}