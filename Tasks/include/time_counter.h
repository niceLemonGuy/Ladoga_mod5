/**
  ******************************************************************************
  * @file    time_counter.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    13/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
#ifndef __TIME_COUNTER_H
#define __TIME_COUNTER_H


#include "main.h"


/* Task ----------------------------------------------------------------------*/
__task void t_Time_Counter(void); // Счётчик локального времени


/* Task ID ------------------------------------------------------------------*/
extern OS_TID tid_Time_Counter;	// ID задачи t_Local_Time_Count


/* Global variables ----------------------------------------------------------------*/
extern U32 unixtime;
extern U32 localtime;



#endif	// __TIME_COUNTER_H
	
	




