/**
  ******************************************************************************
  * @file    wt_uart_COM1.h
  * @author  zhang manxin
  * @version V1.0.0
  * @date    2014-12-09
  * @brief   This file contains all the functions prototypes for the uart_COM1.
  ******************************************************************************
  */ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __WT_UART_COM1_H
#define  __WT_UART_COM1_H


/*
*********************************************************************************************************
*                                              EXTERNS
*********************************************************************************************************
*/

#ifdef   WT_UART_COM1_GLOBALS
#define  WT_UART_COM1_EXT
#else
#define  WT_UART_COM1_EXT  extern
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
#define UART_COM1_RX_Bufer_Length			100
#define UART_COM1_RX_Timeout					100

#define UartCOM1_RX_Event							1
#define UartCOM1_TX_Event							2
#define UartCOM1_TXNG_Event						3

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

void UARTCOM1Thread(void const * argument);
void WT_UART_COM1_Init(void);
static void UartCOM1_Send_StartPrint_Cmd(void);
static void UartCOM1_Send_Print_Cmd(uint16_t cmd_line);
WT_UART_COM1_EXT uint8_t UART_COM1_Buffer_Rx[UART_COM1_RX_Bufer_Length];
WT_UART_COM1_EXT uint8_t UART_COM1_Cnt_Buffer_Rx;
//WT_UART_COM1_EXT uint8_t Is_UART_COM1_Rx_Come;


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif
