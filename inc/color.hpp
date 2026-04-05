#pragma once
#include "common.hpp"

namespace Brass {
    enum class ColorMode {
        NOCOLOR, COLOR16, COLOR256
    };

    enum class TokenColor {
        Normal, Keyword, Type,
        Identifier, Literal,

        Error, Warning, Info
    };

    struct ColorInfo {
        int code16;
        int code256;
    };

    class ColorData {
        ColorMode mode;
        const std::unordered_map<TokenColor, ColorInfo> colorTable = {
            {TokenColor::Normal, {0, 250}},
            {TokenColor::Keyword, {34, 25}},
            {TokenColor::Type, {35, 133}},
            {TokenColor::Identifier, {36, 123}},
            {TokenColor::Literal, {37, 215}},
            {TokenColor::Error, {31, 124}},
            {TokenColor::Warning, {33, 3}},
            {TokenColor::Info, {32, 4}}
        };

        public:
        void print(TokenColor color, string str);
        ColorData(bool quiet);
    };
}