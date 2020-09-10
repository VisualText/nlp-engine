#
LIBOBJ = Aconsh.o arg.o bind.o cc_code.o cc_gen.o cg.o \
cg_global.o cmd.o con_gen.o con_ini.o consh_gen.o consh_kb.o \
dyn.o gen.o help.o ind.o io.o libconsh.o ptr_gen.o \
ptr_ini.o st_gen.o st_ini.o sym_gen.o sym_ini.o u_out.o ui.o

TARG	= libconsh.a
CC	= g++
CFLAGS	= -DLINUX -I../../include/Api -I../include -Wno-write-strings

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

