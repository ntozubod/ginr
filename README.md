# INR redux

This is rebuild of ginr from its origins in the 1980s at the University
of Waterloo.
The goal is to build solid foundation for future work.

Limited documentation in the form of a PDF file was produced in 2010 as part
of the release under GPL version 3:
[PDF](doc/inr_intro.pdf)
This has been converted to markdown using pandoc and a touch of Perl:
[MD](doc/inr_intro.md)

## Installation

INR is a C program that will compile and run comfortably in a LINUX environment
or one sufficiently similar to it.
The only dependencies are *gcc*, *make* and *yacc* (as provided by *bison*).

There is a make file in the src directory that will do the proper build and
produce an executable *ginr*.

## Usage

See [USAGE.md](USAGE.md).
