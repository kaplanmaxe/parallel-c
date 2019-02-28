CXX = gcc
BIN = prime
FILES = prime.c
LIBS = -lm

default: $(BIN)

$(BIN):
	$(CXX) -o $(BIN) $(FILES) $(LIBS)

run: $(BIN)
	./$(BIN)

clean:
	rm ./$(BIN)


.PHONY: $(BIN) run