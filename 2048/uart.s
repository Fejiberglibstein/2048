	.data

li .macro reg, data
    mov  reg, #(data & 0x0000FFFF)
   	movt reg, #((data & 0xFFFF0000) >> 16)
    .endm


	.text


	.global uart_init		; Finished
	.global output_character ; Finished
	.global read_character ; Finished
	.global read_string		; Finished
	.global output_string	; Finished
	.global illuminate_RGB_LED	; Finished
	.global division		; Finished
	.global multiplication	; Finished
	.global timer_interupt_init ;Finished
	.global gpio_btn_and_LED_init ; Finished
	.global Timer_Handler
	.global pwm_with_pwm_generator
	.global Advanced_RGB_LED
	.global string2int
	.global PWM_init_red
	.global spi_init
	.global spi_7_seg

U0FR: 	.equ 0x18


art_init:
	PUSH {r4-r12,lr}	; Spill registers to stack

          				; Your code is placed here

	MOV r0, #0xE618
	MOVT r0, #0x400F
	MOV r1, #1
	STR r1, [r0]

	MOV r0, #0xE608
	MOVT r0, #0x400F
	MOV r1, #1
	STR r1, [r0]

	MOV r0, #0xC030
	MOVT r0, #0x4000
	MOV r1, #0
	STR r1, [r0]


	MOV r0, #0xC024
	MOVT r0, #0x4000
	MOV r1, #8
	STR r1, [r0]

	MOV r0, #0xC028
	MOVT r0, #0x4000
	MOV r1, #44
	STR r1, [r0]

	MOV r0, #0xCFC8
	MOVT r0, #0x4000
	MOV r1, #0
	STR r1, [r0]

	MOV r0, #0xC02C
	MOVT r0, #0x4000
	MOV r1, #0x60
	STR r1, [r0]

	MOV r0, #0xC030
	MOVT r0, #0x4000
	MOV r1, #0x301
	STR r1, [r0]

	MOV r0, #0x451C
	MOVT r0, #0x4000
	LDR r2, [r0]
	MOV r1, #0x03
	ORR r1, r1, r2
	STR r1, [r0]

	MOV r0, #0x4420
	MOVT r0, #0x4000
	LDR r2, [r0]
	MOV r1, #0x03
	ORR r1, r1, r2
	STR r1, [r0]

	MOV r0, #0x452C
	MOVT r0, #0x4000
	LDR r2, [r0]
	MOV r1, #0x11
	ORR r1, r1, r2
	STR r1, [r0]

	POP {r4-r12,lr}  	; Restore registers from stack
	MOV pc, lr


gpio_btn_and_LED_init:
	PUSH {r4-r12,lr}	; Spill registers to stack

          				; Your code is placed here

	MOV r0, #0xE000
	MOVT r0, #0x400F
	LDR r1, [r0, #0x608]
	ORR r1, r1, #0x2A
	STR r1, [r0, #0x608];Clock Enable


 	MOV r0, #0x5000
 	MOVT r0, #0x4002	; Port F

 	LDR r1, [r0, #0x400] ; Config pin direction
 	BIC r1, r1, #0x10
 	ORR r1, r1, #0xE
 	STR r1, [r0, #0x400]

 	LDR r1, [r0, #0x51C] ; Config I/O DEN
 	ORR r1, r1, #0x1E
 	STR r1, [r0, #0x51C]

 	LDR r1, [r0, #0x510] ; Enable pull-up resistor
 	ORR r1, r1, #0x10
 	STR r1, [r0, #0x510]



 	MOV r0, #0x5000
 	MOVT r0, #0x4000	; Port B

 	LDR r1, [r0, #0x400] ; Config pin direction
 	ORR r1, r1, #0xF
 	STR r1, [r0, #0x400]

 	LDR r1, [r0, #0x51C] ; Config I/O DEN
 	ORR r1, r1, #0xF
 	STR r1, [r0, #0x51C]



	MOV r0, #0x7000
 	MOVT r0, #0x4000	; Port D Clock Enable

 	LDR r1, [r0, #0x400] ; Config pin direction
 	BIC r1, r1, #0xF
 	STR r1, [r0, #0x400]

  	LDR r1, [r0, #0x51C] ; Config I/O DEN
 	ORR r1, r1, #0xF
 	STR r1, [r0, #0x51C]


	POP {r4-r12,lr}  	; Restore registers from stack
	MOV pc, lr


output_character:
	PUSH {r4-r12,lr}	; Spill registers to stack

          				; Your code is placed here

	MOV r4, #0xC000			; Loading UART Data Register Address in r4
	MOVT r4, #0x4000

output_character_loop:

	LDRB r5, [r4, #U0FR]	; Loading into r5 the Flag Register
	AND r5, r5, #0x20
	CMP r5, #32				; Flag Checking
	BEQ output_character_loop

	STRB r0, [r4]

	POP {r4-r12,lr}  	; Restore registers from stack
	MOV pc, lr

;------------------------------------------------------------------------------------------------------

read_character:
	PUSH {r4-r12,lr}	; Spill registers to stack

          				; Your code is placed here

	MOV r4, #0xC000
	MOVT r4, #0x4000		; Loading UART Mapped-Memory

read_poll_output_character:
	LDRB r5, [r4, #U0FR]
	AND r5, r5, #16
	CMP r5, #16
	BEQ read_poll_output_character
	LDRB r0, [r4]


	POP {r4-r12,lr}  	; Restore registers from stack
	MOV pc, lr

read_string:
    PUSH {r4-r12,lr}
    MOV r4, r0
    MOV r12, #0

read_string_loop:
    BL read_character

enter_check:
    CMP r0, #0xD        ; Carriage return
    BEQ read_string_exit

;
range_check:
    CMP r0, #0x30       ; '0'
    BLT check_special_chars  ; Check if it's '.' or '-'
    CMP r0, #0x39       ; '9'
    BLE valid_char       ; Digit is valid

check_special_chars:
    CMP r0, #0x2E       ; Decimal point '.'
    BEQ valid_char
    CMP r0, #0x2D       ; Minus sign '-'
    BEQ valid_char
    B loop_back          ; Invalid character

valid_char:
    STRB r0, [r4, r12]
    ADD r12, r12, #1
    BL output_character
    B read_string_loop

loop_back:
    B read_string_loop

read_string_exit:
    MOV r0, #0x00
    STRB r0, [r4, r12]
    MOV r0, #0xD
    BL output_character
    MOV r0, #0xA
    BL output_character
    MOV r0, r4
    POP {r4-r12,lr}
    MOV pc, lr

    .end