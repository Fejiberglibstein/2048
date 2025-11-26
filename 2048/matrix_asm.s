.text

li .macro reg, data
    mov  reg, #(data & 0x0000FFFF)
    movt reg, #((data & 0xFFFF0000) >> 16)
    .endm

gpio_port_b: .equ 0x40005000

    .ref matrix_get_wbuf 

;; Set a pixel in the matrix
;;
;; # Parameters
;; r0: 32 bit color to set. The bit layout for the color format is as follows:
;;     (0000  0 b8 g8 r8  0000  0 b4 g4 r4  0000  0 b2 g2 r2  0000  0 b1 g1 r1)
;; r1: x coordinate
;; r2: y coordinate
    .def matrix_draw_pixel
matrix_draw_pixel:
    push {lr, r11}
    push {r0-r2}
    bl matrix_get_wbuf
    pop {r1-r3} ; Swap r0->r1, r1->r2, r2->r3

    cmp r3, #32
    it ge
    subge r3, r3, #32 ; offset column by matrix height / 2

    add r2, r2, r3, lsl #(6 + 2) ; r2 = (x + (y * 64 * 4))

    .loop 4
        ldrb r3, [r0, r2]    ; Load the byte at matrix_ptr[y][x]
        ite ge               ; Use the previous cmp value
        bfige r3, r1, #4, #3 ; Set the higher address space bits
        bfilt r3, r1, #0, #3 ; Set the lower address space bits
        strb r3, [r0, r2]    ; Update the color
        add r2, r2, #64      ; Move to the next bitplane
        lsr r1, r1, #8       ; Shift to get the next color
    .endloop

    pop {lr, r11}
    bx lr
.end

;; Push a row of the buffer into the matrix
;;
;; # Parameters
;; r0: row to start with
    .def matrix_push_row
matrix_push_row:
    push {r4}
    li r4, gpio_port_b
    mov r1, #0x80 ; Clock high
    mov r2, #0x77 ; RGB high
    .loop 64
        ldrb r3, [r0], #1
        strb r3, [r4, #(0xf7 << 2)] ; Store to rgb pins
        strb r1, [r4, #(0x80 << 2)] ; Set clock high
    .endloop
    pop {r4}
    bx lr
