/**
  ******************************************************************************
  * @file    indication_manager.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    13/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */

#ifndef __INDICATION_MANAGER_H
#define __INDICATION_MANAGER_H


#include "main.h"


#define IND_TM_KEY_PRESSD_ALARM_WAIT		5	// Приостановить тревожную индикацию на время набора кода команды на клавиатуре [сек]


/* Task -----------------------------------------------------------------------------*/
__task void t_Indication_Manager(void);	// Управление индикацией устройства


/* Task ID --------------------------------------------------------------------------*/
extern OS_TID tid_Indication_Manager;		// ID задачи t_Indication_Manager


/** @brief Режим работы индикации @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
enum e_Indication_mod_stat
{
	Ind_Norm_mod =0,				// Обычный режим, режим ожидания
	Ind_TM_Pressed_key =1,	// Индикация при нажатии клавиши на клавиатуре
	Ind_TM_Cmnd_recv =2			// Принята команда от клавиатуры
};


/* Global variables -----------------------------------------------------------------*/ 
extern enum e_Indication_mod_stat Indication_mod_stat; // Режим работы индикации 


/* Global functions -----------------------------------------------------------------*/ 
void Ind_Tm_peep(void);	// Один раз пискнуть зуммером и моргнуть светодиодом на клавиатуре
void Ind_Tm_peep_num(U8 num);	// Пискнуть зуммером и моргнуть светодиодом на клавиатуре заданное кол-во раз
void Ind_Tm_peep_time(U16 time);	// Пищать зуммером и гореть светодиодом на клавиатуре заданное кол-во миллисекунд
void Ind_Tm_led_num(U8 num);	// Моргнуть светодиодом на клавиатуре заданное кол-во раз 



#endif	// __INDICATION_MANAGER_H





