.text

.ref matrix_get_wbuf 
.def matrix_draw_pixel

;; Set a pixel in the matrix
;;
;; # Parameters
;; r0: 32 bit color to set. The bit layout for the color format is as follows:
;;     (0000  0 b8 g8 r8  0000  0 b4 g4 r4  0000  0 b2 g2 r2  0000  0 b1 g1 r1)
;; r1: x coordinate
;; r2: y coordinate
matrix_draw_pixel:
	push {r4, lr, fp}
	push {r0-r3}
	bl matrix_get_wbuf
	mov r4, r0 ; r4 contains the write buffer
	pop {r0-r3}

	cmp r2, #32
	it ge
	subge r2, r2, #32 ; offset column by matrix height / 2

	add r1, r1, r2, lsl #(6 + 2) ; r1 = (x + (y * 64 & 4))

	.loop 4
		ldrb r3, [r4, r1]    ; Load the byte at buf[x][y]
		ite ge               ; Use the previous cmp value
		bfige r3, r0, #4, #3 ; Set the higher address space bits
		bfilt r3, r0, #0, #3 ; Set the lower address space bits
		strb r3, [r4, r1]    ; Update the color
		add r1, r1, #64      ; Move to the next bitplane
		lsr r0, r0, #8       ; Shift to get the next color
	.endloop

	pop {r4, lr, fp}
	bx lr
.end
