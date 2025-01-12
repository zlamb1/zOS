#include "pic.h"

#include "io.h"

void pic_init()
{
    // start init
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    // remap pic
    io_wait();
    // first entry = 32nd entry in IDT
    outb(PIC1_DATA, 0x20);
    io_wait();
    // first entry = 40th entry in IDT
    outb(PIC2_DATA, 0x28);
    io_wait();
    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC1_DATA, 4); 
	io_wait();
    // ICW3: tell Slave PIC its cascade identity (0000 0010)
	outb(PIC2_DATA, 2); 
	io_wait();
    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    outb(PIC1_DATA, ICW4_8086); 
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
    // enable kb int
    outb(PIC1_DATA, 0xFD);
    // disable slave
    outb(PIC2_DATA, 0xFF);
}

void pic_disable() 
{
    // mask PIC 
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}