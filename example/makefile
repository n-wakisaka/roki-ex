INCLUDE=`roki-ex-config -I`
LIB=`roki-ex-config -L`
LINK=`roki-ex-config -l`

CC=gcc
CFLAGS=-ansi -Wall -O3 $(LIB) $(INCLUDE) -funroll-loops -g

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	rm -f *.o *~ core *test
