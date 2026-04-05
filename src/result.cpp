#include "result.hpp"

Brass::Error::Error(string file, const char *msg, int line, int column, ErrorType severity)
{
    this->file = file;
    this->message = msg;
    this->line = line;
    this->column = column;
    this->severity = severity;
}