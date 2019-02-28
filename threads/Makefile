CXX = gcc
BIN = prime_threads
FILES = prime_threads.c
LIBS = -lm -pthread

default: $(BIN)

$(BIN):
	$(CXX) -o $(BIN) $(FILES) $(LIBS)

run: $(BIN)
	./$(BIN)

clean:
	rm ./$(BIN)


.PHONY: $(BIN) run