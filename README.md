# INR remaster

The remaster branch is the result of a rebuild of ginr from its origins.
Its commit history traces back through the redux branch to the original
2.0e version of March 25, 1988.
All of the bug fixes of the master branch have been included as well as
a more detailed change log.
The aggressive reformatting of the master branch has been avoided.

Going forward, the redux branch will be used for further development with
changes merged into the remaster and master branches.

Limited documentation in the form of a PDF file was produced in 2010 as part
of the release under GPL version 3:
[PDF](doc/inr_intro.pdf).
See also [USAGE.md](USAGE.md) extracted from the latex source using pandoc
and with some long-standing errors corrected.

A hopefully trustworthy commit is and will be identified as using a
separate branch 'restable'.
Although the HEAD of this 'remaster' branch contains only tested code, it
should be used with a caution until more testing has been done.

## Installation

INR is a C program that will compile and run comfortably in a LINUX
environment or one sufficiently similar to it.
The only dependencies are *gcc*, *make* and *yacc* (as provided by *bison*).

There is a make file in the src directory that will do the proper build and
produce an executable *ginr*.

## Usage

See [USAGE.md](USAGE.md).
