Brainfucko is a brainfuck interpreter with a REPL.
It allocates a fixed amount of memory for the program and nothing more.
The amount can be changed at compile time by modifying the MEMSIZE constant;
by default it's 30 000 bytes.

## Compiling

With GNU Make or similar:

```
make
```

## Usage

### From a file

```
./brainfucko <filename>
```

The examples directory contains program I've used to test if the interpreter works.

Exit codes:

- 0 – success
- 1 – error in the interpreter
- 2 – brainfuck syntax error
- 3 – brainfuck execution error

Brainfucko will signal an error and stop execution if:

- the program can't be read or memory can't be allocated (exit code 1)
- there's an error in loop syntax ([s and ]s don't match) (exit code 2)
- there's an attempt to move past allowed memory (exit code 3)

### REPL

```
./brainfucko --repl
```

The prompt shows number of the current memory cell and its value:

```
0: 0> +++
0: 3> >++
1: 2> 
```

Any output printed by the brainfuck is always on a separate line.
If a newline is printed then to make that clear two newlines are shown.

```
0: 0> ++++++++++[>+++++<-]>
1: 50> .
2
1: 50> <++++[>----------<-]>.


1: 10>
```

Errors during execution halt the execution, but don't exit the REPL and don't undo
anything that was done before the error was encountered.

```
0: 0> ++++]
ERROR: Bad Syntax: ']' without matching '['
0: 4>
```

Additional commands in the REPL are:

- :h – show help
- :q – quit
- :m – print values of all non-zero memory cells

#### Read, eval... print?

Yeeeah let's just say that the value of the current cell in the prompt is the "print" part of REPL.
