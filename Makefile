SRC_DIR         := src
OUTPUT_DIR      := build
TOOL_DIR        := ${HOME}/opt/cross
BINUTILS_DIR    := ${TOOL_DIR}/bin
TARGET          := x86_64-elf
TARGET32        := i686-elf
AS32            := ${BINUTILS_DIR}/${TARGET32}-as
LD32            := ${BINUTILS_DIR}/${TARGET32}-ld
CC32            := ${BINUTILS_DIR}/${TARGET32}-gcc

ASFLAGS         := -g
LDFLAGS         := -nostdlib
BOOT_CFLAGS     := -nostdlib -ffreestanding -lgcc -T boot.ld
WARNINGS        := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
                   -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
                   -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
                   -Wconversion -Wstrict-prototypes
CFLAGS          := ${WARNINGS} -g -ffreestanding -mno-red-zone -c

STAGE1_OBJ_LIST := ${OUTPUT_DIR}/stage1.o
CRTI_OBJ        := ${OUTPUT_DIR}/crti.o
CRTBEGIN_OBJ    := $(shell ${CC32} ${CFLAGS} -print-file-name=crtbegin.o)
CRTEND_OBJ      := $(shell ${CC32} ${CFLAGS} -print-file-name=crtend.o)
CRTN_OBJ        := ${OUTPUT_DIR}/crtn.o
STAGE2_OBJ_LIST := ${OUTPUT_DIR}/stage2.o ${CRTI_OBJ} ${CRTBEGIN_OBJ} ${OUTPUT_DIR}/stage2_c.o ${CRTEND_OBJ} ${CRTN_OBJ}

PROJDIRS        := ${SRC_DIR}
SRCFILES        := $(wildcard $(PROJDIRS)/*.c)

.PHONY: all dir assemble crt_assemble compile link clean

all: clean dir assemble crt_assemble compile link disk

dir:
	mkdir -p ${OUTPUT_DIR}

assemble: dir
	${AS32} ${SRC_DIR}/stage1.s -o ${OUTPUT_DIR}/stage1.o ${ASFLAGS}
	${AS32} ${SRC_DIR}/stage2.s -o ${OUTPUT_DIR}/stage2.o ${ASFLAGS}

crt_assemble:
	${AS32} ${SRC_DIR}/crti.s -o ${CRTI_OBJ} ${ASFLAGS}
	${AS32} ${SRC_DIR}/crtn.s -o ${CRTN_OBJ} ${ASFLAGS}

compile:
	${CC32} ${SRCFILES} ${CFLAGS} -o ${OUTPUT_DIR}/stage2_c.o

link: assemble crt_assemble compile
	${CC32} ${STAGE1_OBJ_LIST} ${STAGE2_OBJ_LIST} -o ${OUTPUT_DIR}/boot.elf ${BOOT_CFLAGS}
	objcopy -O binary ${OUTPUT_DIR}/boot.elf ${OUTPUT_DIR}/boot.bin

disk: link
	dd if=${OUTPUT_DIR}/boot.bin of=${OUTPUT_DIR}/boot.disk

clean:
	rm -rf build