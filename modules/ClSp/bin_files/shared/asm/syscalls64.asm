section .text
global spoofcall

spoofcall:

    pop     rax                             ; Real return address in rax

    mov     r10, rdi                        ; Store OG rdi in r10
    mov     r11, rsi                        ; Store OG rsi in r11

    mov     rdi, [rsp + 32]                 ; Storing struct in the rdi
    mov     rsi, [rsp + 40]                 ; Storing function to call

    ; ---------------------------------------------------------------------
    ; Storing our original registers
    ; ---------------------------------------------------------------------

    mov     [rdi + 24], r10                 ; Storing OG rdi into param
    mov     [rdi + 88], r11                 ; Storing OG rsi into param
    mov     [rdi + 96], r12                 ; Storing OG r12 into param
    mov     [rdi + 104], r13                ; Storing OG r13 into param
    mov     [rdi + 112], r14                ; Storing OG r14 into param
    mov     [rdi + 120], r15                ; Storing OG r15 into param

    mov     r12, rax                        ; OG code used r12 for ret addr

    ; ---------------------------------------------------------------------
    ; Prepping to move stack args
    ; ---------------------------------------------------------------------

    xor     r11, r11                        ; r11 will hold the # of args that have been "pushed"
    mov     r13, [rsp + 0x30]               ; r13 will hold the # of args total that will be pushed

    mov     r14, 0x200                      ; r14 will hold the offset we need to push stuff
    add     r14, 8
    add     r14, [rdi + 56]                 ; stack size of RUTS
    add     r14, [rdi + 48]                 ; stack size of BTIT
    add     r14, [rdi + 32]                 ; stack size of our gadget frame
    sub     r14, 0x20                       ; first stack arg is located at +0x28 from rsp, so we sub 0x20 from the offset. Loop will sub 0x8 each time

    mov     r10, rsp
    add     r10, 0x30                       ; offset of stack arg added to rsp

.looping:

    xor     r15, r15
    cmp     r11, r13
    je      .finish

    ; Getting location to move the stack arg to
    sub     r14, 8
    mov     r15, rsp
    sub     r15, r14

    ; Procuring the stack arg
    add     r10, 8
    push    QWORD [r10]
    pop     QWORD [r15]                     ; move the stack arg into the right location

    add     r11, 1
    jmp     .looping

.finish:

    ; ----------------------------------------------------------------------
    ; Creating a big 320 byte working space
    ; ----------------------------------------------------------------------

    sub     rsp, 0x200

    ; ----------------------------------------------------------------------
    ; Pushing a 0 to cut off the return addresses after RtlUserThreadStart.
    ; Need to figure out why this cuts off the call stack
    ; ----------------------------------------------------------------------

    push    0

    ; ----------------------------------------------------------------------
    ; RtlUserThreadStart + 0x14 frame
    ; ----------------------------------------------------------------------

    sub     rsp, [rdi + 56]
    mov     r11, [rdi + 64]
    mov     [rsp], r11

    ; ----------------------------------------------------------------------
    ; BaseThreadInitThunk + 0x21 frame
    ; ----------------------------------------------------------------------

    sub     rsp, [rdi + 32]
    mov     r11, [rdi + 40]
    mov     [rsp], r11

    ; ----------------------------------------------------------------------
    ; Gadget frame
    ; ----------------------------------------------------------------------

    sub     rsp, [rdi + 48]
    mov     r11, [rdi + 80]
    mov     [rsp], r11

    ; ----------------------------------------------------------------------
    ; Adjusting the param struct for the fixup
    ; ----------------------------------------------------------------------

    mov     r11, rsi                        ; Copying function to call into r11

    mov     [rdi + 8], r12                  ; Real return address moved into "OG_retaddr"
    mov     [rdi + 16], rbx                 ; original rbx --> "rbx" member
    lea     rbx, [rel fixup]                ; Fixup address into rbx
    mov     [rdi], rbx                      ; Fixup member holds address of fixup
    mov     rbx, rdi                        ; Address of param struct (Fixup) into rbx

    ; ----------------------------------------------------------------------
    ; Syscall stuff. Shouldn't affect perf even if syscall isn't made
    ; ----------------------------------------------------------------------
    mov     r10, rcx
    mov     rax, [rdi + 72]

    jmp     r11

fixup:
    mov     rcx, rbx

    add     rsp, 0x200                  ; Big frame thing
    add     rsp, [rbx + 48]             ; Stack size
    add     rsp, [rbx + 32]             ; Stack size
    add     rsp, [rbx + 56]             ; Stack size

    mov     rbx, [rcx + 16]             ; Restoring OG RBX
    mov     rdi, [rcx + 24]             ; Restoring OG rdi
    mov     rsi, [rcx + 88]             ; Restoring OG rsi
    mov     r12, [rcx + 96]             ; Restoring OG r12
    mov     r13, [rcx + 104]            ; Restoring OG r13
    mov     r14, [rcx + 112]            ; Restoring OG r14
    mov     r15, [rcx + 120]            ; Restoring OG r15
    jmp     QWORD [rcx + 8]