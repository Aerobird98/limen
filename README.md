# Limen

```
++++[>++++<-]>[<+>-]<[>++<-]>[<+>-]<.[+.]
```

Limen means _threshold_ in latin. Limen is Brainfuck. Brainfuck is a turing complete **esoteric** programming language created in _1992_ by Urban Müller. Its name is a reference to the _slang_ term _brainfuck_, which refers to things so **complicated** or **unusual** that they exceed the limits of one's understanding and it is certain that one would realy need to cross a cretain _threshold_ in order to use this language.

Brainfuck is notable for its **minimalism**. At the core of the language is a more-than-compact instruction set, comprising of **8** simple **instructions**, yet in theory with some creative thinking and the ability to break down complex tasks into microscopic steps you are more than capable of writing almost any program one can think of.

## Semantics

Brainfuck uses a machine model consisting of an _infinite_ stream of one-byte values initialized to zeros, an instruction pointer and a stream pointer. The instruction pointer moves trough all the **instructions** which can be used to operate the stream pointer and interact with the stream:

`+` – Increments the value at the stream pointer. The value _wraps around_ to 0 after reaching 127,

`-` – Decrements the value at the stream pointer. The value _wraps around_ to 127 after reaching 0,

`>` – Moves the stream pointer to the next value and _grows the stream_ when needed,

`<` – Moves the stream pointer to the previous value making sure it remains _inside the stream_,

`.` – Outputs the value at the stream pointer as an _ASCII character_,

`,` – Inputs a character and sets the value at the stream pointer to its _ASCII value_,

`[` – If the value at the stream pointer is 0, jumps to the matching `]`,

`]` – If the value at the stream pointer is not 0, jumps to the matching `[`.

In the end, a program is just a series of instructions. Characters besides the eight `+-<>[],.` instructions considered as comments.

## Implementation

This implementation is written ex nihilo and stands for a compact, secure and reliable interpreter for this remarkable language.

- **It’s compact.** – The implementation is rather compact, readable and logical with friendly comments all the way trough. You can skim the whole thing with ease.

- **It’s secure and reliable.** – Memory use is dynamic and strictly contained. The core uses zero static data and it does not leak memory with reliable and user-friendly error handling mechanisms in place.

- **It’s fast and efficient.** – The implementation splits evaluation into compile time and run time; it does all the validation and optimization at compile time while at run time only checks for memory- and stream management errors.

- **It’s made in the name of science!** – Comes with a **REPL** — an iteractive environment that vizualizes the stream and its pointer before every evaluation which makes it well suited for learning and experimentation which is the main reason while Brainfuck exists in the first place.

This implementation is **encoding agnostic**, user code is validated to only contain standard **ASCII** characters in the range of `0-127`. While I admit that it is rather strange, it does make the implementation more secure and reliable which was key troughout development.

## Embeding

This implementation made to be **embedable**, implemented as a small C _library_ consisting of a named `.c` and `.h` file; written in ANSI C with **NO** dependencies other than a few C standard library functions, _realloc_ and _free_.

You only need two files to include in your program, a named `.c` file and `.h` file. Copy those and you are good to go. :) Import the `.h` file to access the implementation. For a usage sample, consult with the `main.c` file which contains the command-line- and REPL implementations of a possible interpreter as well.

## Compile & Conquer

This _library_ itself and a possible _interpreter_ implementation lives on [Github](). To play around with it, sync it down then:

- Configure the `makefile` to suit your needs.
- Run `make` to build like _normal_.
- Run `make clean` to clean built files.
- Go back and copy the `<code>` at the head of this `README`.
- Run `limen <code>` to evaluate it.

If everithing goes well, you will see all the ASCII characters in the range of `32-127`.

```
 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
```

Without *argument*s, it drops you into a **REPL** — an interactive session. You can type in instructions and it will evaluate them immediately while vizualizes the stream and its pointer before every evaluation.

## Learning & Experimentation

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

## Contributing

This repository is mostly read-only. I’m not a full-time developer just a hobbist, but if you like this lil’ implementation and want to improve upon things here, please don’t hesitate to **file an issue** or **open a pull-request**.

I would be glad if this could be useful for someone and as a _humble_ request, please **mention my name and link back to this repository** if you ever use this in any of your own implementations.

# License

It is distributed under the [MIT license][], a short and simple permissive license with conditions _only_ requiring **preservation of copyright and license notices**. Licensed works, modifications, and larger works may be distributed under different terms and without source code.

[github]: https://github.com/Aerobird98/limen
[mit license]: https://github.com/Aerobird98/limen/blob/master/LICENSE.txt
