/******************************************************************************/
/*   (C) Copyright HTL - HOLLABRUNN  2009-2010 All rights reserved. AUSTRIA   */ 
/*                                                                            */ 
/* File Name:   armv10_serbus.h                                               */
/* Autor: 		Jakob Maier                                                   */
/* Version: 	V1.00                                                         */
/* Date: 		23/07/2010                                                    */
/* Description: Library für Serielle Busse (I2C und SPI) für ARM Corttex M3   */
/*              			                                                  */
/******************************************************************************/
/* History: 	07.05.2010: Jakob Maier creation V1.00 (I2C)                  */
/*                - Beinhaltet alle wesentlichen Unterprogramme               */
/*                  die zur Kommunikation über den I2C-Bus                    */
/*              28.02.2011: REJ V1.1                                          */
/*                            - Library aus Funktionen erstellt               */ 
/******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ARMV10_SERBUS_H
#define __ARMV10_SERBUS_H

//Hardware:
//*********
//
//SCL=PB6
//SDA=PB7
//Wichtig: Die Leitungen zur Sicherheit mit 2stk. 4,7kOhm Pull-Up Widerständen an die 3,3V anpassen!
//


 //Master Receiver:
 //****************
 //
 //1 Byte + Stop:  init (-) start(adr+1,1)  -  read(&dat,x)
 //1 Byte + Stop:  init (-)	I2C1_read_1(adr+x,&dat)
 //		   
 //2 Byte + Stop:  init (-) start(adr+1,0)  -  read(&dat,1)  -  read(&dat,x)  
 //2 Byte + Stop:  init (-) start(adr+1,0)  -  I2C1_read_last2(&dat1,&dat2)
 //   
 //3 Byte + Stop:  init (-) start(adr+1,0) (-) read(&dat,0)  -  read(&dat,1)  -  read(&dat,x)
 //3 Byte + Stop:  init (-) start(adr+1,0) (-) read(&dat,0)  -  I2C1_read_last2(&dat1,&dat2)
 //  
 //4 Byte + Stop:  init (-) start(adr+1,0) (-) read(&dat,0) (-) read(&dat,0)  -  read(&dat,1)  -  read(&dat,x)
 //4 Byte + Stop:  init (-) start(adr+1,0) (-) read(&dat,0) (-) read(&dat,0)  -  I2C1_read_last2(&dat1,&dat2)
 // 
 //5 Byte + Stop:  init (-) start(adr+1,0) (-) read(&dat,0) (-) read(&dat,0) (-) read(&dat,0)  -  read(&dat,1)  -  read(&dat,x)
 //...
 //n Byte + Stop:  init (-) I2C1_read_n(adr+x,daten[],anz)
 //
 //Master Transmitter:
 //*******************
 //
 //1 Byte + Stop:  init (-) start(adr+0,x) (-) send(data,1)
 //2 Byte + Stop:  init (-) start(adr+0,x) (-) send(data,0) (-) send(data,1) 
 //3 Byte + Stop:  init (-) start(adr+0,x) (-) send(data,0) (-) send(data,0) (-) send(data,1)
 //...
 //n Byte + Stop (Pause zw. Byteübertragung mögl.): init (-) I2C1_send_n(adr+x,daten[],anz,pause)
 //
 //Legende:
 //********
 //
 // "(-)":  Sofort danach oder nach einer beliebig langen Pause folgt die nächste Operation (Achtung: bei einem Reset kann es zu Error Nr. 0x86 kommen - Die Busleitungen sind noch immer auf 0, da mitten im Datenverkehr abgebrochen wurde) 
 //	" - ":  Sofort danach folgt die nächste Operation (Es muss sofort reagiert werden)
 //


/*-------------------------------------------------- I2C - Fehlercodes: -----------------------------*/
// Ein Fehlercode beinhaltet zwei Arten von Daten: Wo der Fehler auftrat und was der Fehlergrund ist.
// Die ersten 4bit (MSB) geben an, in welcher Routine sich der Fehler ereignet hat (Fehlerposition), die
// letzten 4bit (LSB) geben die Fehlerart an. Ist kein Fehler aufgetreten, wird 0x00 als Code verwendet,
// die Fehlerposition nicht miteinberechnet. 
// Fehlercode-Berechnung:  "Fehlercode" = "Fehlerposition" | "Fehlerart";
//
#define info_success         0x00	//Kein Fehler aufgetreten (immer 0x00)

/*------------ Fehlerposition:---------*/
#define err_at_init			 0x80 	//Fehler in der Initialisierungsroutine
#define err_at_start   	 	 0x40   //Fehler in der Start- und Adressierungsroutine
#define err_at_send			 0x20	//Fehler in der Senderoutine
#define err_at_read			 0x10	//Fehler in der Empfangsroutine

/*----------- Fehlerarten:-------------*/
#define error_I2C1_running   0x01	//Der I2C1-Bus ist bereits in Betrieb und kann nicht mehr initialisiert werden
#define error_I2C1_disabled  0x02	//Es kann keine Startbedingung erzeugt werden, da der Bus außer Betrieb ist (Keine Initialisierung)
#define error_no_master      0x03	//Der I2C1-Bus befindet sich nicht im Mastermode ("start()": Peripherie-Fehler;  "send()": Startbedingung erzeugt?)
#define error_no_transmitter 0x04   //Es können keine Daten gesandt werden, da der Slavebaustein zum lesen Adressiert wurde (R/W-bit muss 0 sein)
#define error_not_addressed  0x05   //Die Hardware wurde nicht adressiert, NACK erhalten (Wurde die richtige Adresse eingestellt?)
#define error_timeout_busy   0x06	//Der Bus kann derzeit noch nicht initialisiert werden, da bereits ein Datenverkehr ausgeführt wird 
                                    //(Leitung auf "LOW" - PullUp-Widerstände vorhanden?)
#define error_timeout_start  0x07   //Bei der Startbedingung ist es zu einem Time-Out gekommen (Ist der Bus belegt (Multimaster-Mode)?,
									//Ist vielleicht noch eine Datenübertragung im gange?)
#define error_timeout_stop	 0x08	//Bei der Stopbedingung ist es zu einem Time-Out gekommen (Ist vielleicht noch eine Datenübertragung im gange?)
#define error_timeout_send   0x09	//Die Daten wurden nicht übertragen (Timeout) (Verwendet der Slave Clock-Streching? 
									//Sind die Busleitungen auf "LOW" gefallen?)
#define error_NACK			 0x0A	//Der Slave hat auf die Daten mit einem NACK geantwortet (Der Fehler liegt auf der Slave-Seite oder es wurde
 									//dessen Übertragungsprotokoll verletzt/beendet)
#define error_no_data		 0x0B	//Bei "read()". Entweder wurde noch kein Baustein adresiert oder nach Beendigung der letzten Übertragung
									//sind keine ausstehenden Daten mehr vorhanden (Adressiert?  "I2C1_data_available()" verwendet?)

									
#ifdef ARMV10_SERBUS_MOD
#define EXPORT
#else
#define EXPORT extern
#endif 

/* ---------------------------Exported functions ---------------------------- */

/******************************************************************************/
/*            U N T E R P R O G R A M M:  I2C_error_message                   */
/*                                                                            */
/* Aufgabe: Gibt eine I2C Fehleranalyse auf UART aus                          */
/* Input:   I2C Fehlercode                                                    */
/* return:	I2C Fehlercode 	                                                  */
/* Remark:  Der Fehler wird als Rückgabewert weitergegeben um das UPro direkt */
/*          in einer Fehlerabfrage verwenden zu können:                       */
/*          "if(I2C_error_message(I2C1_init())!=info_success)"                */
/******************************************************************************/
EXPORT char I2C_error_message(char errorcode);

/******************************************************************************/
/*            U N T E R P R O G R A M M:  I2C1_init                           */
/*                                                                            */
/* Aufgabe: Initialisiert den I2C1-Bus im Standard-Modus (100kHz)             */
/*          SCL=PB6, SDA=PB7 . Es wird die GPIOB und I2C1-Peripherie enabled. */
/*          I2C1 kann mit "I2C1_disable()" disabled werden.                   */
/* Input:   -                                                                 */
/* return:	I2C Fehlercode 	                                                  */
/* Remark:  Nächster Schritt:  "I2C1_start()"                                 */
/*          I2C Fehlerposition 0x80                                           */
/******************************************************************************/
EXPORT char I2C1_init(void);	 						

/******************************************************************************/
/*            U N T E R P R O G R A M M:  I2C1_disable                        */
/*                                                                            */
/* Aufgabe: Schaltet die I2C1-Peripherie ab. Es ist keine Buskommunikation    */
/*          mehr möglich. Die Ports werden als Output konfiguriert und auf    */
/*          1 gesetzt.                                                        */
/* Input:   -                                                                 */
/* return:	-           	                                                  */
/* Remark:                                                                    */
/*                                                                            */
/******************************************************************************/
/*        !!!!!!!!!!!!!!!!! Noch nicht getestet: !!!!!!!!!!!!!!!!!!!!!!        */
EXPORT void I2C1_disable(void);

/******************************************************************************/
/*            U N T E R P R O G R A M M:  I2C1_start                          */
/*                                                                            */
/* Aufgabe: Erzeugt eine Startbedingung und Adressiert einen Slavebaustein.   */
/*          Je nach R/W-Bit ist man Master Transmitter (0) oder Master        */
/*          Receiver(1).                                                      */
/* Input:   "adresse": 7bit Adresse und Read/!Write - Bit   "single":         */
/*          Wenn das R/W-Bit 1 ist und nur ein Byte empfangen werden soll ist */
/*          "single" ungleich 0.                                              */                  
/* return:	Fehlercode     	                                                  */
/* Remark:  I2C Fehlerposition 0x40                                           */
/*          Nächster I2C Schritt: Senden oder Empfangen                       */
/******************************************************************************/
EXPORT char I2C1_start(char adresse, char single);

/******************************************************************************/
/*            U N T E R P R O G R A M M:  I2C1_get_mode()                     */
/*                                                                            */
/* Aufgabe: Übergibt den Status des I2C-Busses (Slave, Master Transmitter     */
/*          oder Master Receiver). Damit kann im Nachhinein festgestellt      */
/*          werden was das R/W-Bit war.                                       */
/* Input:   -                                                                 */
/* return:	0x00: Der Bus ist deaktiviert                                     */
/*          0x01: Der Bus wurde als Slave adressiert                          */
/*          0x02: Master Transmitter                                          */
/*          0x03: Master Receiver                                             */
/* Remark:                                                                    */
/*                                                                            */
/******************************************************************************/
// !!!!!!!!!!!!!!!!!!!!!! Noch nicht getestet !!!!!!!!!!!!!!!!!!!!
EXPORT char I2C1_get_mode(void);

/******************************************************************************/
/*            U N T E R P R O G R A M M:  I2C1_data_available()               */
/*                                                                            */
/* Aufgabe:   Überprüft ob im Master-Receiver Modus Daten die Empfangen       */
/*            wurden zum Abruf bereit stehen. Damit kann im Unterprogramm     */
/*            "I2C1_read()" eine Endlosschleife vermieden werden.             */
/* Input:     -                                                               */
/* return:	  0x00: Keine Daten vorhanden                                     */
/*            0x01: Daten im Datenregister                                    */
/*                                                                            */                  
/* Remark:                                                                    */
/*                                                                            */
/******************************************************************************/
EXPORT char I2C1_data_available(void);

/******************************************************************************/
/*            U N T E R P R O G R A M M:  I2C1_read                           */
/*                                                                            */
/* Aufgabe:  Liest die Daten aus, die vom Slave gesendet wurden. Sind keine   */
/*           Daten vorhanden wird solange gewartet, bis welche empfangen      */
/*           wurden                                                           */
/* Input:    "daten": Variable in die die Daten geschrieben werden.           */
/*           "last": Wenn dass das vorletzte Byte ist dass empfangen werden   */
/*           soll, ist "last" ungleich 0                                      */
/* return:	 Fehlercode                                                       */
/* Remark:   I2C Fehlerposition 0x10                                          */
/*           Nächster Schritt: Lesen oder nach Stop "I2C1_data_available()"   */
/*           bzw. Start oder Disable                                          */  
/******************************************************************************/
EXPORT char I2C1_read(char *daten, char last);

/******************************************************************************/
/*            U N T E R P R O G R A M M:  I2C1_read_1                         */
/*                                                                            */
/* Aufgabe:  Erzeugt eine Startbedingung, Übernimmt die Adressierung, empfangt*/
/*           ein einzelnes Byte und erzeugt dann wieder eine Stopbedingung    */
/* Input:    "adresse": Slaveadresse (Das R/W-Bit wird im UPro auf 1 gesetzt) */
/*           "daten": Variable in die das  empfangene Byte geschrieben wird   */
/* return:	 Fehlercode                                                       */
/* Remark:   I2C Fehlerposition 0x40 und 0x10                                 */
/*           Nächster Schritt: Start oder Disable                             */
/******************************************************************************/
EXPORT int I2C1_read_1(char adresse, char *daten);

/******************************************************************************/
/*            U N T E R P R O G R A M M:  I2C1_read_last2                     */
/*                                                                            */
/* Aufgabe:  Empfängt 2 Bytes und erzeugt dann eine Stopbedingung.            */
/*           Kann bei einem datenempfang ab 2 Bytes immer verwendet werden    */
/*                                                                            */
/* Input:    "daten1": Variable für das vorletzte Empfangene Byte             */
/*           "daten2": Das letzte Empfangsbyte                                */
/*                                                                            */                  
/* return:	Fehlercode     	                                                  */
/* Remark:  I2C Fehlerposition:	 0x10                                         */
/*          Nächster I2CSchritt:  Start oder Disable                          */
/******************************************************************************/
EXPORT char I2C1_read_last2(char *daten1, char *daten2);

/******************************************************************************/
/*            U N T E R P R O G R A M M: I2C1_read_n                          */
/*                                                                            */
/* Aufgabe:  Liest eine bestimmte Anzahl an Daten ein.                        */
/*           Übernimmt auch die Startbedingung, die Adressierung              */
/*           und die Stopbedingung. Das R/W-Bit wird im UPro gesetzt.         */
/* Input:    "adresse":	Adresse des Slave-Bausteins                           */
/*           "daten[]": Array, in das die Empfangsdaten geschrieben werden    */
/*           "anzahl": Wie viele Bytes empfangen werden sollen (ab 1 Byte)    */                  
/* return:	Fehlercode     	                                                  */
/* Remark:  I2C Fehlerposition:	 0x40 und 0x20                                */
/*                                                                            */
/******************************************************************************/
EXPORT char I2C1_read_n(char adresse, char daten[],int anzahl);

/******************************************************************************/
/*            U N T E R P R O G R A M M:   I2C1_send                          */
/*                                                                            */
/* Aufgabe:   Sendet ein einzelnes Byte an den Slave und kann dann eine       */
/*            Stopbedingung erzeugen. Das R/W-Bit muss 0 (Master Transmitter) */
/*            gewesen sein.                                                   */
/*                                                                            */
/* Input:     "daten": Die Sendedaten                                         */
/*            "stop": Wenn dies Variable ungleich 0 ist, wird nach der        */
/*                    Übertragung eine Stopbedingung erzeugt                  */                  
/* return:	     	                                                          */
/* Remark:  I2C Fehlerposition:	 0x20	                                      */
/*          Nächster Schritt:  Senden, oder wenn "stop" ungleich 0:           */
/*							   Start oder Disable                             */
/******************************************************************************/
EXPORT char I2C1_send(char daten,char stop);

/******************************************************************************/
/*            U N T E R P R O G R A M M: I2C1_send_n                          */
/*                                                                            */
/* Aufgabe: Übernimmt die Startbedingung und die Adressierung.                */
/*          Sendet danach eine Reihe an Daten an den Slave wobei dazwischen   */
/*          Pausen eingelegt werden können. Danach kommt es zu einer          */
/*          Stopcondition                                                     */
/* Input:   "adresse": Slaveadresse (Das R/W-Bit wird im UPro auf 0 gesetzt)  */
/*          "daten[]": Array dessen Inhalt an den Slave gesendet wird         */
/*          "anzahl": Anzahl der Bytes aus dem Array die gesendet werden      */
/*          "Pause": Pause in Millisekunden die zwischen den Übertragungen    */                 
/*                   eingelegt wird                                           */
/* return: Fehlercode	     	                                              */
/* Remark: I2C Fehlerposition:	 0x40 und 0x20                                */
/*         Nächster Schritt:  Start oder Disable                              */
/******************************************************************************/
EXPORT char I2C1_send_n(char adresse, char daten[],int anzahl,int pause);


/******************************************************************************/
/*            U N T E R P R O G R A M M: SPI1_Init                            */
/*                                                                            */
/* Aufgabe:  Initialisiert SPI 1 Schnittstelle                                */
/* Input:                                                                     */
/* return:	     	                                                          */
/* Remark:                                                                    */
/*                                                                            */
/******************************************************************************/
EXPORT void SPI1_Init(void);

/******************************************************************************/
/*            U N T E R P R O G R A M M: SPI1_Send                            */
/*                                                                            */
/* Aufgabe:  Sendet 1 Byte via SPI1 Schnittstelle                             */
/* Input:                                                                     */
/* return:	     	                                                          */
/* Remark:                                                                    */
/*                                                                            */
/******************************************************************************/
EXPORT void SPI1_Send(unsigned char send);

/******************************************************************************/
/*            U N T E R P R O G R A M M: SPI1_Receive                         */
/*                                                                            */
/* Aufgabe:  Empfängt 1 Byte von SPI1 Schnittstelle                           */
/* Input:                                                                     */
/* return:	     	                                                          */
/* Remark:                                                                    */
/*                                                                            */
/******************************************************************************/
EXPORT unsigned char SPI1_Receive(void);



#undef  EXPORT
#endif /* __ARMV10_SERBUS_H */

/******************* (C) HTL - HOLLABRUNN  2009-2010 *****END OF FILE****/
