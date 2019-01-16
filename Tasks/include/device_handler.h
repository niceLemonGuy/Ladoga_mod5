/**
  ******************************************************************************
  * @file    device_handler.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    13/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
#ifndef __DEVICE_HANDLER_H
#define __DEVICE_HANDLER_H


#include "main.h"


/* Task -----------------------------------------------------------------------------*/
__task void t_Device_Handler(void);	// Обработка состояний прибора


/* Task ID --------------------------------------------------------------------------*/
extern OS_TID tid_Device_Handler;	// ID задачи t_UO_Status


/* Defines -----------------------------------------------------------------------*/
#define CHATTER_REMOVE		20


/* Structures -----------------------------------------------------------------------*/
// Набор системных статусов и переменных ветвления
#pragma anon_unions

typedef enum _Device_Arm_Stat_t
{
	ARM_OFF,
	ARM_ON,
	ARM_FAIL
}Device_Arm_Stat_t;

typedef union _Device_Status_u
{
	struct {
		U8 v220_err: 1;	// 0:Норма		1:Нарушение основного питания
		U8 acb_err: 1;	// 0:Норма		1:Нарушение резервного питания
		U8 sabotage: 1;	// 0:Норма		1:Вскрытие корпуса
		U8 reserv: 3;
	};
	U8 all;
}Device_Status_u;

typedef struct _Device_Status_t
{
	Device_Status_u new;	// Новый статус прибора
	Device_Status_u old;	// Старый статус прибора
	U32 alarm: 1;		// 0:Норма			1:Тревога
	U32 guard: 1;		// 0:Снят с охраны		1:На охране
	U32 test_mode: 1;	// 0:Рабочий режим		1:Тестовый режим
	U32 on_line: 1;		// 0:Нет связи			1:Есть связь
	U32 session_key: 1;	// 0:Нет сессионного ключа	1:Есть сессионный ключ
	U32 was_connectd: 1;	// 0:Первое поключение		1:Уже подключались к пульту
	U32 link_msg_sent: 1;	// 0:Сообщение о восстановление связи НЕ отправлено 	1:Отправлено
}Device_Status_t;

// Информация о приборе. Структура дублируется во FLASH памяти
typedef struct _Device_Data_t
{
	U8 uo_id[3];		// Назначается при подключении нового УОО, может совпадать с obj_id три младших бита uo_id - № канала связи (всего может быть от 1 до 7 каналов и 2 097 151 uo_id)
	U8 obj_id[3];		// ID объекта охраны (через 1 коммуникатор может контролироваться несколько объектов)
	U8 def_key[16];		// Мастер ключ
	U8 brshs_ser_num[2];	// Серийный номер БРШС-РК
}Device_Data_t;

/* Global variables -----------------------------------------------------------------*/ 
extern Device_Status_t Device_Status;	// Набор системных статусов и переменных ветвления
extern Device_Data_t Device_Data;	//Зеркало данных хранящихся во FLASH


/* Global functions -----------------------------------------------------------------*/
void Device_ARM(U8 user);	// Постановка прибора на охрану
void Device_DISARM(U8 user);	// Снятие прибора с охраны
void Device_Test_Check(void);	// Проверка на тестовый режим
void Device_Test_Handler(char *vers_num);	// Работа прибора в тестовом режиме


#endif // __DEVICE_HANDLER_H
	
	



