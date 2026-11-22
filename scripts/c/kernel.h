#ifndef KERNEL_H
#define KERNEL_H

void move_cursor(unsigned short position);
void set_cursor(char character);
char get_cursor(void);
int getcolor(char color);
unsigned char inb(unsigned short port);
void keyboard_poll(void);
void key_pressed(char key);
void enter_pressed(void);
void backspace_pressed(void);
void user_main(void);

#endif
