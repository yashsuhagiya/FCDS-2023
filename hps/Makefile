FLAGS=-O0 -g -lpthread

# CXX=icpc

RM=rm -f

EXEC=mopp-2018-t0-harmonic-progression-sum

all: $(EXEC)

$(EXEC):
	$(CXX) $(FLAGS) $(EXEC).cpp -c -o $(EXEC).o
	$(CXX) $(EXEC).o $(FLAGS) -o $(EXEC)

clean:
	$(RM) $(EXEC).o $(EXEC)
