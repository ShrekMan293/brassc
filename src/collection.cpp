#include "collection.hpp"
#include "magic_enum/magic_enum.hpp"
#include <algorithm>

namespace Brass {
    void replaceAll(string& str, const string& from, const string& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    collector::collector(vector<Node>& ast) : ast(ast)
    {
        currentModule = string(ast[0].enclosedToken.file);

        if (currentModule.starts_with("./")) {
            currentModule = currentModule.substr(2);
        }
        replaceAll(currentModule, "/", "::");
        currentModule = currentModule.substr(0, currentModule.length() - 6);
    }

    string resolveName(Node& node) {
        string result = "";
        bool cdecl = false;

        switch (node.type)
        {
            case NodeType::MethodDeclarationNode:
                for (auto& child : node.children) {
                    if (child.type == NodeType::TypeNode) {
                        result += createTypeName(child);
                        result += ".";
                        break;
                    }
                }
                [[fallthrough]];
            case NodeType::FunctionDeclarationNode:
                result += node.enclosedToken.value;
                for (auto& child : node.children) {
                    if (child.type == NodeType::FunctionModifierNode && child.enclosedToken.type == TokenType::CDECL) {
                        cdecl = true;
                    } 
                    else if (child.type == NodeType::IdentifierNode) {
                        break;
                    }
                }
                if (!cdecl) {
                    result += '(';
                    for (auto& child : node.children) {
                        if (child.type == NodeType::ParameterDeclarationNode) {
                            for (auto& param : child.children) {
                                if (param.type == NodeType::IdentifierNode) {
                                    for (auto& type : param.children) {
                                        if (type.type == NodeType::TypeNode) {
                                            result += createTypeName(type);
                                            break;
                                        }
                                    }
                                    result += ",";
                                }
                            }
                            break;
                        }
                    }
                    result[result.length() - 1] = ')';
                }
                break;
            case NodeType::VariableDeclarationNode:
                for (auto& child : node.children) {
                    if (child.type == NodeType::IdentifierNode) {
                        result += child.enclosedToken.value;
                        break;
                    }
                }
                break;
            case NodeType::OperatorDeclarationNode:
                for (auto& child : node.children) {
                    if (child.type == NodeType::TypeNode) {
                        result += createTypeName(child);
                        result += ".";
                        result += node.enclosedToken.value;
                        break;
                    }
                }
                break;
            case NodeType::DestructorDeclarationNode:
                result = "~";
                [[fallthrough]];
            case NodeType::ConstructorDeclarationNode:
                for (auto& child : node.children) {
                    if (child.type == NodeType::TypeNode) {
                        result += createTypeName(child);
                    }
                    else if (child.type == NodeType::ParameterDeclarationNode) {
                        result += '(';
                        for (auto& param : child.children) {
                            if (param.type == NodeType::IdentifierNode) {
                                for (auto& type : param.children) {
                                    if (type.type == NodeType::TypeNode) {
                                        result += createTypeName(type);
                                        break;
                                    }
                                }
                                result += ",";
                            }
                        }
                        result[result.length() - 1] = ')';
                    }
                }
                if (!result.ends_with(')')) {
                    result += "()";
                }
                break;
            default: break;
        }

        return result;
    }

    vector<SymbolModifier> resolveModifiers(Node& node) {
        vector<SymbolModifier> result = {};

        for (auto& child : node.children) {
            if (child.type == NodeType::FunctionModifierNode) {
                switch (child.enclosedToken.type)
                {
                    case TokenType::CDECL: result.push_back(SymbolModifier::Cdecl); break;
                    case TokenType::EXTERN: result.push_back(SymbolModifier::Extern); break;
                    case TokenType::ABSTRACT: result.push_back(SymbolModifier::Abstract); break;
                    case TokenType::PURE: result.push_back(SymbolModifier::Pure); break;
                    case TokenType::SEALED: result.push_back(SymbolModifier::Sealed); break;
                    case TokenType::INLINE: result.push_back(SymbolModifier::Inline); break;
                    case TokenType::OVERRIDE: result.push_back(SymbolModifier::Override); break;
                    default: break;
                }
            }
            else if (child.type == NodeType::VariableModifierNode) {
                switch (child.enclosedToken.type)
                {
                    case TokenType::UNMANAGED: result.push_back(SymbolModifier::Unmanaged); break;
                    case TokenType::VOLATILE: result.push_back(SymbolModifier::Volatile); break;
                    case TokenType::MUT: result.push_back(SymbolModifier::Mutable); break;
                    case TokenType::COMPTIME: result.push_back(SymbolModifier::Comptime); break;
                    default: break;
                }
            }
            else if (child.type == NodeType::TypeModifierNode) {
                switch (child.enclosedToken.type)
                {
                    case TokenType::PRIMITIVE: result.push_back(SymbolModifier::Primitive); break;
                    case TokenType::PACKED: result.push_back(SymbolModifier::Packed); break;
                    default: break;
                }
            }
        }

        return result;
    }

    std::pair<string, Symbol> createSymbol(Node& node) {
        Symbol symbol = Symbol();
        switch (node.type)
        {
            case NodeType::OperatorDeclarationNode:
                for (auto& child : node.children) {
                    if (child.type == NodeType::TypeNode) {
                        symbol.type = createTypeName(child);
                        break;
                    }
                }
                symbol.kind = SymbolKind::Operator;
                break;
            case NodeType::VariableDeclarationNode:
                for (auto& child : node.children) {
                    if (child.type == NodeType::TypeNode) {
                        symbol.type = createTypeName(child);
                        break;
                    }
                }
                symbol.kind = SymbolKind::Variable;
                break;
            case NodeType::FunctionDeclarationNode:
            case NodeType::MethodDeclarationNode:
                for (auto& child : node.children) {
                    if (child.type == NodeType::TypeNode) {
                        symbol.type = createTypeName(child);
                        break;
                    }
                }
                symbol.kind = SymbolKind::Function;
                break;
            case NodeType::ConstructorDeclarationNode:
                symbol.type = createTypeName(node);
                symbol.kind = SymbolKind::Constructor;
                break;
            case NodeType::DestructorDeclarationNode:
                symbol.type = createTypeName(node);
                symbol.kind = SymbolKind::Destructor;
                break;
            default: break;
        }
        string name = resolveName(node);
        symbol.modifiers = resolveModifiers(node);

        return {name, symbol};
    }

    unordered_map<string, Symbol> createEnumSymbol(Node& node) {

        std::unordered_map<string, Symbol> symbols = {};

        string name;

        Symbol s;
        s.kind = SymbolKind::Enum;
        s.modifiers = resolveModifiers(node);
        for (auto& child : node.children) {
            if (child.type == NodeType::IdentifierNode) {
                name = child.enclosedToken.value;
            }
            else if (child.type == NodeType::TypeNode) {
                s.type = createTypeName(child);
                break;
            }
        }

        symbols.emplace(name, s);

        for (auto& child : node.children) {
            if (child.type == NodeType::EnumBlockNode) {
                for (auto& enumerator : child.children) {
                    if (enumerator.type == NodeType::IdentifierNode) {
                        Symbol enumeratorSymbol = Symbol();
                        enumeratorSymbol.kind = SymbolKind::Variable;
                        enumeratorSymbol.type = s.type;
                        enumeratorSymbol.modifiers = {};
                        enumeratorSymbol.modifiers.push_back(SymbolModifier::Comptime);
                        symbols.emplace(name + string(".") + string(enumerator.enclosedToken.value), enumeratorSymbol);
                    }
                }
            }
        }
        return symbols;
    }

    unordered_map<string, Symbol> createTypeSymbol(Node& node) {
        unordered_map<string, Symbol> symbols = {};

        string name;
        Symbol s;
        s.kind = SymbolKind::Type;
        s.modifiers = resolveModifiers(node);
        s.type = createTypeName(node);
        symbols.emplace(name, s);
        for (auto& child : node.children) {
            if (child.type == NodeType::TypeBlockNode) {
                for (auto& decl : child.children) {
                    if (decl.type == NodeType::IdentifierNode) {
                        Symbol fieldSymbol = Symbol();
                        fieldSymbol.kind = SymbolKind::Field;
                        for (auto& type : decl.children) {
                            if (type.type == NodeType::TypeNode) {
                                fieldSymbol.type = createTypeName(type);
                                break;
                            }
                        }
                        fieldSymbol.modifiers = {};
                        symbols.emplace(name + string(".") + string(decl.enclosedToken.value), fieldSymbol);
                    }
                }
            }
        }

        return symbols;
    }

    unordered_map<string, Symbol> createImplSymbol(Node& node) {
        unordered_map<string, Symbol> symbols = {};

        string name;
        Symbol s;
        s.kind = SymbolKind::Implementation;
        s.modifiers = resolveModifiers(node);
        for (auto& child : node.children) {
            if (child.type == NodeType::IdentifierNode) {
                name = child.enclosedToken.value;
            }
            else if (child.type == NodeType::TypeNode) {
                s.type = createTypeName(child);
                symbols.emplace(name, s);
            }
            else if (child.type == NodeType::ImplBlockNode) {
                for (auto& member : child.children) {
                    bool cont = false;
                    for (auto& memberChild : member.children) {
                        if (memberChild.type == NodeType::VisibilityNode) {
                            cont = memberChild.enclosedToken.type == TokenType::PUBLIC || memberChild.enclosedToken.type == TokenType::INTERNAL;
                            break;
                        }
                    }

                    if (cont) {
                        string buf = name + "." + resolveName(member);
                        Symbol curSym;
                        curSym.modifiers = resolveModifiers(member);
                        switch (member.type)
                        {
                            case NodeType::FunctionDeclarationNode:
                                curSym.kind = SymbolKind::Function;
                                for (auto& child : member.children) {
                                    if (child.type == NodeType::TypeNode) {
                                        curSym.type = createTypeName(child);
                                        break;
                                    }
                                }
                                break;
                            case NodeType::VariableDeclarationNode:
                                curSym.kind = SymbolKind::Field;
                                for (auto& child : member.children) {
                                    if (child.type == NodeType::TypeNode) {
                                        curSym.type = createTypeName(child);
                                        break;
                                    }
                                }
                                break;
                            case NodeType::ConstructorDeclarationNode:
                                curSym.kind = SymbolKind::Constructor;
                                curSym.type = s.type;
                                break;
                            case NodeType::DestructorDeclarationNode:
                                curSym.kind = SymbolKind::Destructor;
                                curSym.type = "void";
                                break;
                            default: break;
                        }
                        symbols.emplace(buf, curSym);
                    }
                }
            }
        }

        return symbols;
    }

    std::pair<string, std::unordered_map<string, Symbol>> collector::collect()
    {
        std::unordered_map<string, Symbol> symbols = {};

        for (auto& node : ast) {
            switch (node.type)
            {
                case NodeType::FunctionDeclarationNode:
                case NodeType::MethodDeclarationNode:
                case NodeType::OperatorDeclarationNode:
                case NodeType::ConstructorDeclarationNode:
                case NodeType::DestructorDeclarationNode:
                case NodeType::VariableDeclarationNode:
                    for (auto& child : node.children) {
                        if (child.type == NodeType::VisibilityNode && (child.enclosedToken.type == TokenType::PUBLIC || child.enclosedToken.type == TokenType::INTERNAL)) {
                            symbols.insert(createSymbol(node));
                            break;
                        }
                    }
                    break;
                case NodeType::EnumDeclarationNode:
                    for (auto& child : node.children) {
                        if (child.type == NodeType::VisibilityNode && (child.enclosedToken.type == TokenType::PUBLIC || child.enclosedToken.type == TokenType::INTERNAL)) {
                            std::unordered_map<std::string, Brass::Symbol> enumSymbols = createEnumSymbol(node);
                            symbols.insert(enumSymbols.begin(), enumSymbols.end());
                            break;
                        }
                    }
                    break;
                case NodeType::ExceptionDeclarationNode:
                case NodeType::TypeDeclarationNode:
                    for (auto& child : node.children) {
                        if (child.type == NodeType::VisibilityNode && (child.enclosedToken.type == TokenType::PUBLIC || child.enclosedToken.type == TokenType::INTERNAL)) {
                            std::unordered_map<std::string, Brass::Symbol> typeSymbols = createTypeSymbol(node);
                            symbols.insert(typeSymbols.begin(), typeSymbols.end());
                            break;
                        }
                    }
                    break;
                case NodeType::ImplDeclarationNode:
                    for (auto& child : node.children) {
                        if (child.type == NodeType::VisibilityNode && (child.enclosedToken.type == TokenType::PUBLIC || child.enclosedToken.type == TokenType::INTERNAL)) {
                            std::unordered_map<std::string, Brass::Symbol> implSymbols = createImplSymbol(node);
                            symbols.insert(implSymbols.begin(), implSymbols.end());
                            break;
                        }
                    }
                    break;
                default: break;
            }
        }

        return {currentModule, symbols};
    }
};