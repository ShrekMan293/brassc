#include "type.hpp"

Brass::Type::Type(Node &node)
{
    baseType = nullptr;
    innerType = nullptr;
    name = node.enclosedToken.value;
    kind = node.enclosedToken.type == TokenType::IDENTIFIER ? TypeKind::UserDefined : TypeKind::BuiltIn;

    if (node.children[0].type == NodeType::GenericReferenceNode) {
        innerType = new Type(node.children[0].children[1]);
    }

    for (auto& child : node.children) {
        if (child.type == NodeType::ReferenceNode) {
            baseType = new Type(*this);
            name = string(name) + "&";
            kind = TypeKind::Reference;
        }
        else if (child.type == NodeType::PointerNode) {
            baseType = new Type(*this);
            name = string(name) + "*";
            kind = TypeKind::Pointer;
        }
    }
}

Brass::Type::Type(string name)
{
    baseType = nullptr;
    innerType = nullptr;
    name = name;
    kind = TypeKind::BuiltIn;
}

Brass::Type::Type()
{
    baseType = nullptr;
    innerType = nullptr;
    name = "";
    kind = TypeKind::UserDefined;
}

Brass::Type::~Type()
{
    delete innerType;
    delete baseType;
}
