                AREA BLINKEN, CODE, READONLY
                INCLUDE STM32_F103RB_MEM_MAP.INC
                EXPORT __main

__main          PROC
                BL      init_port
                LDR     R1, =GPIOB_ODR
_main_again     LDR     R0, [R1]
                EOR     R0, R0, #0x100
                STR     R0, [R1]
                BL      wait_500ms
                B       _main_again
                ENDP

init_port       PROC
                PUSH    {R0-R2, LR}

                MOV     R2, #0x8
                LDR     R1, =RCC_APB2ENR
                LDR     R0, [R1]
                ORR     R0, R0, R2
                STR     R0, [R1]

                LDR     R1, =GPIOB_CRH
                LDR     R0, [R1]
                LDR     R2, =0xFFFFFFF0
                AND     R0, R0, R2
                MOV     R2, #0x03
                ORR     R0, R0, R2
                STR     R0, [R1]

                POP     {R0-R2, PC}
                ENDP

wait_500ms      PROC
                PUSH    {R0-R2, LR}
                MOV     R0, #0x1F4
                MOV     R1, #0
_wait_ms_loop   MOV     R2, #0x63B
_wait_ms_loop1  SUB     R2, R2, #1
                CMP     R2, R1
                BNE     _wait_ms_loop1
                SUB     R0, R0, #1
                CMP     R0, R1
                BNE     _wait_ms_loop
                POP     {R0-R2, PC}
                ENDP

                END
