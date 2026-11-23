#include "kernel.h"

#define MAX_VARS 64
#define MAX_NAME_LEN 16

typedef struct {
    char name[MAX_NAME_LEN];
    long value;
    int is_used; // Bu slot dolu mu boş mu?
} Variable;

Variable var_table[MAX_VARS];

void messageout(const char* message) {
    for (int i = 0; message[i] != '\0'; i++) {
        if (message[i] == '\n') {
            cursorX = 0;
            if (cursorY < 24) {
                cursorY++;
            }
            continue;
        }

        move_cursor((unsigned short)(80 * cursorY + cursorX));
        set_cursor(message[i]);

        if (cursorX < 79) {
            cursorX++;
        } else if (cursorY < 24) {
            cursorX = 0;
            cursorY++;
        }
    }

    move_cursor((unsigned short)(80 * cursorY + cursorX));
}

void clear_screen() {
    cursorX = 0;
    cursorY = 0;
    
    while(cursorY < 25) {
        while(cursorX < 80) {
            set_cursor(' ');
            cursorX++;
        }
        cursorX = 0;
        cursorY++;
    }

    cursorX = 0;
    cursorY = 0;
    


}

long sum(int a, int b) {
    return a + b;
}

long subtract(int a, int b) {
    return a - b;
}

long multiply(int a, int b) {
    return a * b;
}

long divide(int a, int b) {
    if (b == 0) {
        return 0; // Handle division by zero
    }
    return a / b;
}

//bunu yapacagiz
// nasil yapacagima dair en ufak bi fikrim yok
/*
    simdi soyle
    os icinde calisacak basit bi yazilim dili yapmak istiyom
    uygulama calistirmak icin
    onun icin gerekli fonksiyonlari yaziyorum
*/

int str_equal(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

// Basit string kopyalama
void str_copy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0' && i < MAX_NAME_LEN - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Değişken oluşturur veya varsa değerini günceller
void setVariable(const char* name, long value) {
    // 1. Önce bu isimde değişken zaten var mı kontrol et
    for (int i = 0; i < MAX_VARS; i++) {
        if (var_table[i].is_used && str_equal(var_table[i].name, name)) {
            var_table[i].value = value;
            return;
        }
    }
    // 2. Yoksa boş bir yer bul ve oluştur
    for (int i = 0; i < MAX_VARS; i++) {
        if (!var_table[i].is_used) {
            str_copy(var_table[i].name, name);
            var_table[i].value = value;
            var_table[i].is_used = 1;
            return;
        }
    }
    messageout("Hata: Maksimum degisken limitine ulasildi!\n");
}

// Değişkenin değerini okur
long getVariable(const char* name) {
    for (int i = 0; i < MAX_VARS; i++) {
        if (var_table[i].is_used && str_equal(var_table[i].name, name)) {
            return var_table[i].value;
        }
    }
    return 0; // Bulunamazsa varsayılan 0
}

