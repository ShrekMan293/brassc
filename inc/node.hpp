#pragma once
#include "token.hpp"

namespace Brass {
    enum class NodeType {
        VisibilityNode, FunctionDeclarationNode, FunctionModifierNode,
        ParameterDeclarationNode, VariableDeclarationNode,
        IdentifierNode, TypeDeclarationNode, GenericDeclarationNode,
        TypeBlockNode, EnumDeclarationNode, EnumBlockNode,
        ImplDeclarationNode, ImplBlockNode, OperatorDeclarationNode,
        ConstructorDeclarationNode, DestructorDeclarationNode,
        GenericReferenceNode, TypeModifierNode, ExceptionDeclarationNode,
        UsingStatementNode, CodeBlockNode, BreakNode, ContinueNode,
        DoWhileStmtNode, WhileStmtNode, TimesStmtNode, ForStmtNode,
        ForeachStmtNode, IfElseBlockNode, IfStmtNode, ElifStmtNode,
        ElseStmtNode, SwitchStmtNode, SwitchBlockNode, CaseStmtNode,
        DefaultStmtNode, ReturnStmtNode, TryCatchFinallyNode,
        CatchStmtNode, FinallyStmtNode, ThrowStmtNode, ThrowsStmtNode,
        TypeNode
    };

    struct Node {
        NodeType type;
        Token enclosedToken;
        vector<Node> children = {};
    };
}