#ifndef KERNEL_H
#define KERNEL_H

extern int cursorX;
extern int cursorY;

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
void messageout(const char* message);
void setVariable(const char* name, long value);
long sum(int a, int b);
long subtract(int a, int b);
long multiply(int a, int b);
long divide(int a, int b);
long getVariable(const char* name);




#endif
