
FILES=fibonacci helloworld
CARGS=-lbdf -Wall -Werror -g
CC=g++

build: $(FILES)

clean:
	rm $(FILES) *.bdf || true

%: %.cpp
	$(CC) -o $@ $< $(CARGS)
