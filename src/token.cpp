#include "token.hpp"
#include "magic_enum/magic_enum.hpp"
#include <algorithm>
#include <cctype>

char oppositeCase(char c) {
    if (c >= 'a' && c <= 'z') return c - 32;
    else if (c >= 'A' && c <= 'Z') return c + 32;
    else return c;
}

Brass::TokenType Brass::getTokenFromString(string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return oppositeCase(c);});
    auto result = magic_enum::enum_cast<TokenType>(str);

    if (result.has_value()) {
        if (result.value() >= TokenType::AND && result.value() <= TokenType::THROWS)
            return result.value();
    }

    if (str == "(") return TokenType::LPAREN;
    else if (str == ")") return TokenType::RPAREN;
    else if (str == "[") return TokenType::LBRACK;
    else if (str == "]") return TokenType::RBRACK;
    else if (str == "{") return TokenType::LBRACE;
    else if (str == "}") return TokenType::RBRACE;
    else if (str == "++") return TokenType::PLUS_PLUS;
    else if (str == "--") return TokenType::MINUS_MINUS;
    else if (str == "+") return TokenType::PLUS;
    else if (str == "+=") return TokenType::PLUS_EQUAL;
    else if (str == "-") return TokenType::MINUS;
    else if (str == "-=") return TokenType::MINUS_EQUAL;
    else if (str == "*") return TokenType::STAR;
    else if (str == "*=") return TokenType::STAR_EQUAL;
    else if (str == "/") return TokenType::SLASH;
    else if (str == "/=") return TokenType::SLASH_EQUAL;
    else if (str == "%") return TokenType::MODULO;
    else if (str == "%=") return TokenType::MODULO_EQUAL;
    else if (str == "=") return TokenType::ASSIGN;
    else if (str == "&") return TokenType::BIN_AND;
    else if (str == "&=") return TokenType::BIN_AND_EQUAL;
    else if (str == "|") return TokenType::BIN_OR;
    else if (str == "|=") return TokenType::BIN_OR_EQUAL;
    else if (str == "^") return TokenType::XOR;
    else if (str == "^=") return TokenType::XOR_EQUAL;
    else if (str == "~") return TokenType::BIN_NOT;
    else if (str == "~=") return TokenType::BIN_NOT_EQUAL;
    else if (str == "<<") return TokenType::SHL;
    else if (str == "<<=") return TokenType::SHL_EQUAL;
    else if (str == ">>") return TokenType::SHR;
    else if (str == ">>=") return TokenType::SHR_EQUAL;
    else if (str == "<") return TokenType::LESS;
    else if (str == "<=") return TokenType::LESS_EQUAL;
    else if (str == ">") return TokenType::GREATER;
    else if (str == ">=") return TokenType::GREATER_EQUAL;
    else if (str == "==") return TokenType::EQUALS;
    else if (str == "!=") return TokenType::BANG_EQUALS;
    else if (str == "!") return TokenType::BANG;
    else if (str == "&&") return TokenType::LOG_AND;
    else if (str == "||") return TokenType::LOG_OR;
    else if (str == ",") return TokenType::COMMA;
    else if (str == ".") return TokenType::DOT;
    else if (str == "->") return TokenType::ARROW;
    else if (str == ";") return TokenType::SEMICOLON;
    else if (str == ":") return TokenType::COLON;
    else if (str == "::") return TokenType::DOUBLE_COLON;
    else if (str == "?") return TokenType::QUESTION;
    else return TokenType::UNKNOWN;
}

string Brass::getStringFromToken(Brass::TokenType type) {
    if (type >= TokenType::AND && type <= TokenType::THROWS) {
        string s = (string)magic_enum::enum_name(type);
        std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::tolower(c); });
        return "'" +  s + "'";
    }

    if (type == TokenType::LPAREN) return "'('";
    else if (type == TokenType::RPAREN) return "')'";
    else if (type == TokenType::LBRACK) return "'['";
    else if (type == TokenType::RBRACK) return "']'";
    else if (type == TokenType::LBRACE) return "'{'";
    else if (type == TokenType::RBRACE) return "'}'";
    else if (type == TokenType::PLUS_PLUS) return "'++'";
    else if (type == TokenType::MINUS_MINUS) return "'--'";
    else if (type == TokenType::PLUS) return "'+'";
    else if (type == TokenType::PLUS_EQUAL) return "'+='";
    else if (type == TokenType::MINUS) return "'-'";
    else if (type == TokenType::MINUS_EQUAL) return "'-='";
    else if (type == TokenType::STAR) return "'*'";
    else if (type == TokenType::STAR_EQUAL) return "'*='";
    else if (type == TokenType::SLASH) return "'/'";
    else if (type == TokenType::SLASH_EQUAL) return "'/='";
    else if (type == TokenType::MODULO) return "'%'";
    else if (type == TokenType::MODULO_EQUAL) return "'%='";
    else if (type == TokenType::ASSIGN) return "'='";
    else if (type == TokenType::BIN_AND) return "'&'";
    else if (type == TokenType::BIN_AND_EQUAL) return "'&='";
    else if (type == TokenType::BIN_OR) return "'|'";
    else if (type == TokenType::BIN_OR_EQUAL) return "'|='";
    else if (type == TokenType::XOR) return "'^'";
    else if (type == TokenType::XOR_EQUAL) return "'^='";
    else if (type == TokenType::BIN_NOT) return "'~'";
    else if (type == TokenType::BIN_NOT_EQUAL) return "'~='";
    else if (type == TokenType::SHL) return "'<<'";
    else if (type == TokenType::SHL_EQUAL) return "'<<='";
    else if (type == TokenType::SHR) return "'>>'";
    else if (type == TokenType::SHR_EQUAL) return "'>>='";
    else if (type == TokenType::LESS) return "'<'";
    else if (type == TokenType::LESS_EQUAL) return "'<='";
    else if (type == TokenType::GREATER) return "'>'";
    else if (type == TokenType::GREATER_EQUAL) return "'>='";
    else if (type == TokenType::EQUALS) return "'=='";
    else if (type == TokenType::BANG_EQUALS) return "'!='";
    else if (type == TokenType::BANG) return "'!'";
    else if (type == TokenType::LOG_AND) return "'&&'";
    else if (type == TokenType::LOG_OR) return "'||'";
    else if (type == TokenType::COMMA) return "','";
    else if (type == TokenType::DOT) return "'.'";
    else if (type == TokenType::ARROW) return "'->'";
    else if (type == TokenType::SEMICOLON) return "';'";
    else if (type == TokenType::DOUBLE_COLON) return "'::'";
    else if (type == TokenType::COLON) return "':'";
    else if (type == TokenType::QUESTION) return "'?'";
    else if (type == TokenType::IDENTIFIER) return "identifier";
    else if (type == TokenType::INT_LITERAL) return "integer";
    else if (type == TokenType::FLOAT_LITERAL) return "floating point";
    else if (type == TokenType::STRING_LITERAL) return "string literal";
    else if (type == TokenType::INTERPOLATED_STRING) return "string literal";
    else if (type == TokenType::EOF) return "end of file";
    else return "nothing";
}