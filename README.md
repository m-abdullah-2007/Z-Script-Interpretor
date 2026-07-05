<div align="center">

# ZScript

**A small interpreted scripting language, built from scratch in C++.**

![Language](https://img.shields.io/badge/language-C%2B%2B17-00599C)
![GUI](https://img.shields.io/badge/GUI-raylib-orange)
![Type](https://img.shields.io/badge/type-academic%20project-lightgrey)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)

</div>

---

ZScript is a lexer-to-interpreter pipeline written entirely from first principles for an Object-Oriented Programming semester project. It ships in two forms:

- **Terminal interpreter** — reads a `.txt` script and runs it from the console.
- **ZScript IDE** — a [raylib](https://www.raylib.com/)-based desktop GUI with a built-in code editor, theme picker, and an error popup system.

Both share the same lexer, parser, AST, and interpreter core — only the input/output layer differs.

---

## Table of Contents

- [Highlights](#highlights)
- [Language Reference](#language-reference)
  - [Variables](#variables)
  - [Data Types](#data-types)
  - [Operators](#operators)
  - [Print](#print)
  - [Control Flow](#control-flow)
  - [Scoping](#scoping)
  - [Comments](#comments)
  - [Errors](#errors)
- [Example Program](#example-program)
- [Getting Started](#getting-started)
  - [Option 1 — Prebuilt binary (Windows)](#option-1--prebuilt-binary-windows)
  - [Option 2 — Terminal interpreter from source](#option-2--terminal-interpreter-from-source)
  - [Option 3 — GUI IDE from source](#option-3--gui-ide-from-source)
- [Project Structure](#project-structure)
- [Architecture Overview](#architecture-overview)
- [Known Limitations](#known-limitations)
- [About](#about)

---

## Highlights

- C-like syntax — `let`, `if` / `else`, `while`, `print`
- Three value types: number (`double`), string, and boolean
- Lexical block scoping via a parent-pointer environment chain
- Reference-counted values, so script execution does not leak memory
- Hand-written `MyString` and `myVector<T>` — the interpreter core uses no STL containers
- Detailed syntax and runtime error reporting with line numbers
- A full desktop IDE — theme picker, code editor, scrollable output panel, and error popups — built on raylib

---

## Language Reference

### Variables

Declare with `let`. Re-declaring the same name in the same scope overwrites it; use plain assignment to update an existing variable.

```zscript
let x = 10;
x = x + 1;        // assignment — variable must already exist
```

Assigning to a variable that was never declared with `let` fails silently: `assign()` walks up the scope chain, and if no binding is found anywhere, the statement simply has no effect. Always declare with `let` before assigning.

### Data Types

| Type      | Example         | Falsy when             |
|-----------|-----------------|-------------------------|
| `number`  | `10`, `3.5`     | value is `0`            |
| `string`  | `"hello"`       | value is `""` (empty)   |
| `boolean` | `true`, `false` | value is `false`        |

Numbers are stored internally as `double`. Integers and decimals are written the same way (`10` or `10.5`) — ZScript does not distinguish `int` vs `float` at the language level.

### Operators

| Operator             | Meaning                                                | Works on         |
|----------------------|---------------------------------------------------------|-------------------|
| `+`                  | addition, or string concatenation if either side is a string | number, string |
| `-` `*` `/`          | subtraction, multiplication, division                  | number only       |
| `<` `>` `<=` `>=`    | numeric comparison                                     | number only       |
| `==` `!=`            | equality — compares type **and** value                 | any               |
| `=`                  | assignment                                             | —                 |

Notes:

- `+` between a string and anything else stringifies the non-string side and concatenates — e.g. `"score: " + 10` → `"score: 10"`.
- Dividing by zero throws a runtime error; it does not return `inf` or crash.
- Comparison operators (`<`, `>`, `<=`, `>=`) only accept two numbers — using them on strings or booleans throws a type error.

### Print

```zscript
print(expression);
```

`print` evaluates any expression and writes its string form to the output — the console in the terminal build, the scrollable **OUTPUT** panel in the GUI build.

### Control Flow

**If / else**

```zscript
if (condition) {
    // ...
} else {
    // ...
}
```

The `else` branch is optional. Braces are **required** — there is no single-statement shorthand.

**While loops**

```zscript
while (condition) {
    // ...
}
```

Loops until `condition` evaluates to a falsy value.

### Scoping

Every `{ }` block — the body of an `if`, `else`, or `while` — creates a new child scope. Variables declared with `let` inside a block do not leak out:

```zscript
let outer = 1;
if (true) {
    let inner = 2;   // only visible inside this block
    print(outer);    // 1 — parent scope is visible from inside
}
print(inner);        // ERROR — inner does not exist out here
```

Reading a variable from an enclosing scope works fine, since lookups walk up the parent chain. Declaring a *new* variable inside a block, however, never escapes it.

### Comments

```zscript
// single-line comment — everything after // to end of line is ignored
```

There is no block-comment syntax (`/* ... */`).

### Errors

ZScript reports two kinds of errors, each with the line number where it occurred:

- **Syntax errors** — malformed code caught while reading the file (missing `;`, unmatched `{`, unexpected token, unterminated string, etc.). Execution stops immediately, and only the first syntax error is shown.
- **Runtime errors** — problems found while running otherwise-valid code (undefined variable, type mismatch such as `"a" < 5`, division by zero). The terminal build keeps running and collects every runtime error it hits, then prints the total count at the end. The GUI build surfaces them in a scrollable popup.

---

## Example Program

```zscript
// Variables and arithmetic
let x = 10;
let y = 20;
print(x + y);          // 30

// Strings
let name = "ZScript";
print("Hello, " + name);   // Hello, ZScript

// Conditionals
if (x < y) {
    print("x is smaller");
} else {
    print("x is not smaller");
}

// Loops
let counter = 3;
while (counter > 0) {
    print(counter);
    counter = counter - 1;
}
```

A larger sample covering every language feature is included in [`test.txt`](./BSCS25017_FinalProject/BSCS25017_FinalProject/test.txt).

---

## Getting Started

### Option 1 — Prebuilt binary (Windows)

1. Download `zscript.exe` from [`BSCS25017_FinalProject/BSCS25017_FinalProject`](./BSCS25017_FinalProject/BSCS25017_FinalProject), keeping `raylib.dll` in the same folder.
2. Place the script you want to run next to the executable, or use the GUI's "Open .txt" / filename screen to point at one.
3. Double-click to launch. You will land on the **theme picker** first (Light / Dark), then choose:
   - **Run a file** — type a filename (must sit next to the executable) and press Enter or click Load.
   - **Write code** — opens straight into the editor with a starter snippet.
4. Click **Run** in the toolbar, or edit the code first. Output appears in the panel below the editor; errors open as a popup — press **Esc** or click outside it to dismiss.

### Option 2 — Terminal interpreter from source

Requires only a C++ compiler (C++11 or later) — no external libraries.

```bash
cd BSCS25017_FinalProject/BSCS25017_FinalProject
g++ -std=c++17 -O2 terminal_main.cpp -o zscript
./zscript
```

This reads `test.txt` from the current working directory by default (see `read_file("test.txt")` in `terminal_main.cpp`). To run a different script, edit the `filename` variable in `terminal_main.cpp`'s `main()`, or rename your script to `test.txt`.

> **Note:** `terminal_main.cpp` currently includes `raylib.h`, even though the console build does not call any raylib functions. If you are compiling the terminal version standalone without raylib installed, remove that `#include "raylib.h"` line first.

### Option 3 — GUI IDE from source

Requires [raylib](https://github.com/raysan5/raylib) installed and linkable.

**Windows (MinGW):**
```bash
g++ -std=c++17 -O2 gui_main.cpp -o zscript_ide.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

**Linux:**
```bash
g++ -std=c++17 -O2 gui_main.cpp -o zscript_ide -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

**macOS:**
```bash
g++ -std=c++17 -O2 gui_main.cpp -o zscript_ide -lraylib -framework OpenGL -framework Cocoa -framework IOKit
```

Run the resulting binary:
```bash
./zscript_ide        # or zscript_ide.exe on Windows
```

---

## Project Structure

```
Z-Script-Interpretor/
├── BSCS25017_FinalProject/
│   ├── BSCS25017_FinalProject.slnx          # Visual Studio solution
│   └── BSCS25017_FinalProject/
│       ├── terminal_main.cpp                # Console entry point — reads test.txt, runs it, prints to stdout
│       ├── gui_main.cpp                     # raylib IDE entry point — themes, editor, output panel, popups
│       ├── Lexer.h                          # Tokenizer: source text -> Token stream
│       ├── Parser.h                         # Recursive-descent parser: tokens -> AST
│       ├── Expression.h                     # Expression node classes (literals, variables, binary ops)
│       ├── Statement.h / Statement.cpp      # Statement node classes (let, print, if, while, block, assign)
│       ├── Environment.h                    # Variable scope chain (Binding struct + Environment class)
│       ├── Value.h                          # Runtime value types: NumberValue, StringValue, BoolValue
│       ├── ErrorHandler.h                   # Singleton error collector/printer
│       ├── ZSCRIPT_ERROR.h                  # ZScriptError struct + ErrorType enum, thrown as C++ exceptions
│       ├── MyString.h                       # Custom string class (no std::string in the interpreter core)
│       ├── MyVector.h                       # Custom dynamic array template (no std::vector)
│       ├── test.txt                         # Sample ZScript program exercising every language feature
│       ├── zscript.exe                      # Prebuilt Windows binary
│       └── raylib.dll                       # Required alongside zscript.exe at runtime
├── bscs25017_Design.pptx                    # OOP design slides
├── bscs25017_Documentation.docx             # Design document
└── README.md
```

## Architecture Overview

```
source text
    │
    ▼
Lexer.tokenize()          →  flat array of Token { type, lexeme, line }
    │
    ▼
Parser.parse()             →  Statement* statements[256] (top-level), built via
    │                          recursive descent over operator precedence
    ▼
for each statement:
  statement->execute(env)  →  virtual dispatch into LetStmt / PrintStmt /
    │                          IfStmt / WhileStmt / BlockStmt / AssignStmt
    ▼
expression->evaluate(env)  →  virtual dispatch into NumberLiteral / StringLiteral /
                               BooleanLiteral / VariableExpr / BinaryExpr,
                               returns a reference-counted Value*
```

Both `terminal_main.cpp` and `gui_main.cpp` drive this exact same pipeline. The only difference is what happens to a `print` statement's output:

- **Terminal** — `PrintStmt::execute()` falls back to `cout << ... << endl` when no handler is set.
- **GUI** — `gui_main.cpp` installs a handler via `PrintStmt::set_handler(...)` before each run, redirecting output into the on-screen panel instead of the console.

---

## Known Limitations

- No functions or procedures — every script is a flat sequence of top-level statements (capped at 256) plus nested blocks.
- No arrays, objects, or `let`-typed type annotations.
- No unary operators (no `-x` or `!x`) — see the commented-out `parse_unary()` stub in `Parser.h`.
- Numbers are always `double` internally, so very large integers can lose precision.
- The terminal build's `read_file` only looks in the current working directory.

---

## About

ZScript was designed and built by **Muhammad Abdullah** as the final project for an Object-Oriented Programming course, covering the full pipeline of a programming language — lexer, parser, AST, tree-walking interpreter, and a desktop IDE — implemented entirely from scratch in C++.
