.section .header, "a"
.align 4
.space 0x10

.section .text.startup, "ax"
.global _start

_start:
    ldr     x3, =__bss_start
    ldr     x1, =__bss_end
    mov     w2, #0

bss_clear_loop:
    cmp     x3, x1
    b.eq    bss_done
    str     w2, [x3], #4
    b       bss_clear_loop

bss_done:
    bl      bl1_entry

hang:
    b hang
