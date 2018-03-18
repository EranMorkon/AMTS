;******************************************************************************
;*  Z80 Assembler program                                                     *
;*  Josef Reisinger                                                           *
;*  josef.reisinger@htl-hl.ac.at                                              *
;*  10/07/2017                                                                *
;******************************************************************************

; ---------------------------- PIO 82C55 I/O ---------------------------------
PIO_A:        EQU    $80            ; (INPUT)  
PIO_B:        EQU    $81            ; (OUTPUT) OUT TO LEDS
PIO_C:        EQU    $82            ; (INPUT)  IN from DIP SWITCHES
PIO_CON:      EQU    $83            ; CONTROL BYTE PIO 82C55

; --------------------------- CTC Z80 Timer Counter --------------------------
CTC0          EQU    $00 ; Channel 0
CTC1          EQU    $01 ; Channel 1
CTC2          EQU    $02 ; Channel 2
CTC3          EQU    $03 ; Channel 3

; -------------------------- SIO (USART) ----------------------------------------
SIO_A_D:      EQU    $40            ; Channel A Data Register  
SIO_B_D:      EQU    $41            ; Channel B Data Register
SIO_A_C:      EQU    $42            ; Channel A Control Register
SIO_B_C:      EQU    $43            ; Channel B Control Register


;-------------------------- CONSTANTS ----------------------------------------
RAMTOP:       EQU    $FFFF          ; 32Kb RAM   8000H-FFFFH
COUNTER:      EQU    $8000          ; RAM Counter

;******************************************************************************
;*                   RESET HANDLER                                            *
;*                   Function: Initialize system and start Main Program       *
;******************************************************************************
             ORG    $0000
             DI                    ; Disable interrupt
             LD     SP,RAMTOP      ; Set stack pointer 
             JP     MAIN		  ; jump to Main program


;*******************************************************************
;*                    MAIN PROGRAM                                 *
;*******************************************************************
ORG   $100
MAIN:        LD     A,$99     	; Initialize 8255: PA0-PA7=IN (DIP SWITCHES), PB0-PB7=OUT (LEDS),
             OUT    (PIO_CON),A 	; PC0-PC7=IN, Mode 0 Selection 
         	LD     A,$01      	; Initialize RAM Counter
         	LD     HL,COUNTER  	; Load RAM Counter Address
         	LD     (HL),A  	; Store Counter in RAM cell
AGAIN:	   	LD     A,(HL)  	; Load RAM Counter
         	OUT    (PIO_B),A	; Output RAM Counter to LEDs (PB0-PB7)
         	INC    (HL)  		; Increment RAM Counter
         	CALL   _WAIT      	; UP Warteschleife aufrufen
         	JP     AGAIN      	; Endlos
      


;*******************************************************************
;*                     Warteschleife 0,5s                          *
;*******************************************************************
_WAIT:       LD    D,$FF       ; 
_OUTER:      LD    E,$FF       ; 
_INNER:      DEC   E
        	JP    NZ,_INNER
         	DEC   D
         	JP    NZ,_OUTER
         	RET