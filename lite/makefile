#
LIBOBJ =	Agram.o algo.o ana.o angen.o arg.o Arug.o Arun.o Aseq.o Auser.o Avar.o chars.o \
		check.o code.o dir.o dsem.o dyn.o eana.o fn.o fnrun.o folder.o gen.o genhash.o global.o \
		Gui.o htab.o iaction.o iarg.o ielement.o ielt.o iexpr.o iexprstmt.o ifile.o ifunc.o \
		iifstmt.o intern.o io.o iop.o ipair.o ipnode.o ipost.o ipre.o irecurse.o iregion.o \
		ireturn.o irule.o istmt.o istr.o isugg.o ivar.o iwhilestmt.o lextagger.o line.o lite.o \
		literug.o ln.o mach.o mode.o multi.o nintern.o nlp.o nlppp.o nug.o parse.o pat.o path.o \
		phash.o pn.o post.o postrfa.o pre.o rec.o rfa.o rfasem.o rug.o sem.o seqn.o stab.o \
		starr.o StdAfx.o std.o str.o string.o stub_e.o stub_s.o sym.o tok.o tsym.o uAlgo.o \
		u_ofstream.o u_out.o var.o vtrun.o WNFunctions.o xml.o cmltok.o regexp.o dicttok.o kb.o

# NOTE: RUNEMBED_ to run embedded analyzer! # 09/29/19 AM>
TARG	= lite.a
CC		= g++
CFLAGS	= -DLINUX -I../include/Api/lite -I../include/Api -I/../cs/include \
		-I/usr/local/include -I/usr/local/libiodbc-3.52.12/include \
		-Wno-write-strings -Wno-deprecated

%.o: %.cpp
		$(CC) $(CFLAGS) -c $< -o $@

all: $(TARG)
$(TARG): $(LIBOBJ)
		rm -f $(TARG)
		ar r $(TARG) $(LIBOBJ)
		ranlib $(TARG)
		cp $(TARG) ../lib

clean:
		/bin/rm -f $(TARG) *.o

