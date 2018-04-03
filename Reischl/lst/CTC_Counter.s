;******************************************************************************
;*  Z80 Assembler program                                                     *
;*  Josef Reisinger                                                           *
;*  josef.reisinger@htl-hl.ac.at                                              *
;*  26/04/2015                                                                *
;******************************************************************************

; ---------------------------- PIO 82C55 I/O ---------------------------------
PIO_A:    EQU       $80               ; (INPUT)  
PIO_B:    EQU       $81               ; (OUTPUT) OUT TO LEDS
PIO_C:    EQU       $82               ; (INPUT)  IN from DIP SWITCHES
PIO_CON:  EQU       $83               ; CONTROL BYTE PIO 82C55

; --------------------------- CTC Z80 Timer Counter --------------------------
CTC0      EQU       $00               ; Channel 0
CTC1      EQU       $01               ; Channel 1
CTC2      EQU       $02               ; Channel 2
CTC3      EQU       $03               ; Channel 3

; -------------------------- SIO (USART) ----------------------------------------
SIO_A_D:  EQU       $40               ; Channel A Data Register  
SIO_B_D:  EQU       $41               ; Channel B Data Register
SIO_A_C:  EQU       $42               ; Channel A Control Register
SIO_B_C:  EQU       $43               ; Channel B Control Register


;-------------------------- CONSTANTS ----------------------------------------
RAMTOP:   EQU       $FFFF             ; 32Kb RAM   8000H-FFFFH

;******************************************************************************
;*                   START AFTER RESET,                                       *
;*                   Function....: ready system and restart                   *
;******************************************************************************
    ORG       $0000
    DI                	; Disable interrupt
    LD        SP,RAMTOP  	; Set stack pointer to top off ram
    LD        A,$15       	; Configure CTC Channel 0:No Interrupt, Timer Mode, No Prescaler, 
    OUT       (CTC0),A   	; trigger on positive edge, next word = time constant, Channel continuous result operation
    LD        A,186      	; Write Time constant 186*560ns= 104µs
    OUT       (CTC0),A
AGAIN:    IN        A,(CTC0)     	; Read actual status of CTC Channel 0
    JP        AGAIN  		; Endlos
