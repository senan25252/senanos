#include "kernel.h"



int cursorX;
int cursorY;
char command[79];

void updatecursor() {
    if(cursorX > 79) {
        cursorX = 79;
    }
    else if(cursorX < 0) {
        cursorX = 0;
    }

    if(cursorY > 24) {
        cursorY = 24;
    }
    else if(cursorY < 0) {
        cursorY = 0;
    }

    move_cursor(80 * cursorY + cursorX);
}

static void messageout_long(long value) {
    char text[sizeof(long) * 3 + 2];
    unsigned long magnitude;
    int length = 0;
    int negative = value < 0;

    magnitude = negative ? (unsigned long)(-(value + 1)) + 1UL
                         : (unsigned long)value;

    do {
        text[length++] = (char)('0' + magnitude % 10UL);
        magnitude /= 10UL;
    } while (magnitude != 0);

    if (negative) {
        text[length++] = '-';
    }

    text[length] = '\0';
    for (int left = 0, right = length - 1; left < right; left++, right--) {
        char digit = text[left];
        text[left] = text[right];
        text[right] = digit;
    }

    messageout(text);
}

void user_main(void) {
    setVariable("x", 10);
    messageout_long(getVariable("x"));
    messageout("\n");
    for (;;) {
        updatecursor();
        keyboard_poll();
    }
}

void key_pressed(char key) {
    if(cursorX != 79) {
        set_cursor(key);
        cursorX++;
    }
}

void newLine() {
    cursorY++;
    cursorX = 0;
    updatecursor();
}


void enter_pressed(void) {
    char input[80];
    
    cursorX = 0;

    while (cursorX < 80) {
        updatecursor();
        input[cursorX] = get_cursor();
        cursorX++;
    }
    
    newLine();


    cursorX = 0;

    while (cursorX < 79) {
        set_cursor(input[cursorX]);
        cursorX++;
        updatecursor();
    }


    
    newLine();

}

void backspace_pressed(void) {
    if (cursorX > 0) {
        cursorX--;
        updatecursor();
        set_cursor(' ');
        return;
    }

    if (cursorY <= 0) {
        return;
    }

    cursorY--;

    int column;
    for (column = 79; column >= 0; column--) {
        cursorX = column;
        updatecursor();

        char character = get_cursor();
        if (character != ' ' && character != '\0') {
            break;
        }
    }

    cursorX = column < 0 ? 0 : column + 1;
    if (cursorX == 80) {
        if (cursorY < 24) {
            cursorX = 0;
            cursorY++;
        } else {
            cursorX = 79;
        }
    }

    updatecursor();
}