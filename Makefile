CC=g++
CARGS=-O3 -Wall -Werror -fpic -DIS_LITTLE_ENDIAN=1
CFLAGS=BdfArray.h BdfNamedList.h BdfObject.h BdfTypes.h BdfIndent.h BdfReader.h BdfLookupTable.h BdfHelpers.h BdfError.h BdfStringReader.h BdfReaderHuman.h Bdf.h
COMPILE=BdfArray.o BdfNamedList.o BdfObject.o BdfIndent.o BdfLookupTable.o BdfReader.o BdfHelpers.o BdfError.o BdfStringReader.o BdfReaderHuman.o
OUT=libbdf.so.0

DEB_VERSION=1.2
DEB_ARCH=amd64

DEB_PATH=libbdf-dev_$(DEB_VERSION)_$(DEB_ARCH)
DEB_PATH_CONTROL=$(DEB_PATH)/DEBIAN/control
DEB_PATH_COPYRIGHT=$(DEB_PATH)/DEBIAN/copyright

build: $(COMPILE) $(CFLAGS)
	$(CC) $(COMPILE) -shared -o $(OUT) $(CARGS)
	ln -s libbdf.so.0 libbdf.so || true
	cd tools && make

build-deb: build
	[ -d $(DEB_PATH) ] || mkdir $(DEB_PATH)
	[ -d $(DEB_PATH)/DEBIAN ] || mkdir $(DEB_PATH)/DEBIAN
	[ -d $(DEB_PATH)/usr ] || mkdir $(DEB_PATH)/usr
	[ -d $(DEB_PATH)/usr/lib ] || mkdir $(DEB_PATH)/usr/lib
	[ -d $(DEB_PATH)/usr/include ] || mkdir $(DEB_PATH)/usr/include
	[ -d $(DEB_PATH)/usr/include/bdf ] || mkdir $(DEB_PATH)/usr/include/bdf
	[ -d $(DEB_PATH)/usr/bin ] || mkdir $(DEB_PATH)/usr/bin
	
	printf "\n\
Package: libbdf-dev \n\
Version: $(DEB_VERSION) \n\
Architecture: $(DEB_ARCH) \n\
Essential: no \n\
Priority: optional \n\
Depends: libbdf-dev, libboost-iostreams-dev \n\
Maintainer: Josua Robson \n\
Description: Load, save, and modify the structure for binary data format \n\
" > $(DEB_PATH)/DEBIAN/control
	
	printf "\n\
Files: * \n\
Copyright: 2020, Josua Robson \n\
License: MIT \n\
" > $(DEB_PATH)/DEBIAN/copyright
	
	cp *.h $(DEB_PATH)/usr/include/bdf
	cp $(OUT) $(DEB_PATH)/usr/lib
	cd $(DEB_PATH)/usr/lib && ln -s $(OUT) libbdf.so || true
	find ./tools \( ! -name "*.cpp" \) \( ! -name "Makefile" \) -type f -exec cp {} $(DEB_PATH)/usr/bin ";"
	chmod -R 755 $(DEB_PATH)
	dpkg-deb --build $(DEB_PATH)

clean:
	cd tools && make clean
	rm -r $(DEB_PATH) || true
	rm *.o *.so* $(DEB_PATH).deb || true

install: build
	cd tools && make install
	cp $(OUT) /usr/lib
	[ -d /usr/lib/libbdf.so ] && rm /usr/lib/libbdf.so || true
	ln -s /usr/lib/$(OUT) /usr/lib/libbdf.so
	chmod 0755 /usr/lib/$(OUT)
	chmod 0755 /usr/lib/libbdf.so
	[ -d "/usr/include/bdf" ] || mkdir /usr/include/bdf
	cp *.h /usr/include/bdf
	ldconfig

uninstall:
	cd tools && make uninstall
	[ -d /usr/lib/$(OUT) ] && rm /usr/lib/$(OUT) || true
	[ -d /usr/lib/$(OUT) ] && rm /usr/lib/libbdf.so || true
	[ -d /usr/lib/$(OUT) ] && rm -r /usr/include/bdf || true
	ldconfig

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CARGS)
