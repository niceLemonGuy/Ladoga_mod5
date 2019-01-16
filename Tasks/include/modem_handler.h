/**
  ******************************************************************************
  * @file    modem_handler.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    14/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
	

#ifndef __MODEM_HANDLER_H
#define __MODEM_HANDLER_H


#include "main.h"
#include "at_comnds.h"


/* Defines -------------------------------------------------------------------*/
#define MODEM_WAIT_TIME 				10	// Ожидание ответа от модема [сек]
#define MODEM_MAX_LINK_ERR			5		// Допустимое кол-во ошибок связи


/* Task ---------------------------------------------------------------------*/
__task void t_Main_Modem_Handler(void);	// Работа с модемом по main каналу
__task void t_Aux_Modem_Handler(void);	// Работа с модемом по aux каналу


/* Task ID ------------------------------------------------------------------*/
extern OS_TID tid_Main_Modem_Handler;	// ID задачи t_Main_Modem_Handler
extern OS_TID tid_Aux_Modem_Handler;	// ID задачи t_Aux_Modem_Handler


/* Global Functions ---------------------------------------------------------*/
bool gprs_write_msg(U8* buff, U16 len);	// Отправить сообщение по gprs
bool gprs_recv_msg(U8* buff);						// Получить принятое по gprs сообщение 
void msg_processed(void);								// Сообщение обработано
void select_sim1(void);									// Работать с SIM1
void select_sim2(void);									// Работать с SIM2
void change_sim(void);									// Поменять SIM карту


/* Structs ------------------------------------------------------------------*/
typedef struct _Modem_Stat_t
{
	U8 aux_on: 1;		// 0:Aux не запущен		1:Aux запущен
	U8 err_cnt: 3;	// Счётчик ошибок при работе с активной SIM картой
}Modem_Stat_t;



#endif	// __MODEM_HANDLER_H
	
	
	

	



