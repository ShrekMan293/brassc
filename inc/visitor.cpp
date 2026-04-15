#include "visitor.hpp"

namespace Brass {
    void Visitor::visit(Node &node)
    {
        switch (node.type)
        {
            case NodeType::FunctionDeclarationNode: return visitFunctionDeclarationNode(node);
            case NodeType::ParameterDeclarationNode: return visitParameterDeclarationNode(node);
            case NodeType::VariableDeclarationNode: return visitVariableDeclarationNode(node);
            case NodeType::TypeDeclarationNode: return visitTypeDeclarationNode(node);
            case NodeType::GenericDeclarationNode: return visitGenericDeclarationNode(node);
            case NodeType::EnumDeclarationNode: return visitEnumDeclarationNode(node);
            case NodeType::ImplDeclarationNode: return visitImplDeclarationNode(node);
            case NodeType::OperatorDeclarationNode: return visitOperatorDeclarationNode(node);
            case NodeType::ConstructorDeclarationNode: return visitConstructorDeclarationNode(node);
            case NodeType::DestructorDeclarationNode: return visitDestructorDeclarationNode(node);
            case NodeType::ExceptionDeclarationNode: return visitExceptionDeclarationNode(node);
            case NodeType::MethodDeclarationNode: return visitMethodDeclarationNode(node);

            case NodeType::TypeBlockNode: return visitTypeBlockNode(node);
            case NodeType::EnumBlockNode: return visitEnumBlockNode(node);
            case NodeType::ImplBlockNode: return visitImplBlockNode(node);
            case NodeType::CodeBlockNode: return visitCodeBlockNode(node);
            case NodeType::SwitchBlockNode: return visitSwitchBlockNode(node);
            case NodeType::IfElseBlockNode: return visitIfElseBlockNode(node);

            case NodeType::VisibilityNode: return visitVisibilityNode(node);
            case NodeType::FunctionModifierNode: return visitFunctionModifierNode(node);
            case NodeType::TypeModifierNode: return visitTypeModifierNode(node);
            case NodeType::VariableModifierNode: return visitVariableModifierNode(node);

            case NodeType::UsingStmtNode: return visitUsingStmtNode(node);
            case NodeType::BreakStmtNode: return visitBreakStmtNode(node);
            case NodeType::ContinueStmtNode: return visitContinueStmtNode(node);
            case NodeType::DoWhileStmtNode: return visitDoWhileStmtNode(node);
            case NodeType::WhileStmtNode: return visitWhileStmtNode(node);
            case NodeType::TimesStmtNode: return visitTimesStmtNode(node);
            case NodeType::ForStmtNode: return visitForStmtNode(node);
            case NodeType::ForeachStmtNode: return visitForeachStmtNode(node);
            case NodeType::IfStmtNode: return visitIfStmtNode(node);
            case NodeType::ElifStmtNode: return visitElifStmtNode(node);
            case NodeType::ElseStmtNode: return visitElseStmtNode(node);
            case NodeType::SwitchStmtNode: return visitSwitchStmtNode(node);
            case NodeType::CaseStmtNode: return visitCaseStmtNode(node);
            case NodeType::DefaultStmtNode: return visitDefaultStmtNode(node);
            case NodeType::ReturnStmtNode: return visitReturnStmtNode(node);
            case NodeType::TryCatchFinallyNode: return visitTryCatchFinallyNode(node);
            case NodeType::CatchStmtNode: return visitCatchStmtNode(node);
            case NodeType::FinallyStmtNode: return visitFinallyStmtNode(node);
            case NodeType::ThrowStmtNode: return visitThrowStmtNode(node);
            case NodeType::ThrowsStmtNode: return visitThrowsStmtNode(node);

            case NodeType::BinaryExpressionNode: return visitBinaryExpressionNode(node);
            case NodeType::TernaryExpressionNode: return visitTernaryExpressionNode(node);
            case NodeType::UnaryExpressionNode: return visitUnaryExpressionNode(node);
            case NodeType::NewExpressionNode: return visitNewExpressionNode(node);
            case NodeType::DeleteExpressionNode: return visitDeleteExpressionNode(node);

            case NodeType::IdentifierNode: return visitIdentifierNode(node);
            case NodeType::GenericReferenceNode: return visitGenericReferenceNode(node);
            case NodeType::TypeNode: return visitTypeNode(node);
            case NodeType::PointerNode: return visitPointerNode(node);

            case NodeType::LiteralNode: return visitLiteralNode(node);
            case NodeType::ModuleAccessNode: return visitModuleAccessNode(node);
            case NodeType::ObjectAccessNode: return visitObjectAccessNode(node);
            case NodeType::ArraySubscriptNode: return visitArraySubscriptNode(node);

            case NodeType::ConstructorCallNode: return visitConstructorCallNode(node);
            case NodeType::FunctionCallNode: return visitFunctionCallNode(node);
            case NodeType::ArgumentNode: return visitArgumentNode(node);
            case NodeType::PointerAccessNode: return visitPointerAccessNode(node);
            case NodeType::ReinterpretCastNode: return visitReinterpretCastNode(node);

            default:
                break;
        }
    }

    void Visitor::visitProgram()
    {
        for (Node node : ast) {
            visit(node);
        }
    }
    Visitor::Visitor(vector<Node>& ast) : ast(ast)
    {
    }
}