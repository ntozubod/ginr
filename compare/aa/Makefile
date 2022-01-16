HFILES	= O.h local.h
CFILES 	= stamp.c Acrea.c Aopen.c Aload.c Atrim.c Alameq.c Alamcm.c Aclos.c \
Asubs.c Amin.c Abasic.c Aenum.c Acompose.c Ajoin.c Aretape.c Apowers.c \
Alenmin.c Asseq.c AGMsseq.c ALMsseq.c Aclsseq.c Astems.c T.c V.c R.c U.c S.c \
util.c Lex.c Colon.c
OFILES 	= stamp.o Acrea.o Aopen.o Aload.o Atrim.o Alameq.o Alamcm.o Aclos.o \
Asubs.o Amin.o Abasic.o Aenum.o Acompose.o Ajoin.o Aretape.o Apowers.o \
Alenmin.o Asseq.o AGMsseq.o ALMsseq.o Aclsseq.o Astems.o T.o V.o R.o U.o S.o \
util.o Lex.o Colon.o y.tab.o
SOURCE 	= Makefile $(HFILES) $(CFILES) Parse.y

.c.o:
	gcc -Wall -O -c $*.c

ginr:	$(OFILES)
	gcc -Wall -O $(OFILES) -o ginr

format:	$(CFILES)
	astyle --style=1tbs $(CFILES)

inrx.a:
	echo hi >inrx.a
	rm inrx.a
	ar q inrx.a $(OFILES)
	ranlib inrx.a

install:	ginr
	cp -p ginr ~/bin/ninr

lint:	y.tab.c
	lint $(CFILES) y.tab.c

$(OFILES):	O.h local.h

y.tab.c y.tab.h:	Parse.y
	yacc -d Parse.y

Lex.o:	y.tab.h

clean:
	rm -f $(OFILES) y.tab.h y.tab.c
