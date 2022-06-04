build: bin compile
bin:
	mkdir --parent bin
compile:
	gcc -o bin/signfinder src/*.c -Wall -Wpedantic -lwtsapi32 -lkernel32 -ladvapi32 -static -static-libgcc
installdir:
	mkdir -p /usr/local/bin
install: build installdir
	cp ./bin/signfinder.exe /usr/local/bin/

.PHONY: build compile installdir install
