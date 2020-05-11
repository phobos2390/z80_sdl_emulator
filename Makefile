assembled_bin = build/main_z80bin

emulator: 
	mkdir -p build
	cd build; cmake3 ..
	cd build; make

run: emulator assemble
	build/z80_emulator $(assembled_bin)

valgrind: emulator assemble
	valgrind build/z80_emulator $(assembled_bin)

assemble: emulator
	spasm -I src/assembler src/assembler/main.asm $(assembled_bin)


#build:
#	mkdir -p build
#	cd build; cmake ..


clean: 
	rm -rf build src/emulator/emulator_constants.h src/assembler/assembler_constants.asm
