#
LIBOBJ = Ualgo.o Ucode.o Umisc.o Upre.o Ureg.o user.o

# Note: Need an environment var to point to TEXTAI area.
TEXTAI	= /opt/nlp-engine
TARG	= user.a
CC	= gcc
CFLAGS	= -DLINUX -I$(TEXTAI)/include/Api -I/usr/local/libiodbc-3.52.12/include \
          -Wno-write-strings -Wno-deprecated

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

