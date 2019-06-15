#
LIBOBJ = u_convert.o
TARG	= libprim.a
CC	= g++
CFLAGS	= -g -DLINUX -I../../include/Api -I../cs/include

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARG)
$(TARG): $(LIBOBJ)
	ar r $(TARG) $(LIBOBJ)
	ranlib $(TARG)
	cp $(TARG) ../../lib

clean:
	/bin/rm -f $(TARG) *.o

