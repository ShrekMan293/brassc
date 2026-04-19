# Brass Language Specifications
These are the specifications of the Brass compiler. Tool for contributors

- [Brass Language Specifications](#brass-language-specifications)
- [.bmi Formatting:](#bmi-formatting)
  - [Header](#header)
  - [Magic](#magic)
  - [Modules](#modules)
    - [Header](#header-1)
  - [Symbols](#symbols)
    - [Symbol Kind](#symbol-kind)
    - [Symbol Modifiers](#symbol-modifiers)
      - [Function Modifiers](#function-modifiers)
      - [Variable Modifiers](#variable-modifiers)
      - [Type Modifiers](#type-modifiers)
    - [Type Flags](#type-flags)
  - [Footer](#footer)


# .bmi Formatting:
While you will likely never need this info, it is worth noting.

## Header
Bytes   |   Function
--------|-----------
0-5     | [Magic](#magic)
6-7     | Module Count
8       | Reserved

## Magic
The Brass magic for language 0.0.1 is:

Byte | Value
-----|------
0    | 'B'
1    | 'R'
2    | 'C'
3    | '0'
4    | '0'
5    | '1'

## Modules
Modules have a format similar to the main header:

### Header
Bytes   |   Function
--------|-----------
0-1     | Symbol Count
2-X     | Name (null terminated)

## Symbols
Bytes   |   Function
--------|-----------
0       | [Kind](#symbol-kind)
1       | [Modifiers](#symbol-modifiers)
2-3     | [Type Flag](#type-flags)
4-X     | Name (null terminated)

### Symbol Kind
Value   | Kind
--------|------
```0``` | Variable
```1``` | Function
```2``` | Enum
```3``` | Type
```4``` | Exception
```5``` | Implementation
```6``` | Operator
```7``` | Field
```8``` | Argument
```9``` | Constructor
```10```| Destructor

### Symbol Modifiers
Four symbols need flags:
[functions](#function-modifiers), [variables](#variable-modifiers), [types](#type-modifiers), [and operators](#function-modifiers)

*Functions and operators share modifiers*

#### Function Modifiers
Bits    |   Function
:------:|-----------
0       | C Declaration
1       | Extern
2       | Abstract
3       | Pure
4       | Sealed
5       | Inline
6       | Override
7       | Reserved

#### Variable Modifiers
Bits    |   Function
:------:|-----------
0       | Unmanaged
1       | Volatile
2       | Mutable
3       | Compile Time
4       | Override
5-7     | Reserved

#### Type Modifiers
Bits    |   Function
:------:|-----------
0       | Primitive
1       | Abstract
2       | Sealed
3       | Packed
4-7     | Reserved

### Type Flags
Set the type this symbol carries
Bits    |   Function
:------:|-----------
0-12    | Size (bytes) OR type ID
13      | Signed/Unsigned
14      | Floating Point or Integer
15      | Built in/User

*The type ID is automatically tracked in code*

## Footer
The footer is the magic repeated again to ensure validity