.section .stage2.utility, "ax"

.global random

random:
    rdrand %eax
    jnc random
    ret