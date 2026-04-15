#pragma once
#include "node.hpp"
#include "common.hpp"

namespace Brass {
    class Visitor {
        vector<Node>& ast;

        virtual void visitFunctionDeclarationNode(Node& node) = 0;
        virtual void visitParameterDeclarationNode(Node& node) = 0;
        virtual void visitVariableDeclarationNode(Node& node) = 0;
        virtual void visitTypeDeclarationNode(Node& node) = 0;
        virtual void visitGenericDeclarationNode(Node& node) = 0;
        virtual void visitEnumDeclarationNode(Node& node) = 0;
        virtual void visitImplDeclarationNode(Node& node) = 0;
        virtual void visitOperatorDeclarationNode(Node& node) = 0;
        virtual void visitConstructorDeclarationNode(Node& node) = 0;
        virtual void visitDestructorDeclarationNode(Node& node) = 0;
        virtual void visitExceptionDeclarationNode(Node& node) = 0;
        virtual void visitMethodDeclarationNode(Node& node) = 0;

        virtual void visitTypeBlockNode(Node& node) = 0;
        virtual void visitEnumBlockNode(Node& node) = 0;
        virtual void visitImplBlockNode(Node& node) = 0;
        virtual void visitCodeBlockNode(Node& node) = 0;
        virtual void visitSwitchBlockNode(Node& node) = 0;
        virtual void visitIfElseBlockNode(Node& node) = 0;

        virtual void visitVisibilityNode(Node& node) = 0;
        virtual void visitFunctionModifierNode(Node& node) = 0;
        virtual void visitTypeModifierNode(Node& node) = 0;
        virtual void visitVariableModifierNode(Node& node) = 0;

        virtual void visitUsingStmtNode(Node& node) = 0;
        virtual void visitBreakStmtNode(Node& node) = 0;
        virtual void visitContinueStmtNode(Node& node) = 0;
        virtual void visitDoWhileStmtNode(Node& node) = 0;
        virtual void visitWhileStmtNode(Node& node) = 0;
        virtual void visitTimesStmtNode(Node& node) = 0;
        virtual void visitForStmtNode(Node& node) = 0;
        virtual void visitForeachStmtNode(Node& node) = 0;
        virtual void visitIfStmtNode(Node& node) = 0;
        virtual void visitElifStmtNode(Node& node) = 0;
        virtual void visitElseStmtNode(Node& node) = 0;
        virtual void visitSwitchStmtNode(Node& node) = 0;
        virtual void visitCaseStmtNode(Node& node) = 0;
        virtual void visitDefaultStmtNode(Node& node) = 0;
        virtual void visitReturnStmtNode(Node& node) = 0;
        virtual void visitTryCatchFinallyNode(Node& node) = 0;
        virtual void visitCatchStmtNode(Node& node) = 0;
        virtual void visitFinallyStmtNode(Node& node) = 0;
        virtual void visitThrowStmtNode(Node& node) = 0;
        virtual void visitThrowsStmtNode(Node& node) = 0;

        virtual void visitBinaryExpressionNode(Node& node) = 0;
        virtual void visitTernaryExpressionNode(Node& node) = 0;
        virtual void visitUnaryExpressionNode(Node& node) = 0;
        virtual void visitNewExpressionNode(Node& node) = 0;
        virtual void visitDeleteExpressionNode(Node& node) = 0;

        virtual void visitIdentifierNode(Node& node) = 0;
        virtual void visitGenericReferenceNode(Node& node) = 0;
        virtual void visitTypeNode(Node& node) = 0;
        virtual void visitPointerNode(Node& node) = 0;

        virtual void visitLiteralNode(Node& node) = 0;
        virtual void visitModuleAccessNode(Node& node) = 0;
        virtual void visitObjectAccessNode(Node& node) = 0;
        virtual void visitArraySubscriptNode(Node& node) = 0;

        virtual void visitConstructorCallNode(Node& node) = 0;
        virtual void visitFunctionCallNode(Node& node) = 0;
        virtual void visitArgumentNode(Node& node) = 0;
        virtual void visitPointerAccessNode(Node& node) = 0;
        virtual void visitReinterpretCastNode(Node& node) = 0;

        void visit(Node& node);
        public:
        void visitProgram();
        Visitor(vector<Node>& ast);
    };
}