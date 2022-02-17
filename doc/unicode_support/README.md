# Unicode Support in INR

## Introduction

New functionality has been added to version 2.1.0c in Feb 2022 to support
the use of Unicode processing in UTF-8 format.
At the present time, the focus is on Unicode as expressed as sequences
of octets encoded as UTF-8 with no conversion to code point numbers or
to other transfer formats.
These may be provided at a later time.

UTF-8 is quite suitable for Linux and various similar operating system
environments.
Nonetheless, UTF-16 support should be possible in the future.
Most other encodings in use are reasonably close to one of these two
although GB 18030 might require more work.

Briefly, UTF-8 encoding represents Unicode code points as a series of one
to four octets.
It is a variable length coding that encodes all 7-bit ASCII as itself in
one octet (formerly called a byte) and larger code points appropriately
using multiple continuation octets as needed.
It satisfies a synchronization property that allow text matching for any
valid UTF-8 string query against a valid UTF-8 text by matching only octets.
There can be no misaligned matches.

UTF-16 also has this property as long as alignment on 16-bit short word
is preserved.

Thus, INR works with octet sequences.
For now, three strategies have been partially implemented:

1. There are only 256 separate octet values providing a feasible, though
large alphabet for INR processing.
Large alphabets may contribute to state explosion in multi-tape
transducers since information read can only be stored in the finite
control (i.e., the state) if different actions need to be distinguished.
Whether this is too large an alphabet will, of course, depend on the
application.

2. An obvious solution to this state explosion just mentioned is to split
each 8-bit octet into two 4-bit hex digits (nibbles).
Such an alphabet has 16 values.
However, to avoid breaking the desirable synchronization property it is
likely advantageous to distinguish high order nibbles from low order
nibbles.
The alphabet now has 32 values.

3. Yet another approach considers that for any application only a small number
of code points may actually be used.
Any input can be split into tokens, each of which is a valid UTF-8 octet
sequence corresponding to a single code point.
Although the alphabet size is potentially 1,114,112 (the number of valid
code points), the size is expected to be much smaller in many cases.

## Internal considerations

INR internally represents each token by an integer greater than or equal
to zero.
In previous versions of INR only values of 0 and 1 were reserved for the
empty word and endmarkers respectively although all of the printable
ASCII characters (plus tab and newline) were preloaded so that they would
have an integer ordering corresponding to their character ordering in
the ASCII character set.

Each of these characters has an external representation that is used for
printing.
These were internally the actual single character as C string although
on input and output `'\_'` was used for blank, `'\t'` was used for tab, 
`'\n'` was used for newline, and `'\\'` was used for backslash. 

In version 2.1.0c, this preloading of the token table is extended to all
octets.
Thus indexes 2 to 257 represent the 256 possible octet values ordered by
their character values.
All of the printable characters have string value as before and
unprintable characters are represented by a two digit hexadecimal number
as a C string of length 2.

More preloading is done to handle high and low order nibble values:
indexes 258 to 273 represent the 16 possible high order nibble values and
indexes 274 to 289 represent the 16 possible low order nibble values.
Their string representation of high nibbles are a hex digit preceded by
`[`.
Low nibbles are represented by a hex digit followed by `]`.

For approach number 3, no preloading is done.
The internal index is assigned as each character is encountered.
The external form is the UTF-8 sequence in its string form.
Note that combining forms are treated as separate tokens.

## Changes to Lex.c:

Assign distinct print strings in symbol table to each code point.

Modify `''` strings to admit all octets in the input (from only printable).
Modify `` ` ` `` tokens to admit all octets in the input (from only printable).
Create new `""` strings to map octets in the input to a pair of nibbles: high
and low.

Some examples should clarify all of the above discussion:

`'abc';`

    (START) a 2
    2 b 3
    3 c 4
    4 -| (FINAL)

as before

`"abc";`

    (START) [6 2
    2 1] 3
    3 [6 4
    4 2] 5
    5 [6 6
    6 3] 7
    7 -| (FINAL)

with new high / low nibbles

`` `abc`;``

    (START) abc 2
    2 -| (FINAL)

as before

``( `a` `b` `c` );``

    (START) a 2
    2 b 3
    3 c 4
    4 -| (FINAL)

as before

`"ἀπὸ";`

    (START) [E 2
    2 1] 3
    3 [B 4
    4 C] 5
    5 [8 6
    6 0] 7
    7 [C 8
    8 F] 9
    9 [8 10
    10 0] 11
    11 [E 12
    12 1] 13
    13 [B 14
    14 D] 15
    15 [B 16
    16 8] 17
    17 -| (FINAL)

a little longer

`'ἀπὸ';`

    (START) E1 2
    2 BC 3
    3 80 4
    4 CF 5
    5 80 6
    6 E1 7
    7 BD 8
    8 B8 9
    9 -| (FINAL)

octet form

`` `ἀπὸ`;``

    (START) ἀπὸ 2
    2 -| (FINAL)

unsurprising

``( `ἀ` `π` `ὸ` );``

    (START) ἀ 2
    2 π 3
    3 ὸ 4
    4 -| (FINAL)

broken apart

## Changes to Aload.c:

`:lwds` will admit all octets as does Lex.c.
Only approach 1 is currently supported.

## New code in Aunicode.c:

Some new routines in Aunicode.c have been added to INR to make it easier
to process text in UTF-8 format.

### `:slurp_octets`, `:spit_octets`

`INP = :slurp_octets c.md;`

This is a new function that allows the full content of a file to be read
in in one chunk and converted to a DFA that recognizes exactly one string,
that is, the string of octets.
In this example `c.md` is the name of the file and INP is an INR variable
that will contain the resulting DFA.

`OUT :spit_octets trans.md;`

This is new function that takes a DFA stored in the INR variable OUT and
writes the sequence of octets traced out by the path from the start state
to the final state.
It is the inverse of :slurp_octets.
In this example the output goes to the file `trans.md`.

Note that the DFA should match exactly one string.
At present, this function has undefined behaviour if more that one string
occurs.

### `:slurp_nibbles`, `:spit_nibbles`

Without repeating all the details, these two routines are used in a
similar fashion except that nibbles are read in or written out
respectively.

### `:slurp_utf8`, `:spit_utf8`

Again these two routines repeat the pattern, except that the input is
parsed into UTF-8 sequences and these are looked up or added to the
token table as they are seen.
The alphabet used is detected in the process of reading the file.

The function `:alph` can be used to extract this alphabet from this input
if it is needed.

### `:gen_min`

All of the above spit functions have undefined behaviour if their input
automaton represents more than one string.
An easy way of selecting a unique string to be written is using the new
`:gen_min` fuction.

`OUT = WRK :gen_min;` will conceptually do the following:

1. Calculate the length of the shortest string recognized by WRK.

2. Produce an automaton that recognizes all of the strings of this shortest
length.

3. Select the string that comes first in lexicographic order (alphabetic
order).
This is the output and is represented as a DFA recognizing exactly one
string.

For a lexicographic order to work, the alphabet must be ordered.
For this purpose, the integer value stored in the token symbol table is
used.
This will have the effect that octet and nibble-based automata will have
a natural binary ordering.
Because of a nice property of UTF-8 encoding, this will also correspond to
code point order (not true of UTF-16 by the way).

For the third approach the order will depend on when the code points were
first encountered, and the result might seem less predictable.

## Sample code

Samples contains a programming task using the nibble based approach.
See [md_eqn_nibbles](../../samples/md_eqn_nibbles)
