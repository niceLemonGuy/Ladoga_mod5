/**
  ******************************************************************************
  * @file    at_comnds.c
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    13/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  * <h2><center>&copy; COPYRIGHT 2018 Roman_Yudin</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"


/* Defines -------------------------------------------------------------------*/
#define MODEM_WAIT_TIME 10	// Ожидание ответа от модема 10секунд


/* Variables -----------------------------------------------------------------*/
char sms_buf[512] = {0};	// Буфер для хранения SMS
int sms_len;


/* Enums ---------------------------------------------------------------------*/
AT_Status_t AT_Status = 0;																			// Статус сокета
AT_Start_Schedule_t AT_Start_Schedule = 0;											// Начало работы с модемом
AT_Balance_Schedule_t AT_Balance_Schedule = 0;									// Запрос баланса
AT_Socket_Open_Schedule_t AT_Socket_Open_Schedule = 0;					// Порядок открытия сокета
AT_Socket_Send_Schedule_t AT_Socket_Send_Schedule = 0;					// Порядок передачи сообщения через сокет
AT_Socket_Close_Schedule_t AT_Socket_Close_Schedule = 0;				// Порядок закрытия сокета


/* Structs -------------------------------------------------------------------*/
AT_Command_t AT_Command;
AT_Socket_Msg_t AT_Socket_Msg;
Modem_Info_t Modem_Info;
_SMS_t _SMS;


/* Functions -----------------------------------------------------------------*/
/**
	*	@brief	clear_modem_stats()
	*	@note		Сброс всех планировщиков
	*/
void clear_modem_stats(void)
{
	AT_Status = 0;
	AT_Start_Schedule = 0;
	AT_Balance_Schedule = 0;
	AT_Socket_Open_Schedule = 0;
	AT_Socket_Send_Schedule = 0;
	AT_Socket_Close_Schedule = 0;
}

/**
	*	@brief	AT_ERROR_OK_Handler()
	*	@note		Обработчик признаков окончания обработки команд модемом
	*	@param	data_point: указатель на принятые данные
	*/
U8 AT_ERROR_OK_Handler(char* data_point)
{
	int res = 0, success = 1;
	struct slre_cap caps = {0};
	
	res = slre_match("(OK|ERROR)", data_point, strlen(data_point), &caps, 1, 0); // Найти признак окончания обработки команды модемом
	if(res > 0)
	{	// Признак найден
		if(strstr(data_point, "OK"))
		{	// Команда успешно обработана
			success = 0;	// Успех
		}
		else if(strstr(data_point, "ERROR"))
		{	// Ошибка при обработки команды
			clear_modem_stats();	// Сбросить все планировщики
			AT_Send("AT#ENHRST=1,0\r", NULL, AT_Status, AT_Socket_Close_Schedule); // Перезагрузить модем
		}
		clean_rx_buf();	// Очистить приёмный буфер и флаги
	}
	return success;
}

/**
	*	@brief	AT_Command_Handler()
	*	@note		Обработчик ответов модема на AT команды
	*	@param	stat: статус работы с модемом в планировщике
	*	@param	cmnd_num: номер команды в расписании
	*	@param	data_point: указатель на принятые данные
	*/
void AT_Command_Handler(AT_Status_t stat, U8 cmnd_num, char* data_point)
{
	int res = 0, clen = 0;
	struct slre_cap caps = {0};
	
	switch(stat)
	{
/************************************************************************************************************************************/
// AT_Start
/************************************************************************************************************************************/
		case AT_Start:	// Открытие сокета
			switch(cmnd_num)
			{
				case AT_I4:	//------------------------- Информация о модеме
					res = slre_match("ATI4\r\r\n(.+)\r\n\r\nOK\r\n", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						Modem_Info._model = (char*)malloc(caps.len);	// Выделить память для строки
						if(Modem_Info._model != NULL)	// Если хватает памяти
							memcpy(Modem_Info._model, caps.ptr, caps.len);	// Скопировать строку
						AT_Start_Schedule++;
					}
					break;
				case AT_SELINT:	//--------------------- Выбор стиля интерфейса
						AT_Start_Schedule++;
					break;
				case AT_CMEE:	//----------------------- Подробность выдачи информации от модема
						AT_Start_Schedule++;
					break;
				case AT_CPIN:	//----------------------- Необходимость ввода пина
					res = slre_match("READY", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						AT_Start_Schedule++;
					}
					break;
				case AT_K0:	//------------------------- Флоу контроль
						AT_Start_Schedule++;
					break;
				case AT_CREG:	//----------------------- Регистрация в сети
						res = slre_match("CREG: 0,([0-9])", data_point, strlen(data_point), &caps, 1, 0);
						if(res > 0)
						{
							if(*caps.ptr == '1')
							{
								AT_Start_Schedule++;
							}
						}
					break;
				case AT_COPS:	//----------------------- Выбор оператора
					res = slre_match("COPS:.+\"(.+)\"\r\n", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						Modem_Info._operator = (char*)malloc(caps.len);	// Выделить память для строки
						if(Modem_Info._operator != NULL)	// Если хватает памяти
							memcpy(Modem_Info._operator, caps.ptr, caps.len);	// Скопировать строку
						AT_Start_Schedule++;
					}
					break;
				case AT_CSQ:	//----------------------- Запрос уровня сигнала
						res = slre_match("CSQ: ([0-9]+),[0-9]\r\n", data_point, strlen(data_point), &caps, 1, 0);
						if(res > 0)
						{	// Строка найдена
							Modem_Info._csq = (char*)malloc(caps.len);	// Выделить память для строки
							if(Modem_Info._csq != NULL)	// Если хватает памяти
								memcpy(Modem_Info._csq, caps.ptr, caps.len);	// Скопировать строку
							AT_Start_Schedule = 0;
							AT_Status++;
						}
					break;
					default: 
					break;
			}
			break;
/************************************************************************************************************************************/
// AT_Socket_Open
/************************************************************************************************************************************/
		case AT_Socket_Open:	// Открытие сокета
			switch(cmnd_num)
			{
				case AT_SERVINFO:	//------------------- Информация о сервере
						AT_Socket_Open_Schedule++;
					break;
				case AT_MONI:	//----------------------- Мониторинг сети
						AT_Socket_Open_Schedule++;
					break;
				case AT_CGDCONT:	//------------------- Определить PDP context
						AT_Socket_Open_Schedule++;
					break;
				case AT_SGACTCFGEXT:	//--------------- Разрешить прерывание PDP context
						AT_Socket_Open_Schedule++;
					break;
				case AT_SGACT:	//--------------------- Включить PDP context
						AT_Socket_Open_Schedule++;
					break;
				case AT_SCFGEXT:	//------------------- Настройка конфигураций сокета
						AT_Socket_Open_Schedule++;
					break;
				case AT_SD:	//------------------------- Открытие сокета
						AT_Socket_Open_Schedule++;
					break;
				case AT_SS:	//------------------------- Статус сокета
						AT_Status = AT_Socket_Ready;
					break;
				default: 
					break;
			}
			break;
/************************************************************************************************************************************/
// AT_Socket_Send
/************************************************************************************************************************************/
		case AT_Socket_Send:	// Отправить сообщение через сокет
				
			break;
/************************************************************************************************************************************/
// AT_Socket_Close
/************************************************************************************************************************************/
		case AT_Socket_Close:	// Закрытие сокета
				switch(cmnd_num)
				{
					case AT_SH:	//----------------------- Закрыть сокет
							AT_Socket_Close_Schedule++;
						break;
					case AT_SGACT_PDP_OFF:	//----------- Выключить PDP context
							AT_Socket_Close_Schedule++;
						break;
					case AT_SGACT_PDP_STAT:	//----------- Запрос статуса PDP context
							AT_Socket_Close_Schedule++;
						break;
					case AT_ENHRST:	//------------------- Быстрая перезагрузка модема
							AT_Socket_Open_Schedule = 0;
							AT_Status = 0;
						break;
					default:
						break;
				}
			break;
/************************************************************************************************************************************/
// AT_Socket_Ready
/************************************************************************************************************************************/
		case AT_Socket_Ready:	// Сокет готов
			
		 break;
/************************************************************************************************************************************/
// AT_Socket_Receive
/************************************************************************************************************************************/
		case AT_Socket_Receive:	// Принять сообщение через сокет
			
			break;
/************************************************************************************************************************************/
// AT_Balance
/************************************************************************************************************************************/
		case AT_Balance:		// Запрос баланса и отправка SMS
				switch(cmnd_num)
				{
					case AT_CUSD:	//--------------------- Запрос USSD
						if(!AT_ERROR_OK_Handler(data_point)) 
							AT_Balance_Schedule++;	// Перейти к следующему пункту в планировщике
						break;
					case AT_USSD_Receive:	//------------- Приём USSD
						res = slre_match("CUSD: [0-9],\"(.+)\",[0-9]", data_point, strlen(data_point), &caps, 1, 0);	// Поиск данных в принятой строке
						if(res > 0)
						{	// Данные найдены
							memcpy(_SMS.buf, caps.ptr, caps.len);	// Скопировать принятую строку в буфер для отправки по SMS
							_SMS.len = caps.len;
							clean_rx_buf();	// Очистить приёмный буфер и снять флаги
							AT_Balance_Schedule++;	// Перейти к следующему пункту в планировщике
						}
						break;
					case AT_CMGF:	//--------------------- Включить текстовый режим передачи SMS
						res = slre_match("CMGF", data_point, strlen(data_point), &caps, 1, 0);	// Поиск данных в принятой строке
						if(res > 0)
						{	// Данные найдены
							if(!AT_ERROR_OK_Handler(data_point))	// Если положительный ответ от модема
								AT_Balance_Schedule++;	// Перейти к следующему пункту в планировщике
						}
						break;
					case AT_CSCS:	//--------------------- Задать метод шифрования
						res = slre_match("CSCS", data_point, strlen(data_point), &caps, 1, 0);	// Поиск данных в принятой строке
						if(res > 0)
						{	// Данные найдены
							if(!AT_ERROR_OK_Handler(data_point))  // Если положительный ответ от модема
								AT_Balance_Schedule++;	// Перейти к следующему пункту в планировщике
						}
						break;	
					case AT_CMGS:	//--------------------- Отправить номер телефона для отправки SMS
						AT_Balance_Schedule++;	// Перейти к следующему пункту в планировщике
						break;
					case AT_Send_SMS:	//----------------- Отправить SMS
						res = slre_match(">", data_point, strlen(data_point), &caps, 1, 0); // Приглашение на отправку SMS
						if(res > 0)
						{	// Приглашение получено
							_SMS.len++;
							strcat(_SMS.buf, "\x1a"); // Добавить спец символ прекращения передачи CTRL+Z
							AT_Send(_SMS.buf, _SMS.len, AT_Status, AT_Balance_Schedule);	// Отправить SMS
							clean_rx_buf();	// Очистить приёмный буфер и снять флаги
							AT_Balance_Schedule++;	// Перейти к следующему пункту в планировщике
						}
						break;
					case AT_SMS_OK:	//------------------- Ждать подтверждения отправки SMS
						if(!AT_ERROR_OK_Handler(data_point))
						{ // Положительный ответ от модема
							memset(&_SMS, 0, sizeof _SMS);
							AT_Balance_Schedule = 0;	// Перейти к первому пункту в планировщике
						}
//						else
//						{
//							res = slre_match(">", data_point, strlen(data_point), &caps, 1, 0); // Приглашение на отправку SMS
//							if(res > 0)
//							{	// Если часть SMS не отправилась
//								char* tmp_sms = (char*)malloc(strlen((const char*)sms_buf + clen));	// Выделить память для строки
//								if(tmp_sms != NULL)	// Если хватает памяти
//								{
//									memcpy(tmp_sms, sms_buf + clen, strlen(sms_buf + clen));
//									memset(sms_buf, 0, sizeof sms_buf);	// Очистить SMS буфер
//									memcpy(sms_buf, tmp_sms, strlen(tmp_sms));	// Копировать не отправленную часть в SMS буфер
//									AT_Send(sms_buf, AT_Status, AT_Balance_Schedule);	// Отправить SMS
//									clean_rx_buf();
//								}
//								free(tmp_sms);
//							}
//						}
				}
			break;
		default:
			break;
	}
}

/**
	*	@brief	AT_send()
	*	@note		Отправка АТ команд модему
	*	@param	at_cmd: ссылка на строку с командой
	*	@param	socket_stat: статус сокета на момент отправки команды
	*	@param	cmnd_num: номер команды в расписании
	*/
void AT_Send(char *at_cmd, U16 cmd_len, AT_Status_t socket_stat, U8 cmnd_num)
{
	if(cmd_len == NULL)
	{
		cmd_len = strlen(at_cmd);
	}
	
	USART3_send((uint8_t *)at_cmd, cmd_len);
	
	AT_Command.busy = true;
	AT_Command.sent = at_cmd;
	AT_Command.number = cmnd_num;
	AT_Command.status = socket_stat;
	AT_Command.wait_time = localtime + MODEM_WAIT_TIME;
}

/**
	*	@brief	connect_3_str()
	*	@note		Создать строку из трёх составляющих
	* @return Указатель на строку запрос
	*/
char* connect_3_str(char* cmnd_start, char* buff, uint8_t len, char* cmnd_end)
{	
	char* tmp1 = strcat(cmnd_start, buff);
	char* tmp2 = strcat(tmp1, cmnd_end);
	
	return tmp2;
}

/**
	*	@brief	get_AT_Command_busy()
	*	@note		Получить статус выполнения АТ команды
	* @return AT_Command.busy
	*/
bool get_AT_Command_busy(void)
{
	return AT_Command.busy;
}

/**
	*	@brief	write_msg()
	*	@note		Добавить сообщение в структуру AT_Socket_Msg для отправки через сокет
	* @return 1: сокет не готов к отправке
	*					0: отправка сообщения через сокет будет осуществлена
	*/
bool write_msg(uint8_t* buff, uint8_t len)
{
	if(AT_Status != AT_Socket_Ready) 
	{
		return 1; //ошибка. сокет не готов
	}
	
	AT_Socket_Msg.buff = buff;
	AT_Socket_Msg.len = len;
	AT_Socket_Msg.busy = true;
	
	AT_Status = AT_Socket_Send;
	
	return 0;
}

/**
	*	@brief	__AT_Socket_Send()
	*	@note		Поэтапная отправка через сокет
	*/
void __AT_Socket_Send(void)
{
	switch(AT_Socket_Send_Schedule)
	{
		case AT_SSENDEXT:	//----- Отправить информацию о сообщении
			AT_Send(connect_3_str("AT#SSENDEXT=1,", (char*)AT_Socket_Msg.buff, AT_Socket_Msg.len, "\r"), NULL, AT_Status, AT_Socket_Open_Schedule);
		break;
	case AT_Msg:	//----------- Сообщение
			AT_Send((char*)AT_Socket_Msg.buff, NULL, AT_Status, AT_Socket_Open_Schedule);
		break;
	}
}

/**
	*	@brief	__AT_Start()
	*	@note		Начало работы с модемом
	*/
void __AT_Start(void)
{
	switch(AT_Start_Schedule)
	{
		case AT_I4:	//----------- Информация о модеме
				AT_Send("ATI4\r", NULL, AT_Status, AT_Start_Schedule);
			break;
		case AT_SELINT:	//------- Выбор стиля интерфейса
				AT_Send("AT#SELINT=2\r", NULL, AT_Status, AT_Start_Schedule);
			break;
		case AT_CMEE:	//--------- Подробность выдачи информации от модема
				AT_Send("AT+CMEE=0\r", NULL, AT_Status, AT_Start_Schedule);
			break;
		case AT_CPIN:	//--------- Необходимость ввода пина
				AT_Send("AT+CPIN?\r", NULL, AT_Status, AT_Start_Schedule);
			break;
		case AT_K0:	//----------- Флоу контроль
				AT_Send("AT&K0\r", NULL, AT_Status, AT_Start_Schedule);
			break;
		case AT_CREG:	//--------- Регистрация в сети
				AT_Send("AT+CREG?\r", NULL, AT_Status, AT_Start_Schedule);
			break;
		case AT_COPS:	//--------- Выбор оператора
				AT_Send("AT+COPS?\r", NULL, AT_Status, AT_Start_Schedule);
			break;
		case AT_CSQ:	//--------- Запрос уровня сигнала
				AT_Send("AT+CSQ\r", NULL, AT_Status, AT_Start_Schedule);
			break;
		default: 
			break;
	}
}

/**
	*	@brief	__AT_Socket_Open()
	*	@note		Поэтапное открытие сокета
	*/
void __AT_Socket_Open(void)
{
	switch(AT_Socket_Open_Schedule)
	{
		case AT_SERVINFO:	//----- Информация о сервере
				AT_Send("AT#SERVINFO\r", NULL, AT_Status, AT_Socket_Open_Schedule);
			break;
		case AT_MONI:	//--------- Мониторинг сети
				AT_Send("AT#MONI\r", NULL, AT_Status, AT_Socket_Open_Schedule);
			break;
		case AT_CGDCONT:	//----- Определить PDP context
				AT_Send("AT+CGDCONT=1,\"IP\",\"internet\"\r", NULL, AT_Status, AT_Socket_Open_Schedule);
			break;
		case AT_SGACTCFGEXT:	//- Разрешить прерывание PDP context
				AT_Send("AT#SGACTCFGEXT=1,1\r", NULL, AT_Status, AT_Socket_Open_Schedule);
			break;
		case AT_SGACT:	//------- Включить PDP context
				AT_Send("AT#SGACT=1,1\r", NULL, AT_Status, AT_Socket_Open_Schedule);
			break;
		case AT_SCFGEXT:	//----- Настройка конфигураций сокета
				AT_Send("AT#SCFGEXT=1,1,0,0,0,0\r", NULL, AT_Status, AT_Socket_Open_Schedule);
			break;
		case AT_SD:	//----------- Открытие сокета
				AT_Send("AT#SD=1,0,43553,80.246.243.36,0,0,1\r", NULL, AT_Status, AT_Socket_Open_Schedule);
			break;
		case AT_SS:	//----------- Статус сокета
				AT_Send("AT#SS=1\r", NULL, AT_Status, AT_Socket_Open_Schedule);
			break;
		default: 
			break;
	}
}

/**
	*	@brief	__AT_Socket_Close()
	*	@note		Поэтапное закрытие сокета
	*/
void __AT_Socket_Close(void)
{
	switch(AT_Socket_Close_Schedule)
	{
		case AT_SH:	//---------------- Закрыть сокет
				AT_Send("AT#SH=1\r", NULL, AT_Status, AT_Socket_Close_Schedule);
			break;
		case AT_SGACT_PDP_OFF:	// --- Выключить PDP context
				AT_Send("AT#SGACT=1,0\r", NULL, AT_Status, AT_Socket_Close_Schedule);
			break;
		case AT_SGACT_PDP_STAT:	//---- Запрос статуса PDP context
				AT_Send("AT#SGACT?\r", NULL, AT_Status, AT_Socket_Close_Schedule);
			break;
		case AT_ENHRST:	//------------ Запрос статуса PDP context
				AT_Send("AT#ENHRST=1,0\r", NULL, AT_Status, AT_Socket_Close_Schedule);
			break;
	}
}

/**
	*	@brief	__AT_Balance()
	*	@note		Запрос баланса
	*/
void __AT_Balance(void)
{
	switch(AT_Balance_Schedule)
	{
		case AT_CUSD:	//-------------- Запрос USSD 
			AT_Send("AT+CUSD=1,*105#,0\r", NULL, AT_Status, AT_Balance_Schedule);
			break;
		case AT_USSD_Receive:	//------ Приём USSD
			AT_Command.busy = true;
			AT_Command.wait_time = localtime + MODEM_WAIT_TIME;
			break;
		case AT_CMGF:	//-------------- Включить текстовый режим передачи SMS
			AT_Send("AT+CMGF=1\r", NULL, AT_Status, AT_Balance_Schedule);
			break;
		case AT_CSCS:	//-------------- Задать метод шифрования
			AT_Send("AT+CSCS=\"UCS2\"\r", NULL, AT_Status, AT_Balance_Schedule);
			//AT_Send("AT+CSCS=\"GSM\"\r", AT_Status, AT_Balance_Schedule);
			break;
		case AT_CMGS:	//-------------- Отправить номер телефона для отправки SMS
			AT_Send("AT+CMGS=\"002B00370039003500320032003400390038003900340032\"\r", NULL, AT_Status, AT_Balance_Schedule);
			//AT_Send("AT+CMGS=\"+79522498942\"\r", AT_Status, AT_Balance_Schedule);
		case AT_Send_SMS:	//---------- Отправить SMS
			break;
		case AT_SMS_OK:	//------------ Ждать подтверждения отправки SMS
			break;
	}
}

/**
	*	@brief	clean_rx_buf()
	*	@note		Очистка приёмного буфера и флагов
	*/
void clean_rx_buf(void)
{
	memset(USART3_Rx_buf, 0, sizeof USART3_Rx_buf);
	USART3_Rx_count = 0;
	AT_Command.busy = false;
}


/**
	*	@brief	AT_Check_Recv_Buff()
	*	@note		Изучение приёмного буфера на предмет сообщений от модема
	*/
void AT_Check_Recv_Buff(void)
{
	int ires = 0;
	struct slre_cap caps = {0};
	
	char* ptmp_buf = (char*)malloc(strlen((const char*)USART3_Rx_buf));	// Выделить память для строки
	if(ptmp_buf != NULL)	// Если хватает памяти
		memcpy(ptmp_buf, USART3_Rx_buf, strlen((const char*)USART3_Rx_buf));	// Скопировать строку
	
	// Время ожидания ответа от модема
	if(AT_Command.wait_time < localtime)
	{	// Время вышло
		clean_rx_buf(); // Очистить приёмный буфер и флаги
		clear_modem_stats(); // Очистить статусы
	}
	
	// Обработка ответов от модема
	if(AT_Status == AT_Balance)
	{
		AT_Command_Handler(AT_Status, AT_Balance_Schedule, ptmp_buf);
	}
	else
	{
		//Поиск ответа на АТ команду
		if(AT_Command.busy)
		{
			ires = slre_match("(OK|ERROR)", ptmp_buf, strlen(ptmp_buf), &caps, 1, 0);
			if(ires > 0)
			{
				if(strstr(ptmp_buf, "OK"))
				{
					AT_Command_Handler(AT_Command.status, AT_Command.number, ptmp_buf);
				}
				else if(strstr(ptmp_buf, "ERROR"))
				{
					AT_Socket_Close_Schedule = AT_SH;
					AT_Status = AT_Socket_Close;
				}
				clean_rx_buf();
			}
		}
	}
	
	free(ptmp_buf); // Освободить память
}






/******************* (C) COPYRIGHT 2018 Roman_Yudin *****END OF FILE****/


