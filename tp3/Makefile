EXEC=tp3
CC=g++
CFLAGS=--std=c++14 -O3 -w
SRC=main.cpp $(wildcard *.hpp)

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

clean:
	rm $(EXEC)
