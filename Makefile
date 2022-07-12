.c.o:
	cc -g -c $<

all: dbc clean

dist:
	tar -cvf DBC.tar *
	compress DBC.tar

dbc: DBConv.o dbsetup.o dbproc.o dbconv.h
	cc -g -o dbc DBConv.o dbsetup.o dbproc.o

clobber: clean
	@rm dbc

clean:
	@rm -f *.o
	@rm -f core
	@rm -f DBC.tar
	@rm -f DBC.tar.Z

