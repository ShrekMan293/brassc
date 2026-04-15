#pragma once
#include "common.hpp"

namespace Brass {
    enum class ErrorType {
        INFO, WARN, ERR
    };
    struct Error {
        string file;
        string message;
        int line;
        int column;

        ErrorType severity;

        Error(string file, string msg, int line, int column, ErrorType severity);
    };

    template <typename T>
    struct Result
    {
        vector<T> output = vector<T>();
        vector<Error> errors = vector<Error>();
    };
}