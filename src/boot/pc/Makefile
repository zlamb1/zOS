MSRCDIR := ${GETPATH}
MSRCFILES := ${wildcard ${MSRCDIR}*.S}
MOBJFILES := ${patsubst %.S,${OUTDIR}/%.o,${MSRCFILES}}
MOUTDIR := ${OUTDIR}/${MSRCDIR}
BOOTLD := ${MSRCDIR}boot.ld

OBJFILES += ${MOBJFILES}

machine_all: ${MOBJFILES}

${foreach OBJFILE,${MOBJFILES},${eval ${call MK_S_OBJ,${OBJFILE},${MOUTDIR}}}}

${MOUTDIR}:
	mkdir -p ${MOUTDIR}