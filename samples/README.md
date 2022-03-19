# Samples Directory

In order to understand how to use INR and why it solves any problems that are
not easily solved by other approaches, it helps to see some sample code
presented as use cases.

These can be quite challenging to understand but this is usually because the
problems are combinatorial in nature ultimately.
Quite often a verbal description ia also difficult to understand.

In each of these a description of the problem addressed with a sense of why
this is not easily solved in other ways followed by a description of how
the problem can be described using transduction forms and hence solved
partially or completely.

Some of these involve the use of Markdown since although it is relatively
straight-forward to describe and use, corner cases pop up that can be
challenging.

[md_eqn](md_eqn)

> In this example, an attempt to identify the equations delimited by dollar
signs in a Markdown text is complicated by the fact that dollar signs
also occur in code blocks.
This is actually the result of *pandoc* processing in the `doc` directory
for the conversion of the latex user manual from 1988 for USAGE.md.
Referring to the `doc` directory you can see how Perl was used to hack this
conversion.
