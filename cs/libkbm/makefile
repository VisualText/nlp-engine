#
LIBOBJ = con_.o Akbm.o st.o attr.o dict.o find.o kbm_alloc.o kbm.o kbm_kb.o libkbm.o pret.o ptr.o sym.o
#StdAfx.cpp
TARG	= libkbm.a
CC	= g++
CFLAGS	= -DLINUX -I../../include/Api -I../include -Wno-write-strings

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARG)
$(TARG): $(LIBOBJ)
	ar r $(TARG) $(LIBOBJ)
	ranlib $(TARG)
	cp $(TARG) ../../lib

clean:
	/bin/rm -f $(TARG) *.o

