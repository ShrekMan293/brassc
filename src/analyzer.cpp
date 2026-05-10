#include "analyzer.hpp"

namespace Brass
{
    void analyzer::analyzeNode(Node &node) {
        switch (node.type) {
            // Declaration
            case NodeType::FunctionDeclarationNode:      analyzeFunctionDeclarationNode(node); break;
            //case NodeType::ParameterDeclarationNode:     analyzeParameterDeclarationNode(node); break;
            case NodeType::VariableDeclarationNode:      analyzeVariableDeclarationNode(node); break;
            case NodeType::TypeDeclarationNode:          analyzeTypeDeclarationNode(node); break;
            case NodeType::GenericDeclarationNode:       analyzeGenericDeclarationNode(node); break;
            case NodeType::EnumDeclarationNode:          analyzeEnumDeclarationNode(node); break;
            case NodeType::ImplDeclarationNode:          analyzeImplDeclarationNode(node); break;
            case NodeType::OperatorDeclarationNode:      analyzeOperatorDeclarationNode(node); break;
            case NodeType::ConstructorDeclarationNode:   analyzeConstructorDeclarationNode(node); break;
            case NodeType::DestructorDeclarationNode:    analyzeDestructorDeclarationNode(node); break;
            case NodeType::ExceptionDeclarationNode:     analyzeExceptionDeclarationNode(node); break;
            case NodeType::MethodDeclarationNode:        analyzeMethodDeclarationNode(node); break;

            // Blocks
            case NodeType::TypeBlockNode:                analyzeTypeBlockNode(node); break;
            case NodeType::EnumBlockNode:                analyzeEnumBlockNode(node); break;
            case NodeType::ImplBlockNode:                analyzeImplBlockNode(node); break;
            case NodeType::CodeBlockNode:                analyzeCodeBlockNode(node); break;
            case NodeType::SwitchBlockNode:              analyzeSwitchBlockNode(node); break;
            case NodeType::IfElseBlockNode:              analyzeIfElseBlockNode(node); break;

            // Modifiers
            case NodeType::VisibilityNode:               analyzeVisibilityNode(node); break;
            case NodeType::FunctionModifierNode:         analyzeFunctionModifierNode(node); break;
            case NodeType::TypeModifierNode:             analyzeTypeModifierNode(node); break;
            case NodeType::VariableModifierNode:         analyzeVariableModifierNode(node); break;

            // Statements
            case NodeType::UsingStmtNode:               analyzeUsingStmtNode(node); break;
            case NodeType::BreakStmtNode:               analyzeBreakStmtNode(node); break;
            case NodeType::ContinueStmtNode:            analyzeContinueStmtNode(node); break;
            case NodeType::DoWhileStmtNode:             analyzeDoWhileStmtNode(node); break;
            case NodeType::WhileStmtNode:               analyzeWhileStmtNode(node); break;
            case NodeType::TimesStmtNode:               analyzeTimesStmtNode(node); break;
            case NodeType::ForStmtNode:                 analyzeForStmtNode(node); break;
            case NodeType::ForeachStmtNode:             analyzeForeachStmtNode(node); break;
            case NodeType::IfStmtNode:                  analyzeIfStmtNode(node); break;
            case NodeType::ElifStmtNode:                analyzeElifStmtNode(node); break;
            case NodeType::ElseStmtNode:                analyzeElseStmtNode(node); break;
            case NodeType::SwitchStmtNode:              analyzeSwitchStmtNode(node); break;
            case NodeType::CaseStmtNode:                analyzeCaseStmtNode(node); break;
            case NodeType::DefaultStmtNode:             analyzeDefaultStmtNode(node); break;
            case NodeType::ReturnStmtNode:              analyzeReturnStmtNode(node); break;
            case NodeType::TryCatchFinallyNode:         analyzeTryCatchFinallyNode(node); break;
            case NodeType::CatchStmtNode:               analyzeCatchStmtNode(node); break;
            case NodeType::FinallyStmtNode:             analyzeFinallyStmtNode(node); break;
            case NodeType::ThrowStmtNode:               analyzeThrowStmtNode(node); break;
            case NodeType::ThrowsStmtNode:              analyzeThrowsStmtNode(node); break;

            // Expressions
            case NodeType::BinaryExpressionNode:        analyzeBinaryExpressionNode(node); break;
            case NodeType::TernaryExpressionNode:       analyzeTernaryExpressionNode(node); break;
            case NodeType::UnaryExpressionNode:         analyzeUnaryExpressionNode(node); break;
            case NodeType::NewExpressionNode:           analyzeNewExpressionNode(node); break;
            case NodeType::DeleteExpressionNode:        analyzeDeleteExpressionNode(node); break;

            case NodeType::IdentifierNode:              analyzeIdentifierNode(node); break;
            case NodeType::GenericReferenceNode:        analyzeGenericReferenceNode(node); break;
            case NodeType::TypeNode:                    analyzeTypeNode(node); break;
            case NodeType::PointerNode:                 analyzePointerNode(node); break;

            case NodeType::LiteralNode:                analyzeLiteralNode(node); break;
            case NodeType::ModuleAccessNode:           analyzeModuleAccessNode(node); break;
            case NodeType::ObjectAccessNode:           analyzeObjectAccessNode(node); break;
            case NodeType::ArraySubscriptNode:         analyzeArraySubscriptNode(node); break;

            case NodeType::ConstructorCallNode:        analyzeConstructorCallNode(node); break;
            case NodeType::FunctionCallNode:           analyzeFunctionCallNode(node); break;
            case NodeType::ArgumentNode:               analyzeArgumentNode(node); break;
            case NodeType::PointerAccessNode:          analyzePointerAccessNode(node); break;

            case NodeType::ReinterpretCastNode:        analyzeReinterpretCastNode(node); break;
            case NodeType::ReferenceNode:              analyzeReferenceNode(node); break;
            default: break;
        }
    }

    analyzer::analyzer(vector<Node> &ast, unordered_map<string, std::unordered_map<string, Symbol>> &symbols) : ast(ast), symbols(symbols) {
        currentModule = string(ast[0].enclosedToken.file);

        if (currentModule.starts_with("./")) {
            currentModule = currentModule.substr(2);
        }
        replaceAll(currentModule, "/", "::");
    }

    vector<Error> analyzer::analyze() {
        for (auto& node : ast)
            analyzeNode(node);

        return errors;
    }

    void analyzer::analyzeFunctionDeclarationNode(Node &node) {
        string name = "";
        bool cdecl = false;
        name += node.enclosedToken.value;
        for (auto& child : node.children) {
            if (child.type == NodeType::FunctionModifierNode && child.enclosedToken.type == TokenType::CDECL) {
                cdecl = true;
            } 
            else if (child.type == NodeType::IdentifierNode) {
                break;
            }
        }
        if (!cdecl) {
            name += '(';
            for (auto& child : node.children) {
                if (child.type == NodeType::ParameterDeclarationNode) {
                    for (auto& param : child.children) {
                        if (param.type == NodeType::IdentifierNode) {
                            for (auto& type : param.children) {
                                if (type.type == NodeType::TypeNode) {
                                    name += createTypeName(type);
                                    break;
                                }
                            }
                            name += ",";
                        }
                    }
                    break;
                }
            }
            name[name.length() - 1] = ')';
        }

        if (collection) {
            Symbol s;
            s.kind = SymbolKind::Function;
            s.modifiers = resolveModifiers(node);

            for (auto& child : node.children) {
                if (child.type == NodeType::TypeNode) {
                    s.type = createTypeName(child);
                }
            }

            symbols[currentModule].emplace(name, s);
        }
        else {
            currentType = symbols[currentModule][name].type;
            for (auto& child : node.children) {
                if (child.type == NodeType::CodeBlockNode) {
                    analyzeNode(child);
                    break;
                }
            }
        }
    }
    void analyzer::analyzeVariableDeclarationNode(Node &node) {
        Symbol s;
        s.kind = SymbolKind::Variable;
        s.modifiers = resolveModifiers(node);
        string_view name = "";
        for (auto& child : node.children) {
            if (child.type == NodeType::TypeNode) {
                s.type = createTypeName(child);
            }
            else if (child.type == NodeType::IdentifierNode) {
                name = child.enclosedToken.value;
            }
        }

        if (collection) {
            symbols[currentModule].emplace(name, s);
        }
        else {
            symbolTable.emplace(name, std::make_pair(currentScope, s));
            for (auto& child : node.children) {
                if (child.type == NodeType::BinaryExpressionNode) {
                    string buf = currentType;
                    currentType = s.type;
                    analyzeNode(child);
                    currentType = buf;
                }
            }
        }
    }
    void analyzer::analyzeTypeDeclarationNode(Node &node) {
        if (collection) {
            Symbol s;
            s.kind = SymbolKind::Type;
            s.modifiers = resolveModifiers(node);
            string name = "";
            for (auto& child : node.children) {
                if (child.type == NodeType::IdentifierNode) {
                    name = child.enclosedToken.value;
                    s.type = name;
                    symbols[currentModule].emplace(name, s);
                }
                else if (child.type == NodeType::TypeDeclarationNode) {
                    analyzeNode(child);
                }
                else if (child.type == NodeType::TypeNode) {
                    if (symbols[currentModule].find(createTypeName(child)) == symbols[currentModule].end()) {
                        typeDependencies[createTypeName(child)].push_back(name);
                    }
                }
            }
        }
    }
}
