#pragma once
#include "token.hpp"

namespace Brass {
    enum class NodeType {
        // Declaration
        FunctionDeclarationNode, ParameterDeclarationNode, VariableDeclarationNode,
        TypeDeclarationNode, GenericDeclarationNode, EnumDeclarationNode, 
        ImplDeclarationNode, OperatorDeclarationNode, ConstructorDeclarationNode,
        DestructorDeclarationNode, ExceptionDeclarationNode, MethodDeclarationNode, 

        // Blocks
        TypeBlockNode, EnumBlockNode, ImplBlockNode, CodeBlockNode,
        SwitchBlockNode, IfElseBlockNode, 

        // Modifiers
        VisibilityNode, FunctionModifierNode, TypeModifierNode, VariableModifierNode, 

        // Statements
        UsingStmtNode, BreakStmtNode, ContinueStmtNode, DoWhileStmtNode,
        WhileStmtNode, TimesStmtNode, ForStmtNode, ForeachStmtNode,
        IfStmtNode, ElifStmtNode, ElseStmtNode, SwitchStmtNode, CaseStmtNode,
        DefaultStmtNode, ReturnStmtNode, TryCatchFinallyNode,
        CatchStmtNode, FinallyStmtNode, ThrowStmtNode, ThrowsStmtNode,

        // Expressions
        BinaryExpressionNode, TernaryExpressionNode, UnaryExpressionNode,
        NewExpressionNode, DeleteExpressionNode, 
        
        IdentifierNode, GenericReferenceNode, TypeNode, PointerNode, 
        LiteralNode, ModuleAccessNode, ObjectAccessNode, ArraySubscriptNode, 
        ConstructorCallNode, FunctionCallNode, ArgumentNode, PointerAccessNode,
        ReinterpretCastNode
    };

    struct Node {
        NodeType type;
        Token enclosedToken;
        vector<Node> children = {};
    };
}