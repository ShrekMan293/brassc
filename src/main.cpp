#include "context.hpp"

int main(int argc, char** argv) {
    bool returnTo = false;
    Brass::BrassContext context = Brass::BrassContext(argc, argv, &returnTo);

    if (!returnTo) context.run(&returnTo);

    return returnTo;
}