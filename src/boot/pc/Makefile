ARCHDIR := pc
CFLAGS := -g -c -nostdlib -ffreestanding -fno-strict-aliasing -mno-red-zone
LDFLAGS := -nostdlib -ffreestanding -fno-strict-aliasing -lgcc -Wl,--no-warn-rwx-segments,--build-id=none
SRCDIR := src/boot
SRCFILES := ${wildcard ${SRCDIR}/${ARCHDIR}/*.S}
OBJFILES := ${patsubst ${SRCDIR}/${ARCHDIR}/%.S,${OUTDIR}/${SRCDIR}/%.o,${SRCFILES}}
BOOTBIN := ${OUTDIR}/boot.bin
BOOTELF := ${OUTDIR}/boot.elf
BOOTLD := ${SRCDIR}/${ARCHDIR}/boot.ld

boot_all: ${BOOTBIN}

${BOOTBIN}: ${BOOTELF}
	objcopy -O binary ${BOOTELF} ${BOOTBIN}

${BOOTELF}: ${OBJFILES}
	${CC} ${LDFLAGS} ${OBJFILES} -T ${BOOTLD} -o ${BOOTELF}

${OUTDIR}/${SRCDIR}/%.o: ${SRCDIR}/${ARCHDIR}/%.S | ${OUTDIR}/${SRCDIR}
	${CC} ${CFLAGS} $< -o $@

${OUTDIR}/${SRCDIR}:
	mkdir -p ${OUTDIR}/${SRCDIR}