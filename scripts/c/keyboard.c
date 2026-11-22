#include "kernel.h"

static unsigned char shift_state;
static unsigned char caps_lock;
static unsigned char extended_scancode;

static const char normal_map[128] = {
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0', [0x0C] = '-', [0x0D] = '=',
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e', [0x13] = 'r',
    [0x14] = 't', [0x15] = 'y', [0x16] = 'u', [0x17] = 'i',
    [0x18] = 'o', [0x19] = 'p', [0x1A] = '[', [0x1B] = ']',
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x21] = 'f',
    [0x22] = 'g', [0x23] = 'h', [0x24] = 'j', [0x25] = 'k',
    [0x26] = 'l', [0x27] = ';', [0x28] = '\'', [0x2B] = '\\',
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v',
    [0x30] = 'b', [0x31] = 'n', [0x32] = 'm', [0x33] = ',',
    [0x34] = '.', [0x35] = '/', [0x39] = ' '
};

static const char shifted_map[128] = {
    [0x02] = '!', [0x03] = '@', [0x04] = '#', [0x05] = '$',
    [0x06] = '%', [0x07] = '^', [0x08] = '&', [0x09] = '*',
    [0x0A] = '(', [0x0B] = ')', [0x0C] = '_', [0x0D] = '+',
    [0x10] = 'Q', [0x11] = 'W', [0x12] = 'E', [0x13] = 'R',
    [0x14] = 'T', [0x15] = 'Y', [0x16] = 'U', [0x17] = 'I',
    [0x18] = 'O', [0x19] = 'P', [0x1A] = '{', [0x1B] = '}',
    [0x1E] = 'A', [0x1F] = 'S', [0x20] = 'D', [0x21] = 'F',
    [0x22] = 'G', [0x23] = 'H', [0x24] = 'J', [0x25] = 'K',
    [0x26] = 'L', [0x27] = ':', [0x28] = '"', [0x2B] = '|',
    [0x2C] = 'Z', [0x2D] = 'X', [0x2E] = 'C', [0x2F] = 'V',
    [0x30] = 'B', [0x31] = 'N', [0x32] = 'M', [0x33] = '<',
    [0x34] = '>', [0x35] = '?'
};

static char translate_scancode(unsigned char scancode) {
    char key = normal_map[scancode];
    if (key == 0) {
        return 0;
    }

    if (key >= 'a' && key <= 'z') {
        if ((shift_state != 0) != (caps_lock != 0)) {
            key = (char)(key - 'a' + 'A');
        }
    } else if (shift_state != 0 && shifted_map[scancode] != 0) {
        key = shifted_map[scancode];
    }

    return key;
}

void keyboard_poll(void) {
    if ((inb(0x64) & 1) == 0) {
        return;
    }

    unsigned char scancode = inb(0x60);

    if (scancode == 0xE0) {
        extended_scancode = 1;
        return;
    }

    if (extended_scancode) {
        extended_scancode = 0;
        if (scancode == 0x1C) {
            enter_pressed();
        }
        return;
    }

    unsigned char keycode = scancode & 0x7F;
    unsigned char released = scancode & 0x80;

    if (keycode == 0x2A) {
        shift_state = released ? (shift_state & ~1) : (shift_state | 1);
        return;
    }

    if (keycode == 0x36) {
        shift_state = released ? (shift_state & ~2) : (shift_state | 2);
        return;
    }

    if (released) {
        return;
    }

    if (keycode == 0x3A) {
        caps_lock = !caps_lock;
        return;
    }

    if (keycode == 0x1C) {
        enter_pressed();
        return;
    }

    if (keycode == 0x0E) {
        backspace_pressed();
        return;
    }

    char key = translate_scancode(keycode);
    if (key != 0) {
        key_pressed(key);
    }
}
