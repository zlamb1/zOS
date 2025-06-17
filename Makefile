OUTDIR := build

.PHONY: all clean boot_all

all: boot_all

clean:
	rm -rf ${OUTDIR}

define GETPATH
${dir ${lastword ${MAKEFILE_LIST}}}
endef

define MK_S_OBJ
${1}: ${patsubst ${OUTDIR}/%.o,%.S,${1}} | ${2}
	${CC} ${CFLAGS} ${patsubst ${OUTDIR}/%.o,%.S,${1}} -o ${1}
endef

define MK_C_OBJ
${1}: ${patsubst ${OUTDIR}/%.o,%.c,${1}} | ${2}
	${CC} ${CFLAGS} ${patsubst ${OUTDIR}/%.o,%.c,${1}} -o ${1}
endef

include src/boot/Makefile
