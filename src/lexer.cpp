#include "lexer.hpp"

namespace Brass {
    void lexer::advance(int offset)
    {
        if (((std::size_t)(offset + fpos)) >= source->length()) {
            offset = source->length() - fpos;
        }

        fpos += offset;
        lpos += offset;
    }
    char lexer::peek(int offset)
    {
        if (((std::size_t)(offset + fpos)) >= source->length()) {
            return '\0';
        }

        return source->at(fpos + offset);
    }
    char lexer::current()
    {
        return source->at(fpos);
    }
    bool lexer::atEnd()
    {
        return current() == '\0';
    }
    bool lexer::skipWhitespace()
    {
        if (current() == '\n') {
            line++;
            lpos = 0;
            advance();
            lineStarts.push_back(fpos);
            return true;
        }
        else if (std::isspace(current())) {
            advance();
            return true;
        }
        else if (current() == '/') {
            if (peek() == '/') {
                while (current() != '\n' && !atEnd())
                    advance();
                advance();
                return true;
            }
            else if (peek() == '*') {
                while (peek(-1) != '*' && current() != '/' && !atEnd())
                    advance();
                advance();
                return true;
            }
            return false;
        }

        return false;
    }
    void lexer::lexCurrent()
    {
        if (isalpha(current()) || current() == '_')
            return lexKeyword();
        else
            return lexOperator();
    }
    void lexer::lexKeyword()
    {
        Token t;
        t.column = lpos;
        t.file = file;
        int start = fpos;
        t.line = line;
        
        while ((isalnum(current()) || current() == '_') && !atEnd())
            advance();
        
        t.value = string_view(*source).substr(start, fpos - start);
        t.type = getTokenFromString(string(t.value));
        if (t.type == TokenType::UNKNOWN) t.type = TokenType::IDENTIFIER;

        tokens.push_back(t);
    }
    void lexer::lexOperator()
    {
        if (isdigit(current()))
            return lexNumber();
        else if (current() == '\'')
            return lexChar();
        else if (current() == '"')
            return lexString();
        else if (current() == '$' && peek() == '"')
            return lexInterpolatedString();

        
        Token t;
        t.column = lpos;
        t.file = file;
        int start = fpos;
        t.line = line;

        while (!isalnum(current()) && !std::isspace(current()) && !atEnd()) {
            advance();
        }

        t.value = string_view(*source).substr(start, fpos - start);

        do {
            t.type = getTokenFromString(string(t.value));
            if (t.type == TokenType::UNKNOWN) {
                advance(-1);
                t.value = string_view(*source).substr(start, 1);
            }
            else {
                break;
            }
        }
        while (t.value.length() != 0);

        if (t.type == TokenType::UNKNOWN) {
            error("Unknown token.", t.line, t.column);
            advance();
        }
        else {
            tokens.push_back(t);
        }
    }
    void lexer::lexNumber()
    {
        if (current() == '0') {
            advance();
            if (current() == 'x')
                return lexHexNumber();
            else if (current() == 'b')
                return lexBinaryNumber();
        }

        lexDecimalNumber();
    }
    void lexer::lexBinaryNumber()
    {
        Token t;
        t.column = lpos;
        t.file = file;
        int start = fpos;
        t.line = line;
        t.type = TokenType::INT_LITERAL;

        advance();
        while ((current() == '0' || current() == '1') && !atEnd())
            advance();

        t.value = string_view(*source).substr(start, fpos - start);
        tokens.push_back(t);
    }

    bool isHex(char c) {
        c = std::tolower(c);

        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    void lexer::lexHexNumber()
    {
        Token t;
        t.column = lpos;
        t.file = file;
        int start = fpos;
        t.line = line;
        t.type = TokenType::INT_LITERAL;

        advance();
        while (isHex(current()) && !atEnd())
            advance();

        t.value = string_view(*source).substr(start, fpos - start);
        tokens.push_back(t);
    }
    void lexer::lexDecimalNumber()
    {
        Token t;
        t.column = lpos;
        t.file = file;
        int start = fpos;
        t.line = line;

        bool hasDot = false;
        while ((isdigit(current()) || (current() == '.' && !hasDot)) && !atEnd()) {
            if (current() == '.') hasDot = true;

            advance();
        }

        t.value = string_view(*source).substr(start, fpos - start);
        t.type = hasDot ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL;
        tokens.push_back(t);
    }
    void lexer::lexChar()
    {
        Token t;
        t.column = lpos;
        t.file = file;
        int start = fpos + 1;
        t.line = line;
        t.type = TokenType::INT_LITERAL;
        advance();

        while (current() != '\'' && !atEnd())
            advance();

        t.value = string_view(*source).substr(start, fpos - start);

        if (atEnd()) {
            error("Unterminated char literal.", t.line, t.column);
        }
        advance();
        tokens.push_back(t);
    }
    void lexer::lexString()
    {
        Token t;
        t.column = lpos;
        t.file = file;
        int start = fpos + 1;
        t.line = line;
        t.type = TokenType::STRING_LITERAL;
        advance();

        while (current() != '"' && !atEnd())
            advance();

        t.value = string_view(*source).substr(start, fpos - start);

        if (atEnd()) {
            error("Unterminated string literal.", t.line, t.column);
        }
        advance();
        tokens.push_back(t);
    }
    void lexer::lexInterpolatedString()
    {
        Token t;
        t.column = lpos;
        t.file = file;
        int start = fpos + 2;
        t.line = line;
        t.type = TokenType::INTERPOLATED_STRING;
        t.value = "";
        tokens.push_back(t);
        advance(2);

        while (current() != '"' && !atEnd()) {
            if (current() == '{' && peek(-1) != '\\') {
                advance();
                while (current() != '}' && peek(-1) != '\\' && !atEnd())
                    lexCurrent();
                advance();
                continue;
            }
            advance();
        }

        t.value = string_view(*source).substr(start, fpos - start);

        if (atEnd()) {
            error("Unterminated string literal.", t.line, t.column);
        }
        advance();
        tokens.push_back(t);
    }
    void lexer::error(const char *msg, int line, int column)
    {
        errors.push_back(Error(file, msg, line, column, ErrorType::ERR));
    }
    LexerResult lexer::lexFile()
    {
        lineStarts.push_back(0);
        while (!atEnd()) {
            while (skipWhitespace());

            lexCurrent();
        }
        tokens.push_back((Token){TokenType::EOF, "", file, line, lpos});

        return {tokens, errors, lineStarts};
    }
    lexer::lexer(BrassContext *ctx, string file)
    {
        this->file = file;
        this->source = &ctx->sources[file].source;
    }
}