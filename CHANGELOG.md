# INR Changelog

## 2.1.0g (2023-05-05)

Patch to address some backslashes in tokens causing an assertion failure
in T2.c.
The problem is caused by a failure in maintaining distinct print string forms
for the new P-string string format that accomodate unprintable characters
and malformed unicode within tokens.
These need to be properly escaped in the print string form.

## 2.1.0f (2022-11-23)

Patch to replace usage of bcopy by copymem.
This can be deselected with the #ifdef define USE_BCOPY either in the
source or as a compilation option.

## 2.1.0e (2022-11-20)

Patch to force fopen to use "rb" and "wb" when opening files.
This can be deselected with the #ifdef define NO_FOPEN_BINARY either in the
source or as a compilation option.

## 2.1.0d (2022-04-12)

This is a stable point.
Basic Unicode / UTF-8 support is consolidated.
The main branch will be fast forward commited to this point.

#### src

The bug with the temporary patch in 2.1.0c1 (2022-02-27) has now been fixed.
Changes to O.h, Lex.c, and Parse.y to allocate and re-allocate as necessary
the Alist array.

## 2.1.0c8 (2022-04-11)

This commit attempts to close off changes to provide basic support for
Unicode using the UTF-8 transfer format.
The main changes involve allowing the full repertoire of 256 octets to be
read in, written out and used to form tokens in automata.

The octet sequences that consitute valid UTF-8 encodings are treated as
special only to the extent that they are displayed unescaped when using :pr,
:enum or :prsseq to show automata.
Some changes had to be made internally to allow the full octet range
everywhere and now the only 'magic tokens' are `^^` and `-|`.
In particular, null character and control characters are allowed in strings
as well as all of the possible octets that can occur in UTF-8.

#### src

##### Acompose.c

The change made to compose in the last commit is necessary and proven to
be worth while.
However, I am considering a more complete overhaul of Acompose.c that will
subsume these changes.
As well, for a core component like this, thorough testing is required.

The changes to Atrim.c are mainly syntactic and, by now, well exercised.
There is also a conditional compile define USE_RECURSIVE_TRIM that can be
selected at compile time to revert the behaviour to the older version.

##### Aunicode.c

All mentions of nibbles are removed.
This functionality will be re-introduced later as part of a new
abstraction (B).

Add functions to check for valid UTF-8 and identify printable Unicode
code points.
The first of these is moved from T2.c and a bug fixed.
The second is A_unicode_printable takes an integer argument signaling the
unicode codepoint concerned.
It returns 1 if the string is printable and 0 if not so that :pr (and friends)
should escape it.

The algorithm used is extemely basic.
A code point is considered unprintable if:

(1) the code point is one of the 66 non-characters.

(2) the code point is in the character class C as identified in the unicode.org
file UnicodeData.txt.
These are the code points identified as *control characters*.
Among them are the code points between U+0000 to U+001F and between U+007F and
U+008F.
There are others.

(2) the code point is in the character class Z as identified in the unicode.org
file UnicodeData.txt.
These are the code points identified as *spacing characters* that are
displayed as white space.
Examples include U+0020 and U+00A0 (non-breaking space).
There are others.

The version of UnicodeData.txt is that of early 2022 (version 14).

Note that unassigned code-points are considered printable (for now).
This preserved a bit of compatibility if this file is not updated.

##### T2.c

Use the A_unicode_printable routine to determine whether the external
representation of a unicode will use raw or escaped octets (for readability).
The semantics of the two formats is identical. However, this approach makes
the :pr format a little more useful in some cases.

## 2.1.0c7 (2022-03-19)

This work was actually done in February and has been waiting for the
:save and :pr updates to be made.
It seems to be working fine although as with all of the new code written in
the last couple of months, lots of testing remains to be done.

#### src

##### Atrim.c

Code re-factoring of Atrim.c to replace a deep recursion by iteration.
This version uses goto statments in as structured a way as possible to
mirror the execution path of the recursive form.

##### Acompose.c

Improved version of Acompose.c to have a better time/space tradeoff.
Many lambda transitions can be created as a result of matches being processed.
Formerly these were removed at the end of processing using the routine
A_lamcm.
These take up a lot of space and can result in an exhaustion of memory
before completion.
Since they are easy to detect before being generated, they can be removed
pre-emptively in many cases.

Since the naive solution results in the space explosion then causes a lot
of repeated computation, a memoization capability is added to cache the
results.
Since the lambda transitions occur in chains, memoizing only some of the
values yields almost all of the time savings without all of the space cost.
The current memoization is tuned to remember every alternate lambda transition
in each chain.

## 2.1.0c6 (2022-03-18)

#### src

##### T2.c

The main change in this commit is to the handling of interal vs external
forms for all tokens (or alphabet symbols).
The internal form is a sequence of one or more octets having any of the
256 possible values.
This was done originally for the purpose of handling the :save format where
all such octet sequences are acceptable (and needed for some applications).

NB: There are currently two exceptions to this behaviour that have to do with
the symbols `` `^^` `` and `` `-|` `` which currently have their legacy
meanings of empty work and end-marker.
This is now considered a bug and will be removed at some future date although
it is not presently clear what the best approach would be.

The external form is a sequence of octets that is intended to be printable
(i.e., can be displayed as for :pr).
This format is peppered with back-slash escapes such as `` `\x00` `` as
well as the older `` `\n` `` and `` `\_` `` escapes from the INR in the
past.
The definition of this format is flux but currently treats any
sub-sequence of octets that is a valid UTF-8 encoding (defined below)
as printable and does no escaping.
Any octets that are not a part of a valid UTF-8 sequence are escaped as
described above.

In order to centralize and manage the two forms, a new data type is
introduced named `T2`.
This is a double symbol table that is formed from two instances of `Tn`
and are automatically synchronized as needed.

##### :pr format

The :pr format is as before except that the new external forms are used
for the transition labels.

Because this is centralized, this change is propagated to :enum and :prsseq
as well as a few more obscure places in the code.

##### More than 10 tapes

There has been a restriction in INR to allowing only tapes numbered from
`0` to `9`.
This restriction has only ever been for input and output.
It has now been removed.
All places where tape numbers can be allowed can have any non-negative
tape number.

##### Handling of `.` in tokens

Because `.` is used as part of the syntax of indicating tape number, there
needs to be a way to say that the `.` is just a period sometimes.
This is done using a new back-slash escape so that `` `0\.0` `` is now
the token with three octets `0.0` as its internal representation and
the escaped form as its external representation.

There was a bug that the lexical scanner and parser did not co-operate
properly to preserve the needed distinction.
This has been repaired with the introducion of a new data type `Q.c` and
the introduction of a new non-terminal to the parser.

##### More accurate parsing of UTF-8

In a places in INR it is useful to recognize sub-sequences of octets that
are are valid UTF-8.
There are different levels of strictness that can be applied but the following
seems to be appropriate:

Any code point between `0` and `10FFFF` is allowed (the full range of code
point values) except for the range `D800` to `DFFF` that is invalid because
of the definition of UTF-16.

Only the shortest sequence of octets is allowed for UTF-8 (as specified)
so that, for example, `C080` is not a synonym of `00` but is treated as
ill-formed.

The reasons for these choices is to facilitate a style of programming that
is safe (in a cyber-security sense).

If loose UTF-8 is needed later, it might be considered separately.

##### Use of UTF-8 parsing

This code needs to be centralized but is currently separately implemented in
two places:

(1) In :slurp_utf8 for reading / checking UTF-8 input.

(2) In T2 where the external form is computed from the internal form.

##### INR processing using nibbles

Although breaking of octets into smaller pieces is expected to be useful for
a number of applications, the features added to support nibbles has been
removed.

The only remnant are the undocumented routines :slurp_nibbles and
:spit_nibbles.
These will remain for now although changed and deprecated as an alternative
approach is being planned.

The use of `"` in the expression language is no longer valid and its use
has returned to indicating an error.

#### doc

##### pr_format

Documentation of the :pr format.

##### unicode_support

This is updated to reflect the above changes.

corrections of unicode docs

documentation of pr format

#### samples

The example provided has been converted from using nibbles to using octets.
The changes were easy because of the former design of nibble support.
The former example no longer works as it was because `"` is no longer
handled in the lexical analyser.

## 2.1.0c5 (2022-03-08)

#### src

The :save format is updated to have two more fields in the header
to contain the number of states and a bound on the size of the alphabet.

The code for loading from the save file is made more defensive against
corruption by checking for more types of errors.

#### doc

The documentation is updated to reflect these changes.

## 2.1.0c4 (2022-03-06)

#### src

An updated version of A_save and A_load_save is provided based on a
productive discussion with Valkyrie at the end of February.
This is a mixed text / binary format that is particularly easy to parse and
accurately represents the content of an INR automaton.

A_save is now called when :save is invoked and :read / :load will accept it
as a new format it will read.

#### doc

Documentation of the new :save format is provided.
See: [New Save Format](doc/save_format/README.md)

## 2.1.0c3 (2022-03-04)

#### src

The implementation of P-string usage is continued.
All of the communication between Lex.c and Parse.y has been modified to use
P-strings.

NB: The usage anything that prints out the content of a string will display
unprintable characters for unprintable input.
This is a feature or a bug depending on how you look at it.

NULL bytes are now properly detected in Lex.c and passed to Parse.y.
Further down the processing pipeline, they might not be treated correctly.

## 2.1.0c2 (2022-03-04)

#### src

These modifications follow from a very profitable discussion with Valkyrie
about the use of INR for jrte.
He uses INR transducers to process text of various levels of quality and type
and needs to allow for the possibility that null bytes can occur.
This is actually quite common with various binary formats.
This almost works in INR but there is too much usage of C null-terminated
strings.
Although this is biggish change, it is a good one to make as null-terminated
strings can have a lot of security issues.
As well, the storage usage of INR can be improved by putting all strings in
a dynamically allocated pool.
This provides at least three reasons to make these changes already, and the
structure of INR makes this easier than I thought it would be.

NB: This is only part of the implementation.
Changes have to be made in the rest of INR to work exclusively with the new
string type.

Two new source files named Tn.c and P.c have been added to begin support
for strings that may contain null bytes.
This is done by creating a new object type P_OBJECT that contains storage
with a sequence of octets, and a length field.
The name P has been used refer to the concept of a Pascal-string type.
In this case the length field is a 32-bit int and the format is different.
However, the motivation is exactly the same.

Tn.c is a replacement for T.c that avoids all use of C 'str' operations.
The 'n' is added to the name to indicate that by avoiding null-termination
and always providing string lengths as an explicit argument, some of the
problems with unterminated strings can be avoided.
It is also possible to store null bytes within strings.
For extra safety, all strings in the pool have a redundant null octet at the
end.
They can successfully be used as C strings with the caveat that they will be
truncated if there is an embedded null.

NB: A new kind of unsafe usage occurs with Tn.c compared to T.c.
With the older code, a string returned from T_name would never move.
Tn.c includes an optimization of storing all strings into one large
string pool.
This may be reorganized if a Tn_insert is done on a full table.
If you need persistent access to a string use the index or the returned
value form Tn_Pstr.

In order to test these changes, T.c is removed from the compile and a number
of macros are added to force all usage of T.c to invoke Tn.c.
(See O.h for details.)

## 2.1.0c1 (2022-02-27)

The automata table Alist was never made dynamic but allocated as an array of
size 100.
This is not adequate and is hereby increased to 1000.
In future, it will be made dynamic.
(Bug found by Valkyrie: 2022-02-27)

## 2.1.0c (2022-02-17)

#### src

Start work on Unicode.

A new src file Aunicode.c contains some needed routines.
A function called A_slurp_octets is added to allow a complete file to
be read in, interpreted as a sequence of octets and made into an
automaton that recognizes this sequence as a single word.

An error led to finding that Ssize in S.c needed to be protected from
a zero argument.
An appropriate assertion was added.

A routine called A_slurp_utf8 is added that captures complete sequences
of octets that encode a unicode code point.
A routine called A_spit_utf is added that outputs to a file a string as
would be capturered by A_slurp_utf8.

Lex.c is modified to allow an alternate quoted string format: Single quotes
yields a sequence of octets; double quotes yields a sequence of nibbles.
A nibble is half an octet or 4 bits and provides a more manageable
alphabet size.

Matching routines A_spit_octets and A_spit_nibbles have been added to allow
processed text to be written out.
A routine A_gen_min for extracting the single genealogical minimum (length then
alphabetical) word from the automaton has also been added.
The processing flow can now be:
A_slurp_XXX followed by some transformation followed by A_gen_min to choose
a single output followed by A_spit_XXX.
XXX can be any of `utf8`, `octets`, or `nibbles`, as preferred.

#### doc

The doc directory is reorganized to accomodate more entries in a more
navigable way.

The new features implemented above have been added to the documentation and
pointed to from README.md.

#### samples

This new directory is introduced to contain full use cases with
documentation.

A use case that addresses identifying all equations in a markdown file
that has dollar signs also occuring in code blocks.
This is more challenging than one might expect, but can be done in a
rather direct way using INR with the above new features.

#### test

This will be a home for small tests that can be run by calling a script.
For now it contains the old `egs` directory.
Some of these will be turned into tests or, more likely use cases for the
samples directory.

## 2.1.0b (2022-01-29)

Starting work on a dev branch.

#### src

##### Define SHORT as int

Changing the definition of SHORT to *unsigned int* causes INR to fail in
a number of parts of the code where SHORTs and signed ints are used together.
Although this is a result of sloppiness in code written 40 years ago, most
of the grief can be avoided by making SHORT be a signed int and setting
MAXSHORT to its maximum positive value (017777777777 = 2^31-1).

This approach appears to be working but a lot of testing is required before
this can be considered stable.
INR has always had SHORT set to *unsigned short* with MAXSHORT as its
largest possible value (177777 = 2^16-1), that is, except for a failed
experiment in 2011.

##### Support of Unicode UTF-8

By treating input as octets as opposed to encoded characters, the size of
alphabet becomes a managable small number (256).

The symbol table can be pre-loaded with this whole alphabet in a predefined
order and with a known pre-determined index.
Thus for each octet a *name* will be provided that is indexed by the octet
value.
The name associated with octet having value k will be stored in position
k + 2.

All of the octets corresponding to printable ASCII preserve their
character values from previous versions of INR.
The remainder have a two digit (two octet) hex number to identify their value.

This simple change seems to be quite workable and also largely preserves
backward compatibility.
More functionality is needed to fully support UFT-8 adequately but this is
a start.

##### Echo comments in batch mode

To make the examples more understandable, the output from INR should include
commentary.
An easy way of doing this is to echo comment text when INR is invoked
non-interactively.
A one line modification has been added to Lex.c to achieve this.

#### egs/utf8

Some experiments have been started in the use of UTF-8 in INR.
It appears reasonable to use a subsequential transduction for conversion
between a Unicode code point number and UTF-8 encoding.
There are some caveats.

If this is done with an octet-based alphabet, a state explosion ensues.
However, if base 16 is used the situation is much better.
Note that base 4 or even base 2 are better in this particular example.

Perhaps this might show a way to use transduction in other applications
since state explosion is an ever-present problem.

## 2.1.0a (2022-01-24)

Beginning of INR version 2.1.x and main2022 branch.
To start the src directory is identical to version 2.0.3h with only a
change to the version number.

## 2.0.3h (2022-01-17 updated Jan 24, 2022)

Branch structure of repo changed to consolitate history and prepare for
new work.

The name of the default branch is changed from *remaster* to *main2021*.

## 2.0.3h redux (2022-01-17 updated Jan 20, 2022)

#### doc

The latex input is relocated to a subdirectory of the doc directory.

A script that converts this same latex input to markdown is set up in a
directory named 'md'.
The package pandoc is used together with some special handling done in Perl.
The markdown has been compared against the PDF text to verify an
accurate conversion of difficult cases.
The one case of two dollar signs inside a verbatim block is corrected as a
special case.

## 2.0.3h redux (2022-01-17)

#### src (from version 2.0.4a August 15, 2018 updated December 14, 2021)

In early December 2021, I was attempting a cleanup of INR souce and managed
a number of improvements before starting the redux branch.
These will be added in to the main 'redux' and 'remaster' branches:

1. Re-ordered the declarations of O.h to a more sensible one.

2. Rationalized the #include calls across the source tree.

3. Added an alternative implementation of S.c that directly calls malloc
instead of using the binary buddy system of the original INR.
This is selectable by the compile time ifdef USE_MALLOC_ALLOCATOR.

4. Compiled with the -ansi option to force additional warnings.

5. There was a change in behaviour of :prsseq as a result of removing the
CMS conditional compile code from Aload.c.
A tab character was formerly printed for each transition before the target
state.
This has been restored to avoid breaking the use of diff on :prsseq output.

6. There is a change in the warnings for the isascii declaration if using
the -ansi compile option.
Just remove the use of this macro/function.

## 2.0.3g redux (2022-01-16)

The tryaug branch was an attempt in August 2018 to extend and improve INR.
It did not go very far but there may be some useful mods to merge into redux.

#### src (from version 2.0.4a August 15, 2018 updated Sept 6, 2018)

Most of the mods found are of questionable utility and have not been carried
over.
However, there was some work done on S.c that might prove useful.
These changes have been incorporated.

## 2.0.3f redux (2022-01-14)

The redux branch is now tracing the dev branch to pick up any useful
changes that occur there.

#### src (from version 2.0.3b September 21, 2018)

Acrea.c: Converted function declarations.

Aopen.c: Converted function declatations and added use of assert() macro
calls as an alternative to test and call to Error().

Aload.c: Removed last instance of conditional compile for CMS.

Lex.c: Postponed updating Copyright years.

All .c files: Removed any use of the register declaration.

All .c files: Converted any function declarations to C89 form.

## Branch started from redux (2022-01-13)

The mainenance directories (compare and misc) are deleted as this branch
begins a separate existence.

## 2.0.3e redux (2022-01-13)

At this point, the redux branch agrees with the master except in formatting
of source text and a few extra fixes on the redux side.
These fixes are all either updates to function declarations for C89 or
mods to address warnings from gcc.
They are all tiny and obvious.

The redux branch and the master branch will now be merged after the
documentation and README files are brought up to date.

#### src (from version 2.0.3a December 3, 2021)

On December 3, a bug was detected in U.c when compiled at a high level of
opetimization.
Using gdb it was localized to a line where arithmetic was done in a
different order then expected involving signed and unsigned temporary
variables.
As I understand it, this is undefined behaviour and therefore a programmer
error.
The repair is easily repaired by forcing the temporary to be unsigned.

Since the code in R.c does the same thing, it was repaired at the same time.

## 2.0.3d redux (2022-01-12)

#### src (from version 2.0.3a September 11, 2018)

This brings the redux branch into line with the current stable branch.
Differences in compare result from improvements in the redux branch.

Parse.y: The generated routine yyparse returns an int and that value should
be 0 if the input is valid.
The explicit return in the case of a :done, :quit, :stop, or :bye should
be 0 instead of 1.

Lex.c: The return code from yyparse should be checked.

Acrea.c: Changed a fprintf format from %d to %ld because of 64-bit
architecture.

Makefile: Changed options for compile for gdb.

S.c: Accomodated larger allocations by changing some ints to longs.
This should be checked more carefully in a S.c cleanup pass.

S.c: Made the minimum block size a power of 2 (32 instead of 24) in case
this allocator is used in a context where alignment on power of 2 boundaries
matters.
It will also cause some multiplies and divides in S.c to be optimized to
shifts.

O.h: The Sept 11, 2018 version has the same declarations but is better
organized.
It was just copied over to the redux branch.

## 2.0.3c redux (2022-01-11)

#### src (version 2.0.3 August 15, 2010 done August 15, 2018)

Made many changes to src files to suppress warnings.
Reviewed all of these changes and added a few more.

## 2.0.3b redux (2022-01-10)

Here we are using 'b' suffix since 2.0.3a is already used in the main branch.
I hadn't noticed that the previous commit reused the version 2.0.2a.
Fortunately, this should not be too confusing, as neither of the 2.0.2
versions are important.

#### src (version 2.0.3 August 15, 2010)

The original version of this commit made a number of minor changes that were
all of the good as well as a major change that was not.

The problematic change was an increase in the size of SHORT from 16 bits to
32 bits.
Although this is a desirable change, it uncovers many bugs and should be
explored in another branch devoted to rooting them out.
Note that 2.0.3b is essentially identical to 2.0.2a.

## 2.0.2a redux (2022-01-09)

#### Brief comments on subsequential transduction in INR

Towards the end of the time I was at the University of Waterloo, I was
exploring the implementation and use of Subsequential Transducers.

A *subsequential transducer* is a deterministic finite automaton (DFA)
with an output capability added in a rather obvious way.
Every transition and every final state is augmented with an string of
output symbols.
This underlying DFA recognizes a regular language as before and in the process
of accepting an input string traces out a path deterministically in the DFA
from the start state to a final state.
The corresponding output is formed by concatenating together all of the
outputs encountered on the path.
The output associated with the final state is then added to the end.
In other words, a subsequential tranducer is a DFA with print statements
added to transitions and final states.

This concept is very old and occurs in some of the original papers about
finite automata; however, research interest quickly shifted to
non-deterministic models and did not have the output-before-exit modification.
Such an automaton was called a generalized sequential machine (GSM).
Thus, an alternate name for a subsequential transducer is 'Deterministic
Generalized Sequential Machine with Endmarkers'.

All of the 'sseq' routines included in INR produce a subsequential transducer
given some form of a 2-tape transducer as input or give up if it can't be
done (or there is a bug in conceptualiztion or implementation).

The simplest and most reliable is A_sseq that simply produces such a
transducer from an input transducer that only requires a change in form.
The theory here is well developed (for example, see Christian Choffrut's
2003 paper in TCS 292).

The other 'sseq' routines are more Procrustean in nature, taking a transducer
that does not represent a subsequential function and finding a subsequential
transducer that represents some aspect of the input's behaviour.

GMsseq was one of these more general algorithms and was
the topic of a paper: "Uniformizing Rational Relations
for Natural Language Applications using Weighted Determinization"
that I presented at CIAA 2010.
It the process of preparing this paper, I found a bug in INR's algorithm and
repaired it.
I also discovered a theoretical issue that I couldn't resolve at the time.
There is a particular need for documentation and testing for all of the
sseq routines.

#### src (version 2.0.2 August 3, 2010)

Updated AGMsseq.c to fix a bug found while preparing paper for CIAA 2010.

Added a new routine 'mkdense' whose function is to reassign state
numbers to remove gaps.
( Arrays indexed by the state number work better. )

#### doc (version 2.0.2 August 3, 2010)

Added a more complete bibtex file.

#### egs (version 2.0.2 August 3, 2010)

Added a new example to show GMsseq functionality for the CIAA paper.

## 2.0.1a redux (2022-01-08)

#### src (version 2.0.1 July 29, 2010)

Unchanged.
Only update the date in stamp.c.

#### doc (version 2.0.1 July 29, 2010)

Unchanged.
The changes made in this commit of the ginr branch have been made already
in the redux branch.
A file containing some bibtex references was missed and is now restored.
An preliminary version of the documentation in markdown format is provided
at [doc/inr_intro.md](doc/inr_intro.md).

## 2.0.1a redux (2022-01-07)

#### src (version 2.0.1 July 26, 2010)

Most of the modifications made on July 26, 2010 to add copyright text for
asserting the GPL version 3 license have been made earlier in the redux
branch.
This commit in the redux branch completes the changes made as of
the '3f2120e' commit of ginr.

These changes reflect GPL text that is compiled into the INR executable and
is accessible from the INR interface.
This extent of license notification may seem excessive in 2022; however,
it agrees with what is recommended on the gnu.org website.

Note that from this point the redux branch will follow the ginr branch closely
except for any manual and automatic formatting of program text.
INR version numbers will correspond with an added alpha suffix.

## 2.0.0g redux (2022-01-06)

#### 2.0.0g src (2022-01-06)

The code now compiles but immediately crashes because it tries to use a
storage manager that was designed to run on a VAX 11/780 with 32-bit
addressing and much less RAM running Berkley UNIX.
At that time, the system version of malloc/free was not very good at avoiding
fragmentation.
INR's algorithms need to grow data structures dynamically and so this was
a problem that led to a replacment implementation based on the binary buddy
system.

The original code takes an expansive approach of providing complete
malloc/free support by over-riding the system version.
It assumes that it can create an arena for allocation at the end of the
data segment and uses the system call sbrk to grow the data segment and
arena as needed.
If any other part of the process calls sbrk, this will not work.
This approach is clearly not portable to other environments.

The version set up for IBM VM/CMS involved acquiring a large arena using
the system malloc and then suballocating that using the binary buddy system.
Since when this storage becomes exhausted INR cannot proceed, the primary
allocation was made as large as possible.

This version of the source contains a number of changes made immediately
to the 2.0e version of March 25, 1988 prior to the release of INR under
a GPL3 license in June of 2010.
All of the conditional compilations for VAX, CMS, UNIX and Honeywell
platforms were removed and the storage manager was changed to the second
scheme (the CMS version).

The minimum allocation unit of storage was increased to 24 bytes to
accomodate 64-bit pointer types.
In addition, Saudit was modified to handle the reporting for larger arenas.
All of these changes qualify as routine bug-fixes to allow INR to run on
more modern systems.

#### compare tool (2022-01-06)

The objective of the redux branch is to take a fresh run at making INR
maintainable, fixing bugs, and documenting the process as well as the
resulting product.

In order to do this, it is important to install all of the bug-fixes that
were patched previously in other branches of the repo.
A tool was developed using perl and astyle that removes all white-space
formatting and then re-introduces a formatting that can be used to highlight
real changes.

This code is used to compare the source directory with a reference directory
copied from another source.
The 'c9c1eff' commit from the ginr repository is copied to rsrc and used
to ensure that all updates have been made as of June 2010.

## 2.0.0f redux reformat (2022-01-01)

#### 2.0.0f src (2022-01-01)

1. Filter the source through expand and astyle with the default options.
This will cause tabs (assumed tabs 8) to be replaced by spaces and then the
result to be reindented by the astyle program to a tab setting of 4.

2. Add GPL version 3 license headers.

3. Cleanup.
Repair bad formatting in Parse.y and update stamp.c.

## 2.0.0e redux (2022-01-01)

#### 2.0.0e src (1988-03-25)

The code will compile and run if two bugs are fixed:

1. Aload.c initializes the static global variables fpin and fpout to
non-constant values.
Since these are initialized at the beginning of main, these initializers
can be removed without effect.

2. Because the development environment was a VAX 11/780 running UNIX, an
optimized version of a copy routine used an assembler instruction via an
asm directive.
This optimization can be deselected by commenting out a #define VAX
directive in local.h .

There are more problems to be addressed before INR runs properly, but with
these two changes, it will compile, and run sufficiently well to define
a small finite state machine.
Before other changes will be done, this redux branch will apply some global
reformating changes.

#### 2.0.0e doc pdf (2010-07-29)

The latex source appears to no longer work, at least with default settings.
I am including a pdf file produced in 2010 with minimal changes to the source.
This documentation is not adequate or in the most convenient form at the
present time.

## 2.0.0e redux (2021-12-31)

#### 2.0.0e (1988-03-25)

The contents of this repository are restored as accurately as possible to
the state from 1988-03-25, with a reconstructed directory structure.
Note that this version will no longer compile and contains artifacts from
its early development environment on a DEC VAX 11/780 running Berkley
Unix plus ports to a Honeywell DPS8/49 and an IBM 370 running VM/CMS
environment using the Waterloo C compiler.

#### Why INR redux?

INR was developed between 1983 and 1988 as a student at the University of
Waterloo, and subsequently as a junior member of the faculty of the
Department of Computer Science there.
At the time, software developed by students and faculty were considered to be
the intellectual property of the authors.
This had the effect of encouraging the development of new software-based
technologies and the subsequent spin-off companies, such as, for example,
Maple and OpenText which began then in exactly that way.

Because I was the sole author of this code and never ceded my rights, I
retain these rights.
In 2010, I released a version of INR dated Mar 25, 1988 as ginr under a
GPL version 3 on Google Code with only minor updates to repair a small
number of bugs.
This was done with the knowledge and permission of my then employer.

All of this is to assert my claim to the copyright, and my right to
release the code under the GPL3 license as well as to document any prior
art claims that could later become necessary.
The choice of GPL version 3 was intentional although I am willing to
consider alternative open-source licenses if there is a real need.

The main branch (master) in this repository as of 2021 derives from the
2010 release that migrated to GitHub with the demise of Google Code.
It includes some work that I did in 2018 to clean up the formatting of
the source in preparation for more serious work.
I hadn't been doing C programming for a while so I wasn't sure how this
should best be done but this seemed necessary.

In particular, I wanted to expand tabs to blanks, change the indentation to
4 as well as update the K&R C to ANSI C and correct a number of
inconsistencies that had crept in.
For this purpose, I used a program called astyle (Artistic Style) with
rather agressive settings with the hope of doing this as well as washing
out the inconsistencies.
Unfortuntely, I now look with distaste at the extent of the changes caused by
this work and want to reformat everything to something much closer to
what I originally had.

This work all must be done carefully because my younger self was more
proficient with writing correct code and the older code had been subject to
years of testing through use.
It seems to me that I need to repeat the derivation from the 2010 version to
the present version to ensure bug fixes are properly applied.
This should make the nature of the changes more apparent by use of git diff.

The first step of this process is to get the code into a form that has
consistent formatting that is as readable as possible (at least by me).
Then comes (1) making it compile, (2) making it compile without warnings,
and (3) converting it from K&R C to C89 (ANSI C).

-- J Howard Johnson (2021-12-31)
