all:
	@echo "test, lib, clean"

test: lib 
	build/test_pz80e
	
lib:
	@mkdir -p build
	@cd build; cmake ..
	@cd build; make

clean:
	@rm -rf build
