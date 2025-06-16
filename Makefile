OUTDIR := build

.PHONY: all clean boot_all

all: boot_all

clean:
	rm -rf ${OUTDIR}

include src/boot/pc/Makefile
