# Brass
The Brass syntax is built on the idea of verbosity, because ***verbose code is safe code***.

## Table of Contents
- [Brass](#brass)
  - [Table of Contents](#table-of-contents)
  - [Types](#types)
    - [Custom Types](#custom-types)
      - [Type Declaration](#type-declaration)
      - [Enum Declarations](#enum-declarations)
      - [Exception Declarations](#exception-declarations)
  - [Variables](#variables)
  - [Functions](#functions)
    - [Methods](#methods)
  - [Control Flow](#control-flow)
    - [Loops](#loops)
      - [While Loop](#while-loop)
      - [Do-While Loop](#do-while-loop)
      - [For Loop](#for-loop)
      - [Times Loop](#times-loop)
      - [Foreach Loop](#foreach-loop)
      - [Break/Continue](#breakcontinue)
    - [Switch Block](#switch-block)
    - [Return](#return)
  - [Modifiers](#modifiers)
    - [Visibility Modifiers](#visibility-modifiers)
    - [Function Modifiers](#function-modifiers)
    - [Variable Modifiers](#variable-modifiers)
    - [Using Modifiers](#using-modifiers)
  - [Polymorphism](#polymorphism)
  - [Constructors and Destructors](#constructors-and-destructors)
  - [Implementations](#implementations)
  - [Generics](#generics)
  - [Operator Overloading](#operator-overloading)
  - [Memory](#memory)
    - [Arrays](#arrays)
    - [Normal Pointers](#normal-pointers)
    - [Null](#null)
  - [Exceptions](#exceptions)
    - [Throwing Exceptions](#throwing-exceptions)
  - [Operators](#operators)
  - [Casts](#casts)
    - [Implicit Conversions](#implicit-conversions)
  - [Function Overloading](#function-overloading)
  - [Modules](#modules)
  - [Final Overview](#final-overview)


## Types
The Brass Type System will try to implicitly convert types whenever possible. Here is a table of the built in types:

Name    |Storage|True Type  |Description                    |
:------:|:-----:|:---------:|:-----------------------------:|
u8      | Stack | u8        |Unsigned 8 bit integer         |
s8      | Stack | s8        |Signed 8 bit integer           |
u16     | Stack | u16       |Unsigned 16 bit integer        |
s16     | Stack | s16       |Signed 16 bit integer          |
u32     | Stack | u32       |Unsigned 32 bit integer        |
s32     | Stack | s32       |Signed 32 bit integer          |
u64     | Stack | u64       |Unsigned 64 bit integer        |
s64     | Stack | s64       |Signed 64 bit integer          |
f32     | Stack | f32       |32 bit floating point number   |
f64     | Stack | f64       |64 bit floating point number   |
char    | Stack | u8        |ASCII character                |
bool    | Stack | u8        |Boolean value (0=false, 1=true)|
byte    | Stack | u8        |Unsigned 8 bit integer         |
int     | Stack | s32       |Signed 32 bit integer          |
uint    | Stack | u32       |Unsigned 32 bit integer        |
long    | Stack | s32       |Signed 32 bit integer          |
ulong   | Stack | u64       |Unsigned 64 bit integer        |
float   | Stack | f32       |32 bit floating point number   |
double  | Stack | f64       |64 bit floating point number   |

### Custom Types
There are **three** types of custom types in Brass:

#### Type Declaration
```
type Person {
    age -> s32;
    name -> string;
}
```

#### Enum Declarations
The type must be specified
```
enum AnimalType -> u8 {
    DOG, CAT, FISH,
    BIRD, SNAKE, MONKEY
}
```

#### Exception Declarations
These are no different from [types](#type-declaration), except they tell the compiler they can be thrown, so they hold exception data.
```
exception Error {
    line -> s32;
    code -> string;
}
```

## Variables
To declare a variable in Brass, write:
```
var x -> s32 = 12;
```
***OR***
```
var x -> s32;
```
The type is ***required*** to be declared as Brass is a strongly typed language.

Variables in Brass are inherently immutable.

## Functions
To declare a function in Brass, write:
```
fn greet(name -> string) -> void {
    std::io::println($"Hello, {name}!");
}
```

### Methods
To declare a method in Brass, write:
```
fn (Person) Birthday() -> s32 {
    self.age++;
}
```

## Control Flow
### Loops
There are ***five*** loops in Brass:

#### While Loop
```
while i < 4 {
    std::io::println(i++);
}
```

#### Do-While Loop
```
do {
    std::io::println(i++);
} while i < 4;
```

#### For Loop
```
for i -> s32; i < 12; i++ {
    std::io::println(i);
}
```

#### Times Loop
Times loops are for when you need to run a loop X times, to avoid the long declaration of a [for loop](#for-loop).
```
times 12 {
    std::io::println();
}
```

#### Foreach Loop
Foreach loops can only operate on objects of type list.
```
foreach name in names {
    std::io::println(name);
}
```
#### Break/Continue
```break``` and ```continue``` are used to control the flow of a loop.

```break``` breaks and exits the flow of a loop

```continue``` interrupts and starts a new iteration

### Switch Block
Switch blocks are much faster for primitive types:
```
switch age {
    case 0 {
        ...
    }
    case 1 {
        ...
    }
    default {
        ...
    }
}
```
There is no ```case 0: ...```

Additionally, there are no fallthroughs, to have the same effect on multiple values:
```
case 0 or 1
```

***CASES DO NOT HAVE TO BE SUFFIXED WITH ```break```, NO FALLTHROUGHS***


### Return
```return``` returns a block to the caller. Here are some usages:
```
fn add(a -> s32, b -> s32) -> s32 {
    return a + b;
}
```
```
fn print(str -> String) -> void {
    if str == "" {
        return;
    }

    std::io::println(str);
}
```
```
fn printWithTerminator(str -> String, terminator -> char) -> void {
   return print(str + terminator); 
}
```

## Modifiers
*TODO: Attributes will eventually be added*

### Visibility Modifiers
Name            |Scope      |
----------------|-----------|
```public```    |Global     |
```protected``` |Derivatives|
```internal```  |Module     |
```local```     |File       |
```private```   |Type       |

### Function Modifiers
```inline```    - Compiler hint to inline the function

```cdecl```     - Compiler hint to use C Calling Convention

```extern```    - Compiler hint that function has no body and is declared elsewhere

```pure```      - Guarantees compiler that function has no side effects

```sealed```    - Prevents overriding of function

```abstract```  - Only allowed in abstract types, says derivatives define function

### Variable Modifiers
```unmanaged``` - Compiler hint not to manage the variable

```volatile```  - Compiler hint that variable may be modified

```mut```       - Sets variable as mutable

```comptime```  - Compiler hint to resolve variable at compile time

### Using Modifiers
Modifiers are in order of:

**\<VISIBILTIY MODIFIER>** (fn or var) **\<OTHER MODIFIERS>**

## Polymorphism
To derive a type from another, do:
```
type String from List<Char> {

}
```

To prevent derivatives:
```
type sealed String from List<Char> {

}
```

To override a function:
```
fn override ToString(fmt -> string = "") {

}
```

To implement abstract types:
```
type abstract Container<T> {

}
```
Yes. I just hinted that there are generics.

## Constructors and Destructors
Brass has constructors and destructors, exactly like C++
```
String(data -> List<char>) {

}

~String() {

}
```

To call a constructor, run:
```var str -> String = String(list);```

To call a destructor, run:
```delete str;```

Types also have a ```default``` value that is a compile time resolution.

## Implementations
A huge feature in Brass. The fact that it's buried down here is even insane.

Implementations are essentially contained programs, their data cannot be edited from outside. And instances cannot be used as data.

```
impl Life -> s32 {
    
}
```

To declare an object of type Life:
```
var tony -> Life* = new Life;
```
**BE SURE NOT TO CALL THE CONSTRUCTOR**

The entry point of an implementation is a constructor:
```
impl Life throws InvalidOperation, ArgumentException -> s32 {
    Life(father -> Person, mother -> Person) {

    }
}
```

To call an implementation:
```
var result -> s32 = tony(father, mother);
```

## Generics
Generics can be applied to types:
```
type List<T> {

}
```
or functions:
```
fn <T> Print(arg -> T) {

}
```

## Operator Overloading
A select group of operators can be overloaded using the syntax:
```
operator String +(char c) {
    
}
```
[Find it here.](#operators)

## Memory
### Arrays
**BRASS DOES NOT HAVE TRADITIONAL ARRAYS**

I know. **Shocking.**

Brass does not have arrays because the arrays you're thinking enough are unnecessary. 
```c
int x[4];
```

Not a thing in Brass. If you want to declare an array, it is:
```
var x -> int* = new int[4];
```
Brass will automatically delete an object unless instructed not to.

To manually delete an array:
```
delete[] x;
```

***BUT***

If this array was not declared in the codebase, that line will not work. Because the compiler has no size.
```c++
int* ptr = new int[4];
brassCode(ptr, 4);
```
Then in Brass:
```
fn cdecl brassCode(ptr -> int*, size -> s32) {
    delete[size] ptr;
}
```

### Normal Pointers
The exact same as C in most cases:
```
var x -> int = 0;
var ptr -> int* = &x;
```

To get a pointer from a constructor:
```
var ptr -> Person* = new Person("Noah");
```

To get a pointer from a primitive type:
```
var ptr -> int* = new int;
```

### Null
```null``` is a keyword signifying that a variable has not been assigned yet.

Variables can be made nullable using ```?```:
```
var val -> int?;
```

## Exceptions
Try, catch, finally, as standard
```
try {

} catch Error {

} finally {

}
```
***OR***
```
try {

} catch e -> Error {

} finally {

}
```

### Throwing Exceptions
To throw an exception:
```
throw e;
```

***BUT***

If rethrowing an already caught exception:
```
throw;
```

Functions in Brass are by default ```noexcept```, to allow a function to throw a type:
```
fn runCode(code -> List<byte>) throws Error -> s32 {

}
```

## Operators
Operator|Precedence |Overloadable?  |Arity  |
:------:|:---------:|:-------------:|:-----:|
```=``` |```0```    |No             |Binary |
```+=```|```0```    |No             |Binary |
```-=```|```0```    |No             |Binary |
```*=```|```0```    |No             |Binary |
```/=```|```0```    |No             |Binary |
```%=```|```0```    |No             |Binary |
```&=```|```0```    |No             |Binary |
```\|=```|```0```   |No             |Binary |
```^=```|```0```    |No             |Binary |
```~=```|```0```    |No             |Binary |
```<<=```|```0```   |No             |Binary |
```>>=```|```0```   |No             |Binary |
```?``` |```1```    |No             |Ternary|
```\|\|```|```2```  |No             |Binary |
```&&```|```3```    |No             |Binary |
```\|```|```4```    |Yes            |Binary |
```^``` |```6```    |Yes            |Binary |
```&``` |```7```    |Yes            |Binary |
```==```|```8```    |Yes            |Binary |
```!=```|```8```    |Yes            |Binary |
```is```|```8```    |No             |Binary |
```<``` |```9```    |Yes            |Binary |
```<=```|```9```    |Yes            |Binary |
```>``` |```9```    |Yes            |Binary |
```>=```|```9```    |No             |Binary |
```or```|```10```   |No             |Binary |
```and```|```11```   |No            |Binary |
```<<```|```12```   |Yes            |Binary |
```>>```|```13```   |Yes            |Binary |
```+``` |```14```   |Yes            |Binary |
```-``` |```14```   |Yes            |Binary |
```*``` |```15```   |Yes            |Binary |
```/``` |```15```   |Yes            |Binary |
```%``` |```15```   |Yes            |Binary |
```sizeof``` |```16```|No           |Unary  |
```typeof``` |```16```|No           |Unary  |
```new``` |```16``` |No             |Unary  |
```delete``` |```16``` |No          |Unary  |
```&``` |```16```   |No             |Unary  |
```*``` |```16```   |No             |Unary  |
```!``` |```16```   |No             |Unary  |
```~``` |```16```   |Yes            |Unary  |
```+``` |```16```   |No             |Unary  |
```-``` |```16```   |No             |Unary  |
```++a```|```16```  |No             |Unary  |
```--a```|```16```  |No             |Unary  |
```a.b```|```17```  |No             |Binary |
```a->b```|```17``` |No             |Binary |
```[]```|```17```   |No             |Unary  |
```()```|```17```   |No             |Unary  |
```cast```|```17``` |[Yes](#casts)  |Unary  |
```a++```|```18```  |No             |Unary  |
```a--```|```18```  |No             |Unary  |
```a--```|```18```  |No             |Unary  |
```a::b```|```19``` |No             |Binary |

## Casts
Casts are made by writing constructors, for example:
```
String(data -> List<char>) {

}
```
**All custom casts in Brass are explicit.**

### Implicit Conversions
Implicit conversions in Brass are only with primitive types

- Integers convert up or down (converting to smaller bit width will throw a warning)
- Floating point numbers are the same, integers and floating point numbers convert to each other
- Pointers convert to integers but not floating points

## Function Overloading
Functions in Brass can be overriden. Functions are distinguished by parameters, they ***CANNOT*** be distinguished by return type
```
fn add(a -> s32, b -> s32) -> s32 {

}

fn add(a -> s16, b -> s16) -> s16 {

}
```

## Modules
Modules are not explicitly declared in Brass

To import a module, use the keyword ```using```

Take this file structure:
```
src/
|----main.brass
|----lexer.brass
|----parser.brass
|----codegen/
        |----ir.brass
        |----asm.brass
```
From ```main.brass```, the code would be:
```
using lexer;
using parser;
using codegen::ir;
using codegen::asm;
```

***Modules are explained more in [USAGE.md](USAGE.md)***

## Final Overview
Brass is a large and only growing language. Here is a full list of keywords and where to find them:

Keyword     |Location                       |Keyword    |Location                           |Keyword    |Location                           |
:----------:|:-----------------------------:|:---------:|:---------------------------------:|:---------:|:---------------------------------:|
u8          |[Types](#types)                |var        |[Variables](#variables)            |extern     |[Modifiers](#function-modifiers)   |
s8          |[Types](#types)                |fn         |[Functions](#functions)            |pure       |[Modifiers](#function-modifiers)   |
u16         |[Types](#types)                |while      |[Loops](#while-loop)               |sealed     |[Modifiers](#function-modifiers)   |
s16         |[Types](#types)                |do         |[Loops](#do-while-loop)            |abstract   |[Modifiers](#function-modifiers)   |
u32         |[Types](#types)                |for        |[Loops](#for-loop)                 |unmanaged  |[Modifiers](#variable-modifiers)   |
s32         |[Types](#types)                |times      |[Loops](#times-loop)               |volatile   |[Modifiers](#variable-modifiers)   |
u64         |[Types](#types)                |foreach    |[Loops](#foreach-loop)             |mut        |[Modifiers](#variable-modifiers)   |
s64         |[Types](#types)                |break      |[Loops](#breakcontinue)            |comptime   |[Modifiers](#variable-modifiers)   |
f32         |[Types](#types)                |continue   |[Loops](#breakcontinue)            |from       |[Polymorphism](#polymorphism)      |
f64         |[Types](#types)                |switch     |[Switch](#switch-block)            |override   |[Polymorphism](#polymorphism)      |
char        |[Types](#types)                |case       |[Switch](#switch-block)            |abstract   |[Oops, duplicate](#polymorphism)   |
bool        |[Types](#types)                |default    |[Switch](#switch-block)            |new        |[Memory](#memory)                  |
byte        |[Types](#types)                |return     |[Returns](#return)                 |delete     |[Memory](#memory)                  |
int         |[Types](#types)                |public     |[Visibility](#visibility-modifiers)|operator   |[Operators](#operator-overloading) |
uint        |[Types](#types)                |protected  |[Visibility](#visibility-modifiers)|try        |[Try-Catch](#exceptions)           |
long        |[Types](#types)                |internal   |[Visibility](#visibility-modifiers)|catch      |[Try-Catch](#exceptions)           |
ulong       |[Types](#types)                |local      |[Visibility](#visibility-modifiers)|finally    |[Try-Catch](#exceptions)           |
float       |[Types](#types)                |private    |[Visibility](#visibility-modifiers)|throw      |[Exceptions](#exceptions)          |
double      |[Types](#types)                |inline     |[Modifiers](#function-modifiers)   |throws     |[Exceptions](#exceptions)          |
type        |[Custom Types](#custom-types)  |cdecl      |[Modifiers](#function-modifiers)   |using      |[Modules](#modules)                |
enum        |[Custom Types](#custom-types)  |and        |[Operators](#operators)            |or         |[Operators](#operators)            |
exception   |[Custom Types](#custom-types)  |typeof     |[Operators](#operators)            |impl       |[Implementations](#implementations)|
is          |[Operators](#operators)        |null       |[Null Types](#null)                |self       |[Methods](#methods)                |