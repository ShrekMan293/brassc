#include "parser.hpp"

namespace Brass {
    void Parser::advance(int off)
    {
        if (curPos + off >= tokens->size()) {
            off = tokens->size() - curPos;
        }

        curPos += off;
    }
    Token Parser::peek(int off)
    {
        if (curPos + off >= tokens->size()) {
            off = tokens->size() - curPos;
        }

        return tokens->at(curPos + off);
    }
    Token Parser::current()
    {
        return tokens->at(curPos);
    }
    bool Parser::atEnd()
    {
        return current() == TokenType::EOF;
    }
    void Parser::parseTopLevelStatement()
    {
        Node* visibility = nullptr;
        switch (current())
        {
        case TokenType::PUBLIC:
        case TokenType::INTERNAL:
        case TokenType::LOCAL:
            visibility = new Node();
            visibility->enclosedToken = current();
            visibility->type = NodeType::VisibilityNode;
            advance();
            [[fallthrough]];
        case TokenType::FN: return parseFunctionDecl(visibility);
        case TokenType::VAR: return parseVarDecl(visibility, result);
        case TokenType::TYPE: return parseTypeDecl(visibility);
        case TokenType::ENUM: return parseEnumDecl(visibility);
        case TokenType::IMPL: return parseImplDecl(visibility);
        case TokenType::OPERATOR: return parseOperatorDecl(visibility);
        case TokenType::IDENTIFIER: return parseConstructorDecl(visibility);
        case TokenType::BIN_NOT: return parseDestructorDecl(visibility);
        case TokenType::EXCEPTION: return parseExceptionDecl(visibility);
        case TokenType::USING: return parseUsingStmt();
        default:
            throw ParseError("Unexpected token as top level statement.", current().line, current().column, false, blockDepth);
        }
    }
    vector<Node> Parser::parseFunctionModifiers()
    {
        vector<Node> result = {};
        switch (peek())
        {
        case TokenType::CDECL:
        case TokenType::EXTERN:
        case TokenType::ABSTRACT:
        case TokenType::PURE:
        case TokenType::SEALED:
        case TokenType::INLINE:
        case TokenType::OVERRIDE:
            advance();
            result.push_back(makeCurrentNode(NodeType::FunctionModifier));
            break;
        default:
            return result;
        }
    }
    void Parser::parseFunctionDecl(Node *visibility)
    {
        Node decl = makeCurrentNode(NodeType::FunctionDeclaration);

        vector<Node> modifiers = parseFunctionModifiers();
        bool isExtern = false;
        for (auto& node : modifiers) {
            if (node.enclosedToken.type == TokenType::EXTERN)
                isExtern = true;
        }
        decl.children.push_back(*visibility);
        decl.children.insert(decl.children.end(), modifiers.begin(), modifiers.end());

        expect(TokenType::IDENTIFIER);
        decl.enclosedToken.start = current().start;
        decl.enclosedToken.length = current().length;

        expect(TokenType::LPAREN);
        Node parameterNode = makeCurrentNode(NodeType::ParameterDeclarationNode);
        vector<Node> assignments = {};
        while (peek() != TokenType::RPAREN && !atEnd()) {
            expect(TokenType::IDENTIFIER);
            Node param = makeCurrentNode(NodeType::IdentifierNode);
            expect(TokenType::ARROW);
            advance();
            Node type = parseType();
            param.children.push_back(type);
            parameterNode.children.push_back(param);

            if (peek() == TokenType::ASSIGN) {
                advance();
                assignments.push_back(parseExpr(&param));
            }
            if (peek() == TokenType::COMMA) {
                advance();
                if (peek() == TokenType::RPAREN) {
                    throw ParseError("Expected parameter declaration following ','.", peek().line, peek().column, false, blockDepth);
                }
            }
        }

        decl.children.push_back(parameterNode);
        decl.children.insert(decl.children.end(), assignments.begin(), assignments.end());
        advance();

        if (atEnd()) {
            throw ParseError("Unterminated function declaration.", peek().line, peek().column, false, blockDepth);
        }

        if (isExtern) {
            expect(TokenType::SEMICOLON);
            advance();
        }
        else {
            expect(TokenType::LBRACE);
            parseInnerStatement(decl.children);
        }

        result.push_back(decl);
    }
    void Parser::parseVarDecl(Node *visibility, vector<Node>& tree)
    {
        Node decl = makeCurrentNode(NodeType::VariableDeclarationNode);
        expect(TokenType::IDENTIFIER);
        Node iden = makeCurrentNode(NodeType::IdentifierNode);
        expect(TokenType::ARROW);
        advance();
        Node type = parseType();

        decl.children.push_back(iden);
        decl.children.push_back(type);
        if (peek() == TokenType::SEMICOLON) {
            advance(2);
        }
        else {
            expect(TokenType::ASSIGN);
            decl.children.push_back(parseExpr(&iden));
            expect(TokenType::SEMICOLON);
            advance();
        }

        tree.push_back(decl);
    }
    void Parser::parseTypeDecl(Node *visibility)
    {
        Node decl = makeCurrentNode(NodeType::VariableDeclarationNode);
        expect(TokenType::IDENTIFIER);
        Node iden = makeCurrentNode(NodeType::IdentifierNode);

        decl.children.push_back(iden);
        if (peek() == TokenType::LESS) {
            advance();
            decl
        }
    }
    Node Parser::makeCurrentNode(NodeType type)
    {
        Node n;
        n.enclosedToken = current();
        n.type = type;

        return n;
    }
    void Parser::expect(TokenType type)
    {
        advance();
        if (current() != type) {
            throw ParseError("Expected " + getStringFromToken(type) + " but got " + getStringFromToken(current().type) + ".", 
                    current().line, current().column, false, blockDepth);
        }
    }
    Result<Node> Parser::parseFile()
    {
        while (!atEnd()) {
            try {
                parseTopLevelStatement();
            } catch (ParseError& e) {
                errors.push_back(Error(current().file, e.msg.c_str(), e.line, e.col, ErrorType::ERR));
                
                while (blockDepth != e.depth && blockDepth != 0) {
                    if (current() == TokenType::LBRACE) {
                        blockDepth++;
                    }
                    else if (current() == TokenType::RBRACE) {
                        blockDepth--;
                    }
                    else if (current() == TokenType::EOF) {
                        break;
                    }

                    advance();
                }
            }
        }
    }
    Parser::Parser(vector<Token> *tokens)
    {
        this->tokens = tokens;
    }

    ParseError::ParseError(string msg, int line, int col, bool severe, int depth)
    {
        this->msg = msg;
        this->line = line;
        this->col = col;
        this->severe = severe;
        this->depth = depth;
    }
}