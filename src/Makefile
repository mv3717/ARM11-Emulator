all:
	(mkdir -p output)
	(cd assembler; make)
	(cd emulator; make)
	(cd output; ./assemble ../../program/gpio.s kernel.img)

clean:
	(cd assembler; make clean)
	(cd emulator; make clean)
	(cd output; rm *)
	(rmdir output)
