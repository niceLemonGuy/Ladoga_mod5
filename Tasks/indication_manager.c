/**
  ******************************************************************************
  * @file    indication_manager.c
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    11/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */

/* Includes ------------------------------------------------------------------------*/
#include "includes.h"


/* Task ID -------------------------------------------------------------------------*/
OS_TID tid_Indication_Manager;		// Управление индикацией устройства


/* Global variables ----------------------------------------------------------------*/
enum e_Indication_mod_stat Indication_mod_stat = Ind_Norm_mod; // Режим работы индикации 
	
	
/* Functions -----------------------------------------------------------------------*/
/**
  * @brief  Ind_Tm_peep();
	*	@note		Один раз пискнуть зуммером и моргнуть светодиодом на клавиатуре
	*/
void Ind_Tm_peep(void)
{
	TM_LED_ON();
	TM_ZUM_ON();
	os_dly_wait(90);
	TM_LED_OFF();
	TM_ZUM_OFF();
}

/**
  * @brief  Ind_Tm_peep_num();
	*	@note		Пискнуть зуммером и моргнуть светодиодом на клавиатуре заданное кол-во раз
	*/
void Ind_Tm_peep_num(U8 num)
{
	while(num--)
	{
		TM_LED_ON();
		TM_ZUM_ON();
		os_dly_wait(90);
		TM_LED_OFF();
		TM_ZUM_OFF();
		os_dly_wait(90);
	}
}

/**
  * @brief  Ind_Tm_led_num();
	*	@note		Моргнуть светодиодом на клавиатуре заданное кол-во раз
	*/
void Ind_Tm_led_num(U8 num)
{
	while(num--)
	{
		TM_LED_ON();
		os_dly_wait(90);
		TM_LED_OFF();
		os_dly_wait(90);
	}
}

/**
  * @brief  Ind_Tm_peep_time();
	*	@note		Пищать зуммером и гореть светодиодом на клавиатуре заданное кол-во миллисекунд
	*/
void Ind_Tm_peep_time(U16 time)
{
	TM_LED_ON();
	TM_ZUM_ON();
	os_dly_wait(time);
	TM_LED_OFF();
	TM_ZUM_OFF();
}

/*-----------------------------------------------------------------------------------
 *        Task 'Indication_Manager' * Управление индикацией устройства *
 *----------------------------------------------------------------------------------*/
__task void t_Indication_Manager(void)
{	
	U32 alarm_wait_time = 0;
	bool alarm_old_stat = false;
	
	if(!Device_Status.guard)
	{
		LED_GUARD_GREEN();
	}
	
	for(;;)
	{		
		os_dly_wait(200);
		
		switch(Indication_mod_stat)
		{
			case Ind_Norm_mod:	// Обычный режим, режим ожидания
				break;
			case Ind_TM_Pressed_key: // Индикация при нажатии клавиши
				Ind_Tm_peep();
				Indication_mod_stat = Ind_Norm_mod;
				if(Device_Status.alarm)
					alarm_wait_time = localtime + IND_TM_KEY_PRESSD_ALARM_WAIT;
				break;
			case Ind_TM_Cmnd_recv: // Принята команда от клавиатуры
				Ind_Tm_peep_num(2);
				Indication_mod_stat = Ind_Norm_mod;
				break;
		}
		
		// Тревожная индикация
		if(Device_Status.guard && Device_Status.alarm && localtime > alarm_wait_time)
		{
			LED_GUARD_RED();
			Ind_Tm_peep_time(500);
			LED_GUARD_OFF();
			alarm_wait_time = localtime;
			
			if(alarm_old_stat == false)
			{
				alarm_old_stat = true;
			}
		}
		else if(!Device_Status.guard && alarm_old_stat == true)
		{
			alarm_old_stat = false;
		}
	}
}



