    .data

li .macro reg, data		;macro to use li r0, #0x000010000 etc
	mov reg, #(data & 0x0000FFFF)
	movt reg, #((data & 0xFFFF0000) >> 16)
	.endm

    .include "include/tiva.s"

    .text

    .global accel_init


accel_init:

    push {r4-r12, lr}

    li r0, sysctl_base_addr   ; system control base address

    ; Enable system clock i2c module

    add r4, r0, #SYSCTL_RCGCI2C
    ldr r1, [r4]
    orr r1, r1, #0x2 ; enable i2c module 1
    str r1, [r4]

    ; Enable GPIO port clock

    add r4, r0, #SYSCTL_RCGCGPIO
    ldr r1, [r4]
    orr r1, r1, #0x1  ; enable clock for GPIO port A
    str r1, [r4]

    ; Configure GPIO pins

    li r0, gpio_port_a_base_addr  ; Port A base address

    ; Set GPIO pins as Alternate Functions

    add r4, r0, #GPIO_AFSEL
    ldr r1, [r4]
    orr r1, r1, #0xC0  ; set pins PA6 and PA7 as alt function (11000000)
    str r1, [r4]

    ; Select Pull up resistor
    add r4, r0, #GPIO_PUR
    ldr r1, [r4]
    orr r1, r1, #0xC0  ; enable pull-ups on PA6 and PA7
    str r1, [r4]

    ; Select Digital for pins
    add r4, r0, #GPIO_DEN
    ldr r1, [r4]
    orr r1, r1, #0xC0  ; enable digital function on PA6 and PA7
    str r1, [r4]

    ; Enable I2CSDA pin for open-drain operation
    add r4, r0, #GPIO_ODR
    ldr r1, [r4]
    orr r1, r1, #0x80  ; set PA7 (SDA) as open-drain (bit 7)
    str r1, [r4]

    ; Configure PMCn field in GPIOPCTL to assign I2C to pins
    add r4, r0, #GPIO_PCTL
    ldr r1, [r4]
    bic r1, r1, #0xFF000000  ; clearing PMC6 and PMC7 fields
    orr r1, r1, #0x33000000  ; set PMC6=3 and PMC7=3 for I2C0 function
    str r1, [r4]

    ; Initialize I2C master ( write 0x10 )

    li r0, I2C_1_base_addr  ; I2C module 1 base address

    add r4, r0, #I2C_MCR
    ldr r1, [r4]
    orr r1, #0x10  ; set MFE (Master Function Enable) bit
    str r1, [r4]

    ; Set SCL speed ( I2CMTPR )
    add r4, r0, #I2C_MTPR
    mov r1, #39  ; TPR = 39 for 100kHz SCL for 80MHz system clock
    str r1, [r4]

    ; Enable I2C module



    ; Read WHO_AM_I
    ; Write register address
WHO_AM_I:

	BL delay

    add r4, r0, #I2C_MSA
    mov r1, #0x32  ; LSM303AGR write address (0x19 << 1)
    str r1, [r4]
    
    add r4, r0, #I2C_MDR
    mov r1, #0x0F  ; WHO_AM_I register address 
    strb r1, [r4]
    
    add r4, r0, #I2C_MCS
    mov r1, #0x07  ; START + RUN + STOP 
    str r1, [r4]
    
wait_write:
    ldr r1, [r4]
    tst r1, #0x01  ; check BUSBUSY bit
    bne wait_write
    
    ; Read data
    add r4, r0, #I2C_MSA
    mov r1, #0x33  ; LSM303AGR read address (0x19 << 1 | 1)
    str r1, [r4]
    
    add r4, r0, #I2C_MCS
    mov r1, #0x07  ; START + RUN + STOP
    str r1, [r4]
    
wait_read:
    ldr r1, [r4]
    tst r1, #0x01  ; check BUSY bit
    bne wait_read
    
    ; Read WHO_AM_I value (should be 0x33)
    add r4, r0, #I2C_MDR
    ldr r5, [r4]  ; WHO_AM_I value stored in r5

    cmp r5, #0x33
    bne WHO_AM_I

	pop {r4-r12, pc}




delay:

	push {r4-r12, lr}

	li r12, 0x4240

loop:

	subs r12, r12, #1
	bne loop

	pop {r4-r12, lr}
	mov pc, lr


    .end


