CXX = gcc
BIN = prime_process
FILES = prime_process.c
LIBS = -lm

default: $(BIN)

$(BIN):
	$(CXX) -o $(BIN) $(FILES) $(LIBS)

run: $(BIN)
	./$(BIN)

clean:
	rm prime_process

.PHONY: $(BIN) run

