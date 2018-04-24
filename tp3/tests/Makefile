EXEC=tests
CC=g++
CFLAGS=--std=c++14 -O3 -w
SRC=tests-main.o ../block.hpp ../tower.hpp tests-tower.cpp

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

tests-main.o:
	$(CC) $(CFLAGS) tests-main.cpp -c

clean:
	rm $(EXEC)
