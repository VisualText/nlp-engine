#
LIBOBJ = word1.o word10.o word11.o word12.o word13.o word14.o word15.o \
word16.o \
word17.o word18.o word19.o word2.o word20.o word21.o word22.o word23.o \
word24.o word25.o word26.o word27.o word28.o word29.o word3.o word30.o \
word4.o word5.o word6.o word7.o word8.o word9.o \
wordarrs.o

TARG	= words.a
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

