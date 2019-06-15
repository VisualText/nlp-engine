#
LIBOBJ = dir.o io.o libprim.o list.o prim.o sm.o str.o dyn.o u_out.o
TARG	= libprim.a
CC	= g++
CFLAGS	= -g -DLINUX -I../../include/Api -I../include

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARG)
$(TARG): $(LIBOBJ)
	ar r $(TARG) $(LIBOBJ)
	ranlib $(TARG)
	cp $(TARG) ../../lib

clean:
	/bin/rm -f $(TARG) *.o

