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

void user_main(void) {
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