#include "symbol.hpp"

string Brass::createTypeName(const Node &node)
{
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
            result += createTypeName(child.children[0]);
            result += ">";
        }
    }
    return result;
}