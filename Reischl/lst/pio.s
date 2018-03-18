;******************************************************************************
;*  Z80 Assembler program                                                     *
;*  Josef Reisinger                                                           *
;*  josef.reisinger@htl-hl.ac.at                                              *
;*  26/04/2015                                                                *
;******************************************************************************

;************************* HARDWARE IO ADR ************************************
; PIO 82C55 I/O
PIO1A:        EQU    $80            ; INPUT  - DIP SWITCHES
PIO1B:        EQU    $81            ; OUTPUT - LEDS
PIO1C:        EQU    $82            ; (INPUT)  
PIO1CONT:     EQU    $83            ; CONTROL BYTE PIO 82C55

;*************************** CONSTANTS ****************************************
RAMTOP:       EQU    $FFFF          ; 32Kb RAM   8000H-FFFFH


;******************************************************************************
;*                   START AFTER RESET,                                       *
;*                   Function....: ready system and restart                   *
;******************************************************************************
    ORG    $0000
    ;DI                 	; Disable interrupt
    ;LD     SP,RAMTOP   	; Set stack pointer to top off ram
    LD     A,$99        	; PA0-PA7=IN (DIP SWITCHES), PB0-PB7=OUT (LEDS),
; PC0-PC7=IN, Mode 0 Selection 
    
    OUT    (PIO1CONT),A    
    IN     A,(PIO1CONT)

AGAIN:
    IN     A,(PIO1A)    	; Read actual status of Switches (PA0-PA7)
    OUT    (PIO1B),A    	; Output Status to LEDs (PB0-PB7)
    JP     AGAIN        	; Endlos
