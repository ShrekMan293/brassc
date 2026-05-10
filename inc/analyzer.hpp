#pragma once
#include "node.hpp"
#include "symbol.hpp"
#include "result.hpp"
#include "collection.hpp"

namespace Brass {
    class analyzer {
        vector<Node>& ast;
        unordered_map<string, std::unordered_map<string, Symbol>>& symbols;
        unordered_map<string, std::pair<size_t, Symbol>> symbolTable = {};
        vector<Error> errors = {};
        bool collection = false;
        string currentModule;

        string currentType = "void";
        size_t currentScope = 0;

        unordered_map<string, vector<string>> typeDependencies = {};

        inline void error(string file, string message, int line, int column) {
            errors.emplace_back(file, message, line, column, ErrorType::ERR);
        }
        inline void warning(string file, string message, int line, int column) {
            errors.emplace_back(file, message, line, column, ErrorType::WARN);
        }
        void note(string file, string message, int line, int column) {
            errors.emplace_back(file, message, line, column, ErrorType::INFO);
        }

        inline void popScope() {
            for (auto& [name, sym] : symbolTable) {
                if (sym.first == currentScope) {
                    symbolTable.erase(name);
                }
            }

            currentScope--;
        }

        // Declarations
        void analyzeFunctionDeclarationNode(Node& node);
        void analyzeVariableDeclarationNode(Node& node);
        void analyzeTypeDeclarationNode(Node& node);
        void analyzeGenericDeclarationNode(Node& node);
        void analyzeEnumDeclarationNode(Node& node);
        void analyzeImplDeclarationNode(Node& node);
        void analyzeOperatorDeclarationNode(Node& node);
        void analyzeConstructorDeclarationNode(Node& node);
        void analyzeDestructorDeclarationNode(Node& node);
        void analyzeExceptionDeclarationNode(Node& node);
        void analyzeMethodDeclarationNode(Node& node);

        // Blocks
        void analyzeTypeBlockNode(Node& node);
        void analyzeEnumBlockNode(Node& node);
        void analyzeImplBlockNode(Node& node);
        void analyzeCodeBlockNode(Node& node);
        void analyzeSwitchBlockNode(Node& node);
        void analyzeIfElseBlockNode(Node& node);

        // Modifiers
        void analyzeVisibilityNode(Node& node);
        void analyzeFunctionModifierNode(Node& node);
        void analyzeTypeModifierNode(Node& node);
        void analyzeVariableModifierNode(Node& node);

        // Statements
        void analyzeUsingStmtNode(Node& node);
        void analyzeBreakStmtNode(Node& node);
        void analyzeContinueStmtNode(Node& node);
        void analyzeDoWhileStmtNode(Node& node);
        void analyzeWhileStmtNode(Node& node);
        void analyzeTimesStmtNode(Node& node);
        void analyzeForStmtNode(Node& node);
        void analyzeForeachStmtNode(Node& node);
        void analyzeIfStmtNode(Node& node);
        void analyzeElifStmtNode(Node& node);
        void analyzeElseStmtNode(Node& node);
        void analyzeSwitchStmtNode(Node& node);
        void analyzeCaseStmtNode(Node& node);
        void analyzeDefaultStmtNode(Node& node);
        void analyzeReturnStmtNode(Node& node);
        void analyzeTryCatchFinallyNode(Node& node);
        void analyzeCatchStmtNode(Node& node);
        void analyzeFinallyStmtNode(Node& node);
        void analyzeThrowStmtNode(Node& node);
        void analyzeThrowsStmtNode(Node& node);

        // Expressions
        void analyzeBinaryExpressionNode(Node& node);
        void analyzeTernaryExpressionNode(Node& node);
        void analyzeUnaryExpressionNode(Node& node);
        void analyzeNewExpressionNode(Node& node);
        void analyzeDeleteExpressionNode(Node& node);

        void analyzeIdentifierNode(Node& node);
        void analyzeGenericReferenceNode(Node& node);
        void analyzeTypeNode(Node& node);
        void analyzePointerNode(Node& node);

        void analyzeLiteralNode(Node& node);
        void analyzeModuleAccessNode(Node& node);
        void analyzeObjectAccessNode(Node& node);
        void analyzeArraySubscriptNode(Node& node);

        void analyzeConstructorCallNode(Node& node);
        void analyzeFunctionCallNode(Node& node);
        void analyzeArgumentNode(Node& node);
        void analyzePointerAccessNode(Node& node);

        void analyzeReinterpretCastNode(Node& node);
        void analyzeReferenceNode(Node& node);
        void analyzeNode(Node& node);

        public:
        analyzer(vector<Node>& ast, unordered_map<string, std::unordered_map<string, Symbol>>& symbols);
        vector<Error> analyze();
    }
}