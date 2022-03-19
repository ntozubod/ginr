# A improved display format for INR

## Introduction

INR from the earliest days needed to be able to read in and to display
finite automata.
This output format was intended to be human readable, at least in the
cases where the number of transitions was relatively few.
Large automata almost always contain enough complexity that their structure
is usually not obvious at all.

On input this format was intended to be extremely forgiving, accepting as
state names anything that did not invalidate proper parsing.
In practice, this meant that new lines, tabs and blanks needed to be escaped
as '\n', '\t', and '\_', respectively.
Null bytes ('\x00') were not properly handled and lead to an erroneous
premature termination of any string in which they occurred.
This wasn't intentional but a side effect of how the C language usually
treats character strings.

Transition labels had the same rules as state names except that '.' was
treated as a separator between a tape number and the symbol part of the
label.
If the transition label began with a single digit followed by a '.', this
was treated as a tape number between 0 and 9.
It was stripped off and the remainder of the string was used as the label.

There were two special 'state names' that were used to identify the start
state (assumed to be unique) and all of the final states.

Two 'magic labels' were used to identify empty word transitions (lambda or
epsilon transitions), end-markers in the case of deterministic multi-tape
transducers, and as a part of the indication of which states are to be
considered as final.
The number of tapes was inferred from the usage pattern.
It was assumed to be one more than the largest tape number seen while
reading in the automaton.

The :pr format was intended to by quite readable and to present automata
as they were usually considered as a collection of transitions,
each made up of a 'from state', a 'transition label', and a 'to state'.
The emphasis was on readability and this is the format displayed when INR
is used interactively.

## Enhancements to classical functionality

This format has been, for the most part, quite successful so it will be
mostly continued as it was.

However, the limit on only ten possible tape numbers will be lifted
by allowing two or more digits to precede a '.' that precede the
transition label.
This could break old programs that expect `12.345`, for example to be
a token.
If this is desired an extra back-slash will be needed ( `12\.345` ) to
escape the '.'.

The other big change is that all values of octets except the three delimiters
space, tab, and new line in any of the from state name, the transition label,
or the to state name.
If they are needed, they can use the INR escapes '\_', '\t', and '\n' although
it also possible to use actual spaces, tabs, and newlines as long as they
are preceded by a back-slash.
In fact, any octet preceded by a back-slash will be passed though and not
have its conventional meaning.

Nonetheless, it is always possible to indicate an octet with the escape
'\xHH', where the two H's represent hex digits.

Note, that the format is extremely forgiving on input so that it is easy
to generate from other language environments.
For example, back-slash escaping every octet should work.

On output, the format is much more constrained and designed to be maximally
readable.
There are some design questions that remain since some valid UTF-8 sequences
should be passed though whereas others will be converted to '\xHH' escapes.
Details need to be worked out.
