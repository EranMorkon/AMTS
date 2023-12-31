
;******************************************************************************
; * @file    STM32_F103RB_MEM_MAP.IC
; * @author  Josef Reisinger
; * @version V2.0
; * @date    22-November-2017
; * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer Include File. 
; *          This file contains the most important peripheral register's definitions
;            and memory mapping for STM32F10x Performance line, 

PERIPH_BB_BASE      EQU     0x42000000        

PERIPH_BASE     	EQU  	0x40000000 
APB1PERIPH_BASE     EQU     PERIPH_BASE			
APB2PERIPH_BASE 	EQU  	PERIPH_BASE + 0x10000
AHBPERIPH_BASE  	EQU     PERIPH_BASE + 0x20000

GPIOA_BASE			EQU		APB2PERIPH_BASE + 0x0800
GPIOA_CRL 			EQU 	GPIOA_BASE
GPIOA_CRH 			EQU 	GPIOA_BASE+0x04
GPIOA_IDR 			EQU 	GPIOA_BASE+0x08
GPIOA_ODR 			EQU 	GPIOA_BASE+0x0c
GPIOA_BSRR 			EQU 	GPIOA_BASE+0x10
GPIOA_BRR 			EQU 	GPIOA_BASE+0x14
GPIOA_LCKR 			EQU 	GPIOA_BASE+0x18


GPIOB_BASE  		EQU     APB2PERIPH_BASE + 0x0C00
GPIOB_CRL 			EQU 	GPIOB_BASE
GPIOB_CRH 			EQU 	GPIOB_BASE+0x04
GPIOB_IDR 			EQU 	GPIOB_BASE+0x08
GPIOB_ODR 			EQU 	GPIOB_BASE+0x0c
GPIOB_BSRR 			EQU 	GPIOB_BASE+0x10
GPIOB_BRR 			EQU 	GPIOB_BASE+0x14
GPIOB_LCKR 			EQU 	GPIOB_BASE+0x18


GPIOC_BASE  		EQU     APB2PERIPH_BASE + 0x1000
GPIOC_CRL 			EQU 	GPIOC_BASE
GPIOC_CRH 			EQU 	GPIOC_BASE+0x04
GPIOC_IDR 			EQU 	GPIOC_BASE+0x08
GPIOC_ODR 			EQU 	GPIOC_BASE+0x0c
GPIOC_BSRR 			EQU 	GPIOC_BASE+0x10
GPIOC_BRR 			EQU 	GPIOC_BASE+0x14
GPIOC_LCKR 			EQU 	GPIOC_BASE+0x18


USART1_BASE     	EQU     APB2PERIPH_BASE + 0x3800
USART1_SR 			EQU 	USART1_BASE 
USART1_DR	 		EQU 	USART1_BASE+0x04
USART1_BRR	 		EQU 	USART1_BASE+0x08
USART1_CR1	 		EQU 	USART1_BASE+0x0C
USART1_CR2	 		EQU 	USART1_BASE+0x10
USART1_CR3	 		EQU 	USART1_BASE+0x14
USART1_GTPR	 		EQU 	USART1_BASE+0x18

USART2_BASE         EQU     APB1PERIPH_BASE + 0x4400
USART2_SR 			EQU 	USART2_BASE 
USART2_DR	 		EQU 	USART2_BASE+0x04
USART2_BRR	 		EQU 	USART2_BASE+0x08
USART2_CR1	 		EQU 	USART2_BASE+0x0C
USART2_CR2	 		EQU 	USART2_BASE+0x10
USART2_CR3	 		EQU 	USART2_BASE+0x14
USART2_GTPR	 		EQU 	USART2_BASE+0x18


RCC_BASE        	EQU     AHBPERIPH_BASE + 0x1000
RCC_CR 				EQU 	RCC_BASE
RCC_CFGR 			EQU 	RCC_BASE+0x04
RCC_CIR 			EQU 	RCC_BASE+0x08
RCC_APB2RSTR 		EQU 	RCC_BASE+0x0C
RCC_APB1RSTR 		EQU 	RCC_BASE+0x10
RCC_AHBENR 			EQU 	RCC_BASE+0x14
RCC_APB2ENR 		EQU 	RCC_BASE+0x18
RCC_APB1ENR 		EQU 	RCC_BASE+0x1C
RCC_BDCR 			EQU 	RCC_BASE+0x20
RCC_CSR 			EQU 	RCC_BASE+0x24
RCC_AHBRSTR 		EQU 	RCC_BASE+0x28
RCC_CFGR2 			EQU 	RCC_BASE+0x2C

RCC_APB2ENR_IOPAEN	EQU 0x0004           
RCC_APB2ENR_IOPBEN  EQU	0x0008           
RCC_APB2ENR_IOPCEN  EQU 0x0010           
RCC_APB2ENR_USART1EN EQU 0x4000			 


                END
				