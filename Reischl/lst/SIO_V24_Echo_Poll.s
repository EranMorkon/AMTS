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
CR:       EQU    $0D    
LF:       EQU    $0A
SPACE:        EQU    $20    

;******************************************************************************
;*                   RESET HANDLER                                            *
;*                   Function: Start Main Programm                            *
;******************************************************************************
             ORG    $0000
             JP     MAIN          ; jump to Main program


;******************************************************************************
;*                   NMI HANDLER                                              *
;*                   Handles  NMI Interrupt Request                           *
;******************************************************************************
             ORG    $0066
         LD     HL,NMI_TEXT      ; Send NMI to V24
         CALL   SIO_PUT_STRING
         RETN

;******************************************************************************
;*                          MAIN PROGRAM                                      *
;******************************************************************************
         ORG   $100
MAIN:    DI               ; Disable interrupt
         LD     SP,RAMTOP ; Set stack pointer 
         CALL   PIO_INIT  	; Init PIO (8255)
         CALL   CTC_INIT 	; Initialize CTC Channel 1 for 9600 Baud (SIO Channel A)
         CALL   SIO_INIT 	; Initialize SIO for character based transfer (9600,n,8,1)
         LD     HL,START_TEXT   	; Send Welcome Text to V24
         CALL   SIO_PUT_STRING
_AGAIN:      CALL   SIO_GET_CHAR	; Wait for Character on V24
             LD     A,E         	; Output Error to LED's
         OUT    (PIO_B),A
         CALL   SIO_PUT_CHAR    	; Echo Character to V24
         JP     _AGAIN    
      ; 

;******************************************************************************
;*                          Initialize PIO (8255)                              *
;******************************************************************************
PIO_INIT:    LD    A,$99       ; Init PIO 8255 Control Word:
                               ; PA0-PA7=IN (DIP SWITCHES)
                               ; PB0-PB7=OUT (LEDS),
         OUT   (PIO_CON),A     ; PC0-PC7=IN, Mode 0 Selection 
         RET

;******************************************************************************
;*                 Initialize CTC Channel 1 (SIO Channel A Clock)              *
;******************************************************************************
CTC_INIT:    LD    A,$05 	; Init Timer Counter - Channel 1 
         OUT   (CTC1),A 	; for Baudrate 9600 (No Interrupt, Timer Mode, PSC=16,
                         	; trigger on positive edge
         LD    A,$0C       ; Write Time constant 12*16*552ns= 105,98s  
         OUT   (CTC1),A
         RET


;******************************************************************************
;*               Initialize SIO Channel A for character based transfer         *
;******************************************************************************
SIO_INIT:    LD    A,$30         ; Write to WR0 Register --> Error Reset
         OUT   (SIO_A_C),A
         LD    A,$18         ; Write to WR0 Register --> Channel Reset
         OUT   (SIO_A_C),A
         LD    A,$04         ; Select WR4 Register  
         OUT   (SIO_A_C),A
         LD    A,$04         ; CLK*1, 1STOP Bit, No Parity  
         OUT   (SIO_A_C),A
         LD    A,$03         ; Select WR3 Register  
         OUT   (SIO_A_C),A
         LD    A,$C1         ; RX enable,8 Data Bits
         OUT   (SIO_A_C),A
         LD    A,$05         ; Select WR5 Register  
         OUT   (SIO_A_C),A
         LD    A,$68         ; TX enable,8 Data Bits, DTR inactive, RTS inactive, Break off
         OUT   (SIO_A_C),A
         LD    A,$1              ; Select WR1 Register  
         OUT   (SIO_A_C),A
         LD    A,$0              ; No Interrupts for Rx and Tx Characters    
             OUT   (SIO_A_C),A
         RET


 
;******************************************************************************
;*          Send one Character Via SIO Channel A(Polling Mode)                *
;*          D- Register: Character to send (ASCII Code)                       *
;******************************************************************************
SIO_PUT_CHAR:   PUSH  AF
_TX_READY:      IN    A,(SIO_A_C)	; Read RRO Register
                BIT   2,A       		; TX Buffer empty ?
            JP    Z,_TX_READY       	; No --> Wait
            LD    A,D           		; load character in A
            OUT   (SIO_A_D),A      	; Send character (Transfer Buffer)
            POP   AF
            RET


;******************************************************************************
;*          Receive one Character Via SIO Channel A(Polling Mode)             *
;*          D- Register: Character received (ASCII Code)                  *
;*          E-Register:  Error Code                       *
;******************************************************************************
SIO_GET_CHAR:   PUSH  AF
_RX_READY:      IN    A,(SIO_A_C)   ; Read RRO Register
                BIT   0,A           ; RX Character Available ?
            JP    Z,_RX_READY       ; No --> Wait
            IN    A,(SIO_A_D)       ; Store character
            LD    D,A               
            LD     A,$01            ; Select WR1 Register
            OUT   (SIO_A_C),A       
            IN    A,(SIO_A_C)       ; Read Error Register
            LD    E,A               ; store Error Status
            AND   $70               ; only D6(CRC Framing Error),D5(Rx Overrun Error) und D4 (Parity Error)
            JP    Z,_RX_EXIT        ; return if no error    
            LD    A,$30             ; reset Error
                OUT   (SIO_A_C),A
_RX_EXIT:   POP   AF
            RET


;******************************************************************************
;*                     SEND STRING to V24 via SIO                             *
;*                     HL: contains start address of string               *
;******************************************************************************
SIO_PUT_STRING:  PUSH  AF
_NEXT_CHAR:      LD    A,(HL)          ; get character
                 CP    $00             ; END of String ?
                 JP    Z,_TEXT_END
             LD    D,A
             CALL  SIO_PUT_CHAR    ; send character
             INC   HL          ; next character
                 JP    _NEXT_CHAR
_TEXT_END:       POP   AF
                 RET


;******************************************************************************
;*                   TEXT DEFINITIONS                                         *
;******************************************************************************
START_TEXT:  DEFB CR,LF,'Z','8','0',SPACE,'D','E','M','O',SPACE,'V','1','.','0',$00
NMI_TEXT:    DEFB CR,LF,'N','M','I',$00