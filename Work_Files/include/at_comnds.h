/**
  ******************************************************************************
  * @file    at_comnds.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    10/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT_COMNDS_H
#define __AT_COMNDS_H


#include "main.h"
#include "usart.h"


/* Types ---------------------------------------------------------------------*/
typedef enum _Main_AT_Status_t
{
	AT_Modem_Start,
	AT_Socket_Open,
	AT_Socket_Ready,
	AT_Socket_Send,
	AT_Socket_Receive,
	AT_Socket_Close,
}Main_AT_Status_t;

typedef enum _Aux_AT_Status_t
{
	AT_Modem_Poll,
	AT_Balance
}Aux_AT_Status_t;

typedef enum _AT_Start_Schedule_t
{
	ATE0,									// Отключить эхо
	AT_I4,								// Информация о модеме
	AT_SELINT,						// Выбор стиля интерфейса
	AT_CMEE,							// Подробность выдачи информации от модема
	AT_K0,								// Флоу контроль
	AT_SLED,							// Режим работы светодиода
	AT_CPIN,							// Готовность SIM карты
	AT_CPBR,							// Запросить телефонную книгу SIM карты
	AT_SII								// Включить aux канал
}AT_Start_Schedule_t;

typedef enum _AT_Socket_Open_Schedule_t
{
	AT_SS,								// Запрос статуса сокета
//	AT_SGACT_REQ,					// Запрос статуса PDP context
	AT_COPS,							// Имя оператора связи
	AT_SERVINFO,					// Информация о сервере
	AT_MONI,							// Мониторинг сети
	AT_CGDCONT,						// Определить PDP context
	AT_SGACTCFGEXT,				// Разрешить прерывание PDP context
	AT_SGACT,							// Включить PDP context
	AT_SCFGEXT,						// Настройка конфигураций сокета
	AT_SD,								// Открытие сокета
}AT_Socket_Open_Schedule_t;

typedef enum _AT_Socket_Send_Schedule_t
{
	AT_SSENDEXT,					// Отправить информацию о сообщении	
	AT_Msg,								// Сообщение
}AT_Socket_Send_Schedule_t;

typedef enum _AT_Socket_Receive_Schedule_t
{
	AT_SRING,							// Принять подтверждение о получении сообщения
	AT_SRECV							// Принять сообщение через сокет
}AT_Socket_Receive_Schedule_t;

typedef enum _AT_Socket_Close_Schedule_t
{
	AT_SH,								// Закрыть сокет
	//AT_SGACT_PDP_OFF,			// Выключить PDP context
}AT_Socket_Close_Schedule_t;

typedef enum _AT_Modem_Poll_Schedule_t
{
	AT_CSQ,								// Информация о модеме
	AT_CREG,							// Запрос статуса регистрации в сети GSM
	AT_CGREG							// Запрос статуса регистрации в сети GPRS
}AT_Modem_Poll_Schedule_t;

typedef enum _AT_Balance_Schedule_t
{
	AT_CUSD,							// Оправить запрос баланса
	AT_USSD_Receive				// Принять USSD
}AT_Balance_Schedule_t;


/* Structs -------------------------------------------------------------------*/
typedef struct _Main_AT_Command_t
{
	Main_AT_Status_t status;
	U8 number;
	bool busy;
	char* sent;
	U32 wait_time;
	U8 err_cnt;
}Main_AT_Command_t;

typedef struct _Aux_AT_Command_t
{
	Aux_AT_Status_t status;
	U8 number;
	bool busy;
	char* sent;
	U32 wait_time;
}Aux_AT_Command_t;

typedef struct _AT_Socket_Tx_Msg_t
{
	U8* buff;
	U16 ilen;
	char clen[4];
	bool busy;
}AT_Socket_Tx_Msg_t;

typedef struct _AT_Socket_Rx_Msg_t
{
	bool msg_received;
	U8 buff[USART3_Rx_buf_SIZE];
	U16 len;
}AT_Socket_Rx_Msg_t;

typedef struct _Modem_Info_t
{
	char _model[20];
	char _operator[10];
	char _csq[3];
	char _PDP_addr[30];
	char _ussd_balance[10];
	char _ip[16];
	char _port[6];
	char _gsm_stat;
	char _gprs_stat;
}Modem_Info_t;

typedef struct _Balance_t
{
	char msg[252];
	U8 msg_len;
}Balance_t;

/* Global variables ----------------------------------------------------------*/


/* Global Structs ------------------------------------------------------------*/


/* Global Functions ----------------------------------------------------------*/



#endif // __AT_COMNDS_H




