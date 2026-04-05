#pragma once
#include "common.hpp"

namespace Brass {
    enum class ErrorType {
        INFO, WARN, ERR
    };
    struct Error {
        string file;
        const char* message;
        int line;
        int column;

        ErrorType severity;

        Error(string file, const char* msg, int line, int column, ErrorType severity);
    };

    template <typename T>
    struct Result
    {
        vector<T> output = vector<T>();
        vector<Error> errors = vector<Error>();
    };
}