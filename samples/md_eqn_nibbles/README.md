# Find equations in Markdown text

One of the problems that came up during the conversion of the 1988
documentation writen in Latex was that equations were passed through
without change and displayed as they occur in the input.

There is a question of what to do with these equations when they appear
since GitHub provides no equation support itself.
One solution is to expand `$<<TEXT>>$` to
`<img src="https://render.githubusercontent.com/render/math?math=<<TEXT>>">`
thus using a service that renders the math as an image and displays the
resulting image.
This is acceptable although a bit of overkill for that nature of the math
used in this INR documentation.

However, whatever is done, it is necessary to identify all of the equations
for processing.
What makes this more complicated is that `$` is used also as operator in
INR that is used for the general operation of renumbering, deleting and
duplicating tapes for multitape automata.
All of these usages must not be mistaken for equations.

Fortunately, all of these usages occur in code literal blocks signaled by
back quotes or in a quote block marked with a greater-than sign and five
spaces.
This seems pretty tedious and error prone to change by hand and relatively
straight-forward to program in a scripting language like Perl.

This is what was done in the previous conversion although it turned out
to be harder to do correctly than one would first expect and it became
clear that INR itself could do this better if support for reading in a
text, writing out a text, as well as doing character level text processing
was performed.

These are the goals of providing Unicode UTF-8 support.

In this sample, we will use :slurp_nibbles and :spit_nibbles to read the
needed text file in and output the result.
The new double quoted string expressions will be used to create nibble
sequences for matching purposes.
Finally, a new function :gen_min will be used to select the single string
that is the shortest with minimum in lexicographic ordering used to break
ties.

These features all work together to allow for a solution to the problem
although the :slurp_octets, :spit_octets or the :slurp_utf8, :spit_utf8
routines could also be used.

The approach is to:

1. Slurp the contents of the raw file `c.md` as produced by pandoc.
   :slurp_nibbles will break each octet seen into a high octet and a low
   octet.
   These octets are distinguished to avoid spurious matching at a later
   stge.

2. Define the finite alphabet of possibilities: 16 high octets plus 16
   low octets yields an alphabet size of 32.
   Note that the :slurp_octets approach would require an alphabet of size
   256 and :slurp_utf8 would require other accomodations.
   Nibble contains the alphabet and Nibbles is an automaton that matches
   a possibly non-zero sequence of nibbles.
   The implicit constraint of alternation of nibble type can be ignored
   here.
   Copy_nibbles is a transducer that copies any nibble sequence exactly.

3. The three kinds of code blocks are defined as a transducer that copies
   everything between a start sequence of nibbles defined by double quoted
   string and a corresponding end sequence of nibbles.
   Furthermore, a constraint is applied to ensure that the code block does
   not contain an earlier termination marker.
   INR does not have a shortest match operator but this is more direct
   and precise and generalizes better.

4. The two kinds of equation blocks are defined for single and double dollar
   sign forms.
   This is done in the same way as for code blocks.

5. An automaton is defined that matches anything that does not contain one
   of the above blocks.

6. The Code blocks and the background block are converted into tranducers
   tha copy their input including the beginning and end markers.

7. The Equation blocks are modified to copy only the internal text
   preceded and followed by a more distinctive set of markers.
   At this stage a more complex process could be applied.

8. A transducer is easily constructed from the 6 pieces defined above since
   no nesting can occur.
   If there is an error caused by an unmatched marker, the input will need
   to be modified to throw in a matching end marker to allow the parse.
   Otherwise, our transducer to be fine as is.

9. Apply the complete transducer to the input nibble sequence.
   From the way this is constructed above, the output should be unique.
   However, with more complex cases, a single output must be chosen using
   :gen_min.

10. Write the result out using :spit_nibbles.

Notice that the only place where we actually had to look at the format of
the nibbles was in the definition of the alphabet.
A pending change is to make this a builtin function.
Such builtins will also be defined for :slurp_octets and :slurp_utf8.
