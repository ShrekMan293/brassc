#pragma once
#include "common.hpp"
#include "node.hpp"
#include "result.hpp"

namespace Brass {
    struct ParseError {
        string msg;
        int line;
        int col;
        bool severe;
        int depth;

        ParseError(string msg, int line, int col, bool severe, int depth);
    };

    class parser {
        vector<Node> result = {};
        vector<Error> errors = {};

        vector<Token>* tokens;
        size_t curPos = 0;
        int blockDepth = 0;
        bool inLoop = false;

        void advance(int off=1);
        Token peek(int off=1);
        Token current();
        bool atEnd();

        void parseTopLevelStatement();
        void parseInnerStatement(vector<Node>& tree);

        // Land ahead, leave on
        vector<Node> parseFunctionModifiers();
        vector<Node> parseVariableModifiers();
        vector<Node> parseTypeModifiers();

        // Land on, leave ahead
        void parseFunctionDecl(Node* visibility, vector<Node>& tree);
        void parseVarDecl(Node* visibility, vector<Node>& tree);

        void parseTypeDecl(Node* visibility);
        void parseTypeBlock(vector<Node>& tree);

        void parseEnumDecl(Node* visibility);
        void parseEnumBlock(vector<Node>& tree);

        void parseImplDecl(Node* visibility);
        void parseImplBlock(vector<Node>& tree);

        void parseOperatorDecl(Node* visibility);
        void parseConstructorDecl(Node* visibility, vector<Node>& tree);
        void parseDestructorDecl(Node* visibility, vector<Node>& tree);
        void parseExceptionDecl(Node* visibility);
        void parseUsingStmt();

        void parseDoWhileStmt(vector<Node>& tree);
        void parseWhileStmt(vector<Node>& tree);
        void parseTimesStmt(vector<Node>& tree);
        void parseForStmt(vector<Node>& tree);
        void parseForeachStmt(vector<Node>& tree);
        void parseIfStmt(vector<Node>& tree);

        void parseSwitchStmt(vector<Node>& tree);
        void parseSwitchBlock(vector<Node>& tree);

        void parseReturnStmt(vector<Node>& tree);

        void parseTryCatchStmt(vector<Node>& tree);
        void parseThrowStmt(vector<Node>& tree);

        Node makeCurrentNode(NodeType type);
        void expect(TokenType type);

        // Land on, leave on
        Node parseType();
        Node parseExpr(Node* lhs=nullptr, int minPrecedence=0);
        Node parseTernary(Node lhs, Token op);
        Node parsePrimary();
        Node parsePreLiteral();
        Node parseInterpolatedString();
        Node parsePreIden();
        Node parseIdentifier();
        Node parseNewExpr();
        Node parseDeleteExpr();
        Node parseArguments();
        Node parseReinterpretCast();

        bool isOperator(TokenType type);
        int opPrecedence(TokenType type);

        public:
        Result<Node> parseFile();
        parser(vector<Token>* tokens);
    };
}