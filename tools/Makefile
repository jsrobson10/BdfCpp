
FILES=bdfconvert bdfedit
CARGS=-L .. -lbdf -Bstatic -lboost_iostreams -O3 -Wall -Werror -L ".."
CC=g++

build: $(FILES)

install: build
	cp $(FILES) /usr/bin

uninstall:
	cd /usr/bin && rm $(FILES) || true

clean:
	rm $(FILES) || true

%: %.cpp
	$(CC) -o $@ $< $(CARGS)
