BOOTBIN := ${OUTDIR}/boot.bin
BOOTELF := ${OUTDIR}/boot.elf
CFLAGS := -g -c -m32 -fno-pie -nostdlib -ffreestanding -fno-strict-aliasing -mno-red-zone -I${GETPATH}include
LDFLAGS := -m32 -nostdlib -ffreestanding -fno-strict-aliasing -lgcc -Wl,--no-warn-rwx-segments,--build-id=none,-z,noexecstack
KSRCDIR := ${GETPATH}kern
KSRCFILES := ${wildcard ${KSRCDIR}/*.c}
KOBJFILES := ${patsubst %.c,${OUTDIR}/%.o,${KSRCFILES}}
KOUTDIR := ${OUTDIR}/${KSRCDIR}

OBJFILES += ${KOBJFILES}

boot_all: machine_all ${KOBJFILES} ${BOOTBIN}

${foreach OBJFILE,${KOBJFILES},${eval ${call MK_C_OBJ,${OBJFILE},${KOUTDIR}}}}

${KOUTDIR}:
	mkdir -p ${KOUTDIR}

include ${GETPATH}pc/Makefile

${BOOTBIN}: ${BOOTELF}
	objcopy -O binary ${BOOTELF} ${BOOTBIN}

${BOOTELF}: ${OBJFILES} ${BOOTLD}
	${CC} ${LDFLAGS} ${OBJFILES} -T ${BOOTLD} -o ${BOOTELF}