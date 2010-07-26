HFILES	= O.h local.h
CFILES 	= stamp.c Acrea.c Aopen.c Aload.c Atrim.c Alameq.c Alamcm.c Aclos.c \
Asubs.c Amin.c Abasic.c Aenum.c Acompose.c Ajoin.c Apowers.c Aretape.c \
Alenmin.c Astems.c Asseq.c Aclsseq.c ALMsseq.c AGMsseq.c T.c V.c R.c U.c S.c \
util.c
OFILES 	= stamp.o Acrea.o Aopen.o Aload.o Atrim.o Alameq.o Alamcm.o Aclos.o \
Asubs.o Amin.o Abasic.o Aenum.o Acompose.o Ajoin.o Apowers.o Aretape.o \
Alenmin.o Astems.o Asseq.o Aclsseq.o ALMsseq.o AGMsseq.o T.o V.o R.o U.o S.o \
util.o y.tab.o
SOURCE 	= Makefile $(HFILES) localc.h localh.h locals.h $(CFILES) inrat.y \
compcms.exec cwinr.exec comphw.exec
SUNFILES = Makefile O.h locals.h $(CFILES) inrat.y
CMSFILES = O.h localc.h $(CFILES) y.tab.c compcms.exec cwinr.exec
HWFILES  = O.h localh.h $(CFILES) y.tab.c comphw.exec

.c.o:
	cc -O -c $*.c

tinr:	$(OFILES)
	cc -O $(OFILES) -o tinr

inrx.a:
	echo hi >inrx.a
	rm inrx.a
	ar q inrx.a $(OFILES)
	ranlib inrx.a

install:	tinr inrx.a
	mv tinr /u/jhjohnson/bin/inrx
	mv inrx.a /u/jhjohnson/lib/inrx.a

lint:	y.tab.c
	lint $(CFILES) y.tab.c

$(OFILES):	O.h local.h

y.tab.c:	inrat.y
	yacc inrat.y

hpr:
	pr $(SOURCE) | lpr -Phw

lpr:
	pr $(SOURCE) | lpr

clean:
	rm $(OFILES) y.tab.c

installer:
	rcp /u/jhjohnson/bin/inrx water:/u/jhjohnson/bin/inrx

installdragon:
	rcp /u/jhjohnson/bin/inrx watdragon:/u/jhjohnson/bin/inrx
	rcp /u/jhjohnson/lib/inrx.a watdragon:/u/jhjohnson/lib/inrx.a
	rcp -p $(SOURCE) watdragon:/u/jhjohnson/INR/1.1e
	
sol:
	rcp -p $(SUNFILES) watsol:/u/jhjohnson/INR/1.1e
	rsh watsol "cd /u/jhjohnson/INR/1.1e; mv locals.h local.h; make"

installsol:	sol
	rsh watsol "cd /u/jhjohnson/INR/1.1e; make install"

cleansol:
	rsh watsol "cd /u/jhjohnson/INR/1.1e; make clean"

installall:	install installer installdragon installsol

cms:
	Uenc $(CMSFILES) > INR11E.UENC
	vmcopy watdcs%jjohnson if=INR11E.UENC name=INR11E.UENC width=80
	rm INR11E.UENC

hw:
	hsend -m -u jhjohnson/rata $(HWFILES)
