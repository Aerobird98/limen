# Limen

```brainfuck
++++[>++++<-]>[<+>-]<[>++<-]>[<+>-]<.[+.]
```

Limen means _threshold_ in latin. Limen is Brainfuck which is a turing complete **esoteric** programming language created in _1992_ by Urban Müller. Its name is a reference to the _slang_ term _brainfuck_, which refers to things so **complicated** or **unusual** that they exceed the limits of one's understanding and it is certain that one would realy need to cross a cretain _threshold_ in order to use this language.

Brainfuck is notable for its **minimalism**. At the core of the language is a more-than-compact instruction set, comprising of **8** simple **instructions**, yet in theory with some creative thinking and the ability to break down complex tasks into microscopic steps you are more than capable of writing almost any program one can think of.

From now on Brainfuck is the language and Limen refers to the implementation. In most cases when you see Brainfuck it refers to Limen.

## Semantics

Brainfuck uses a machine model consisting of an _infinite_ stream of one-byte values initialized to zeros, an instruction pointer and a stream pointer. The instruction pointer moves trough all the **instructions** which can be used to operate the stream pointer and interact with the stream:

`+` – Increments the value at the stream pointer. The value _wraps around_ to 0 after reaching 127,

`-` – Decrements the value at the stream pointer. The value _wraps around_ to 127 after reaching 0,

`>` – Moves the stream pointer to the next value and _grows the stream_ when needed,

`<` – Moves the stream pointer to the previous value making sure it remains _on the stream_,

`.` – Outputs the value at the stream pointer as an _ASCII character_,

`,` – Inputs a character and sets the value at the stream pointer to its _ASCII value_,

`[` – If the value at the stream pointer is 0, jumps to the matching `]`,

`]` – If the value at the stream pointer is not 0, jumps to the matching `[`.

In the end, a program is just a series of instructions. Characters besides the eight `+-<>[],.` instructions considered as comments. The eight `+-<>[],.` instructions cannot be used as comment-characters. However if you want to use the entire set of available characters wrap them up in `( )` parenthesis, courtesy of Limen. They function much like `/* */` block comments in C and other languages, but unlike those, block comments can nest here. This is handy because it lets us easily comment out an entire block of code, even if it already contains block-comments.

## Implementation

Limen is written ex nihilo and stands for a compact, secure and reliable library and interpreter for this remarkable language.

- **It’s compact and clean.** – Limen is rather compact, readable and logical with friendly comments all the way trough. You can skim the whole thing with ease in just an afternoon.

- **It’s secure and reliable.** – Memory use is dynamic and strictly contained. The core uses zero static data and it does not leak memory. Limen has reliable and user-friendly error handling mechanisms in place.

- **It’s fast and efficient.** – Splits evaluation into lex-parse-compile- and run time; it does all the validation and optimization at lex-parse-compile- while at run time only checks for memory- and stream management errors.

- **It’s made in the name of science!** – Comes with a **REPL** — an iteractive environment that vizualizes the stream and its pointer before every evaluation which makes it well suited for learning and experimentation which is the main reason why Brainfuck exists in the first place. (Comming soon.)

Limen is **encoding agnostic**, user code and data are validated to only contain standard **ASCII** characters in the range of `0-127`. While I admit that it is rather strange, it does make the implementation more secure and reliable which was key troughout development. All other characters are ignored.

## Embeding

Limen is made to be **embedable**, implemented as a small C _library_ consisting of only a `.c` and an `.h` file; written in ANSI C with **NO** dependencies other than a _few_ C standard library functions.

You only need two files to include in your program, the `limen.c` file and `limen.h` file. Copy those and you are good to go. Import the `limen.h` file to access the implementation.

In its most simple form you only need to:

```c
// Declare state.
State state;

// Initialize state.
initState(&state);

// Run eval on a piece of code, manipulating state and returning a result.
Result result = eval(&state, "++++[>++++<-]>[<+>-]<[>++<-]>[<+>-]<.[+.]", "");

// Handle result, visualize state, etc.
if (result == RESULT_OK) {
    fprintf(stdout, "%s\n", state.response.values);
} else {
    fprintf(stderr, "Error: Unhandled error.");
}

// Free state.
freeState(&state);
```

For a more complete usage-sample, consult with the `main.c` file which contains an implementation of a possible interpreter.

## Get, Compile & Conquer

Limen lives on [Github](). To play around with it, sync it down then:

- Configure the `makefile` to suit your needs.
- Run `make` to build like _normal_.
- Run `make install` to install the program.
- Go back and copy the `<code>` at the head of this `README`, `<data>` is only needed when you want to use the `,` instruction in your code otherwise leave it empty.
- Run `limen <code> <data>`.

  If everything goes well, you will see an empty stream and all the ASCII characters in the range of `32-127`.

  ```
  [*0]
  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
  ```

  Without *argument*s, it drops you into a **REPL** — an interactive session. You can type in instructions and it will evaluate them immediately while vizualizes the stream and its pointer before every evaluation. (Comming soon.)

- Run `make clean` to clean all built files.
- Run `make uninstall` if you are not satisfied enough.

## Learning & Experimentation

This section covers the usage of the language according to my implementation.

### Basics

To understand how Brainfuck works, imagine an _infinite_ array of values called a stream full of zeros:

```
[0][0][0][0]...
```

There is a pointer represented by an `*` visualy. When you run your program it points to the first value on the stream:

```
[*0][0][0][0]...
```

If you move the pointer right with an `>` instruction, it moves from value `x` to value `x + 1`:

```
[0][*0][0][0]...
```

And if you move it back left with an `<` instruction, it moves from value `x` to value `x - 1`:

```
[*0][0][0][0]...
```

If you move it again left with an `<` instruction, it terminates your program with an error:

```
Error: Array underflow.
```

If you increase a value with an `+` instruction:

```
[*1][0][0][0]...
```

If you increase again:

```
[*2][0][0][0]...
```

And if you then decrease it with an `-` instruction:

```
[*1][0][0][0]...
```

If you decrease again:

```
[*0][0][0][0]...
```

If you decrease when the value where the pointer is zero, it wraps around to `127`.
The stream changes from this:

```
[*0][0][0][0]...
```

To this:

```
[*127][0][0][0]...
```

If you increase when the value where the pointer is `127`, it wraps around to zero.
The stream changes from this:

```
[*127][0][0][0]...
```

To this:

```
[*0][0][0][0]...
```

### Input & Output

Behold an ASCII table. It contains all the possible values and characters that could exist in Brainfuck.

| value | character | value | character | value | character | value | character |
| ----- | --------- | ----- | --------- | ----- | --------- | ----- | --------- |
| 0     | NUL       | 32    | Space     | 64    | @         | 96    | `         |
| 1     | SOH       | 33    | !         | 65    | A         | 97    | a         |
| 2     | STX       | 34    | "         | 66    | B         | 98    | b         |
| 3     | ETX       | 35    | #         | 67    | C         | 99    | c         |
| 4     | EOT       | 36    | $         | 68    | D         | 100   | d         |
| 5     | ENQ       | 37    | %         | 69    | E         | 101   | e         |
| 6     | ACK       | 38    | &         | 70    | F         | 102   | f         |
| 7     | BEL       | 39    | '         | 71    | G         | 103   | g         |
| 8     | BS        | 40    | (         | 72    | H         | 104   | h         |
| 9     | TAB       | 41    | )         | 73    | I         | 105   | i         |
| 10    | LF        | 42    | \*        | 74    | J         | 106   | j         |
| 11    | VT        | 43    | +         | 75    | K         | 107   | k         |
| 12    | FF        | 44    | ,         | 76    | L         | 108   | l         |
| 13    | CR        | 45    | -         | 77    | M         | 109   | m         |
| 14    | SO        | 46    | .         | 78    | N         | 110   | n         |
| 15    | SI        | 47    | /         | 79    | O         | 111   | o         |
| 16    | DLE       | 48    | 0         | 80    | P         | 112   | p         |
| 17    | DC1       | 49    | 1         | 81    | Q         | 113   | q         |
| 18    | DC2       | 50    | 2         | 82    | R         | 114   | r         |
| 19    | DC3       | 51    | 3         | 83    | S         | 115   | s         |
| 20    | DC4       | 52    | 4         | 84    | T         | 116   | t         |
| 21    | NAK       | 53    | 5         | 85    | U         | 117   | u         |
| 22    | SYN       | 54    | 6         | 86    | V         | 118   | v         |
| 23    | ETB       | 55    | 7         | 87    | W         | 119   | w         |
| 24    | CAN       | 56    | 8         | 88    | X         | 120   | x         |
| 25    | EM        | 57    | 9         | 89    | Y         | 121   | y         |
| 26    | SUB       | 58    | :         | 90    | Z         | 122   | z         |
| 27    | ESC       | 59    | ;         | 91    | [         | 123   | {         |
| 28    | FS        | 60    | <         | 92    | \\        | 124   | \|        |
| 29    | GS        | 61    | =         | 93    | ]         | 125   | }         |
| 30    | RS        | 62    | >         | 94    | ^         | 126   | ~         |
| 31    | US        | 63    | ?         | 95    | \_        | 127   | DEL       |

With an `.` instruction, you can print the value where the pointer is as an ASCII character.

Imagine the stream as:

```
[*97][0][0][0][0]...
```

When you use an `.` instruction now, it prints a lowercase `a`. If the value where the pointer is would be `66` it would print an uppercase `A`, etc..

An `,` instruction works the opposite, it reads a character from the provided user data or prompt and sets the value where the pointer is to its corresponding ASCII value.

Imagine the prompt as:

```
aA
```

And the stream as:

```
[*0][0][0][0][0]...
```

When you use an `,` instruction now, the stream becomes:

```
[*97][0][0][0][0]...
```

If you use an `.` instruction now, it prints:

```
a
```

Move the pointer to the right with an `>` instruction and the stream becomes:

```
[97][*0][0][0][0]...
```

And use an `,` instruction again, the stream will be:

```
[97][*66][0][0][0]...
```

If you use an `.` instruction now, it prints:

```
A
```

### Loop

A loop consists of an opening `[` instruction and a closing `]` instruction. Imagine them like a body of a `while` loop in C where the condition corresponds to the value where the pointer is. A value of zero terminates the loop while any other value makes sure it goes on.

Imagine your program as:

```brainfuck
++[]
```

At run-time the stream looks like this:

```
[*2][0][0][0][0]...
```

To explain, that program increments the value where the pointer is by two and opens a loop that corresponds to `while (2) {}` in C, an infinite loop that goes on forever.

A finite loop could be expressed as:

```brainfuck
++[-]
```

It corresponds to `int x = 2; while(x) { --x; }` in C. At run-time the stream looks like this:

```
[*2][0][0][0]...        First iteration opens loop.
[*1][0][0][0]...        Second iteration goes on.
[*0][0][0][0]...        Third iteration closes loop.
```

To explain, that program increments the value where the pointer is by two, opens a loop and decrements the value where the pointer is by one on every iteration. When it reaches zero, terminates the loop.

Another occurence of a finite loop would be:

```brainfuck
+[>]
```

It corresponds to `int x = 1; int y = 0; while (x) { x = y; }` in C. At run-time the stream looks like this:

```
[*1][0][0][0]...        First iteration opens loop.
[1][*0][0][0]...        Second iteration closes loop.
```

To explain, that program increments the value where the pointer is by one, opens a loop, moves the pointer right and since the value where the pointer is now zero, terminates the loop. This demonstrates that you do not need to close the loop at the value where it openned. However that makes the location of the pointer less deterministic.

### Tricks

There are many little tricks you can use to make Brainfuck easier.

To shift a value from one location to the right:

```brainfuck
++++[>+<-]     (shift 4 on the left one location to the right)
++++[>>+<<-]   (shift 4 on the left two locations to the right)
++++[>>>+<<<-] (shift 4 on the left three locations to the right)
```

To shift a value from one location to the left:

```brainfuck
>++++[<+>-]     (shift 4 on the right one location to the left)
>++++[<<+>>-]   (shift 4 on the right two locations to the left)
>++++[<<<+>>>-] (shift 4 on the right three locations to the left)
```

To multiply a value on one location with another on the right:

```brainfuck
++[>++++<-] (multiply 2 on the left by 4 on the right)
++++[>++<-] (multiply 4 on the left by 2 on the right)
```

To multiply a value on one location with another on the left:

```brainfuck
>++[<++++>-] (multiply 2 on the right by 4 on the left)
>++++[<++>-] (multiply 4 on the right by 2 on the left)
```

## Contributing

This repository is mostly read-only. I’m not a full-time developer just a hobbist, but if you like this lil’ implementation and want to improve upon things here, please don’t hesitate to **file an issue** or **open a pull-request**.

I would be glad if this could be useful for someone.

# License

It is distributed under the [MIT license][], a short and simple permissive license with conditions _only_ requiring **preservation of copyright and license notices**. Licensed works, modifications, and larger works may be distributed under different terms and without source code.

As a _humble_ request, please **mention my name and link back to this repository** if you ever use this in any of your own implementations.

[github]: https://github.com/Aerobird98/limen
[mit license]: https://github.com/Aerobird98/limen/blob/master/LICENSE.txt
