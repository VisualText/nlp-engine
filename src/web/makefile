#
LIBOBJ = StdAfx.o tear.o url.o urls.o util.o web.o

TARG	= web.a
CC	= g++
CFLAGS	= -g -DLINUX -I../../include/Api -Wno-write-strings

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARG)
$(TARG): $(LIBOBJ)
	rm -f $(TARG)
	ar r $(TARG) $(LIBOBJ)
	ranlib $(TARG)
	cp $(TARG) ../../lib

clean:
	/bin/rm -f $(TARG) *.o

