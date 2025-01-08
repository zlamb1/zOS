void loader_main(void);

const char* a = "THIS IS A GLOBAL";

static char c = 't';

void loader_main(void)
{
    // write 'C' to video memory
    char* vmem = (char*) 0xB8000;
    *vmem = c;
    for (;;);
}