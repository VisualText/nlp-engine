#
LIBOBJ = Ualgo.o Ucode.o Umisc.o Upre.o Ureg.o user.o

# Note: Need an environment var to point to TEXTAI area.
TEXTAI	= /u/nlpsearch/dev
TARG	= user.a
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

