#pragma once
#include "context.hpp"
#include "token.hpp"
#include "result.hpp"

namespace Brass {
    class lexer
    {
    private:
        vector<Token> tokens = {};
        vector<Error> errors = {};
        vector<size_t> lineStarts = {};
        string file;
        string* source;

        size_t fpos = 0;
        int line = 1;
        int lpos = 1;

        void advance(int offset=1);
        char peek(int offset=1);
        char current();
        bool atEnd();

        bool skipWhitespace();
        void lexCurrent();
        void lexKeyword();
        void lexOperator();

        void lexNumber();
        void lexBinaryNumber();
        void lexHexNumber();
        void lexDecimalNumber();

        void lexChar();
        void lexString();
        void lexInterpolatedString();

        void error(const char* msg, int line, int column);
    public:
        LexerResult lexFile();
        lexer(BrassContext *ctx, string file);
    };
}