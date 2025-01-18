SRC_DIR         := src
OUTPUT_DIR      := build
OBJ_OUTPUT_DIR  := ${OUTPUT_DIR}/obj
TOOL_DIR        := ${HOME}/opt/cross
BINUTILS_DIR    := ${TOOL_DIR}/bin
TARGET          := x86_64-elf
TARGET32        := i686-elf
AS32            := ${BINUTILS_DIR}/${TARGET32}-as
LD32            := ${BINUTILS_DIR}/${TARGET32}-ld
CC32            := ${BINUTILS_DIR}/${TARGET32}-gcc

INCLUDEDIRS     := include ${shell find drivers -type d -name include}
INCLUDEFLAGS    := ${foreach dir,${INCLUDEDIRS},-I${dir}}

ASFLAGS         := -nostdlib -ffreestanding -g -c ${INCLUDEFLAGS}
LDFLAGS         := -nostdlib -ffreestanding -fno-strict-aliasing -lgcc -Wl,--no-warn-rwx-segments -g
WARNINGS        := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
                   -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
                   -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
                   -Wconversion -Wstrict-prototypes
CFLAGS          := ${WARNINGS} ${INCLUDEFLAGS} -g -ffreestanding -fno-strict-aliasing -mno-red-zone

STAGE1_OBJ_LIST := ${OBJ_OUTPUT_DIR}/stage1.o

CRTI_OBJ        := ${OBJ_OUTPUT_DIR}/crti.o
CRTBEGIN_OBJ    := ${shell ${CC32} ${CFLAGS} -print-file-name=crtbegin.o}
CRTEND_OBJ      := ${shell ${CC32} ${CFLAGS} -print-file-name=crtend.o}
CRTN_OBJ        := ${OBJ_OUTPUT_DIR}/crtn.o

PROJDIRS        := ${SRC_DIR} ${shell find drivers -type d -name src}
SRCFILES        := ${strip ${foreach dir,${PROJDIRS},${wildcard ${dir}/*.c}}}
HDRFILES        := ${wildcard ${INCLUDE_DIR}/*.h}
OBJFILES        := ${patsubst %.c,${OBJ_OUTPUT_DIR}/%.o, ${SRCFILES}}

STAGE2_OBJ_LIST := ${OBJ_OUTPUT_DIR}/stage2.o ${CRTI_OBJ} ${CRTBEGIN_OBJ} ${OBJFILES} ${CRTEND_OBJ} ${CRTN_OBJ}

.PHONY: all assemble link usb clean

all: assemble link disk

${OUTPUT_DIR}:
	mkdir -p ${OUTPUT_DIR}

${OBJ_OUTPUT_DIR}:
	mkdir -p ${OBJ_OUTPUT_DIR}

assemble: ${OBJ_OUTPUT_DIR}
	${CC32} ${SRC_DIR}/stage1.S -o ${OBJ_OUTPUT_DIR}/stage1.o ${ASFLAGS}
	${CC32} ${SRC_DIR}/stage2.S -o ${OBJ_OUTPUT_DIR}/stage2.o ${ASFLAGS}
	${CC32} ${SRC_DIR}/crti.S -o ${CRTI_OBJ} ${ASFLAGS}
	${CC32} ${SRC_DIR}/crtn.S -o ${CRTN_OBJ} ${ASFLAGS}

define OBJ_RULE
${OBJ_OUTPUT_DIR}/${patsubst %.c,%.o,${1}}: ${1}
	mkdir -p $${dir $$@}
	${CC32} ${CFLAGS} -c $$< -o $$@
endef

${foreach srcpath,${SRCFILES},${eval ${call OBJ_RULE,${srcpath}}}}

link: assemble boot.ld ${SRCFILES} ${OBJFILES}
	${CC32} ${STAGE1_OBJ_LIST} ${STAGE2_OBJ_LIST} -o ${OUTPUT_DIR}/boot.elf ${LDFLAGS} -T boot.ld 
	objcopy -O binary ${OUTPUT_DIR}/boot.elf ${OUTPUT_DIR}/boot.bin

disk: link
	dd if=${OUTPUT_DIR}/boot.bin of=${OUTPUT_DIR}/boot.disk

usb: disk
	sudo dd if=${OUTPUT_DIR}/boot.disk of=/dev/sdb bs=4M status=progress conv=fsync
	sudo dd if=/dev/sdb of=${OUTPUT_DIR}/verify.disk bs=4M count=1

clean:
	rm -rf build