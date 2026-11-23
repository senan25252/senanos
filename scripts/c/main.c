#include "kernel.h"
#include <limits.h>



int cursorX;
int cursorY;
char command[81];
char *commandWords[COMMAND_MAX_WORDS];
int commandWordCount;

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
    messageout("welcome to senanos \n");
    for (;;) {
        updatecursor();
        keyboard_poll();
    }
}

static int isCommandSeparator(char character) {
    return character == ' ' || character == '\t' ||
           character == '\n' || character == '\r';
}

static int parseLongToken(const char* text, long* value) {
    int index = 0;
    int negative = 0;
    unsigned long magnitude = 0;

    if (text[index] == '-' || text[index] == '+') {
        negative = text[index] == '-';
        index++;
    }
    if (text[index] == '\0') {
        return 0;
    }

    unsigned long limit = negative
        ? (unsigned long)LONG_MAX + 1UL
        : (unsigned long)LONG_MAX;

    for (; text[index] != '\0'; index++) {
        if (text[index] < '0' || text[index] > '9') {
            return 0;
        }

        unsigned long digit = (unsigned long)(text[index] - '0');
        if (magnitude > (limit - digit) / 10UL) {
            return 0;
        }
        magnitude = magnitude * 10UL + digit;
    }

    if (negative) {
        *value = magnitude == (unsigned long)LONG_MAX + 1UL
            ? LONG_MIN
            : -(long)magnitude;
    } else {
        *value = (long)magnitude;
    }
    return 1;
}

static int parseCommandValue(const char* text, long* value) {
    return parseLongToken(text, value) || tryGetVariable(text, value);
}

void proccesCommand(const char input[80]) {
    int inputIndex = 0;
    int inputLength = 0;
    commandWordCount = 0;

    while (inputLength < 80 && input[inputLength] != '\0') {
        command[inputLength] = input[inputLength];
        inputLength++;
    }
    command[inputLength] = '\0';

    while (inputIndex < inputLength) {
        while (inputIndex < inputLength &&
               isCommandSeparator(input[inputIndex])) {
            command[inputIndex] = '\0';
            inputIndex++;
        }

        if (inputIndex >= inputLength) {
            break;
        }

        if (commandWordCount >= COMMAND_MAX_WORDS) {
            break;
        }

        commandWords[commandWordCount++] = &command[inputIndex];
        while (inputIndex < inputLength &&
               !isCommandSeparator(command[inputIndex])) {
            inputIndex++;
        }
        if (inputIndex < inputLength) {
            command[inputIndex++] = '\0';
        }
    }

    if (commandWordCount == 0) {
        return;
    }

    if (str_equal(commandWords[0], "out")) {
        if (commandWordCount < 2) {
            messageout("Kullanim: out <metin>");
            return;
        }

        for (int i = 1; i < commandWordCount; i++) {
            if (i > 1) {
                messageout(" ");
            }
            messageout(commandWords[i]);
        }
        return;
    }

    if(str_equal(commandWords[0], "clear")) {
        clear_screen();
        return;
    }

    if(str_equal(commandWords[0], "sum")) {
        if (commandWordCount < 3) {
            messageout("Kullanim: sum <sayi1> <sayi2>");
            return;
        }

        long a;
        long b;
        if (!parseCommandValue(commandWords[1], &a) ||
            !parseCommandValue(commandWords[2], &b)) {
            messageout("Hata: Sayi veya degisken bulunamadi");
            return;
        }

        long result = sum(a, b);
        messageout_long(result);
        return;
    }

    if(str_equal(commandWords[0], "get")) {
        if (commandWordCount < 2) {
            messageout("Kullanim: get <degisken_adi>");
            return;
        }

        long value;
        if (!tryGetVariable(commandWords[1], &value)) {
            messageout("Hata: Degisken bulunamadi");
            return;
        }
        messageout_long(value);
        return;
    }

    if(str_equal(commandWords[0], "set")) {
        if (commandWordCount < 3) {
            messageout("Kullanim: set <degisken_adi> <deger>");
            return;
        }

        long value;
        if (!parseCommandValue(commandWords[2], &value)) {
            messageout("Hata: Deger sayi veya degisken olmali");
            return;
        }

        setVariable(commandWords[1], value);
        return;
    }

    if(str_equal(commandWords[0], "help")) {
        messageout("Kullanim: <komut> [parametreler]");
        messageout("\n");
        messageout("Komutlar:");
        messageout("\n");
        messageout("  out <metin> - Metni ekrana yazdirir");
        messageout("\n");
        messageout("  clear - Ekrani temizler");
        messageout("\n");
        messageout("  sum <sayi|degisken> <sayi|degisken> - Iki degeri toplar");
        messageout("\n");
        messageout("  get <degisken_adi> - Degiskenin degerini ekrana yazdirir");
        messageout("\n");
        messageout("  set <degisken_adi> <deger> - Degiskeni olusturur veya degerini gunceller");
        return;
    }

    if(str_equal(commandWords[0], "multiply")) {
        if (commandWordCount < 3) {
            messageout("Kullanim: multiply <sayi1> <sayi2>");
            return;
        }

        long a;
        long b;
        if (!parseCommandValue(commandWords[1], &a) ||
            !parseCommandValue(commandWords[2], &b)) {
            messageout("Hata: Sayi veya degisken bulunamadi");
            return;
        }
        long result = multiply(a, b);
        messageout_long(result);
        return;
    }

    if(str_equal(commandWords[0], "divide")) {
        if (commandWordCount < 3) {
            messageout("Kullanim: divide <sayi1> <sayi2>");
            return;
        }

        long a;
        long b;
        if (!parseCommandValue(commandWords[1], &a) ||
            !parseCommandValue(commandWords[2], &b)) {
            messageout("Hata: Sayi veya degisken bulunamadi");
            return;
        }
        if (b == 0) {
            messageout("Hata: Sifira bolme hatasi");
            return;
        }
        long result = divide(a, b);
        messageout_long(result);
        return;
    }

    if(str_equal(commandWords[0], "subtract")) {
        if (commandWordCount < 3) {
            messageout("Kullanim: subtract <sayi1> <sayi2>");
            return;
        }

        long a;
        long b;
        if (!parseCommandValue(commandWords[1], &a) ||
            !parseCommandValue(commandWords[2], &b)) {
            messageout("Hata: Sayi veya degisken bulunamadi");
            return;
        }
        long result = subtract(a, b);
        messageout_long(result);
        return;
    }

    messageout("Bilinmeyen komut");
}

void key_pressed(char key) {
    if (cursorX < 79) {
        set_cursor(key);
        cursorX++;
        updatecursor();
    }
}

void newLine() {
    cursorY++;
    cursorX = 0;
    updatecursor();
}


void enter_pressed(void) {
    char input[80];

    int inputRow = cursorY;
    for (int column = 0; column < 80; column++) {
        input[column] =
            get_screen_character((unsigned short)(inputRow * 80 + column));
    }

    cursorY = inputRow;
    cursorX = 0;
    newLine();
    proccesCommand(input);
    if (commandWordCount > 0 && !str_equal(commandWords[0], "clear")) {
        newLine();
    }
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

    updatecursor();
}