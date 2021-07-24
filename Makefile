CC=g++-10
CARGS=-g -Wall -Werror -fpic
CFLAGS=BdfList.hpp BdfNamedList.hpp BdfObject.hpp BdfTypes.hpp BdfIndent.hpp BdfReader.hpp BdfLookupTable.hpp BdfHelpers.hpp BdfError.hpp BdfStringReader.hpp BdfReaderHuman.hpp Bdf.hpp
COMPILE=BdfList.o BdfNamedList.o BdfObject.o BdfIndent.o BdfLookupTable.o BdfReader.o BdfHelpers.o BdfError.o BdfStringReader.o BdfReaderHuman.o
OUT=libbdf.so.0

BIN_VERSION=1.4
BIN_ARCH=amd64

DEB_PATH=libbdf-dev_$(BIN_VERSION)_$(BIN_ARCH)
DEB_PATH_CONTROL=$(DEB_PATH)/DEBIAN/control
DEB_PATH_COPYRIGHT=$(DEB_PATH)/DEBIAN/copyright

build: $(COMPILE) $(CFLAGS)
	$(CC) $(COMPILE) -shared -o $(OUT) $(CARGS)
	ln -s $(OUT) libbdf.so || true
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
Depends: libbdf-dev, libboost-iostreams1.71.0 \n\
Maintainer: Josua Robson \n\
Description: Load, save, and modify the structure for binary data format \n\
" > $(DEB_PATH)/DEBIAN/control
	
	printf "\n\
Files: * \n\
Copyright: 2020, Josua Robson \n\
License: MIT \n\
" > $(DEB_PATH)/DEBIAN/copyright
	
	cp *.hpp $(DEB_PATH)/usr/include/bdf
	cp $(OUT) $(DEB_PATH)/usr/lib
	cd $(DEB_PATH)/usr/lib && ln -s $(OUT) libbdf.so || true
	find ./tools \( ! -name "*.cpp" \) \( ! -name "Makefile" \) -type f -exec cp {} $(DEB_PATH)/usr/bin ";"
	chmod -R 755 $(DEB_PATH)
	dpkg-deb --build $(DEB_PATH)

clean:
	cd tools && make clean
	rm -rf $(DEB_PATH) $(PAC_PATH) *.o *.so* *.deb *.pkg.tar.zstd || true

install: build
	cd tools && make install
	cp $(OUT) /usr/lib
	[ -L /usr/lib/libbdf.so ] && rm -f /usr/lib/libbdf.so || true
	ln -s /usr/lib/$(OUT) /usr/lib/libbdf.so
	chmod 0755 /usr/lib/$(OUT)
	chmod 0755 /usr/lib/libbdf.so
	[ -d "/usr/include/bdf" ] || mkdir /usr/include/bdf
	cp *.hpp /usr/include/bdf
	ldconfig

uninstall:
	cd tools && make uninstall
	[ -e /usr/lib/$(OUT) ] && rm -f /usr/lib/$(OUT) || true
	[ -L /usr/lib/libbdf.so ] && rm -f /usr/lib/libbdf.so || true
	[ -d /usr/include/bdf ] && rm -rf /usr/include/bdf || true
	ldconfig

%.o: %.cpp $(DEPS)
	$(CC) $(CARGS) -c -o $@ $<
