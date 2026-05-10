#include "type.hpp"

// FIX FILE

string parseNodeToType(Brass::Node& node) {
    string result = string(node.enclosedToken.value);

    for (auto& child : node.children) {
        if (child.type == Brass::NodeType::ReferenceNode) {
            result += "&";
        }
        else if (child.type == Brass::NodeType::PointerNode) {
            result += "*";
        }
        else if (child.type == Brass::NodeType::GenericReferenceNode) {
            result += "<";
            result += parseNodeToType(child.children[0]);
            result += ">";
        }
    }
    return result;
}

Brass::Type::Type(Node &node) : Type(parseNodeToType(node))
{
}

Brass::Type::Type(string name)
{
    for (size_t i = name.length() - 1; i > 0; i--) {
        if (name[i] == '&') {
            innerType = new Type(name.substr(0, i));
            kind = TypeKind::Reference;
            return;
        }
        else if (name[i] == '*') {
            innerType = new Type(name.substr(0, i));
            kind = TypeKind::Pointer;
            return;
        }
        else if (name[i] == '>') {
            size_t j = i;
            int angleCount = 1;
            while (j > 0) {
                j--;
                if (name[j] == '>') {
                    angleCount++;
                }
                else if (name[j] == '<') {
                    angleCount--;
                    if (angleCount == 0) {
                        innerType = new Type(name.substr(j + 1, i - j - 1));
                        baseType = new Type(name.substr(0, j));
                        kind = TypeKind::UserDefined;
                        return;
                    }
                }
            }
        }
    }

    if (name == "void" || name == "u8" || name == "s8" ||
         name == "u16" || name == "s16" || name == "u32" || name == "s32" ||
          name == "u64" || name == "s64" || name == "f32" || name == "f64" ||
        name == "char" || name == "bool" || name == "byte" || name == "int" ||
        name == "uint" || name == "long" || name == "ulong" || name == "float" || name == "double") {
        kind = TypeKind::BuiltIn;
    }
    else {
        kind = TypeKind::UserDefined;
    }

    baseType = nullptr;
    innerType = nullptr;
    name = name;
}

Brass::Type::Type(Type* type)
{
    baseType = type->baseType ? new Type(*type->baseType) : nullptr;
    innerType = type->innerType ? new Type(*type->innerType) : nullptr;
    name = type->name;
    kind = type->kind;
}

Brass::Type::Type()
{
    baseType = nullptr;
    innerType = nullptr;
    name = "void";
    kind = TypeKind::BuiltIn;
}

Brass::Type::~Type()
{
    delete innerType;
    delete baseType;
}
