/**
  ******************************************************************************
  * @file    usart.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    10/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
#ifndef __USART_H
#define __USART_H


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "defines.h"


/* Defines -------------------------------------------------------------------*/
#define USART1_Tx_buf_SIZE			MAX_TCP_PKT_LEN
#define USART1_Rx_buf_SIZE			MAX_TCP_PKT_LEN
#define USART3_Tx_buf_SIZE			MAX_TCP_PKT_LEN
#define USART3_Rx_buf_SIZE			MAX_TCP_PKT_LEN
#define UART5_Tx_buf_SIZE				128
#define UART5_Rx_buf_SIZE				128
	
	
/* Global variables ----------------------------------------------------------*/
extern U8 USART1_Tx_buf[USART1_Tx_buf_SIZE], USART1_Rx_buf[USART1_Rx_buf_SIZE];
extern U16 USART1_Rx_count, USART1_Rx_pointer;
extern bool USART1_Rx_ready;

extern U8 USART3_Tx_buf[USART3_Tx_buf_SIZE], USART3_Rx_buf[USART3_Rx_buf_SIZE];
extern U16 USART3_Rx_count, USART3_Rx_pointer;
extern bool USART3_Rx_ready;

extern U8 UART5_Tx_buf[UART5_Tx_buf_SIZE], UART5_Rx_buf[UART5_Rx_buf_SIZE];
extern U16 UART5_Tx_count, UART5_Tx_pointer, UART5_Rx_count, UART5_Rx_pointer;
extern bool UART5_Tx_ready, UART5_Rx_ready;

	

/* Global functions -----------------------------------------------------------*/
void USART1_Send(U8 *data, U8 data_len);
void USART1_Rx_Clear(void);
void USART3_Send(U8 *data, U16 data_len);
void USART3_Rx_Clear(void);
void UART5_Send(U8 *data, U16 data_len);
void UART5_Rx_Clear(void);
bool UART5_recv_stat(void);
	
	
#endif	// __USART_H
