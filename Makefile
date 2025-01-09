SRC_DIR         := src
INCLUDE_DIR     := include
OUTPUT_DIR      := build
OBJ_OUTPUT_DIR  := ${OUTPUT_DIR}/obj
TOOL_DIR        := ${HOME}/opt/cross
BINUTILS_DIR    := ${TOOL_DIR}/bin
TARGET          := x86_64-elf
TARGET32        := i686-elf
AS32            := ${BINUTILS_DIR}/${TARGET32}-as
LD32            := ${BINUTILS_DIR}/${TARGET32}-ld
CC32            := ${BINUTILS_DIR}/${TARGET32}-gcc

ASFLAGS         := -g
LDFLAGS         := -nostdlib
BOOT_CFLAGS     := -nostdlib -ffreestanding -lgcc -T boot.ld -Wl,--no-warn-rwx-segments -g
WARNINGS        := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
                   -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
                   -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
                   -Wconversion -Wstrict-prototypes
CFLAGS          := ${WARNINGS} -I${INCLUDE_DIR} -g -ffreestanding -mno-red-zone

STAGE1_OBJ_LIST := ${OBJ_OUTPUT_DIR}/stage1.o

CRTI_OBJ        := ${OBJ_OUTPUT_DIR}/crti.o
CRTBEGIN_OBJ    := $(shell ${CC32} ${CFLAGS} -print-file-name=crtbegin.o)
CRTEND_OBJ      := $(shell ${CC32} ${CFLAGS} -print-file-name=crtend.o)
CRTN_OBJ        := ${OBJ_OUTPUT_DIR}/crtn.o

PROJDIRS        := ${SRC_DIR}
SRCFILES        := ${wildcard ${SRC_DIR}/*.c}
HDRFILES        := ${wildcard ${INCLUDE_DIR}/*.h}
OBJFILES        := ${patsubst %.c,${OBJ_OUTPUT_DIR}/%.o, ${notdir ${SRCFILES}}}

STAGE2_OBJ_LIST := ${OBJ_OUTPUT_DIR}/stage2.o ${CRTI_OBJ} ${CRTBEGIN_OBJ} ${OBJFILES} ${CRTEND_OBJ} ${CRTN_OBJ}

.PHONY: all assemble link clean

all: assemble link disk

${OUTPUT_DIR}:
	mkdir -p ${OUTPUT_DIR}

${OBJ_OUTPUT_DIR}:
	mkdir -p ${OBJ_OUTPUT_DIR}

assemble: ${OBJ_OUTPUT_DIR}
	${AS32} ${SRC_DIR}/stage1.s -o ${OBJ_OUTPUT_DIR}/stage1.o ${ASFLAGS}
	${AS32} ${SRC_DIR}/stage2.s -o ${OBJ_OUTPUT_DIR}/stage2.o ${ASFLAGS}
	${AS32} ${SRC_DIR}/crti.s -o ${CRTI_OBJ} ${ASFLAGS}
	${AS32} ${SRC_DIR}/crtn.s -o ${CRTN_OBJ} ${ASFLAGS}

${OBJ_OUTPUT_DIR}/%.o: ${SRC_DIR}/%.c | ${OBJ_OUTPUT_DIR}
	${CC32} ${CFLAGS} -c $< -o $@

link: assemble ${SRCFILES} ${OBJFILES}
	${CC32} ${STAGE1_OBJ_LIST} ${STAGE2_OBJ_LIST} -o ${OUTPUT_DIR}/boot.elf ${BOOT_CFLAGS}
	objcopy -O binary ${OUTPUT_DIR}/boot.elf ${OUTPUT_DIR}/boot.bin

disk: link
	dd if=${OUTPUT_DIR}/boot.bin of=${OUTPUT_DIR}/boot.disk

clean:
	rm -rf build