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
        case TokenType::PROTECTED:
        case TokenType::PRIVATE:
            visibility = new Node();
            visibility->enclosedToken = current();
            visibility->type = NodeType::VisibilityNode;
            advance();
            [[fallthrough]];
        case TokenType::FN: return parseFunctionDecl(visibility, result);
        case TokenType::VAR: return parseVarDecl(visibility, result);
        case TokenType::TYPE: return parseTypeDecl(visibility);
        case TokenType::ENUM: return parseEnumDecl(visibility);
        case TokenType::IMPL: return parseImplDecl(visibility);
        case TokenType::OPERATOR: return parseOperatorDecl(visibility);
        case TokenType::IDENTIFIER: return parseConstructorDecl(visibility, result);
        case TokenType::BIN_NOT: return parseDestructorDecl(visibility, result);
        case TokenType::EXCEPTION: return parseExceptionDecl(visibility);
        case TokenType::USING: return parseUsingStmt();
        default:
            throw ParseError("Unexpected token as top level statement.", current().line, current().column, false, blockDepth);
        }
    }
    void Parser::parseInnerStatement(vector<Node> &tree)
    {
        Node code = makeCurrentNode(NodeType::CodeBlockNode);
        advance();

        while (current() != TokenType::RBRACE && !atEnd()) {
            switch (current())
            {
            case TokenType::VAR: parseVarDecl(nullptr, code.children); break;
            case TokenType::DO: parseDoWhileStmt(code.children); break;
            case TokenType::WHILE: parseWhileStmt(code.children); break;
            case TokenType::TIMES: parseTimesStmt(code.children); break;
            case TokenType::FOR: parseForStmt(code.children); break;
            case TokenType::FOREACH: parseForeachStmt(code.children); break;
            case TokenType::IF: parseIfStmt(code.children); break;
            case TokenType::SWITCH: parseSwitchStmt(code.children); break;
            case TokenType::BREAK: {
                Node stmt = Node();
                stmt.enclosedToken = current();
                stmt.type = NodeType::BreakNode;
                code.children.push_back(stmt);
                advance();
                break;
            }
            case TokenType::CONTINUE: {
                Node stmt = Node();
                stmt.enclosedToken = current();
                stmt.type = NodeType::ContinueNode;
                code.children.push_back(stmt);
                advance();
                break;
            }
            case TokenType::RETURN: parseReturnStmt(code.children); break;
            case TokenType::TRY: parseTryCatchStmt(code.children); break;
            case TokenType::THROW: parseThrowStmt(code.children); break;
            default: parseExpr(); advance(); break;
            }
        }
        tree.push_back(code);
        advance();
    }
    vector<Node> Parser::parseFunctionModifiers()
    {
        vector<Node> result = {};
        while (!atEnd())
        {
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
                result.push_back(makeCurrentNode(NodeType::FunctionModifierNode));
                break;
            default:
                return result;
            }
        }

        return result;
    }

    vector<Node> Parser::parseTypeModifiers()
    {
        vector<Node> result = {};
        while (!atEnd())
        {
            switch (peek())
            {
            case TokenType::PRIMITIVE:
            case TokenType::ABSTRACT:
            case TokenType::SEALED:
                advance();
                result.push_back(makeCurrentNode(NodeType::TypeModifierNode));
                break;
            default:
                return result;
            }
        }

        return result;
    }

    void Parser::parseFunctionDecl(Node *visibility, vector<Node>& tree)
    {
        Node decl = makeCurrentNode(NodeType::FunctionDeclarationNode);

        vector<Node> modifiers = parseFunctionModifiers();
        bool isExtern = false;
        for (auto& node : modifiers) {
            if (node.enclosedToken.type == TokenType::EXTERN)
                isExtern = true;
        }
        if (visibility != nullptr) decl.children.push_back(*visibility);
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
            throw ParseError("Unterminated function declaration.", current().line, current().column, false, blockDepth);
        }
        if (peek() == TokenType::THROWS) {
            advance();
            Node throws = makeCurrentNode(NodeType::ThrowsStmtNode);
            advance();
            throws.children.push_back(parseIdentifier());
            decl.children.push_back(throws);
        }

        expect(TokenType::ARROW);
        advance();
        decl.children.push_back(parseType());

        if (isExtern) {
            expect(TokenType::SEMICOLON);
            advance();
        }
        else {

            expect(TokenType::LBRACE);
            parseInnerStatement(decl.children);
        }

        tree.push_back(decl);
    }

    void Parser::parseVarDecl(Node *visibility, vector<Node>& tree)
    {
        Node decl = makeCurrentNode(NodeType::VariableDeclarationNode);
        if (visibility != nullptr) decl.children.push_back(*visibility);

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
        Node decl = makeCurrentNode(NodeType::TypeDeclarationNode);

        vector<Node> modifiers = parseTypeModifiers();
        if (visibility != nullptr) decl.children.push_back(*visibility);
        decl.children.insert(decl.children.end(), modifiers.begin(), modifiers.end());

        expect(TokenType::IDENTIFIER);
        Node iden = makeCurrentNode(NodeType::IdentifierNode);

        decl.children.push_back(iden);
        if (peek() == TokenType::LESS) {
            advance();
            decl.children.push_back(parseGenericDeclaration());
        }
        if (peek() == TokenType::FROM) {
            advance(2);
            decl.children.push_back(parseType());
        }

        expect(TokenType::LBRACE);
        parseTypeBlock(decl.children);

        result.push_back(decl);
    }
    void Parser::parseTypeBlock(vector<Node>& tree)
    {
        Node block = makeCurrentNode(NodeType::TypeBlockNode);

        while (peek() != TokenType::RBRACE && !atEnd()) {
            expect(TokenType::IDENTIFIER);
            Node decl = makeCurrentNode(NodeType::IdentifierNode);
            expect(TokenType::ARROW);
            advance();
            Node type = parseType();
            decl.children.push_back(type);
            block.children.push_back(decl);
            expect(TokenType::SEMICOLON);
        }
        advance();

        if (atEnd()) {
            throw ParseError("Unterminated type declaration.", current().line, current().column, false, blockDepth);
        }
        advance();

        tree.push_back(block);
    }

    void Parser::parseEnumDecl(Node *visibility)
    {
        Node decl = makeCurrentNode(NodeType::EnumDeclarationNode);
        if (visibility != nullptr) decl.children.push_back(*visibility);

        expect(TokenType::IDENTIFIER);
        Node iden = makeCurrentNode(NodeType::IdentifierNode);

        expect(TokenType::ARROW);
        advance();
        Node type = parseType();

        decl.children.push_back(iden);
        decl.children.push_back(type);
        expect(TokenType::LBRACE);
        parseEnumBlock(decl.children);

        result.push_back(decl);
    }
    void Parser::parseEnumBlock(vector<Node>& tree) {
        Node decl = makeCurrentNode(NodeType::EnumBlockNode);

        while (peek() != TokenType::RBRACE && !atEnd()) {
            expect(TokenType::IDENTIFIER);
            Node iden = makeCurrentNode(NodeType::IdentifierNode);
            decl.children.push_back(iden);

            if (peek() == TokenType::ASSIGN) {
                advance();
                decl.children.push_back(parseExpr(&iden));
            }
            if (peek() == TokenType::COMMA) {
                advance();
                if (peek() == TokenType::RPAREN) {
                    throw ParseError("Expected enum member declaration following ','.", peek().line, peek().column, false, blockDepth);
                }
            }
        }
        advance();

        if (atEnd()) {
            throw ParseError("Unterminated enum declaration.", current().line, current().column, false, blockDepth);
        }
        advance();
        
        tree.push_back(decl);
    }

    void Parser::parseImplDecl(Node *visibility)
    {
        Node decl = makeCurrentNode(NodeType::ImplDeclarationNode);
        if (visibility != nullptr) decl.children.push_back(*visibility);

        expect(TokenType::IDENTIFIER);
        Node iden = makeCurrentNode(NodeType::IdentifierNode);

        decl.children.push_back(iden);
        if (peek() == TokenType::FROM) {
            advance(2);
            decl.children.push_back(parseType());
        }

        expect(TokenType::ARROW);
        advance();
        Node type = parseType();
        decl.children.push_back(type);

        expect(TokenType::LBRACE);
        parseImplBlock(decl.children);

        result.push_back(decl);
    }

    void Parser::parseImplBlock(vector<Node>& tree)
    {
        Node decl = makeCurrentNode(NodeType::ImplBlockNode);
        advance();

        while (current() != TokenType::RBRACE && !atEnd()) {
            Node* visibility = nullptr;
            switch (current())
            {
            case TokenType::PUBLIC:
            case TokenType::INTERNAL:
            case TokenType::LOCAL:
            case TokenType::PROTECTED:
            case TokenType::PRIVATE:
                visibility = new Node();
                visibility->enclosedToken = current();
                visibility->type = NodeType::VisibilityNode;
                advance();
                [[fallthrough]];
            case TokenType::FN: return parseFunctionDecl(visibility, decl.children);
            case TokenType::VAR: return parseVarDecl(visibility, decl.children);
            case TokenType::IDENTIFIER: return parseConstructorDecl(visibility, decl.children);
            case TokenType::BIN_NOT: return parseDestructorDecl(visibility, decl.children);
            default:
                throw ParseError("Unexpected token as impl statement.", current().line, current().column, false, blockDepth);
            }
        }

        if (atEnd()) {
            throw ParseError("Unterminated impl declaration.", current().line, current().column, false, blockDepth);
        }
        advance();

        tree.push_back(decl);
    }

    void Parser::parseOperatorDecl(Node *visibility)
    {
        Node decl = makeCurrentNode(NodeType::OperatorDeclarationNode);
        if (visibility != nullptr) decl.children.push_back(*visibility);
        advance();
        Node type = parseType();
        advance();

        decl.children.push_back(type);
        switch (current())
        {
        case TokenType::BIN_OR:
        case TokenType::XOR:
        case TokenType::BIN_AND:
        case TokenType::EQUALS:
        case TokenType::BANG_EQUALS:
        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::SHL:
        case TokenType::SHR:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::MODULO:
        case TokenType::BIN_NOT:
            decl.enclosedToken = current();
            break;
        case TokenType::LPAREN:
            decl.enclosedToken = current();
            expect(TokenType::RPAREN);
            decl.enclosedToken.length = 2;
            break;
        default:
            throw ParseError("Unexpected token, cannot overload this operator.", current().line, current().column, false, blockDepth);
        }

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
            throw ParseError("Unterminated function declaration.", current().line, current().column, false, blockDepth);
        }
        if (peek() == TokenType::THROWS) {
            advance();
            Node throws = makeCurrentNode(NodeType::ThrowsStmtNode);
            advance();
            throws.children.push_back(parseIdentifier());
            decl.children.push_back(throws);
        }

        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);

        result.push_back(decl);
    }

    void Parser::parseConstructorDecl(Node *visibility, vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::ConstructorDeclarationNode);
        if (visibility != nullptr) decl.children.push_back(*visibility);

        if (peek() == TokenType::LESS) {
            advance();
            decl.children.push_back(parseGenericDeclaration());
            expect(TokenType::GREATER);
        }

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
            throw ParseError("Unterminated constructor declaration.", current().line, current().column, false, blockDepth);
        }
        if (peek() == TokenType::THROWS) {
            advance();
            Node throws = makeCurrentNode(NodeType::ThrowsStmtNode);
            advance();
            throws.children.push_back(parseIdentifier());
            decl.children.push_back(throws);
        }

        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);

        tree.push_back(decl);
    }

    void Parser::parseDestructorDecl(Node *visibility, vector<Node> &tree)
    {
        advance();
        Node decl = makeCurrentNode(NodeType::DestructorDeclarationNode);
        if (visibility != nullptr) decl.children.push_back(*visibility);

        if (peek() == TokenType::LESS) {
            advance();
            decl.children.push_back(parseGenericDeclaration());
            expect(TokenType::GREATER);
        }

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
            throw ParseError("Unterminated destructor declaration.", current().line, current().column, false, blockDepth);
        }
        if (peek() == TokenType::THROWS) {
            advance();
            Node throws = makeCurrentNode(NodeType::ThrowsStmtNode);
            advance();
            throws.children.push_back(parseIdentifier());
            decl.children.push_back(throws);
        }

        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);

        tree.push_back(decl);
    }

    void Parser::parseExceptionDecl(Node *visibility)
    {
        Node decl = makeCurrentNode(NodeType::ExceptionDeclarationNode);

        vector<Node> modifiers = parseTypeModifiers();
        if (visibility != nullptr) decl.children.push_back(*visibility);
        decl.children.insert(decl.children.end(), modifiers.begin(), modifiers.end());

        expect(TokenType::IDENTIFIER);
        Node iden = makeCurrentNode(NodeType::IdentifierNode);

        decl.children.push_back(iden);
        if (peek() == TokenType::FROM) {
            advance(2);
            decl.children.push_back(parseType());
        }

        expect(TokenType::LBRACE);
        parseTypeBlock(decl.children);

        result.push_back(decl);
    }

    void Parser::parseUsingStmt()
    {
        Node decl = makeCurrentNode(NodeType::UsingStatementNode);

        while (peek() != TokenType::SEMICOLON && !atEnd()) {
            if (peek() == TokenType::IDENTIFIER || peek() == TokenType::STAR) {
                advance();
                Node mod = makeCurrentNode(NodeType::IdentifierNode);
                decl.children.push_back(mod);
            }
            else {
                advance();
                throw ParseError("Unknown module given.", current().line, current().column, false, blockDepth);
            }

            if (peek() == TokenType::DOUBLE_COLON) {
                advance();
                if (peek() == TokenType::SEMICOLON) {
                    throw ParseError("Expected module declaration following '::'.", peek().line, peek().column, false, blockDepth);
                }
            }
        }
        expect(TokenType::SEMICOLON);
        advance();

        result.push_back(decl);
    }

    void Parser::parseDoWhileStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::DoWhileStmtNode);
        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);
        advance(-1);
        expect(TokenType::WHILE);
        Node _while = makeCurrentNode(NodeType::WhileStmtNode);
        advance();
        _while.children.push_back(parseExpr());
        decl.children.push_back(_while);
        expect(TokenType::SEMICOLON);
        advance();
        tree.push_back(decl);
    }

    void Parser::parseWhileStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::WhileStmtNode);
        advance();
        decl.children.push_back(parseExpr());
        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);
        tree.push_back(decl);
    }

    void Parser::parseTimesStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::TimesStmtNode);
        advance();
        decl.children.push_back(parseExpr());
        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);
        tree.push_back(decl);
    }

    void Parser::parseForStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::ForStmtNode);

        if (peek() != TokenType::SEMICOLON) {
            expect(TokenType::IDENTIFIER);
            Node vardec = makeCurrentNode(NodeType::VariableDeclarationNode);
            Node iden = makeCurrentNode(NodeType::IdentifierNode);

            expect(TokenType::ARROW);
            advance();
            Node type = parseType();
            vardec.children.push_back(iden);
            vardec.children.push_back(type);
            decl.children.push_back(vardec);

            if (peek() == TokenType::ASSIGN) {
                advance();
                decl.children.push_back(parseExpr(&iden));
            }
        }

        expect(TokenType::SEMICOLON);

        if (peek() != TokenType::SEMICOLON) {
            advance();
            decl.children.push_back(parseExpr());
        }

        expect(TokenType::SEMICOLON);

        if (peek() != TokenType::LBRACE) {
            advance();
            decl.children.push_back(parseExpr());
        }

        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);

        tree.push_back(decl);
    }

    void Parser::parseForeachStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::ForeachStmtNode);
        expect(TokenType::IDENTIFIER);
        decl.children.push_back(makeCurrentNode(NodeType::IdentifierNode));
        expect(TokenType::IN);
        advance();
        decl.children.push_back(parsePrimary());
        
        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);

        tree.push_back(decl);
    }

    void Parser::parseIfStmt(vector<Node> &tree)
    {
        Node parent = makeCurrentNode(NodeType::IfElseBlockNode);

        Node decl = makeCurrentNode(NodeType::IfStmtNode);
        advance();
        decl.children.push_back(parseExpr());
        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);
        parent.children.push_back(decl);

        while (current() == TokenType::ELIF && !atEnd()) {
            Node elifDecl = makeCurrentNode(NodeType::ElifStmtNode);
            advance();
            decl.children.push_back(parseExpr());
            expect(TokenType::LBRACE);
            parseInnerStatement(decl.children);
            parent.children.push_back(elifDecl);
        }
        if (current() == TokenType::ELSE) {
            Node elseDecl = makeCurrentNode(NodeType::ElseStmtNode);
            expect(TokenType::LBRACE);
            parseInnerStatement(decl.children);
            parent.children.push_back(elseDecl);
        }

        tree.push_back(parent);
    }

    void Parser::parseSwitchStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::SwitchStmtNode);
        advance();
        decl.children.push_back(parsePrimary());

        expect(TokenType::LBRACE);
        parseSwitchBlock(decl.children);

        tree.push_back(decl);
    }

    void Parser::parseSwitchBlock(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::SwitchBlockNode);
        advance();

        while (current() == TokenType::RBRACE && !atEnd())
        {
            Node val;
            if (peek() == TokenType::CASE) {
                val = makeCurrentNode(NodeType::CaseStmtNode);

                advance();
                val.children.push_back(parsePreLiteral());

                while (peek() == TokenType::OR) {
                    advance(2);
                    val.children.push_back(parsePreLiteral());
                }
            }
            else if (peek() == TokenType::DEFAULT) {
                val = makeCurrentNode(NodeType::DefaultStmtNode);
            }
            else {
                throw ParseError("Expected case or default in switch block.", peek().line, peek().column, true, blockDepth - 1);
            }

            decl.children.push_back(val);
            expect(TokenType::LBRACE);
            parseInnerStatement(decl.children);
        }
        if (atEnd()) {
            throw ParseError("Unterminated switch block.", current().line, current().column, false, blockDepth);
        }
        advance();

        tree.push_back(decl);
    }

    void Parser::parseReturnStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::ReturnStmtNode);
        if (peek() != TokenType::SEMICOLON) {
            advance();
            decl.children.push_back(parseExpr());
        }

        expect(TokenType::SEMICOLON);
        advance();
        tree.push_back(decl);
    }

    void Parser::parseTryCatchStmt(vector<Node> &tree)
    {
        Node block = makeCurrentNode(NodeType::TryCatchFinallyNode);
        expect(TokenType::LBRACE);
        parseInnerStatement(block.children);
        advance(-1);

        expect(TokenType::CATCH);
        Node _catch = makeCurrentNode(NodeType::CatchStmtNode);
        advance();
        _catch.children.push_back(parsePrimary());
        block.children.push_back(_catch);
        expect(TokenType::LBRACE);
        parseInnerStatement(_catch.children);

        if (current() == TokenType::FINALLY) {
            Node finally = makeCurrentNode(NodeType::FinallyStmtNode);
            expect(TokenType::LBRACE);
            parseInnerStatement(finally.children);
            block.children.push_back(finally);
        }

        tree.push_back(block);
    }

    void Parser::parseThrowStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::ThrowStmtNode);
        if (peek() != TokenType::SEMICOLON) {
            advance();
            decl.children.push_back(parsePrimary());
        }
        expect(TokenType::SEMICOLON);
        advance();
        tree.push_back(decl);
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

    Node Parser::parseType()
    {
        Node parent = makeCurrentNode(NodeType::TypeNode);
        parent.children.push_back(parseIdentifier());
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
       // result.push_back(makeCurrentNode(NodeType::EndOfFileNode));
       return {result, errors};
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