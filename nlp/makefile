#
OBJS = main.o Uanalyze.o u_out.o

# Note: Need an environment var to point to NLPDEV area.
NLPDEV	= /dev/nlp/nlp-engine
APP	= /dev/nlp/nlp-engine/nlp
TARG	= nlp.exe
#CC	= gcc
CC	= g++
LIBD	= $(NLPDEV)/lib
LIBS	= $(APP)/user/user.a $(LIBD)/lite.a $(LIBD)/libconsh.a $(LIBD)/libkbm.a \
	  $(LIBD)/libprim.a $(LIBD)/words.a $(APP)/lib/kb.a \
	  /usr/lib64/libiodbc.so /usr/lib64/libiodbcinst.so -ldl
#	  /usr/local/lib/libiodbc.a /usr/local/lib/libiodbcinst.a -ldl
#	  -ldl

CFLAGS	= -g -DLINUX -I$(NLPDEV)/include/Api -I/usr/local/include -I/usr/include/libiodbc -L$(LIBD)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARG)
$(TARG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
		chmod 755 $(TARG)

clean:
	/bin/rm -f $(TARG) *.o

