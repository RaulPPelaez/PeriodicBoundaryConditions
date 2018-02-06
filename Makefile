
all:
	$(MAKE) -C src/
	mkdir -p bin
	mv src/pbc bin
clean:
	rm -rf bin
	$(MAKE) -C src/ clean
