#
LIBOBJ = Cc_code.o Con0.o Con_ini.o kb.o kb_setup.o Ptr0.o Ptr_ini.o St0.o \
	StdAfx.o St_ini.o Sym0.o Sym10.o Sym11.o Sym12.o Sym13.o Sym14.o \
	Sym15.o Sym16.o Sym17.o Sym18.o Sym19.o Sym1.o Sym20.o Sym21.o \
	Sym22.o Sym23.o Sym24.o Sym25.o Sym26.o Sym27.o Sym28.o Sym29.o \
	Sym2.o Sym30.o Sym31.o Sym32.o Sym33.o Sym34.o Sym35.o Sym36.o \
	Sym37.o Sym38.o Sym39.o Sym3.o Sym40.o Sym41.o Sym42.o Sym43.o \
	Sym44.o Sym45.o Sym46.o Sym47.o Sym48.o Sym4.o Sym5.o Sym6.o \
	Sym7.o Sym8.o Sym9.o Sym_ini.o

# Note: Need an environment var to point to TEXTAI area.
TEXTAI	= /u/nlpsearch/dev
TARG	= kb.a
CC	= gcc
CFLAGS	= -DLINUX -I$(TEXTAI)/include/Api

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARG)
$(TARG): $(LIBOBJ)
	ar r $(TARG) $(LIBOBJ)
	ranlib $(TARG)
	cp -f $(TARG) ../lib
	chmod 644 $(TARG)

clean:
	/bin/rm -f $(TARG) *.o

