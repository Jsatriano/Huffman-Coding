CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS = -lm

all: encode decode entropy

encode: encode.o node.o pq.o code.o io.o huffman.o stack.o
	$(CC) -g -o encode encode.o node.o pq.o code.o io.o huffman.o stack.o

decode: decode.o node.o pq.o code.o io.o huffman.o stack.o
	$(CC) -g -o decode decode.o node.o pq.o code.o io.o huffman.o stack.o 

entropy: entropy.o
	$(CC) -g -o entropy entropy.o $(LFLAGS)

encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c

entropy.o: entropy.c
	$(CC) $(CFLAGS) -c entropy.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

code.o: code.c
	$(CC) $(CFLAGS) -c code.c

io.o: io.c
	$(CC) $(CFLAGS) -c io.c

huffman.o: huffman.c
	$(CC) $(CFLAGS) -c huffman.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

clean:
	rm -f encode decode entropy *.o

format:
	clang-format -i -style=file *.c *.h
