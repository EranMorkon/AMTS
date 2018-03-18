;******************************************************************************
;*  Z80 Assemblerprogramm                                                     *
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
;*                   SIO INTERRUPT HANDLER                                    *
;*                   Function: Start Main Programm                            *
;******************************************************************************
             ORG    $000C
         DEFW   RX_CHA_AVAILABLE

             ORG    $000E
         DEFW   SPEC_RX_CONDITION


;******************************************************************************
;*                   NMI HANDLER                                              *
;*                   Handles  NMI Interupt Request                            *
;******************************************************************************
             ORG    $0066
         LD     HL,NMI_TEXT      ; Send NMI to V24
         CALL   SIO_PUT_STRING
         RETN


;******************************************************************************
;*                          MAIN PROGRAM                                      *
;******************************************************************************
         ORG   $100
MAIN:        DI                     ; Disable interrupt
             LD     SP,RAMTOP       ; Set stack pointer 
         CALL   PIO_INIT        ; Init PIO (8255)
         CALL   CTC_INIT        ; Initalize CTC Channl1 for 9600 Baud (SIO Channel A)
         CALL   SIO_INIT        ; Inialize SIO for charecter based transfer (9600,n,8,1)

         LD     HL,START_TEXT   ; Send Welcome Text to V24
         CALL   SIO_PUT_STRING

         LD     A,0
         LD     I,A             ; Load I Register with zero
         IM     2               ; Set Interupt 2
         EI                 ; Enable Interrupt

         LD     A,$01           ; Initalize moving light
_AGAIN:      OUT    (PIO_B),A       ; output Moving light to LED
         RL     A               ; next bit
         CALL   _WAIT       ; wait 0,5 s
         JP     _AGAIN      ; endless
       

;******************************************************************************
;*                          Initalize PIO (8255)                              *
;******************************************************************************
PIO_INIT:    LD    A,$99       ; Init PIO 8255 Control Word:
                                   ; PA0-PA7=IN (DIP SWITCHES)
                               ; PB0-PB7=OUT (LEDS),
         OUT   (PIO_CON),A     ; PC0-PC7=IN, Mode 0 Selection 
         RET

;******************************************************************************
;*                 Initalize CTC Channel 1 (SIO Channel A Clock)              *
;******************************************************************************
CTC_INIT:    LD    A,$05       ; Init Timer Counter - Channel 1 
         OUT   (CTC1),A        ; for Baudrate 9600 (No Interrupt, Timer Mode, PSC=16,
                               ; trigger on positive edge
         LD    A,$0C       ; Write Time constant 12*16*552ns= 105,98s  
         OUT   (CTC1),A
         RET


;******************************************************************************
;*               Initalize SIO Channel A for character based transfer         *
;*               Interupt on Received characters on Channel A                 */
;******************************************************************************
SIO_INIT:    LD    A,$30         ; Write to WR0 Register --> Error Reset
         OUT   (SIO_A_C),A
         LD    A,$18         ; Write to WR0 Register --> Channel Reset
         OUT   (SIO_A_C),A
         LD    A,$04         ; Select WR4 Register  
         OUT   (SIO_A_C),A
         LD    A,$04         ; CLK*1, 1STOP Bit, No Parity  
         OUT   (SIO_A_C),A

         CALL   A_RTS_ON         ; TX on,TX 8 Bit,DTR inactive, RTS active; Break off

         LD    A,$1              ; Select WR1 Register Channel B    
         OUT   (SIO_B_C),A
         LD    A,$04         ; no Interrupt on Channel B, status affects Vector
             OUT   (SIO_B_C),A       ; 
         LD    A,$2              ; Select WR2 Register Channel B    
         OUT   (SIO_B_C),A
         LD    A,$00         ; Definition Interrupt vector. Bits D3,D2,D1  are changed according to
             OUT   (SIO_B_C),A       ; RX condition (see interupt vector table)
                 
         LD    A,$1              ; Select WR1 Register  
         OUT   (SIO_A_C),A
         LD    A,$18         ; Interupts on all RX Characters, Parity is not a spec RX Condition 
             OUT   (SIO_A_C),A       ; Buffer overrun ist a special condition, TX no Interrupt
 
         CALL  SIO_A_EN          ; Enable RX Channel A
         RET 


;******************************************************************************
;*                           Enable RX Channel A                             *
;******************************************************************************
SIO_A_EN:    LD    A,$03         ; Select WR3 Register  
         OUT   (SIO_A_C),A
         LD    A,$C1         ; RX enable,8 Data Bits
         OUT   (SIO_A_C),A
         RET

;******************************************************************************
;*                           Disable RX Channel A                             *
;******************************************************************************
SIO_A_DI:    LD    A,$03         ; Select WR3 Register  
         OUT   (SIO_A_C),A
         LD    A,$C0         ; RX diable,8 Data Bits
         OUT   (SIO_A_C),A
         RET

;******************************************************************************
;*                           Channel A RTS inactive (RTS = 1)                 *
;******************************************************************************
A_RTS_OFF:   LD    A,$05         ; Select WR5 Register  
         OUT   (SIO_A_C),A
         LD    A,$68             ; TX on,TX 8 Bit, DTR inactive,RTS inactive; Break off,
         OUT   (SIO_A_C),A
         RET

;******************************************************************************
;*                           Channel A RTS inactive (RTS=0)                   *
;******************************************************************************
A_RTS_ON:    LD    A,$05         ; Select WR5 Register  
         OUT   (SIO_A_C),A
         LD    A,$6A             ; TX on,TX 8 Bit,DTR inactive, RTS active; Break off
         OUT   (SIO_A_C),A
         RET

;******************************************************************************
;*          Send one Character Via SIO Channel A(Polling Mode)                *
;*          D- Register: Character to send (ASCII Code)                       *
;******************************************************************************
SIO_PUT_CHAR:   PUSH  AF
_TX_READY:      IN    A,(SIO_A_C)       ; Read RRO Register
                BIT   2,A       ; TX Buffer empty ?
            JP    Z,_TX_READY       ; No --> Wait
            LD    A,D           ; load character in A
            OUT   (SIO_A_D),A       ; Send character (Transfer Buffer)
            POP   AF
            RET





;******************************************************************************
;*                     SEND STRING to V24 via SIO                             *
;*                     HL: contains start address of string               *
;******************************************************************************
SIO_PUT_STRING:  PUSH  AF
_NEXT_CHAR:      LD    A,(HL)          ; get charcter
                 CP    $00             ; END of String ?
                 JP    Z,_TEXT_END
             LD    D,A
             CALL  SIO_PUT_CHAR    ; send character
             INC   HL          ; next character
                 JP    _NEXT_CHAR
_TEXT_END:       POP   AF
                 RET


;******************************************************************************
;*               INTERUTPT HANDLE SIO CHANNEL A CHARACTER RECEIVE             *
;******************************************************************************
RX_CHA_AVAILABLE: PUSH  AF
                  CALL  A_RTS_OFF
          IN    A,(SIO_A_D)       ; Read RX Character
              LD    D,A               ; laod Character in D
          CALL  SIO_PUT_CHAR      ; Echo Char back to Host

_NEXT_RX_CHAR:    LD    A,$0              ;Select RR0 Register
          OUT   (SIO_A_C),A
                  IN    A,(SIO_A_C)       ; Read RRO Register
                  BIT   0,A       ; RX Character Available ?
              JP    Z,_NEXT_TX_CHAR   ; No --> OK
          IN    A,(SIO_A_D)       ; Read that character
              LD    D,A               ; load Character in D
          CALL  SIO_PUT_CHAR      ; Echo Char back to Host
              JP    _NEXT_RX_CHAR

_NEXT_TX_CHAR:    LD    A,$1              ; Select RR1 Register
          OUT   (SIO_A_C),A
          IN    A,(SIO_A_C)       ; Read RR1 Register
          BIT   0,A           ; ALL Characters sent ?
          JP    Z,_NEXT_TX_CHAR

_EO_CH_AV:        EI
          CALL  A_RTS_ON
          POP   AF
          RETI

;******************************************************************************
;*               INTERUTPT HANDLE SIO CHANNEL A ERROR                         *
;******************************************************************************
SPEC_RX_CONDITION
                 JP     MAIN          ; Restart -> jump to Main program (RESTART)


;*******************************************************************
;*                     Warteschleife 0,5s                          *
;*******************************************************************
_WAIT:           LD    B,$FF       ; 
_OUTER:          LD    C,$FF       ; 
_INNER:          DEC   C
             JP    NZ,_INNER
             DEC   B
             JP    NZ,_OUTER
             RET

;******************************************************************************
;*                   TEXT DEFINITIONS                                         *
;******************************************************************************
START_TEXT:  DEFB CR,LF,'Z','8','0',SPACE,'D','E','M','O',SPACE,'V','1','.','0',$00
NMI_TEXT:    DEFB CR,LF,'N','M','I',$00
