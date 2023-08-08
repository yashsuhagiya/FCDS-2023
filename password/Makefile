FLAGS=-O3 -lcrypto -pthread -lm

CC=gcc

RM=rm -f

EXEC=mopp-2019-t2-brute-force-password-cracking

all: $(EXEC)

$(EXEC):
	$(CC) $(FLAGS) mopp-2019-t2-brute-force-password-cracking.c -c -o mopp-2019-t2-brute-force-password-cracking.o -lpthread -latomic
	$(CC) mopp-2019-t2-brute-force-password-cracking.o -o $(EXEC) $(FLAGS)

run:
	./$(EXEC)

clean:
	$(RM) mopp-2019-t2-brute-force-password-cracking.o $(EXEC)