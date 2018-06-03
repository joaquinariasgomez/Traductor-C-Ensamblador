# C subset to assembly translator.
This translator is made using Bison/Yacc and Flex/Lex. We designed an ascendent grammar for the syntactic analysis.

**Developers:**
- José Joaquín Arias Gómez-Calcerrada [@empiremay](https://github.com/empiremay)
- Aarón Arias Pérez [@aaronariasperez](https://github.com/aaronariasperez)

**Dependencies:**
- flex
- bison
- g++ c++11 standard

**Compilation and execution:**
There is a script to compile the project in Linux. 
You can also compile it manually:
- flex lexico.l
- bison -d parser.y
- g++ TablaVariables.cpp node.cpp parser.tab.c lex.yy.c -std=c++11 -lfl -lm -o main

There are two input examples inside the project (input_example1.c and input_example2.c) so you can try it executing:
- ./script # to compile (or you can do it manually)
- ./main < input_example1.c # also try with input_example2.c
