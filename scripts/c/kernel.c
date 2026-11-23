
#include "kernel.h"

static inline void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

unsigned char inb(unsigned short port) {
    unsigned char value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void move_cursor(unsigned short position) {
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)(position >> 8));
}

static unsigned short get_cursor_position(void) {
    outb(0x3D4, 0x0F);
    unsigned short position = inb(0x3D5);
    outb(0x3D4, 0x0E);
    position |= (unsigned short)inb(0x3D5) << 8;
    return position;
}

int getcolor(char color) {
    switch(color) {
        case 'r':
            return 0x04;
        case 'g':
            return 0x02;
        case 'b':
            return 0x01;
        case 'y':
            return 0x06;
        case 'm':
            return 0x05;
        case 'c':
            return 0x03;
        case 'w':
            return 0x07;
        default:
            return 0x07; // Default to white if unknown color

    }
}

void set_cursor(char character) {
    unsigned short position = get_cursor_position();
    if (position >= 80 * 25) {
        position = 0;
    }

    volatile unsigned char *video_memory =
        (volatile unsigned char *)0xB8000;
    video_memory[position * 2] = (unsigned char)character;
}

char get_screen_character(unsigned short position) {
    if (position >= 80 * 25) {
        return '\0';
    }

    volatile unsigned char *video_memory =
        (volatile unsigned char *)0xB8000;
    return (char)video_memory[position * 2];
}

char get_cursor(void) {
    return get_screen_character(get_cursor_position());
}



void user_function() {
    user_main();
}









__attribute__((section(".text.kernel_entry")))
void kernel_main(void) {
    volatile unsigned char *video_memory =
        (volatile unsigned char *)0xB8000;

        
    for (int i = 0; i < 2000; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = getcolor('w');
    }

    user_function();

    for (;;) {
        keyboard_poll();
    }
}






