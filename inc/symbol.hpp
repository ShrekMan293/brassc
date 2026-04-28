#pragma once
#include "type.hpp"

namespace Brass {
    enum class SymbolKind : uint8_t {
		Variable,
		Function,
		Enum,
		Type,
		Exception,
		Implementation,
		Operator,
		Field,
		Argument,
        Constructor,
        Destructor,
    };

    enum class SymbolModifier {
		Cdecl,
		Extern,
		Abstract,
		Pure,
		Sealed,
		Inline,
		Override,
		Unmanaged,
		Volatile,
		Mutable,
		Comptime,
		Primitive,
		Packed,
    };

    struct Symbol {
        SymbolKind kind;
        vector<SymbolModifier> modifiers = {};
        Type type;
    };
}