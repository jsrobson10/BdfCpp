CC=g++
CARGS=-O3 -Wall -Werror -fpic -DIS_LITTLE_ENDIAN=1
CFLAGS=BdfArray.h BdfNamedList.h BdfObject.h BdfTypes.h BdfIndent.h BdfReader.h BdfLookupTable.h BdfHelpers.h BdfError.h BdfStringReader.h BdfReaderHuman.h Bdf.h
COMPILE=BdfArray.o BdfNamedList.o BdfObject.o BdfIndent.o BdfLookupTable.o BdfReader.o BdfHelpers.o BdfError.o BdfStringReader.o BdfReaderHuman.o
OUT=libbdf.so.0

build: $(COMPILE) $(CFLAGS)
	$(CC) $(COMPILE) -shared -o $(OUT) $(CARGS)

clean:
	rm *.o *.so*

install:
	cp $(OUT) /usr/lib
	ln -s /usr/lib/$(OUT) /usr/lib/libbdf.so
	chmod 0755 /usr/lib/$(OUT)
	chmod 0755 /usr/lib/libbdf.so
	[ -d "/usr/include/bdf" ] || mkdir /usr/include/bdf
	cp *.h /usr/include/bdf
	ldconfig

uninstall:
	rm /usr/lib/$(OUT)
	rm /usr/lib/libbdf.so
	rm -r /usr/include/bdf
	ldconfig

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CARGS)
