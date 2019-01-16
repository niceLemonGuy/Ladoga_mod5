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
	U8 i;
	bool OK = true;
	
	if((!Device_Status.on_line && !Device_Status.test_mode) || TAMPER_INPUT_READ())
	{
		// Индикация неудачной постановки
		Ind_Tm_peep_num(3);
		return; // Нет связи и не тестовые режим. Постановка на охрану запрещена
	}
	
	if(Device_Status.guard)
	{
		Ind_Tm_peep();
		return; // Устройство и так на охране
	}
	
	// Перечислить все возможные УО
	for(i = 0; i < MAX_UO_NUM; i++)
	{
		// Поиск УО по сетевому адресу
		if(RF_UO_Data[i].Info.Chan_info._Class == RF_CLASS_1)
		{// Найден УО
			if(RF_UO_Data[i].Status.CL_1.Ch_stat || RF_UO_Data[i].Status.CL_1.UO_stat.All8)
			{// УО не готов к постановке
				// Отправить сообщение о невзятии
				Event_Queue_Make(XPR_EVCLASS_ALARM, XPR_EVCLASS_ALARM_ARMFAIL, ACT_EVENT_START, DEVICE_ADDR, user);
				OK = false;
				break;
			}
		}
	}
	if(!OK)
	{
		// Поиск УО по сетевому адресу
		if(RF_UO_Data[i].Info.Chan_info._Class == RF_CLASS_1)
		{// Найден УО
			if(RF_UO_Data[i].Status.CL_1.Ch_stat || RF_UO_Data[i].Status.CL_1.UO_stat.All8)
			{// УО не готов к постановке
				// Отправить сообщение о неготовности УО
				Event_Queue_Make(XPR_EVCLASS_INFO, XPR_EVCLASS_INFO_UO_NOT_READY, ACT_EVENT_START, RF_UO_Data[i].Info.Chan_info.Ch_addr, 0);
				// Индикация неудачной постановки
				Ind_Tm_peep_num(3);
			}
		}
	}
	else
	{// Проверка на готовность пройдена
		// Отправить сообщение о постановке на охрану
		Event_Queue_Make(XPR_EVCLASS_ARM, XPR_EVCLASS_ARM_ARM, ACT_EVENT_START, DEVICE_ADDR, user);
		Device_Status.guard = true;	// Поднять флаг "ОХРАНА"
		Ind_Tm_led_num(30);	// Индикация постановки на охрану
		Ind_Tm_peep_time(2000);	// Звуковое оповещение постановки на охрану
		LED_GUARD_RED();
	}
}

/** 
	* @brief	Device_DISARM()
	* @note		Снятие прибора с охраны
	* @param	user: пользователь
	*/
void Device_DISARM(U8 user)
{
	if(!Device_Status.guard)
	{
		Ind_Tm_peep();
		return;
	}
	// Отправить сообщение о снятии с охраны
	Event_Queue_Make(XPR_EVCLASS_ARM, XPR_EVCLASS_ARM_ARM, ACT_EVENT_TERMINATION, DEVICE_ADDR, user);
	Device_Status.guard = false;	// Опустить флаг "ОХРАНА"
	Device_Status.alarm = false;	// Опустить флаг "ТРЕВОГА"
	Ind_Tm_peep();	// Оповестить об окончании снятия с охраны звуковым сигналом
	LED_GUARD_GREEN();
}


/*-----------------------------------------------------------------------------------
 *        Task 'Device_Handler'	* Обработка состояний прибора *
 *----------------------------------------------------------------------------------*/
__task void t_Device_Handler(void)
{	
	if(TEST_INPUT_READ())	// Кнопка тест отжата
	{
		LED_ACB_GREEN();
	}
	else
	{
		LED_ACB_RED();
	}
	
	for(;;)
	{
		os_dly_wait(200);
		
		// События по УОО
		if(Device_Status.old.all != Device_Status.new.all)
		{// Если изменился статус УОО
			if(!Device_Status.guard) // Не на охране
			{
				if(Device_Status.old.acb_err != Device_Status.new.acb_err) { // Состояние аккумуляторной батареи
					if(Event_Queue_Make(XPR_EVCLASS_POWER, XPR_EVCLASS_POWER_BATTLOW, Device_Status.new.acb_err, DEVICE_ADDR, 0)) // Батарея разряжена
						Device_Status.old.acb_err = Device_Status.new.acb_err; // Обновить статус 
				}
				if(Device_Status.old.v220_err != Device_Status.new.v220_err) { // Состояние основного питания
					if(Event_Queue_Make(XPR_EVCLASS_POWER, XPR_EVCLASS_POWER_RESERVE, Device_Status.new.v220_err, DEVICE_ADDR, 0)) // Переход на резервное элеткропитание
						Device_Status.old.v220_err = Device_Status.new.v220_err; // Обновить статус 
				}				
				if(Device_Status.old.sabotage != Device_Status.new.sabotage) { // Состояние корпуса
					if(Event_Queue_Make(XPR_EVCLASS_INTERNAL, XPR_EVCLASS_SABOTAGE_TAMPER, Device_Status.new.sabotage, DEVICE_ADDR, 0))  // Корпус вскрытие (не тревожное)
						Device_Status.old.sabotage = Device_Status.new.sabotage; // Обновить статус 
				}
			}
			else // На охране
			{
				if(Device_Status.old.acb_err != Device_Status.new.acb_err) { // Состояние аккумуляторной батареи
					if(Event_Queue_Make(XPR_EVCLASS_MALFUNCTION, XPR_EVCLASS_MALFUNCTION_SUPPLY, Device_Status.new.acb_err, DEVICE_ADDR, 0)) // Нарушено питание
						if(Event_Queue_Make(XPR_EVCLASS_POWER, XPR_EVCLASS_POWER_BATTLOW, Device_Status.new.acb_err, DEVICE_ADDR, 0)) // Батарея разряжена
							Device_Status.old.acb_err = Device_Status.new.acb_err; // Обновить статус 
				}
				if(Device_Status.old.v220_err != Device_Status.new.v220_err) { // Состояние основного питания
					if(Event_Queue_Make(XPR_EVCLASS_MALFUNCTION, XPR_EVCLASS_MALFUNCTION_SUPPLY, Device_Status.new.v220_err, DEVICE_ADDR, 0)) // Нарушено питание
						if(Event_Queue_Make(XPR_EVCLASS_POWER, XPR_EVCLASS_POWER_RESERVE, Device_Status.new.v220_err, DEVICE_ADDR, 0)) // Переход на резервное элеткропитание
							Device_Status.old.v220_err = Device_Status.new.v220_err; // Обновить статус 
				}				
				if(Device_Status.old.sabotage != Device_Status.new.sabotage) { // Состояние корпуса
					if(Event_Queue_Make(XPR_EVCLASS_SABOTAGE, XPR_EVCLASS_SABOTAGE_TAMPER, Device_Status.new.sabotage, DEVICE_ADDR, 0)) // Корпус вскрытие
						Device_Status.old.sabotage = Device_Status.new.sabotage; // Обновить статус 
				}
			}
		}
		
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
		
		/* Состояние сети АКБ *****************************************/
		if(TEST_INPUT_READ())	// Кнопка тест отжата
		{
			if(Device_Status.old.acb_err != 0)
			if(++chatter_cnt_test > CHATTER_REMOVE)
			{
				Device_Status.new.acb_err = 0;
				chatter_cnt_test = 0;
				LED_ACB_GREEN();
			}
			
		}
		else // Кнопка тест нажата
		{				
			if(Device_Status.old.acb_err == 0)
			if(++chatter_cnt_test > CHATTER_REMOVE)
			{
				Device_Status.new.acb_err = 1;
				chatter_cnt_test = 0;
				LED_ACB_RED();
			}
			
		}
		
//		if(ADC_buf[3] < 0x0fff)
//		{
//			if(Device_Status.old.acb_err == 0)
//				Device_Status.new.acb_err = 1;
//			LED_ACB_GREEN();
//		}
//		else
//		{
//			if(Device_Status.old.acb_err != 0)
//				Device_Status.new.acb_err = 0;
//			LED_ACB_RED();
//		}
		
//		if(localtime > acb_timer)
//		{
//			if(acb_err_cnt > 10)
//			{// АКБ отсутствует или разряжен
//				if(Device_Status.old.acb_err == 0)
//					Device_Status.new.acb_err = 1;
//				LED_ACB_RED();
//			}
//			else
//			{// АКБ в норме
//				if(Device_Status.old.acb_err != 0)
//					Device_Status.new.acb_err = 0;
//				LED_ACB_GREEN();
//			}
//			acb_err_cnt = 0;
//			acb_timer = localtime + ACB_LISTEN_TIMEOUT;
//		}
//		else
//		{
//			if(ADC_buf[1] < 0x0fff)
//			{
//				acb_err_cnt++;
//			}
//		}
		
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

	
	
	




