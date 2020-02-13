#
LIBOBJ = analyzer.o auxiliary.o ehash.o rhash.o run.o

TEXTAI	= /opt/nlp-engine
TARG	= run.a
CC	= gcc
LIBD    = $(TEXTAI)/lib
LIBKB   = $(TEXTAI)/nlp/kb
LIBS    = $(LIBD)/lite.a $(LIBD)/libconsh.a $(LIBD)/libkbm.a \
          $(LIBD)/libprim.a $(LIBD)/words.a $(LIBKB)/kb.a \
          /usr/lib/libiodbc.a /usr/lib/libiodbcinst.a -ldl
CFLAGS	= -g -DLINUX -I$(TEXTAI)/include/Api -I/usr/local/include \
          -Wno-write-strings -Wno-deprecated -L$(LIBD)

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

