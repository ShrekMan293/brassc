#pragma once
#include "token.hpp"

namespace Brass {
    enum class NodeType {
        VisibilityNode, FunctionDeclaration, FunctionModifier,
        ParameterDeclarationNode, VariableDeclarationNode,
        IdentifierNode, TypeDeclarationNode, GenericDeclarationNode,
    };

    struct Node {
        NodeType type;
        Token enclosedToken;
        vector<Node> children = {};
    };
}