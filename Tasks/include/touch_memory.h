/**
  ******************************************************************************
  * @file    touch_memory.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    13/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
	

#ifndef __TOUCH_MEMORY_H
#define __TOUCH_MEMORY_H


#include "main.h"


/* Task ---------------------------------------------------------------------*/
__task void t_Touch_Memory(void);	// Работа с устройствами ввода с помощью 1-Wire интерфейса


/* Task ID ------------------------------------------------------------------*/
extern OS_TID tid_Touch_Memory;	// ID задачи t_Touch_Memory


/* Defines ------------------------------------------------------------------*/
#define TM_BUF_LEN		8




#endif	// __TOUCH_MEMORY_H
	
	
	




