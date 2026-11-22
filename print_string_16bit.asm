bits 16

print_string:
    pusha
    mov si, bx

print_string_loop:
    lodsb
    test al, al
    jz print_string_done

    push si
    xor bh, bh
    mov ah, 0x0e
    int 0x10
    pop si

    jmp print_string_loop

print_string_done:
    popa
    ret