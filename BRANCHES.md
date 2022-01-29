# Organization of *ginr* repo branches

The branch history of INR has been a little confusing.
The several attempts to sort this out have all been incomplete and
added to the confusion.
This has been brought in better order.

There are three main branches with the names of *main*, *stable*, and *dev*.

The *main* branch traces the development of INR beyond its 1988 roots at the
University of Waterloo, and the several previous attempts at consolidation.
It will represent the current version that is reasonable solid and shows no
known bugs.
However, it will be introducing changes to behaviour and might break old
scripts.
Since the current test suite needs improvement, there also may be bugs
introduced that are not yet detected.

The *stable* branch will trail behind the *main* branch and, hopefully, be
more reliable and 'stable'.
At the present time, no changes in behaviour will be made to this branch
although bugs will be patched as they are detected in other branches.

The *dev* branch will explore new ideas although day-to-day work will happen
elsewhere.

#### Version numbers

Version numbers have been a bit chaotic in the past and are now in a bit
better order.
Since everything is based on a 1988 version with a version number of 2.0e,
all versions that do not involve changes in behaviour other than bug
fixes will now have numbers of the form 2.0.x.
Updates to the src directory will be signaled by a change to the *x* part.

Now that the consolidation is mostly complete and new development involves
change in behaviour, versin numbers will be of the form 2.1.x with the
same promise of changes as the src changes.

#### Historic branches

Because there are a number of old *main* branches, they are identified
as *main2021*, *main2018*, *main2010*, and *main1988*.

The *main1988* branch points to the original material from Mar 25, 1988.
As it is, it doesn't compile although the changes required are quite minor.
This has been cleaned up as a result of recent work and this branch will
point to this.

The *main2010* branch points to the material from the initial release of
INR under a GPL version 3 license on Google Code.
This was migrated to GitHub when Google Code went away.
This version has had many issues addressed but there are still problems
including a change of SHORT to 'unsigned int' that was buggy.

The *main2018* branch was an earlier attempt at consolidation.
In order to make the code more readable, it was agressively reformatted.
A number of other minor changes were made that resulted in a possible
addition of bugs.

The *main2021* branch is the current consolidation and includes a more
reasoned reformatting, combined with updating the source for ANSI C and
a general cleanup.
The main improvement over *main2018*, however, is a re-examination of all
of the updates from old branches.
As a result, it should be more reliable.
The current *main* branch is a descendent of *main2021* but will diverge
if bugs discovered in new work are backported to version 2.0.x.
