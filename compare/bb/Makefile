HFILES	= O.h local.h
CFILES 	= stamp.c Acrea.c Aopen.c Aload.c Atrim.c Alameq.c Alamcm.c Aclos.c \
Asubs.c Amin.c Abasic.c Aenum.c Acompose.c Ajoin.c Apowers.c Aretape.c \
Alenmin.c Astems.c Asseq.c Aclsseq.c ALMsseq.c AGMsseq.c T.c V.c R.c U.c S.c \
util.c Lex.c Colon.c
OFILES 	= stamp.o Acrea.o Aopen.o Aload.o Atrim.o Alameq.o Alamcm.o Aclos.o \
Asubs.o Amin.o Abasic.o Aenum.o Acompose.o Ajoin.o Apowers.o Aretape.o \
Alenmin.o Astems.o Asseq.o Aclsseq.o ALMsseq.o AGMsseq.o T.o V.o R.o U.o S.o \
util.o Lex.o Colon.o y.tab.o
SOURCE 	= Makefile $(HFILES) localc.h localh.h locals.h $(CFILES) Parse.y \
compcms.exec cwinr.exec comphw.exec

.c.o:
	gcc -Wall -O -c $*.c

ginr:	$(OFILES)
	gcc -Wall -O $(OFILES) -o ginr

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
