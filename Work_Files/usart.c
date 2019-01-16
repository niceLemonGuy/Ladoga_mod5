/**
  ******************************************************************************
  * @file    usart.c
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    13/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"


/* Variables -----------------------------------------------------------------*/
U8 USART1_Tx_buf[USART1_Tx_buf_SIZE], USART1_Rx_buf[USART1_Rx_buf_SIZE];
U16 USART1_Rx_count = 0, USART1_Rx_pointer = 0;
bool USART1_Rx_ready = true;

U8 USART3_Tx_buf[USART3_Tx_buf_SIZE], USART3_Rx_buf[USART3_Rx_buf_SIZE];
U16 USART3_Rx_count = 0, USART3_Rx_pointer = 0;
bool USART3_Rx_ready = true;

U8 UART5_Tx_buf[UART5_Tx_buf_SIZE], UART5_Rx_buf[UART5_Rx_buf_SIZE];
U16 UART5_Tx_count = 0, UART5_Tx_pointer = 0, UART5_Rx_count = 0, UART5_Rx_pointer = 0;
bool UART5_Tx_ready = true;

	
/* Functions -----------------------------------------------------------------*/
/**
	*	@brief	USART1_Send()
	*	@note		Send message via serial port 1
	* @param  data: указатель на буфер отправки
	* @param  data_len: размер сообщения
	*/
void USART1_Send(U8 *data, uint8_t data_len)
{
	memcpy(USART1_Tx_buf, data, data_len);	
  DMA_SetCurrDataCounter (DMA1_Channel4, data_len);
	DMA_Cmd (DMA1_Channel4, ENABLE);
}

/**
	*	@brief	USART1_Rx_Clear()
	*	@note		Clear USART1 Rx buffer
	*/
void USART1_Rx_Clear(void)
{
	memset(USART1_Rx_buf, 0, USART1_Rx_buf_SIZE);
	USART1_Rx_count = NULL;
}

/**
	*	@brief	USART3_Send()
	*	@note		Send message via serial port 3
	* @param  data: указатель на буфер отправки
	* @param  data_len: размер сообщения
	*/
void USART3_Send(U8 *data, U16 data_len)
{
	memcpy(USART3_Tx_buf, data, data_len);	
  DMA_SetCurrDataCounter (DMA1_Channel2, data_len);
	DMA_Cmd (DMA1_Channel2, ENABLE);
}

/**
	*	@brief	USART3_Rx_Clear()
	*	@note		Clear USART3 Rx buffer
	*/
void USART3_Rx_Clear(void)
{
	memset(USART3_Rx_buf, 0, USART3_Rx_buf_SIZE);
	USART3_Rx_count = NULL;
}

/**
	*	@brief	UART5_Send()
	*	@note		Send message via serial port 5
	* @param  data: указатель на буфер отправки
	* @param  data_len: размер сообщения
	*/
void UART5_Send(U8 *data, U16 data_len)
{
	U8 err_count = 0;
	//Очистить буфер передачи
	memset(UART5_Tx_buf, 0, UART5_Tx_buf_SIZE);
	//Копировать данные в буфер передачи
	memcpy(UART5_Tx_buf, data, data_len);
	UART5_Tx_count = data_len;
	//Включить прерывание на передачу
	USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
	os_dly_wait(1);
	//Ждать окончания передачи или сброс по счётчику
	while(!UART5_Tx_ready && ++err_count < 100)
	{
		os_dly_wait(1);
	}
}

/**
	*	@brief	UART5_Rx_Clear()
	*	@note		Clear UART5 Rx buffer
	*/
void UART5_Rx_Clear(void)
{
	memset(UART5_Rx_buf, 0, UART5_Rx_buf_SIZE);
	UART5_Rx_count = NULL;
}

/**
	*	@brief	UART5_recv_ok()
	*	@note		Read receive status
	* @retval Результат
	*/
bool UART5_recv_stat(void)
{
	return USART_GetITStatus(UART5, USART_IT_RXNE);
}






