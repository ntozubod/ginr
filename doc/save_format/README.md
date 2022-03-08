# A new save/transfer format for INR

## Introduction

INR from early on had two different file formats for automata:

1. The :pr format was intended to by quite readable and to present automata
as they were usually considered as a collection of transitions,
each made up of a 'from state', a 'transition label', and a 'to state'.
The emphasis was on readability and this is the format displayed when INR
is used interactively.

2. The :save format was intended to provide a binary format that
required minimal space and was suitable as a archive and transfer format
to other applications written in C.
A number of shortcuts were made that resulted in this format not being
portable across versions or platforms.
The priority was on small file size on a shared computer with only 1GB of
disk space (at the time a huge amount).

At the present time, the small size requirement is mostly gone because
of very efficient compression strategies and very large external storage
devices.
However, interoperability is now extremely important and this can even
include portability between platforms (different computer languages or
operating systems).

Thus the :save format is now being moved forward to fill this role.
It is still a binary format and is subject to corruption as a result of
automatic conversions.
For example, it currently requires the use of the newline character as
a line terminator and this requirement cannot be relaxed without a change
to the specification.

The :pr format will be more resilient to these types of conversions and
other formats may be introduced.

## Specification

#### Two line types

The :save format has two types of lines of variable length ending with the
newline character ('\x0A'):

1. The first line is a header line that contains five fields:
a 'magic number', the number of tapes in the automaton, as well as the
number of transitions, the number of states, and the size of the alphabet.
This is to facilitate the pre-allocation of arrays in the receiving
environment.

2. Each remaining line contains information about one transition including
the 'from state', the 'to state', the 'tape number', and the
'transition label'.

Each line contains a number of fields separated by the tab character
('\x09').

No extra tabs, blanks, or new lines are allowed with one exception to be
explained.

#### Three field types

There are three types of fields:

1. The most common type of field is a numeric value written in base 10
using the octets '\x30' to '\x39' to represent the digits 0 to 9.
The value of this field is the value of the integer so indicated.
For the 'tape number' field a preceding minus sign ('x2d') is allowed.

2. A *magic number* of `INR210` is used in the first field of the first
line to signal that the type of the file is and INR save file version 2.1.0.

3. A raw text field made up of a sequence of any of the 256 possible octet
values is the third type of field.
Since this field can contain any of `blank`, `tab`, `newline`, or
`null byte` and can contain any valid or invalid UTF-8 text, the length of
the field is signaled by a numeric preceding field.
This is the noted exeption to extra tabs, blanks, or new lines.
Anything can occur in this field.

#### The format

##### Header Line

Field one contains the 'magic number' `INR210`.

Field two has the number of tapes.

Field three has the number of rows.

Field four has the number of states.
This is actually one greater than the largest state number used but in INR
this will usually be the same thing.

Field five has the size of the alphabet.
This is actually one greater than the high water mark of the label number 
used when the pre-allocated token symbol table is used.
It contains the 'magic values' 0 and 1, followed by 256 reserved values to
represent the possible octets.

##### Transition Line

Field one contains the 'from state' as an integer value.
The value `0` is special and indicates the start state as usual in INR.
The value `1` is also special and cannot occur as a `from state`.
It indicates a final state and can only be used in the 'to state' field.
All other integer values are possible although INR tries to keep these
values as small as it can.

Field two contains the 'to state' as an integer value.
Any positive integer value is allowed although `0` and `1` have their
special meanings.

Field three contains the 'tape number' that the transition applies to.
A 'magic' value of `-1` is used for lambda (or epsilon) transitions.

Field four is a length field that is used to warn of how many octets the
next 'raw binary' field contains.
It can be any non-negative integer value.
The value of `0` is special and indicates that there are no octets in the
following field.
It is used for endmarker transitions and the lambda (epsilon) transitions.

Field five contains a sequence of octets of any value and of exactly the
length specified in field four.

## A tiny example

`'abc';`

    (START) a 2
    2 b 3
    3 c 4
    4 -| (FINAL)

`'abc' :dfamin :save ttt;`

    INR210	1	4	5	102
    0	2	0	1	a
    2	3	0	1	b
    3	4	0	1	c
    4	1	0	0	

Note here that `:dfamin` is needed here to ensure that minimization is done.
Just as `:pr` doesn't force minimization, `:save` also will not.
The old version of INR did force minimization.
This is a change.

Note also that the high water mark for the alphabet occurs with the label `c`
('\x63` == 99).
The value of 102 is 99 plus 3.

`'abc' :update;`

    (START) a 2
    2 ^^ 3
    4 ^^ 5
    3 b 4
    6 -| (FINAL)
    5 c 6

`'abc' :update :save uuu;`

    INR210	1	6	7	102
    0	2	0	1	a
    2	3	-1	0	
    4	5	-1	0	
    3	4	0	1	b
    6	1	0	0	
    5	6	0	1	c

The precise files [`ttt`](ttt) and [`uuu`](uuu) are included in this
directory for reference.

## Reference Implementations

Since this format needs to be read by code written in different languages,
it is useful to see examples that are closer in spirit to the target.
By far the easiest to understand are implementations that simulate a finite
state automaton that processes input one character at a time.
There are relatively few corner cases to deal with.

An example of this is the implementation used by INR itself:
[`src/Asave.c`](../../src/Asave.c)

Since I want to use Perl for testing there will be an idiomatic Perl
version based on line and field parsing with the requisite corner cases.

Am INR transduction will also be provided that handles the format with a
bound limitation imposed on the binary field length.
Having this unbounded leads to a constraint that cannot be checked by a
finite state machine.
