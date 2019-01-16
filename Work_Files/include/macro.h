/**
  ******************************************************************************
  * @file    macro.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    11/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  */
#ifndef __MACRO_H
#define __MACRO_H


#include "main.h"
#include "gpio_defines.h"



/** @breaf Макросы для работы со светодиодами @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
#define LED_INS_OFF()					{ GPIO_SetBits(INS_GREEN_PORT, INS_GREEN_PIN); GPIO_SetBits(INS_RED_PORT, INS_RED_PIN); }
#define LED_INS_RED()					{ GPIO_SetBits(INS_GREEN_PORT, INS_GREEN_PIN); GPIO_ResetBits(INS_RED_PORT, INS_RED_PIN); }
#define LED_INS_GREEN()				{ GPIO_ResetBits(INS_GREEN_PORT, INS_GREEN_PIN); GPIO_SetBits(INS_RED_PORT, INS_RED_PIN); }
#define LED_ACB_OFF()					{ GPIO_SetBits(ACB_GREEN_PORT, ACB_GREEN_PIN); GPIO_SetBits(ACB_RED_PORT, ACB_RED_PIN); }
#define LED_ACB_RED()					{ GPIO_SetBits(ACB_GREEN_PORT, ACB_GREEN_PIN); GPIO_ResetBits(ACB_RED_PORT, ACB_RED_PIN); }
#define LED_ACB_GREEN()				{ GPIO_ResetBits(ACB_GREEN_PORT, ACB_GREEN_PIN); GPIO_SetBits(ACB_RED_PORT, ACB_RED_PIN); }
#define LED_V220_OFF()				{ GPIO_SetBits(V220_GREEN_PORT, V220_GREEN_PIN); GPIO_SetBits(V220_RED_PORT, V220_RED_PIN); }
#define LED_V220_RED()				{ GPIO_SetBits(V220_GREEN_PORT, V220_GREEN_PIN); GPIO_ResetBits(V220_RED_PORT, V220_RED_PIN); }
#define LED_V220_GREEN()			{ GPIO_ResetBits(V220_GREEN_PORT, V220_GREEN_PIN); GPIO_SetBits(V220_RED_PORT, V220_RED_PIN); }
#define LED_GUARD_OFF()				{ GPIO_SetBits(GUARD_GREEN_PORT, GUARD_GREEN_PIN); GPIO_SetBits(GUARD_RED_PORT, GUARD_RED_PIN); }
#define LED_GUARD_RED()				{ GPIO_SetBits(GUARD_GREEN_PORT, GUARD_GREEN_PIN); GPIO_ResetBits(GUARD_RED_PORT, GUARD_RED_PIN); }
#define LED_GUARD_GREEN()			{ GPIO_ResetBits(GUARD_GREEN_PORT, GUARD_GREEN_PIN); GPIO_SetBits(GUARD_RED_PORT, GUARD_RED_PIN); }
#define ALL_LED_OFF()					{ LED_INS_OFF(); LED_ACB_OFF(); LED_V220_OFF(); LED_GUARD_OFF(); }
#define ALL_RED_LED()					{ LED_INS_RED(); LED_ACB_RED(); LED_V220_RED(); LED_GUARD_RED(); }
#define ALL_GREEN_LED()				{ LED_INS_GREEN(); LED_ACB_GREEN(); LED_V220_GREEN(); LED_GUARD_GREEN(); }

/** @breaf Макросы для опроса пинов @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
#define TEST_INPUT_READ()			GPIO_ReadInputDataBit(TEST_PORT, TEST_PIN)	// Считать пин TEST
#define TAMPER_INPUT_READ()		GPIO_ReadInputDataBit(TAMPER_PORT, TAMPER_PIN)	// Считать пин TAMPER

/** @breaf Макросы для работы с Touch_Memory @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
#define TM_LED_ON()				GPIO_SetBits(TM_VD_PORT, TM_VD_PIN);			// Зажечь светодиод ТМ
#define TM_LED_OFF()			GPIO_ResetBits(TM_VD_PORT, TM_VD_PIN);		// Погасить светодиод ТМ
#define TM_ZUM_ON()				GPIO_SetBits(TM_ZUM_PORT, TM_ZUM_PIN);		// Включить звуковую индикацию
#define TM_ZUM_OFF()			GPIO_ResetBits(TM_ZUM_PORT, TM_ZUM_PIN);	// Выключить звуковую индикацию

/** @breaf Макросы для работы с модемом @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
#define SELECT_SIM1()			GPIO_ResetBits(SIMSEL_PORT, SIMSEL_PIN);	// Работать с SIM1
#define SELECT_SIM2()			GPIO_SetBits(SIMSEL_PORT, SIMSEL_PIN);		// Работать с SIM2
#define READ_SIM_NUM()		GPIO_ReadInputDataBit(SIMSEL_PORT, SIMSEL_PIN)	// Узнать какая симкарта активна считав пин SIMSEL
#define CHANGE_SIM()			{ if(READ_SIM_NUM()) SELECT_SIM1() else SELECT_SIM2() }	// Поменять SIM карту


#endif // __MACRO_H
	



