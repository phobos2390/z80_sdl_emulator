all:
	@echo "test, lib, clean"

test: lib 
	build/test_pz80e

valgrind: lib
	valgrind build/test_pz80e
	
lib:
	@mkdir -p build
	@cd build; cmake3 ..
	@cd build; make

clean:
	@rm -rf build
