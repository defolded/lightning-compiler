# lightning - a simple programing language I wish I had when I was a kid

## Installation
### Prerequisites
You need to have these tools installed:
* `g++` or `clang` -- C++ Compiler 
* `nasm` -- Netwide Assembler
* `ld` -- The GNU Linker
* `cmake` -- To build the project
* `git` -- The version-control program to download the repository

### Download the repository
Run the following command in a [`bash`](https://en.wikipedia.org/wiki/Bash_(Unix_shell)) shell.
```bash
git clone https://github.com/defolded/lightning
cd lightning
```
Now you are in the `lightning` directory

### Configure the build
Now run 
```bash
cmake -S . -B build/
```
This will setup everything for the next step.

### Build the compiler executable
Now you need to run 
```bash
cmake --build build/
```

This will create the `lightning` executable in the `build\` directory. 

### Run executable
The basic usage is simple. Just use 
```bash
lightning <input file>
```
This command will generate an `out` file from the input file which you can execute

### Edit code
Edit file
```bash
main.ly
```

## Syntax 
### Basic Syntax
lightning uses spaces, dots, or commas to separate tokens.
To calculate an expression or if you want to return a variable use parentheses: `(` and `)`.

### Basic Keywords
There are some basic keywords in the language right now. </br>
These include:
* `буде` -- Declare variables.
* `якщо` -- Add an `if` statement.
* `повернути` -- Add a `return` statement and exit the program.
* `то` -- Does not have a functionality, used primarily for conciseness in language.
