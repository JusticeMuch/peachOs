section .asm

extern init21h_handler
extern no_interrupt_handler
global no_interrupt
global init21h
global idt_load

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

init21h:
    cli
    pushad
    call init21h_handler
    popad
    sti 
    iret

no_interrupt:
    cli
    pushad
    call no_interrupt_handler
    popad
    sti 
    iret