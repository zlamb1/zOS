OUTPUT_DIR := build

.PHONY: all dir bootloader_assemble bootloader_link clean

all: clean dir bootloader_assemble bootloader_link disk

dir:
	mkdir -p ${OUTPUT_DIR}

bootloader_assemble: dir
	as defines.s -o ${OUTPUT_DIR}/defines.o -g
	as stage1.s -o ${OUTPUT_DIR}/stage1.o -g
	as stage2.s -o ${OUTPUT_DIR}/stage2.o -g

bootloader_link: bootloader_assemble
	ld ${OUTPUT_DIR}/stage1.o -o ${OUTPUT_DIR}/stage1.bin -nostdlib -T stage1.ld
	ld ${OUTPUT_DIR}/stage2.o -o ${OUTPUT_DIR}/stage2.bin -nostdlib -T stage2.ld
	cat ${OUTPUT_DIR}/stage1.bin ${OUTPUT_DIR}/stage2.bin > ${OUTPUT_DIR}/boot.bin

disk: bootloader_link
	dd if=${OUTPUT_DIR}/boot.bin of=${OUTPUT_DIR}/boot.disk

clean:
	rm -rf build