/**
  ******************************************************************************
  * @file    device_status.c
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    13/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
	

/* Includes ------------------------------------------------------------------------*/
#include "includes.h"


#define ACB_LISTEN_TIMEOUT		30	// Время чтения АЦП аккумулятора


/* Task ID -------------------------------------------------------------------------*/
OS_TID tid_Device_Handler;	// ID задачи t_Device_Handler
	

/* Global structures ----------------------------------------------------------------*/
Device_Status_t Device_Status; // Набор системных статусов и переменных ветвления
Device_Data_t Device_Data;	// Информация о приборе. Структура дублируется во FLASH памяти


/* Private variables ---------------------------------------------------------------*/
U8 chatter_cnt_tamper = 0;
U8 chatter_cnt_test = 0;
char test_str[100] = {0};


/* Private defines -----------------------------------------------------------------*/


/* Functions -----------------------------------------------------------------------*/
/** 
	* @brief	Device_Clear()
	* @note		Стереть данные авторизации устройства
	*	@retval Результат
	*/
void Device_Clear(void)
{
	if(!TAMPER_INPUT_READ())
	{
		Flash_Erase_Device_Data();
		ALL_RED_LED();
		os_dly_wait(1000);
	}
	else 
	{
		// Проверка светодиодов
		ALL_RED_LED();
		os_dly_wait(1000);
		ALL_GREEN_LED();
		os_dly_wait(1000);
		ALL_LED_OFF();
	}
}

/** 
	* @brief	Device_Test_Check()
	* @note		Проверка на тестовый режим
	*/
void Device_Test_Check(void)
{
	if(!TEST_INPUT_READ())
	{
		Device_Status.test_mode = true;
		
		// Стереть устройство
		Device_Clear();
	}
}

/** 
	* @brief	make_test_msg()
	* @note		Создать тестовое сообщение
	* @param	vers_num: указатель на номер версии
	*/
void make_test_msg(char *vers_num)
{
	char SN_10[10] = {0};
	uint32_t ser_num = Serial_Number();
	// Очистить буфер
	memset(test_str, 0, sizeof(test_str));
	// Конвертация из чисел в символы
	itoa(ser_num, SN_10);
	// Добавить информацию о SIM карте
	if(!READ_SIM_NUM())	// Узнать номер SIM карты
		strcat(test_str, "SIM_1 ");
	else 
		strcat(test_str, "SIM_2 ");
	// Добавить серийный номер
	strcat(test_str, "SN:");
	strcat(test_str, SN_10);
	// Добавить версию программы
	strcat(test_str, " Ver:");
	strcat(test_str, vers_num);
	strcat(test_str, "\n");
}

/** 
	* @brief	Device_Test_Handler()
	* @note		Работа прибора в тестовом режиме
	* @param	vers_num: указатель на номер версии
	*/
void Device_Test_Handler(char *vers_num)
{	
	bool sent = false;
	if(Device_Status.test_mode)
	{// Если прибор в тестовом режиме
		
		// Проверка GSM
		while(1)
		{
			os_dly_wait(1000);

			if(Device_Status.on_line && !sent)
			{// Есть связь
				LED_INS_GREEN();
				// Создать тестовую строку
				make_test_msg(vers_num);
				// Отправить по GPRS
				gprs_write_msg((U8 *)test_str, strlen(test_str));
				sent = true;
			}
			else
			{
				sent = false;
				LED_INS_RED();
			}
		}
	}
}

/** 
	* @brief	Device_ARM()
	* @note		Постановка прибора на охрану
	* @param	user: пользователь
	*/
void Device_ARM(U8 user)
{
}

/** 
	* @brief	Device_DISARM()
	* @note		Снятие прибора с охраны
	* @param	user: пользователь
	*/
void Device_DISARM(U8 user)
{
}


/*-----------------------------------------------------------------------------------
 *        Task 'Device_Handler'	* Обработка состояний прибора *
 *----------------------------------------------------------------------------------*/
__task void t_Device_Handler(void)
{	
	for(;;)
	{
		os_dly_wait(200);
		
		// Start ADC1 Software Conversion
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		
		/* Cостояние тампера *******************************************/
		if(TAMPER_INPUT_READ())	// Кнопка тампера отжата
		{
			if(Device_Status.old.sabotage == 0)
			if(++chatter_cnt_tamper > CHATTER_REMOVE)
			{
				Device_Status.new.sabotage = 1;
				chatter_cnt_tamper = 0;
			}
			if(Device_Status.test_mode)
			{
				TM_LED_ON();
			}
		}
		else // Кнопка тампера нажата
		{
			if(Device_Status.old.sabotage != 0)
			if(++chatter_cnt_tamper > CHATTER_REMOVE)
			{
				Device_Status.new.sabotage = 0;
				chatter_cnt_tamper = 0;
				TM_LED_OFF();
			}
		}
		
		/* Состояние сети 220В *****************************************/
		if(!ADC_buf[0])
		{
			if(Device_Status.old.v220_err == 0)
				Device_Status.new.v220_err = 1;
			LED_V220_RED();
		}
		else
		{
			if(Device_Status.old.v220_err != 0)
				Device_Status.new.v220_err = 0;
			LED_V220_GREEN();
		}
	}
}

	
	
	




