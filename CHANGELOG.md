# INR redux Changelog

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
