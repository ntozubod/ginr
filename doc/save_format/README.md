# A new save/transfer format for INR

## Introduction

INR from early on had two different file formats for automata:

1. The :pr format was intended to by quite readable and present automata
as they were usually considered as a collection of transitions.
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
However, interoperability is extremely important now even between platforms.
What is reading the file may be written in a different language and run on
a different operating system.

Thus the :save format is now being moved forward to fill this role.

## Specification

The :save format has two types of lines of variable length ending with the
newline character ('\x0A'):

1. The first line is a header line that contains a 'magic number', and
warns of the number of tapes in the automaton and the number of rows.
This is to facilitate the pre-allocation of arrays in the receiving
environment.

2. Each remaining line contains information about one transition including
the from state, the to state, the tape number, and the transition label.

Each line contains a number of fields separated by the tab character
('\x09').

No extra tabs, blanks, or new lines are allowed with one exception to be
explained.

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

#### The format

##### Header Line

Field one contains the 'magic number' `INR210`.

Field two has the number of tapes.

Field three has the number of rows.

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

`'abc' :min :save ttt;`

    INR210	1	4
    0	2	0	1	a
    2	3	0	1	b
    3	4	0	1	c
    4	1	0	0	

Note here that `:min` is needed here to ensure that minimization is done.

`'abc' :update;`

    (START) a 2
    2 ^^ 3
    4 ^^ 5
    3 b 4
    6 -| (FINAL)
    5 c 6

`'abc' :update :save uuu;`

    INR210	1	6
    0	2	0	1	a
    2	3	-1	0	
    4	5	-1	0	
    3	4	0	1	b
    6	1	0	0	
    5	6	0	1	c

## Parsing the save format in C

In order to show how easy the new save format is to parse the following
is the core of the code that INR uses:

        c = getc( fp );
    
    /* Magic Number */
    
        if ( c != 'I' ) { goto FAIL_FORMAT; }  c = getc( fp );
        if ( c != 'N' ) { goto FAIL_FORMAT; }  c = getc( fp );
        if ( c != 'R' ) { goto FAIL_FORMAT; }  c = getc( fp );
        if ( c != '2' ) { goto FAIL_FORMAT; }  c = getc( fp );
        if ( c != '1' ) { goto FAIL_FORMAT; }  c = getc( fp );
        if ( c != '0' ) { goto FAIL_FORMAT; }  c = getc( fp );
        if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );
    
    /* Number of tapes */
    
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            number_tapes = c - '0'; c = getc( fp );
    
        while ( c != '\t' ) {
            if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            number_tapes = number_tapes * 10  +  ( c - '0' );
                c = getc( fp );
            if ( number_tapes >= MAXSHORT ) { goto FAIL_FORMAT; }
        }
        A-> A_nT = number_tapes;
        if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );
    
    /* Number of rows */
    
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            number_rows = c - '0'; c = getc( fp );
    
        while ( c != '\n' ) {
            if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            number_rows = number_rows * 10  +  ( c - '0' );
                c = getc( fp );
            if ( number_rows >= MAXSHORT ) { goto FAIL_FORMAT; }
        }
        if ( c != '\n' ) { goto FAIL_FORMAT; } c = getc( fp );
    
    /* Get a row */
    
    NEXT_ROW:
    
    /* From state */
    
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            from_state = c - '0'; c = getc( fp );
    
        while ( c != '\t' ) {
            if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            from_state = from_state * 10  +  ( c - '0' );
                c = getc( fp );
            if ( from_state >= MAXSHORT ) { goto FAIL_FORMAT; }
        }
        if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );
    
    /* To state */
    
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            to_state = c - '0'; c = getc( fp );
    
        while ( c != '\t' ) {
            if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            to_state = to_state * 10  +  ( c - '0' );
                c = getc( fp );
            if ( to_state >= MAXSHORT ) { goto FAIL_FORMAT; }
        }
        if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );
    
    /* Tape number */
    
        if ( c == '-' ) {
            c = getc( fp );
            if ( c != '1' ) { goto FAIL_FORMAT; } c = getc( fp );
            tape_no = (-1);
        } else {
            if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
                tape_no = c - '0'; c = getc( fp );
    
            while ( c != '\t' ) {
                if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
                tape_no = tape_no * 10  +  ( c - '0' );
                    c = getc( fp );
                if ( tape_no >= MAXSHORT ) { goto FAIL_FORMAT; }
            }
        }
        if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );
    
    /* Token length */
    
        if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            length = c - '0'; c = getc( fp );
    
        while ( c != '\t' ) {
            if ( c < '0' || c > '9' ) { goto FAIL_FORMAT; }
            length = length * 10  +  ( c - '0' );
                c = getc( fp );
            if ( length >= MAXSHORT ) { goto FAIL_FORMAT; }
        }
        if ( c != '\t' ) { goto FAIL_FORMAT; } c = getc( fp );
    
    /* Get a token */
    
        if ( length + 1 >= Ssize( buffer ) ) {
            buffer = Srealloc( buffer, length + 1 );
        }
    
        i = 0;
        while ( i < length ) {
            buffer[ i++ ] = c;
            c = getc( fp );
            if ( c == EOF ) { goto FAIL_FORMAT; }
        }
        buffer[ i ] = '\0';
        if ( c != '\n' ) { goto FAIL_FORMAT; } c = getc( fp );
    
    /* Process a line here */
    
        if ( tape_no == -1 ) {
            A = A_add( A, from_state, 0, to_state );
            if ( length != 0 ) { goto FAIL_FORMAT; }
        } else if ( length == 0 ) {
            label = 1 * number_tapes + tape_no;
            A = A_add( A, from_state, label, to_state );
        } else {
            index = Tn_insert( Tn_Sigma, buffer, length );
            label = index * number_tapes + tape_no;
            A = A_add( A, from_state, label, to_state );
        }
    
        if ( c != EOF ) { goto NEXT_ROW; }
    
    /* Done: package up result */
    
        Sfree( buffer );
        return ( A );
    
    FAIL_FORMAT:

Parsing input can be very tedious in low level languages.
Nonetheless, the translation to code is quite direct.
