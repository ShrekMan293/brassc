#include "parser.hpp"
#include "magic_enum/magic_enum.hpp"

namespace Brass {
    void parser::advance(int off)
    {
        if (curPos + off >= tokens->size()) {
            off = tokens->size() - curPos - 1;
        }

        curPos += off;

        if (current() == TokenType::LBRACE) blockDepth++;
        else if (current() == TokenType::RBRACE) blockDepth--;
    }
    Token parser::peek(int off)
    {
        if (curPos + off >= tokens->size()) {
            return tokens->at(tokens->size() - 1);
        }

        return tokens->at(curPos + off);
    }
    Token parser::current()
    {
        return tokens->at(curPos);
    }
    bool parser::atEnd()
    {
        return current() == TokenType::EOF;
    }
    void parser::parseTopLevelStatement()
    {
        try {
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
                throw ParseError("Unexpected token as top level statement.", current().line, current().column, true, 0);
            }
        } catch (ParseError& err) {
            errors.push_back(Error(current().file, err.msg.c_str(), err.line, err.col, ErrorType::ERR));
            if (err.severe) {
                while (this->blockDepth != 0 && !atEnd()) {
                    advance();
                }
                advance();
            } else {
                while (!atEnd()) {
                    if (current() == TokenType::RBRACE || current() == TokenType::EOF) {
                        break;
                    }

                    advance();
                }
                advance();
            }
        }
    }
    void parser::parseInnerStatement(vector<Node> &tree)
    {
        int blockDepth = this->blockDepth - 1;
        Node code = makeCurrentNode(NodeType::CodeBlockNode);
        advance();

        while (current() != TokenType::RBRACE && !atEnd()) {
            try {
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
                    case TokenType::LBRACE: parseInnerStatement(code.children); break;
                    default: parseExpr(); expect(TokenType::SEMICOLON); advance(); break;
                }
            } catch (ParseError& e) {
                errors.push_back(Error(current().file, e.msg.c_str(), e.line, e.col, ErrorType::ERR));
                if (e.severe) {
                    while (this->blockDepth != blockDepth && this->blockDepth != 0 && !atEnd()) {
                        advance();
                    }
                    advance();
                } else {
                    while (!atEnd()) {
                        if (current() == TokenType::LBRACE || current() == TokenType::RBRACE
                            || current() == TokenType::SEMICOLON || current() == TokenType::EOF) {
                            break;
                        }

                        advance();
                    }
                    advance();
                }
            }
        }
        tree.push_back(code);
        advance();
    }
    vector<Node> parser::parseFunctionModifiers()
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

    vector<Node> parser::parseTypeModifiers()
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

    void parser::parseFunctionDecl(Node *visibility, vector<Node>& tree)
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

    void parser::parseVarDecl(Node *visibility, vector<Node>& tree)
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
        if (peek() != TokenType::SEMICOLON) {
            expect(TokenType::ASSIGN);
            advance(-1);
            decl.children.push_back(parseExpr(&iden));
        }

        expect(TokenType::SEMICOLON);
        advance();
        tree.push_back(decl);
    }

    void parser::parseTypeDecl(Node *visibility)
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
            Node generic = makeCurrentNode(NodeType::GenericDeclarationNode);
            do {
                expect(TokenType::IDENTIFIER);
                generic.children.push_back(makeCurrentNode(NodeType::IdentifierNode));
                if (peek() == TokenType::COMMA) {
                    advance(2);
                    if (current() == TokenType::GREATER) {
                        throw ParseError("Expected type following ','.", current().line, current().column, false, blockDepth);
                    }
                }
            } while (current() != TokenType::GREATER);
            advance();

            decl.children.push_back(generic);
        }
        if (peek() == TokenType::FROM) {
            advance(2);
            decl.children.push_back(parseType());
        }

        expect(TokenType::LBRACE);
        parseTypeBlock(decl.children);

        result.push_back(decl);
    }
    void parser::parseTypeBlock(vector<Node>& tree)
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

    void parser::parseEnumDecl(Node *visibility)
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
    void parser::parseEnumBlock(vector<Node>& tree) {
        Node decl = makeCurrentNode(NodeType::EnumBlockNode);

        while (peek() != TokenType::RBRACE && !atEnd()) {
            expect(TokenType::IDENTIFIER);
            Node iden = makeCurrentNode(NodeType::IdentifierNode);
            decl.children.push_back(iden);

            if (peek() == TokenType::ASSIGN) {
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

    void parser::parseImplDecl(Node *visibility)
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

    void parser::parseImplBlock(vector<Node>& tree)
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

    void parser::parseOperatorDecl(Node *visibility)
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

    void parser::parseConstructorDecl(Node *visibility, vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::ConstructorDeclarationNode);
        if (visibility != nullptr) decl.children.push_back(*visibility);

        if (peek() == TokenType::LESS) {
            advance();
            Node generic = makeCurrentNode(NodeType::GenericDeclarationNode);
            do {
                expect(TokenType::IDENTIFIER);
                generic.children.push_back(makeCurrentNode(NodeType::IdentifierNode));
                if (peek() == TokenType::COMMA) {
                    advance(2);
                    if (current() == TokenType::GREATER) {
                        throw ParseError("Expected type following ','.", current().line, current().column, false, blockDepth);
                    }
                }
            } while (current() != TokenType::GREATER);
            advance();

            decl.children.push_back(generic);
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

    void parser::parseDestructorDecl(Node *visibility, vector<Node> &tree)
    {
        advance();
        Node decl = makeCurrentNode(NodeType::DestructorDeclarationNode);
        if (visibility != nullptr) decl.children.push_back(*visibility);

        if (peek() == TokenType::LESS) {
            advance();
            Node generic = makeCurrentNode(NodeType::GenericDeclarationNode);
            do {
                expect(TokenType::IDENTIFIER);
                generic.children.push_back(makeCurrentNode(NodeType::IdentifierNode));
                if (peek() == TokenType::COMMA) {
                    advance(2);
                    if (current() == TokenType::GREATER) {
                        throw ParseError("Expected type following ','.", current().line, current().column, false, blockDepth);
                    }
                }
            } while (current() != TokenType::GREATER);
            advance();

            decl.children.push_back(generic);
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

    void parser::parseExceptionDecl(Node *visibility)
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

    void parser::parseUsingStmt()
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

    void parser::parseDoWhileStmt(vector<Node> &tree)
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

    void parser::parseWhileStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::WhileStmtNode);
        advance();
        decl.children.push_back(parseExpr());
        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);
        tree.push_back(decl);
    }

    void parser::parseTimesStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::TimesStmtNode);
        advance();
        decl.children.push_back(parseExpr());
        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);
        tree.push_back(decl);
    }

    void parser::parseForStmt(vector<Node> &tree)
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

    void parser::parseForeachStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::ForeachStmtNode);
        expect(TokenType::IDENTIFIER);
        decl.children.push_back(makeCurrentNode(NodeType::IdentifierNode));
        expect(TokenType::IN);
        advance();
        decl.children.push_back(parseExpr());
        
        expect(TokenType::LBRACE);
        parseInnerStatement(decl.children);

        tree.push_back(decl);
    }

    void parser::parseIfStmt(vector<Node> &tree)
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
            elifDecl.children.push_back(parseExpr());
            expect(TokenType::LBRACE);
            parseInnerStatement(elifDecl.children);
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

    void parser::parseSwitchStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::SwitchStmtNode);
        advance();
        decl.children.push_back(parseExpr());

        expect(TokenType::LBRACE);
        parseSwitchBlock(decl.children);

        tree.push_back(decl);
    }

    void parser::parseSwitchBlock(vector<Node> &tree)
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

    void parser::parseReturnStmt(vector<Node> &tree)
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

    void parser::parseTryCatchStmt(vector<Node> &tree)
    {
        Node block = makeCurrentNode(NodeType::TryCatchFinallyNode);
        expect(TokenType::LBRACE);
        parseInnerStatement(block.children);
        advance(-1);

        expect(TokenType::CATCH);
        Node _catch = makeCurrentNode(NodeType::CatchStmtNode);
        advance();
        _catch.children.push_back(parseExpr());
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

    void parser::parseThrowStmt(vector<Node> &tree)
    {
        Node decl = makeCurrentNode(NodeType::ThrowStmtNode);
        if (peek() != TokenType::SEMICOLON) {
            advance();
            decl.children.push_back(parseExpr());
        }
        expect(TokenType::SEMICOLON);
        advance();
        tree.push_back(decl);
    }

    Node parser::makeCurrentNode(NodeType type)
    {
        Node n;
        n.enclosedToken = current();
        n.type = type;

        return n;
    }
    void parser::expect(TokenType type)
    {
        bool severe = type == TokenType::LBRACE || type == TokenType::RBRACE;

        advance();
        if (current() != type) {
            throw ParseError("Expected " + getStringFromToken(type) + " but got " + getStringFromToken(current().type) + ".", 
                    current().line, current().column, severe, blockDepth);
        }
    }

    Node parser::parseType()
    {
        Node parent = makeCurrentNode(NodeType::TypeNode);

        if (peek() == TokenType::LESS) {
            advance(2);
            Node generic = makeCurrentNode(NodeType::GenericReferenceNode);
            do {
                generic.children.push_back(parseType());
                
                if (peek() == TokenType::COMMA) {
                    advance(2);
                    if (current() == TokenType::GREATER) {
                        throw ParseError("Expected type following ','.", current().line, current().column, false, blockDepth);
                    }
                }
            } while (current() != TokenType::GREATER);
            parent.children.push_back(generic);
        }
        while (peek() == TokenType::STAR) {
            advance();
            parent.children.push_back(makeCurrentNode(NodeType::PointerNode));
        }

        return parent;
    }

    bool parser::isOperator(TokenType type) {
        switch (type)
        {
            case TokenType::ASSIGN:
            case TokenType::PLUS_EQUAL:
            case TokenType::MINUS_EQUAL:
            case TokenType::STAR_EQUAL:
            case TokenType::SLASH_EQUAL:
            case TokenType::MODULO_EQUAL:
            case TokenType::BIN_AND_EQUAL:
            case TokenType::BIN_OR_EQUAL:
            case TokenType::XOR_EQUAL:
            case TokenType::BIN_NOT_EQUAL:
            case TokenType::SHL_EQUAL:
            case TokenType::SHR_EQUAL: 
            case TokenType::QUESTION: 
            case TokenType::LOG_OR: 
            case TokenType::LOG_AND: 
            case TokenType::BIN_OR: 
            case TokenType::XOR: 
            case TokenType::BIN_AND: 
            case TokenType::OR: 
            case TokenType::AND: 
            case TokenType::EQUALS:
            case TokenType::BANG_EQUALS:
            case TokenType::IS: 
            case TokenType::LESS:
            case TokenType::LESS_EQUAL:
            case TokenType::GREATER:
            case TokenType::GREATER_EQUAL: 
            case TokenType::SHL: 
            case TokenType::SHR: 
            case TokenType::PLUS:
            case TokenType::MINUS: 
            case TokenType::STAR:
            case TokenType::SLASH:
            case TokenType::MODULO: return true;
            default: return false;
        }
    }

    int parser::opPrecedence(TokenType type) {
        switch (type)
        {
            case TokenType::ASSIGN:
            case TokenType::PLUS_EQUAL:
            case TokenType::MINUS_EQUAL:
            case TokenType::STAR_EQUAL:
            case TokenType::SLASH_EQUAL:
            case TokenType::MODULO_EQUAL:
            case TokenType::BIN_AND_EQUAL:
            case TokenType::BIN_OR_EQUAL:
            case TokenType::XOR_EQUAL:
            case TokenType::BIN_NOT_EQUAL:
            case TokenType::SHL_EQUAL:
            case TokenType::SHR_EQUAL: return 0;
            case TokenType::QUESTION: return 1;
            case TokenType::LOG_OR: return 2;
            case TokenType::LOG_AND: return 3;
            case TokenType::BIN_OR: return 4;
            case TokenType::XOR: return 5;
            case TokenType::BIN_AND: return 6;
            case TokenType::OR: return 7;
            case TokenType::AND: return 8;
            case TokenType::EQUALS:
            case TokenType::BANG_EQUALS:
            case TokenType::IS: return 9;
            case TokenType::LESS:
            case TokenType::LESS_EQUAL:
            case TokenType::GREATER:
            case TokenType::GREATER_EQUAL: return 10;
            case TokenType::SHL: return 11;
            case TokenType::SHR: return 12;
            case TokenType::PLUS:
            case TokenType::MINUS: return 13;
            case TokenType::STAR:
            case TokenType::SLASH:
            case TokenType::MODULO: return 14;
        default:
            throw ParseError("Expected operator.", current().line, current().column, false, blockDepth);
        }
    }

    Node makeBinary(Node lhs, Token op, Node rhs) {
        Node result;
        result.type = NodeType::BinaryExpressionNode;
        result.enclosedToken = op;
        result.children.push_back(lhs);
        result.children.push_back(rhs);

        return result;
    }

    Node parser::parseExpr(Node *lhs_ptr, int minPrecedence)
    {
        Node lhs;
        if (lhs_ptr == nullptr) lhs = parsePrimary();
        else lhs = *lhs_ptr;

        while (isOperator(peek()) && opPrecedence(peek()) >= minPrecedence) {
            advance();
            Token op = current();
            minPrecedence = opPrecedence(op);

            if (op == TokenType::QUESTION) {
                advance();
                lhs = parseTernary(lhs, op);
            }
            else {
                advance();
                Node rhs = parseExpr(nullptr, minPrecedence + 1);
                lhs = makeBinary(lhs, op, rhs);
            }
        }

        return lhs;
    }

    Node parser::parseTernary(Node lhs, Token op)
    {
        Node result;
        result.enclosedToken = op;
        result.type = NodeType::TernaryExpressionNode;
        result.children.push_back(lhs);
        result.children.push_back(parseExpr());
        expect(TokenType::COLON);
        advance();
        result.children.push_back(parseExpr());

        return result;
    }

    Node parser::parsePrimary()
    {
        switch (current())
        {
        case TokenType::LPAREN: {
            advance();
            Node result = parseExpr();
            expect(TokenType::RPAREN);
            return result;
        }
        case TokenType::SIZEOF: {
            Node result = makeCurrentNode(NodeType::UnaryExpressionNode);
            expect(TokenType::LPAREN);
            advance();
            result.children.push_back(parseExpr());
            expect(TokenType::RPAREN);
            return result;
        }
        case TokenType::TYPEOF: {
            Node result = makeCurrentNode(NodeType::UnaryExpressionNode);
            expect(TokenType::LPAREN);
            advance();
            result.children.push_back(parseExpr());
            expect(TokenType::RPAREN);
            return result;
        }
        case TokenType::NEW: return parseNewExpr();
        case TokenType::DELETE: return parseDeleteExpr();
        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS:
        case TokenType::BIN_AND:
        case TokenType::STAR: return parsePreIden();
        case TokenType::BIN_NOT:
        case TokenType::BANG:
        case TokenType::PLUS:
        case TokenType::MINUS: return parsePreLiteral();
        case TokenType::IDENTIFIER: return parseIdentifier();
        case TokenType::INT_LITERAL:
        case TokenType::FLOAT_LITERAL:
        case TokenType::STRING_LITERAL: return makeCurrentNode(NodeType::LiteralNode);
        case TokenType::INTERPOLATED_STRING: return parseInterpolatedString();
        default:
            throw ParseError("Unexpected primary token.", current().line, current().column, false, blockDepth);
        }
    }

    Node parser::parsePreLiteral()
    {
        Node result = makeCurrentNode(NodeType::UnaryExpressionNode);
        advance();
        result.children.push_back(makeCurrentNode(NodeType::LiteralNode));

        return result;
    }

    Node parser::parseInterpolatedString()
    {
        if (current().length != 0) {
            throw ParseError("Unexpected primary token.", current().line, current().column, false, blockDepth);
        }

        Node result = makeCurrentNode(NodeType::LiteralNode);

        advance();
        while (current() != TokenType::INTERPOLATED_STRING && !atEnd()) {
            result.children.push_back(parseExpr());
            advance();
        }
        result.children.push_back(makeCurrentNode(NodeType::LiteralNode));

        return result;
    }

    Node parser::parsePreIden()
    {
        Node result = makeCurrentNode(NodeType::UnaryExpressionNode);
        advance();
        result.children.push_back(parseIdentifier());

        return result;
    }

    Node parser::parseIdentifier()
    {
        Node result = makeCurrentNode(NodeType::IdentifierNode);

        if (peek() == TokenType::DOUBLE_COLON) {
            advance();
            result.type = NodeType::ModuleAccessNode;
            expect(TokenType::IDENTIFIER);
            result.children.push_back(parseIdentifier());
        }
        if (peek() == TokenType::DOT) {
            advance();
            result.type = NodeType::ObjectAccessNode;
            expect(TokenType::IDENTIFIER);
            result.children.push_back(parseIdentifier());
        }
        if (peek() == TokenType::ARROW) {
            advance();
            result.type = NodeType::PointerAccessNode;
            expect(TokenType::IDENTIFIER);
            result.children.push_back(parseIdentifier());
        }

        if (peek() == TokenType::LPAREN) {
            advance();
            Node iden = result;
            result = makeCurrentNode(NodeType::FunctionCallNode);
            result.children.push_back(iden);
            result.children.push_back(parseArguments());
        }

        if (peek() == TokenType::PLUS_PLUS || peek() == TokenType::MINUS_MINUS) {
            advance();
            result.children.push_back(makeCurrentNode(NodeType::UnaryExpressionNode));
        }

        return result;
    }

    Node parser::parseNewExpr()
    {
        Node result = makeCurrentNode(NodeType::NewExpressionNode);
        advance();
        Node parent = parseType();

        if (peek() == TokenType::LPAREN) {
            advance();
            Node type = parent;
            parent = makeCurrentNode(NodeType::ConstructorCallNode);
            parent.children.push_back(type);
            parent.children.push_back(parseArguments());
        } else if (peek() == TokenType::LBRACK) {
            advance();
            Node type = parent;
            parent = makeCurrentNode(NodeType::ArraySubscriptNode);
            advance();
            parent.children.push_back(type);
            parent.children.push_back(parseExpr());
            expect(TokenType::RBRACK);
        }

        result.children.push_back(parent);
        return result;
    }

    Node parser::parseDeleteExpr()
    {
        Node result = makeCurrentNode(NodeType::DeleteExpressionNode);

        if (peek() == TokenType::LBRACK) {
            advance(2);
            if (current() != TokenType::RBRACK) {
                result.children.push_back(parseExpr());
                expect(TokenType::RBRACK);
            }
        }
        advance();

        result.children.push_back(parsePrimary());
        return result;
    }

    Node parser::parseArguments()
    {
        Node result = makeCurrentNode(NodeType::ArgumentNode);

        while (peek() != TokenType::RPAREN && !atEnd()) {
            advance();
            result.children.push_back(parseExpr());
            if (peek() == TokenType::COMMA) {
                advance();
                if (peek() == TokenType::RPAREN) {
                    throw ParseError("Expected argument following ','.", peek().line, peek().column, false, blockDepth);
                }
            }
        }
        if (atEnd()) {
            throw ParseError("Unterminated function call.", current().line, current().column, false, blockDepth);
        }
        advance();

        return result;
    }

    Result<Node> parser::parseFile()
    {
        while (!atEnd()) {
            parseTopLevelStatement();
        }
       result.push_back(makeCurrentNode(NodeType::EndOfFileNode));
       return {result, errors};
    }
    parser::parser(vector<Token> *tokens)
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