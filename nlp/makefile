#
OBJS = main.o Uanalyze.o u_out.o

NLP	= /opt/nlp-engine
TARG	= nlp.exe
CC	= g++
LIBD	= $(NLP)/lib
LIBS	= $(LIBD)/lite.a $(LIBD)/libconsh.a $(LIBD)/libkbm.a \
	  $(LIBD)/libprim.a $(LIBD)/words.a $(NLP)/nlp/kb/kb.a \
          $(NLP)/nlp/run/run.a /usr/local/lib/libiodbc.a -ldl

CFLAGS	= -g -DLINUX -I$(NLP)/include/Api -L$(LIBD) \
          -I/usr/local/libiodbc-3.52.12/include \
          -Wno-write-strings -Wno-deprecated

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARG)
$(TARG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
		chmod 755 $(TARG)

clean:
	/bin/rm -f $(TARG) *.o

