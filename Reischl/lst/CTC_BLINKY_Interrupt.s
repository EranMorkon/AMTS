;******************************************************************************
;*  Z80 Assemblerprogramm                                                     *
;*  Josef Reisinger                                                           *
;*  josef.reisinger@htl-hl.ac.at                                              *
;*  26/04/2015                                                                *
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
COUNTER:      EQU    $8000      ; RAM Counter

;******************************************************************************
;*                   RESET HANDLER                                            *
;*                   Function: Initalize system and start Main Programm       *
;******************************************************************************
             ORG    $0000
             DI                    ; Disable interrupt
             LD     SP,RAMTOP      ; Set stack pointer 
             JP     MAIN          ; jump to Main program

;*******************************************************************
;*                    MAIN PROGRAM                                 *
;*******************************************************************
             ORG    $0100
MAIN:        LD     HL,COUNTER  ; Reset RAM Counter
         LD     A,$00       
         LD     (HL),A

         ; ------------------ Initialize PIO ----------------
         LD     A,$99 	; Initialize 8255: PA0-PA7=IN (DIP SWITCHES), PB0-PB7=OUT (LEDS),
             OUT    (PIO_CON),A   ; PC0-PC7=IN, Mode 0 Selektion    

         ;------------------ Configure CTC -----------------------
         LD     A,$A5    	; Configure CTC Channel 0:Interrupt, Timer Mode, Prescaler = 256, 
         OUT    (CTC0),A	; trigger on positive edge, next word = time constant, Channel continuous result operation
         LD     A,$FF    	; Write Time constant 255*256*552ns= 36,03ms
         OUT    (CTC0),A    
         LD     A,$A8    	; Loading Interrupt Vector register
         OUT    (CTC0),A 	; trigger on positive edge, next word = time constant, Channel continuous result operation
         
         ;---------------------- Configure Interrupt -----------------
         LD     A,$01      ; Loading Interrupt Register
         LD     I,A
         IM     2          ; Interrupt Mode 2   
         EI                ; Enable Interrupt    

         ;--------------------- Main Program  --------------------------
AGAIN:       JP     AGAIN    ; Endlos             

     
;******************************************************************************
;*                   INTERRUPT SERVICE ROUTINE                                *
;*                   CTC Channel 0                                            *
;******************************************************************************
             ORG    $01A8
         DEFW   _INT_CTC    
_INT_CTC:    PUSH   AF
             PUSH   HL
         LD     HL,COUNTER
             LD     A,(HL)     ; Read Counter
         INC    A              ; Increment Counter
         CP     $07        ; 252,21ms reached?
         JP     NZ,_END_INT
         LD     A,$00	     ; Reset Counter
         IN     A,(PIO_B)      ; Toggle LED's with 2Hz
         CPL
         OUT    (PIO_B),A      ; 
_END_INT:    LD     (HL),A     ; Store Counter
             POP    HL
             POP    AF
             EI                ; Entry Point of Interrupt Service Routine
         RETI
