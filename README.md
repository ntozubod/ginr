# INR main2022

Beginning of *main2022* branch and INR version 2.1.0x.

This branch will begin with the exploration of the conversion of SHORT to
32-bit and an introduction of Unicode support.

## Branch reorganization

The branch history of INR has been a little confusing.
The several attempts to sort this out have all been incomplete and
added to the confusion.
In order to move toward a simpler structure with one *main* branch and
one *stable* branch, the previous candidate branches need to be named in
a consistent manner.

The *remaster* branch was changed to *main2021*.
After the reorganization is complete, *remaster* will disappear.

The *stable* branch will be merged forward to agree with *master* and then,
after merging, track the *main2021* branch.
It will represent a 'stable' commit in the the *main2021* branch with
*main2021* sometimes leading to allow for bug fixes.
The *main2021* branch will correspond to INR version 2.0.x.

For completeness, *main2018*, *main2010*, and *main1988* branches have also
been created to facilitate access to previous notions of what a 'main'
branch should be.
The branches *master* and *remaster* will eventually disappear.

Its commit history of *main2022* and *main2021* traces back through the
*redux* branch to the *main2010* branch, re-applying all of the changes
made by the *main2018* branch without the agressive reformatting.

Once all of this is in place, a new *main* branch will be introduced based on
the *main2022* branch.

## Installation

INR is a C program that will compile and run comfortably in a LINUX
environment or one sufficiently similar to it.
The only dependencies are *gcc*, *make* and *yacc* (as provided by *bison*).

There is a make file in the src directory that will do the proper build and
produce an executable *ginr*.

## Usage

Limited documentation in the form of a PDF file was produced in 2010 as part
of the release under GPL version 3:
[PDF](doc/inr_intro.pdf).
See also [USAGE.md](USAGE.md) extracted from the latex source using pandoc
and with some long-standing errors corrected.
