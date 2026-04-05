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

    class Parser {
        vector<Node> result = {};
        vector<Error> errors = {};

        vector<Token>* tokens;
        size_t curPos = 0;
        int blockDepth = 0;

        void advance(int off=1);
        Token peek(int off=1);
        Token current();
        bool atEnd();

        void parseTopLevelStatement();
        void parseInnerStatement(vector<Node>& tree);

        // Land on, leave ahead
        vector<Node> parseFunctionModifiers();
        void parseFunctionDecl(Node* visibility);
        void parseVarDecl(Node* visibility, vector<Node>& tree);
        void parseTypeDecl(Node* visibility);
        void parseEnumDecl(Node* visibility);
        void parseImplDecl(Node* visibility);
        void parseOperatorDecl(Node* visibility);
        void parseConstructorDecl(Node* visibility);
        void parseDestructorDecl(Node* visibility);
        void parseExceptionDecl(Node* visibility);
        void parseUsingStmt();

        Node makeCurrentNode(NodeType type);
        void expect(TokenType type);

        // Land on, leave on
        Node parseType();
        Node parseExpr(Node* lhs=nullptr, int minPrecedence=0);

        public:
        Result<Node> parseFile();
        Parser(vector<Token>* tokens);
    };
}