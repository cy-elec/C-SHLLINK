#Makefile for compiling static library

LIBDIR = ../com/
LIBSRC = ../src/
LIB_RN = cshllink
LIBN = $(LIBDIR)lib$(LIB_RN).a

CC = gcc
CFLAGS = -Wall

src = $(wildcard $(LIBSRC)*.c)

all: $(LIBN)
	@echo -e "\n############\nBuilt all\n############"

$(src):
	@-$(CC) -c $@ $(CFLAGS) -o $(basename $(notdir $@)).o

$(LIBN): $(src) $(wildcard *.o)
	-rm $(LIBN)
	@ar -cvq $(LIBN) *.o
	ar -t $(LIBN)
	@echo -e "\n############\nBuilt llib\n############"
	
	cp $(wildcard $(LIBSRC)*.h) $(LIBDIR)
	@echo -e "\n############\nCopied llib\n############"
	@rm $(LIB_RN).o

exmpl: $(LIBN)
	$(CC) ../exmpl/exmpl.c $(CFLAGS) -o exmpl.o -I$(LIBSRC) -L$(LIBDIR) -lcshllink
	-./exmpl.o
	rm exmpl.o

clean:
	-rm *.o
