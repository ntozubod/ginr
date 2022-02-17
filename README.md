# INR

A consolidation of the INR code base has been largely completed and new
work is beginning.

Three main ideas are being introduced: (1) larger maximums for the number
of states and the alphabet size, (2) some support for Unicode, and
(3) some better handling of I/O for non-interactive use.

These changes facilitate a number of potentially important use cases
associated with low level Unicode text processing, as well as helping with
the development of better test material and documentation.

## Installation

INR is a C program that will compile and run comfortably in a LINUX
environment or one sufficiently similar to it.
The only dependencies are *gcc*, *make* and *yacc* (as provided by *bison*).

There is a *make* file in the src directory that will do the proper build and
produce an executable *ginr*.

## Usage

Limited documentation in the form of a PDF file was produced in 2010 as part
of the release under GPL version 3:
[PDF](doc/intro_1988/inr_intro.pdf).
See also [USAGE.md](USAGE.md) as derived from this.

For information on new support for processing Unicode text files
see: [Unicode Support](doc/unicode_support/README.md)
