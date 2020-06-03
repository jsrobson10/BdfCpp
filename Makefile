CC=g++
CFLAGS=BdfArray.h BdfNamedList.h BdfObject.h BdfTypes.h BdfIndent.h headers.h helpers.h

build: tests.o BdfArray.o BdfNamedList.o BdfObject.o BdfTypes.o BdfIndent.o helpers.o
	$(CC) $(CFLAGS) tests.o BdfArray.o BdfNamedList.o BdfObject.o BdfTypes.o BdfIndent.o helpers.o -o tests

clean:
	rm *.o tests

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $<
