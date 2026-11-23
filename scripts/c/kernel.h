#ifndef KERNEL_H
#define KERNEL_H

#define COMMAND_MAX_WORDS 40

extern int cursorX;
extern int cursorY;
extern char *commandWords[COMMAND_MAX_WORDS];
extern int commandWordCount;

void move_cursor(unsigned short position);
void set_cursor(char character);
char get_cursor(void);
char get_screen_character(unsigned short position);
int getcolor(char color);
unsigned char inb(unsigned short port);
void keyboard_poll(void);
void key_pressed(char key);
void enter_pressed(void);
void backspace_pressed(void);
void user_main(void);
void proccesCommand(const char input[80]);
void clear_screen(void);
int str_equal(const char* a, const char* b);
void messageout(const char* message);
void setVariable(const char* name, long value);
int tryGetVariable(const char* name, long* value);
long sum(long a, long b);
long subtract(long a, long b);
long multiply(long a, long b);
long divide(long a, long b);
long getVariable(const char* name);




#endif
