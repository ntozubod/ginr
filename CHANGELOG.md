# INR redux Changelog

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
