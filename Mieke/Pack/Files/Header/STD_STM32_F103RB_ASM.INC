;******************************************************************************
;*   (C) Copyright HTL - HOLLABRUNN  2009-2010 All rights reserved. AUSTRIA   * 
;*                                                                            * 
;* File Name:   std32_f103rb_std.inc                                          *
;* Autor: 		Josef Reisinger                                               *
;* Version: 	V1.00                                                         *
;* Date: 		25/10/2010                                                    *
;* Description: Standard Library für ARM Corttex M3                           *
;******************************************************************************
;* History: V1.00  creation										              *
;* 			                                                                  *
;******************************************************************************
			
	EXTERN init_leds_switches				
	EXTERN set_leds
	EXTERN get_actual_switches
	EXTERN wait_ms
	EXTERN wait_10us
	EXTERN uart_init	
	EXTERN uart_put_char	  
	EXTERN uart_get_char
	EXTERN uart_put_string
	EXTERN uart_clear
	EXTERN uart_put_hex
	EXTERN nib2asc
	EXTERN hex2bcd	
	EXTERN asc2nib

					
	END				