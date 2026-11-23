[org 0x7c00]
[bits 16]

%ifndef KERNEL_SECTORS
    %define KERNEL_SECTORS 10
%endif

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    mov [boot_drive], dl

    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb boot_error
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz boot_error

    xor ax, ax
    mov ds, ax
    mov es, ax

    mov dl, [boot_drive]
    mov ah, 0x41
    mov bx, 0x55aa
    int 0x13
    jc read_kernel_chs
    cmp bx, 0xaa55
    jne read_kernel_chs
    test cx, 1
    jz read_kernel_chs

    mov si, disk_address_packet
    mov ah, 0x42
    mov dl, [boot_drive]
    int 0x13
    jnc kernel_loaded

read_kernel_chs:
    xor ax, ax
    mov es, ax
    mov bx, 0x9000
    mov byte [chs_sector], 2
    mov byte [chs_head], 0
    mov byte [chs_cylinder], 0
    mov word [chs_remaining], KERNEL_SECTORS
.read_sector:
    mov ah, 0x02
    mov al, 1
    mov ch, [chs_cylinder]
    mov cl, [chs_sector]
    mov dh, [chs_head]
    mov dl, [boot_drive]
    push bx
    int 0x13
    pop bx
    jc boot_error

    add bx, 512
    inc byte [chs_sector]
    cmp byte [chs_sector], 19
    jb .next_sector
    mov byte [chs_sector], 1
    inc byte [chs_head]
    cmp byte [chs_head], 2
    jb .next_sector
    mov byte [chs_head], 0
    inc byte [chs_cylinder]
.next_sector:
    dec word [chs_remaining]
    jnz .read_sector

kernel_loaded:

    xor ax, ax
    mov ds, ax
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

boot_error:
    xor ax, ax
    mov ds, ax
    mov si, boot_error_message
.print:
    lodsb
    test al, al
    jz .halt_start
    mov ah, 0x0e
    int 0x10
    jmp .print
.halt_start:
    cli
.halt:
    hlt
    jmp .halt

boot_drive:
    db 0

boot_error_message:
    db "Boot error", 0

chs_sector:
    db 0
chs_head:
    db 0
chs_cylinder:
    db 0
chs_remaining:
    dw 0

align 4
disk_address_packet:
    db 0x10, 0
    dw KERNEL_SECTORS
    dw 0x9000
    dw 0
    dq 1

align 8
gdt_start:
    dq 0
    dq 0x00cf9a000000ffff
    dq 0x00cf92000000ffff
    dq 0x00af9a000000ffff
    dq 0x00cf92000000ffff
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000

    mov edi, 0x1000
    xor eax, eax
    mov ecx, 3072
    rep stosd

    mov dword [0x1000], 0x2003
    mov dword [0x2000], 0x3003
    mov dword [0x3000], 0x0083

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    mov eax, 0x1000
    mov cr3, eax

    mov ecx, 0xc0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    jmp 0x18:long_mode

[bits 64]
long_mode:
    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, 0x80008
    mov rax, 0x9000
    jmp rax

times 510 - ($ - $$) db 0
dw 0xaa55
