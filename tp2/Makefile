EXEC=tp2
CC=g++
CFLAGS=--std=c++14 -O3
SRC= $(wildcard *.cpp *.hpp)

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

clean:
	rm $(EXEC)
