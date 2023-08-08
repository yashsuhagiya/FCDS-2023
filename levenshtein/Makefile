CC = g++

.PHONY = all clean

all : lev

clean :
	rm -rf mopp-2018-t2-levenshtein

lev : mopp-2018-t2-levenshtein.cc
	${CC} -O3 mopp-2018-t2-levenshtein.cc -o mopp-2018-t2-levenshtein -pthread -lpthread

run :
	./mopp-2018-t2-levenshtein
	