CC=g++
CARGS=-O3 -Wall -Werror -fpic -DIS_LITTLE_ENDIAN=1
CFLAGS=BdfArray.h BdfNamedList.h BdfObject.h BdfTypes.h BdfIndent.h BdfReader.h BdfLookupTable.h BdfHelpers.h BdfError.h BdfStringReader.h BdfReaderHuman.h Bdf.h
COMPILE=BdfArray.o BdfNamedList.o BdfObject.o BdfIndent.o BdfLookupTable.o BdfReader.o BdfHelpers.o BdfError.o BdfStringReader.o BdfReaderHuman.o
OUT=libbdf.so.0

DEB_VERSION=1.0
DEB_ARCH=amd64

DEB_PATH=libbdf-dev_$(DEB_VERSION)_$(DEB_ARCH)
DEB_PATH_CONTROL=$(DEB_PATH)/DEBIAN/control
DEB_PATH_COPYRIGHT=$(DEB_PATH)/DEBIAN/copyright

build: $(COMPILE) $(CFLAGS)
	$(CC) $(COMPILE) -shared -o $(OUT) $(CARGS)
	[ -d libbdf.so ] || ln -s libbdf.so.0 libbdf.so

build-deb: build
	cd tools && make
	[ -d $(DEB_PATH) ] || mkdir $(DEB_PATH)
	[ -d $(DEB_PATH)/DEBIAN ] || mkdir $(DEB_PATH)/DEBIAN
	[ -d $(DEB_PATH)/usr ] || mkdir $(DEB_PATH)/usr
	[ -d $(DEB_PATH)/usr/lib ] || mkdir $(DEB_PATH)/usr/lib
	[ -d $(DEB_PATH)/usr/include ] || mkdir $(DEB_PATH)/usr/include
	[ -d $(DEB_PATH)/usr/include/bdf ] || mkdir $(DEB_PATH)/usr/include/bdf
	[ -d $(DEB_PATH)/usr/bin ] || mkdir $(DEB_PATH)/usr/bin
	
	echo "Package: libbdf-dev" > $(DEB_PATH_CONTROL)
	echo "Version: $(DEB_VERSION)" >> $(DEB_PATH_CONTROL)
	echo "Architecture: $(DEB_ARCH)"  >> $(DEB_PATH_CONTROL)
	echo "Essential: no" >> $(DEB_PATH_CONTROL)
	echo "Priority: optional" >> $(DEB_PATH_CONTROL)
	echo "Depends: libbdf-dev, libboost-iostreams-dev" >> $(DEB_PATH_CONTROL)
	echo "Maintainer: Josua Robson" >> $(DEB_PATH_CONTROL)
	echo "Description: Used to read and write binary data format" >> $(DEB_PATH_CONTROL)
	
	echo "Files: *" > $(DEB_PATH_COPYRIGHT)
	echo "Copyright: 2020, Josua Robson" >> $(DEB_PATH_COPYRIGHT)
	echo "License: MIT" >> $(DEB_PATH_COPYRIGHT)
	
	cp *.h $(DEB_PATH)/usr/include/bdf
	cp $(OUT) $(DEB_PATH)/usr/lib
	[ -d $(DEB_PATH)/usr/lib/libbdf.so.0 ] || cd $(DEB_PATH)/usr/lib && ln -s $(OUT) libbdf.so
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
