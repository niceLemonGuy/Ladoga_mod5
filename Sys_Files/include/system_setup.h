/**
  ******************************************************************************
  * @file    system_setup.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    13/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */

  
#ifndef __SYSTEM_SETUP_H
#define __SYSTEM_SETUP_H


#include "main.h"


/* Privat defines -------------------------------------------------------------------*/
// Количество АЦП задействованных в этом устройстве
#define MAX_ADC_NUM			4

// ADC1 DR register base address
#define DR_ADDRESS      ((uint32_t)0x4001244C)
 
 
/* Global variables -----------------------------------------------------------------*/ 
extern U16 ADC_buf[];	// Буфер для измерений каналов АЦП 

 
 
 
#endif	// __SYSTEM_SETUP_H
  
  
  



