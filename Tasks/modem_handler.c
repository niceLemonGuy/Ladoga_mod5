/**
  ******************************************************************************
  * @file    modem_handler.c
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    14/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
	

/* Includes ------------------------------------------------------------------------*/
#include "includes.h"
#include "at_comnds.h"


/* Task ID -------------------------------------------------------------------------*/
OS_TID tid_Main_Modem_Handler;	// ID задачи t_Main_Modem_Handler
OS_TID tid_Aux_Modem_Handler;	// ID задачи t_Aux_Modem_Handler


/* Variables -----------------------------------------------------------------*/
U8 modem_err_cnt = 0;
U32 tcp_recv_timeout = 0;


/* Enums ---------------------------------------------------------------------*/
Main_AT_Status_t Main_AT_Status = 0;				// Статус main канала
AT_Start_Schedule_t AT_Start_Schedule = 0;			// Начало работы с модемом
AT_Socket_Open_Schedule_t AT_Socket_Open_Schedule = 0;		// Порядок открытия сокета
AT_Socket_Send_Schedule_t AT_Socket_Send_Schedule = 0;		// Порядок передачи сообщения через сокет
AT_Socket_Receive_Schedule_t AT_Socket_Receive_Schedule = 0;	// Порядок получения сообщения через сокет
AT_Socket_Close_Schedule_t AT_Socket_Close_Schedule = 0;	// Порядок закрытия сокета

Aux_AT_Status_t Aux_AT_Status = 0;				// Статус aux канала
AT_Modem_Poll_Schedule_t AT_Modem_Poll_Schedule = 0;		// Периодические опросы модема
AT_Balance_Schedule_t AT_Balance_Schedule = 0;			// Запрос баланса


/* Structs -------------------------------------------------------------------*/
Modem_Stat_t Modem_Stat;		// Статусы модема
Main_AT_Command_t Main_AT_Command;	// АТ команды main канала
Aux_AT_Command_t Aux_AT_Command;	// АТ команды aux канала
AT_Socket_Tx_Msg_t AT_Socket_Tx_Msg;	// Сообщение для отправки по GPRS
AT_Socket_Rx_Msg_t AT_Socket_Rx_Msg;	// Сообщение принятое по GPRS
Modem_Info_t Modem_Info;		// Информация о модеме и операторе
Balance_t balance;			// Структура для запроса и хранения баланса


/* Functions -----------------------------------------------------------------*/
/**
  * @brief	main_clear_stats()
  * @note	Сброс всех планировщиков main канала
  */
void main_clear_stats(void)
{
	Main_AT_Status = NULL;
	AT_Start_Schedule = NULL;
	AT_Socket_Open_Schedule = NULL;
	AT_Socket_Send_Schedule = NULL;
	AT_Socket_Close_Schedule = NULL;
}

/**
 * @brief	aux_clear_stats()
 * @note	Сброс всех планировщиков main канала
 */
void aux_clear_stats(void)
{
	Aux_AT_Status = NULL;
	AT_Modem_Poll_Schedule = NULL;
	AT_Balance_Schedule = NULL;
}

/**
 * @brief	main_clean_rx_buf()
 * @note	Очистка приёмного буфера и флагов main канала
 */
void main_clean_rx_buf(void)
{
	USART3_Rx_Clear();
	Main_AT_Command.busy = false;
}

/**
 * @brief	aux_clean_rx_buf()
 * @note	Очистка приёмного буфера и флагов aux канала
 */
void aux_clean_rx_buf(void)
{
	USART1_Rx_Clear();
	Aux_AT_Command.busy = false;
}

/**
 * @brief	num_to_char()
 * @note	Конвертировать число в строку
 * @param	pbuf: буфер для записи строки
 * @param	buf_len: размер буфера
 * @param	num: конвертируемое число
 */
void num_to_char(char *pbuf, U8 buf_len, U16 num)
{
	U8 i, j;
	// Конвертировать цифры в символы
	for (i = 1; i<buf_len + 1; i++)
	{
		pbuf[buf_len - i] = (num % 10) + 48;
		num /= 10;
	}
	// Убрать нули перед числом в строке
	for (i = 1; i<buf_len; i++)
	{
		if(pbuf[0] == '0')
		{
			for (j = 0; j<buf_len - 1;j++)
			{
				pbuf[j] = pbuf[j + 1];
			}
			pbuf[buf_len - 1] = 0;
		}
	}
	pbuf[buf_len-1] = 0;
}

/**
 * @brief	char_to_num()
 * @note	Конвертировать строку в число
 * @param	pbuf: буфер для записи строки
 * @param	buf_len: размер буфера
 * @param	num: конвертируемое число
 */
U16 char_to_num(const char *pbuf, U8 buf_len)
{
	U8 i, j, x;
	U16 num = 0, tmp;

	// Конвертировать символы в цифры
	for(i = 0, j = buf_len - 1; i < buf_len; i++, j--)
	{
		tmp = pbuf[j] - 48;
		if(i != 0)
		{
			for(x = 0; x < i; x++)
				tmp *= 10;
		}
		num += tmp;
	}
	return num;
}

/**
 * @brief	Main_AT_send()
 * @note	Отправка АТ команд модему по aux каналу
 * @param	at_cmd: ссылка на строку с командой
 * @param	cmd_len: размер команды (если ноль, то вычисляется размер строки)
 * @param	cmnd_num: номер команды в расписании
 */
void Main_AT_Send(U8 *at_cmd, U16 cmd_len, U8 cmnd_num)
{
	if(cmd_len == NULL)
	{
		cmd_len = strlen((char *)at_cmd);
	}
	
	USART3_Send((uint8_t *)at_cmd, cmd_len);
	
	Main_AT_Command.busy = true;
	Main_AT_Command.sent = (char *)at_cmd;
	Main_AT_Command.number = cmnd_num;
	Main_AT_Command.status = Main_AT_Status;
	Main_AT_Command.wait_time = localtime + MODEM_WAIT_TIME;
}

/**
 * @brief	Aux_AT_send()
 * @note	Отправка АТ команд модему по aux каналу
 * @param	at_cmd: ссылка на строку с командой
 * @param	cmd_len: размер команды (если ноль, то вычисляется размер строки)
 * @param	cmnd_num: номер команды в расписании
 */
void Aux_AT_Send(char *at_cmd, U16 cmd_len, U8 cmnd_num)
{
	if(cmd_len == NULL)
	{
		cmd_len = strlen(at_cmd);
	}
	
	USART1_Send((uint8_t *)at_cmd, cmd_len);
	
	Aux_AT_Command.busy = true;
	Aux_AT_Command.sent = at_cmd;
	Aux_AT_Command.number = cmnd_num;
	Aux_AT_Command.status = Aux_AT_Status;
	Aux_AT_Command.wait_time = localtime + MODEM_WAIT_TIME;
}

/**
 * @brief	Main_AT_Command_Handler()
 * @note	Обработчик ответов модема на AT команды по main каналу
 * @param	at_stat: статус работы main канала
 * @param	cmnd_num: номер команды в расписании
 * @param	data_point: указатель на принятые данные
 */
void Main_AT_Command_Handler(Main_AT_Status_t at_stat, U8 cmnd_num, char* data_point)
{
	int res = 0;
	char *tmp_adr;
	struct slre_cap caps = {0};
	
	switch(at_stat)
	{
/************************************************************************************************************************************/
// AT_Start
/************************************************************************************************************************************/
		case AT_Modem_Start:	// Начало работы модема
			switch(cmnd_num)
			{
				case ATE0:	//------------------------- Отключить эхо
					AT_Start_Schedule++;
					break;
				case AT_I4:	//------------------------- Информация о модеме
					res = slre_match("\r\n(.+)\r\n\r\nOK\r\n", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
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
				case AT_K0:	//------------------------- Флоу контроль
					AT_Start_Schedule++;
					break;
				case AT_SLED:	//----------------------- Режим работы светодиода
					AT_Start_Schedule++;
					break;
				case AT_CPIN:	//----------------------- Необходимость ввода пина
					res = slre_match("READY", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						AT_Start_Schedule++;
					}
					break;
				case AT_CPBR: //----------------------- Запросить телефонную книгу SIM карты
					res = slre_match("\\+CPBR:.+\"11\".+?(\".+?\")\r\n", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						memset(Modem_Info._PDP_addr, 0, sizeof Modem_Info._PDP_addr);	// Очистить строку
						memcpy(Modem_Info._PDP_addr, caps.ptr, caps.len);	// Скопировать строку
					}
					res = slre_match("\\+CPBR:.+\"14\".+?(\".+?\")\r\n", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						memset(Modem_Info._ussd_balance, 0, sizeof Modem_Info._ussd_balance);	// Очистить строку
						memcpy(Modem_Info._ussd_balance, caps.ptr, caps.len);	// Скопировать строку
					}
					res = slre_match("\\+CPBR:.+\"20\".+?(\".+?\")\r\n", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						memset(Modem_Info._ip, 0, sizeof Modem_Info._port);	// Очистить строку
						memcpy(Modem_Info._ip, caps.ptr+1, caps.len-2);	// Скопировать строку без ковычек
					}
					res = slre_match("\\+CPBR:.+\"21\".+?(\".+?\")\r\n", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						memset(Modem_Info._port, 0, sizeof Modem_Info._port);	// Очистить строку
						memcpy(Modem_Info._port, caps.ptr+1, caps.len-2);	// Скопировать строку без ковычек
					}
					AT_Start_Schedule++;
					break;
				case AT_SII:	//----------------------- Включить aux канал
					Modem_Stat.aux_on = true;
					Main_AT_Status = AT_Socket_Open;
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
				case AT_SS:	//------------------------- Запрос статуса сокета
					res = slre_match("SS: 1,([0-9])", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{
						if(*caps.ptr == '0')
						{
							AT_Socket_Open_Schedule++;
						}
						else
						{
							Main_AT_Send((U8 *)"AT#SH=1\r", NULL, cmnd_num);
							AT_Socket_Open_Schedule++;
						}
					}
					break;
//				case AT_SGACT_REQ:	//----------------- Запрос статуса PDP context
//					res = slre_match("SGACT: 1,([0-9])", data_point, strlen(data_point), &caps, 1, 0);
//					if(res > 0)
//					{
//						if(*caps.ptr == '0')
//						{
//							AT_Socket_Open_Schedule++;
//						}
//						else
//						{
//							Main_AT_Send((U8 *)"AT#SGACT=1,0\r", NULL, cmnd_num);
//							AT_Socket_Open_Schedule++;
//						}
//					}
//					break;
				case AT_COPS:	//----------------------- Имя оператора связи
					res = slre_match("COPS:.+\"(.+)\"\r\n", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						memcpy(Modem_Info._operator, caps.ptr, caps.len);	// Скопировать строку
						AT_Socket_Open_Schedule++;
					}
					else if(++Main_AT_Command.err_cnt > 20)
					{
						change_sim();
						Main_AT_Command.err_cnt = 0;
					}
					break;
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
						Main_AT_Status = AT_Socket_Ready;
					break;
					break;
				default: 
					break;
			}
			break;
/************************************************************************************************************************************/
// AT_Socket_Send
/************************************************************************************************************************************/
		case AT_Socket_Send:	// Отправить сообщение через сокет
			switch(cmnd_num)
			{
				case AT_SSENDEXT:	//------------------- Запрос на отправку сообщения
					main_clean_rx_buf();
					AT_Socket_Send_Schedule = AT_Msg;
					break;
				case AT_Msg:	//----------------------- Отправка сообщения
					AT_Socket_Send_Schedule = NULL;
					Main_AT_Status = AT_Socket_Ready;
					// В тестовом режиме переходим на другую SIM карту, после отправки сообщения
					if(Device_Status.test_mode)
						change_sim();
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
			switch(cmnd_num)
			{
				case AT_SRING:
					res = slre_match("SRING: 1,([0-9]+)", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{
						AT_Socket_Rx_Msg.len = char_to_num(caps.ptr, caps.len);
						AT_Socket_Receive_Schedule = AT_SRECV;
					}
					break;
				case AT_SRECV:
					res = slre_match("SRECV: 1,([0-9]+)\r\n", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{
						tmp_adr = &data_point[strlen("\r\n#SRECV: 1,\r\n") + caps.len];
						if(strstr(tmp_adr + char_to_num(caps.ptr, caps.len), "\r\n\r\nOK"))
						{
							memcpy(AT_Socket_Rx_Msg.buff, tmp_adr, char_to_num(caps.ptr, caps.len));
							AT_Socket_Rx_Msg.msg_received = true;
							Main_AT_Status = AT_Socket_Ready;
							AT_Socket_Receive_Schedule = NULL;
							modem_err_cnt = 0;
						}
					}
					break;
				default:
					break;
			}
			break;
/************************************************************************************************************************************/
// AT_Socket_Close
/************************************************************************************************************************************/
		case AT_Socket_Close:	// Закрытие сокета
			switch(cmnd_num)
			{
				case AT_SH:	//----------------------- Закрыть сокет
					Main_AT_Status = AT_Socket_Open;
					AT_Socket_Open_Schedule = AT_SD;
					break;
//				case AT_SGACT_PDP_OFF:	//----------- Выключить PDP context
//						AT_Socket_Close_Schedule++;
//					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

/**
	*	@brief	Aux_AT_Command_Handler()
	*	@note		Обработчик ответов модема на AT команды по aux каналу
	* @param	at_stat: статус работы aux канала
	*	@param	cmnd_num: номер команды в расписании
	*	@param	data_point: указатель на принятые данные
	*/
void Aux_AT_Command_Handler(Aux_AT_Status_t at_stat, uint8_t cmnd_num, char* data_point)
{
	int res = 0;
	struct slre_cap caps = {0};
	
	switch(at_stat)
	{
/************************************************************************************************************************************/
// AT_Modem_Poll
/************************************************************************************************************************************/
		case AT_Modem_Poll:	// Открытие сокета
			switch(cmnd_num)
			{
				case AT_CSQ:	//----------------------- Запрос уровня сигнала
					res = slre_match("CSQ: ([0-9]+),[0-9]", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{	// Строка найдена
						memset(Modem_Info._csq, 0, sizeof Modem_Info._csq);	// Очистить строку
						memcpy(Modem_Info._csq, caps.ptr, caps.len);	// Скопировать строку
						AT_Modem_Poll_Schedule++;
					}
					break;
				case AT_CREG:	//----------------------- Регистрация в сети GSM
					res = slre_match("CREG: 0,([0-9])", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{
						Modem_Info._gsm_stat = *caps.ptr;
						AT_Modem_Poll_Schedule++;
					}
					break;
				case AT_CGREG:	//--------------------- Регистрация в сети GPRS
					res = slre_match("CGREG: 0,([0-9])", data_point, strlen(data_point), &caps, 1, 0);
					if(res > 0)
					{
						Modem_Info._gprs_stat = *caps.ptr;
						aux_clear_stats();
					}
					break;
				default: 
					break;
			}
			break;
/************************************************************************************************************************************/
// AT_Balance
/************************************************************************************************************************************/
		case AT_Balance:		// Запрос баланса и отправка SMS
			switch(cmnd_num)
			{
				case AT_CUSD:	//--------------------- Запрос USSD
					AT_Balance_Schedule++;	// Перейти к следующему пункту в планировщике
					break;
				case AT_USSD_Receive:	//------------- Приём USSD
					res = slre_match("CUSD: [0-9],\"(.+)\",[0-9]", data_point, strlen(data_point), &caps, 1, 0);	// Поиск данных в принятой строке
					if(res > 0)
					{	// Данные найдены
						if(caps.len > sizeof(balance.msg))
							caps.len = sizeof(balance.msg);
						memset(&balance, 0, sizeof balance);
						memcpy(balance.msg, caps.ptr, caps.len);	// Скопировать принятую строку в буфер для отправки по SMS
						balance.msg_len = caps.len;
						aux_clean_rx_buf();	// Очистить приёмный буфер и снять флаги
						aux_clear_stats();
					}
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

/**
 * @brief	__AT_Modem_Start()
 * @note	Начало работы с модемом
 */
void __AT_Modem_Start(void)
{
	U8 cmnd_num = AT_Start_Schedule;
	
	switch(cmnd_num)
	{
		case ATE0:	//----------- Включить эхо
			Main_AT_Send((U8 *)"ATE0\r", NULL, cmnd_num);
			break;
		case AT_I4:	//----------- Информация о модеме
			Main_AT_Send((U8 *)"ATI4\r", NULL, cmnd_num);
			break;
		case AT_SELINT:	//------- Выбор стиля интерфейса
			Main_AT_Send((U8 *)"AT#SELINT=2\r", NULL, cmnd_num);
			break;
		case AT_CMEE:	//--------- Подробность выдачи информации от модема
			Main_AT_Send((U8 *)"AT+CMEE=0\r", NULL, cmnd_num);
			break;
		case AT_K0:	//----------- Флоу контроль
			Main_AT_Send((U8 *)"AT&K0\r", NULL, cmnd_num);
			break;
		case AT_SLED:	//--------- Режим работы светодиода
			Main_AT_Send((U8 *)"AT#SLED=2\r", NULL, cmnd_num);
			break;
		case AT_CPIN:	//--------- Готовность SIM карты
			Main_AT_Send((U8 *)"AT+CPIN?\r", NULL, cmnd_num);
			break;
		case AT_CPBR: //--------- Запросить телефонную книгу SIM карты
			Main_AT_Send((U8 *)"AT+CPBR=1,30\r", NULL, cmnd_num);
			break;
		case AT_SII:	//--------- Включить aux канал
			Main_AT_Send((U8 *)"AT#SII=1\r", NULL, cmnd_num);
			break;
		default:
			break;
	}
}

/**
 * @brief	__AT_Socket_Open()
 * @note	Поэтапное открытие сокета
 */
void __AT_Socket_Open(void)
{
	char at_tmp[64];
	U8 cmnd_num = AT_Socket_Open_Schedule;
	
	switch(cmnd_num)
	{
		case AT_SS:	//----------- Запрос статуса сокета
			Main_AT_Send((U8 *)"AT#SS=1\r", NULL, cmnd_num);
			break;
//		case AT_SGACT_REQ:	//--- Запрос статуса PDP context
//			Main_AT_Send((U8 *)"AT#SGACT?\r", NULL, cmnd_num);
//			break;
		case AT_COPS:	//--------- Имя оператора связи
			Main_AT_Send((U8 *)"AT+COPS?\r", NULL, cmnd_num);
			break;
		case AT_SERVINFO:	//----- Информация о сервере
			Main_AT_Send((U8 *)"AT#SERVINFO\r", NULL, cmnd_num);
			break;
		case AT_MONI:	//--------- Мониторинг сети
			Main_AT_Send((U8 *)"AT#MONI\r", NULL, cmnd_num);
			break;
		case AT_CGDCONT:	//----- Определить PDP context
			if(!strlen(Modem_Info._PDP_addr))
			{
				Main_AT_Send((U8 *)"AT+CGDCONT=1,\"IP\",\"internet\"\r", NULL, cmnd_num);
			}
			else
			{
				sprintf(at_tmp, "AT+CGDCONT=1,\"IP\",%s\r", (const char *)Modem_Info._PDP_addr);
				Main_AT_Send((U8 *)at_tmp, NULL, cmnd_num);
			}
			break;
		case AT_SGACTCFGEXT:	//- Разрешить прерывание PDP context
			Main_AT_Send((U8 *)"AT#SGACTCFGEXT=1,1\r", NULL, cmnd_num);
			break;
		case AT_SGACT:	//------- Включить PDP context
			Main_AT_Send((U8 *)"AT#SGACT=1,1\r", NULL, cmnd_num);
			break;
		case AT_SCFGEXT:	//----- Настройка конфигураций сокета
			Main_AT_Send((U8 *)"AT#SCFGEXT=1,1,0,0,0,0\r", NULL, cmnd_num);
			break;
		case AT_SD:	//----------- Открытие сокета
			sprintf(at_tmp, "AT#SD=1,0,%s,%s,0,0,1\r", (const char *)Modem_Info._port, (const char *)Modem_Info._ip);
			Main_AT_Send((U8 *)at_tmp, NULL, cmnd_num);
			break;
		default: 
			break;
	}
}

/**
 * @brief	__AT_Socket_Send()
 * @note	Поэтапная отправка через сокет
 */
void __AT_Socket_Send(void)
{
	char at_tmp[32];
	U8 cmnd_num = AT_Socket_Send_Schedule;
	
	switch(cmnd_num)
	{
		case AT_SSENDEXT:	//------- Отправить информацию о сообщении
			sprintf(at_tmp, "AT#SSENDEXT=1,%s\r", (const char *)AT_Socket_Tx_Msg.clen);
			Main_AT_Send((U8 *)at_tmp, NULL, cmnd_num);
			break;
		case AT_Msg:	//----------- Сообщение
			Main_AT_Send(AT_Socket_Tx_Msg.buff, AT_Socket_Tx_Msg.ilen, cmnd_num);
			break;
		default:
			break;
	}
}

/**
 * @brief	__AT_Socket_Receive()
 * @note	Поэтапная отправка через сокет
 */
void __AT_Socket_Receive(void)
{
	U8 cmnd_num = AT_Socket_Receive_Schedule;
	
	switch(cmnd_num)
	{
		case AT_SRING:	//------- Принять подтверждение о получении сообщения

			break;
		case AT_SRECV:	//------- Принять сообщение через сокет
			Main_AT_Send((U8 *)"AT#SRECV=1,500\r", NULL, cmnd_num);
			break;
		default:
			break;
	}
}

/**
 * @brief	__AT_Socket_Close()
 * @note	Поэтапное закрытие сокета
 */
void __AT_Socket_Close(void)
{
	U8 cmnd_num = AT_Socket_Close_Schedule;
	
	switch(cmnd_num)
	{
		case AT_SH:	//---------------- Закрыть сокет
			Main_AT_Send((U8 *)"AT#SH=1\r", NULL, cmnd_num);
			break;
//		case AT_SGACT_PDP_OFF:	// --- Выключить PDP context
//			Main_AT_Send((U8 *)"AT#SGACT=1,0\r", NULL, cmnd_num);
//			break;
	}
}

/**
 * @brief	__AT_Modem_Poll()
 * @note	Периодический опрос модема
 */
void __AT_Modem_Poll(void)
{
	U8 cmnd_num = AT_Modem_Poll_Schedule;
	
	switch(cmnd_num)
	{
		case AT_CSQ:	//-------------- Запрос уровня сигнала
			Aux_AT_Send("AT+CSQ\r", NULL, cmnd_num);
			break;
		case AT_CREG:	//-------------- Запрос статуса регистрации в сети GSM
			Aux_AT_Send("AT+CREG?\r", NULL, cmnd_num);
			break;
		case AT_CGREG:	//------------ Запрос статуса регистрации в сети GPRS
			Aux_AT_Send("AT+CGREG?\r", NULL, cmnd_num);
			break;
		default:
			break;
	}
}

/**
 * @brief	__AT_Balance()
 * @note	Запрос баланса
 */
void __AT_Balance(void)
{	
	char at_tmp[16];
	U8 cmnd_num = AT_Balance_Schedule;
	
	switch(cmnd_num)
	{
		case AT_CUSD:	//-------------- Запрос USSD
			sprintf(at_tmp, "AT+CUSD=1,%s\r", (const char *)Modem_Info._ussd_balance);
			Aux_AT_Send(at_tmp, NULL, cmnd_num);
			break;
		case AT_USSD_Receive:	//------ Приём USSD
			Aux_AT_Command.busy = true;
			Aux_AT_Command.wait_time = localtime + MODEM_WAIT_TIME;
			break;
		default:
			break;
	}
}

/**
 * @brief	change_sim()
 * @note	Сменить сим карту, перезапустить модем, сбросить статусы
 */
void change_sim(void)
{
	CHANGE_SIM();	// Поменять SIM карту
	Main_AT_Send((U8 *)"AT#ENHRST=1,0\r", NULL, NULL);	// Перезапустить модем
	main_clear_stats(); // Очистить статусы
	Device_Status.on_line = false;
	Modem_Stat.err_cnt = NULL;
	modem_err_cnt = 0;
}

/**
 * @brief	select_sim1()
 * @note	Выбрать SIM1, перезапустить модем, сбросить статусы
 */
void select_sim1(void)
{
	SELECT_SIM1();	// Работать с SIM1
	Main_AT_Send((U8 *)"AT#ENHRST=1,0\r", NULL, NULL);	// Перезапустить модем
	main_clear_stats(); // Очистить статусы
	Device_Status.on_line = false;
	Modem_Stat.err_cnt = NULL;
}

/**
 * @brief	select_sim2()
 * @note	Выбрать SIM2, перезапустить модем, сбросить статусы
 */
void select_sim2(void)
{
	SELECT_SIM2();	// Работать с SIM2
	Main_AT_Send((U8 *)"AT#ENHRST=1,0\r", NULL, NULL);	// Перезапустить модем
	main_clear_stats(); // Очистить статусы
	Device_Status.on_line = false;
	Modem_Stat.err_cnt = NULL;
}

/**
 * @brief	gprs_write_msg()
 * @note	Добавить сообщение в структуру AT_Socket_Msg для отправки через сокет
 * @param	buff: буфер с сообщением
 * @param	len: размер сообщения
 * @return	1:сокет не готов к отправке
 *		0:отправка сообщения через сокет будет осуществлена
 */
bool gprs_write_msg(U8* buff, U16 len)
{
	if(Main_AT_Status != AT_Socket_Ready) 
	{
		return 1; // Ошибка. Сокет не готов
	}
	
	AT_Socket_Tx_Msg.ilen = len;
	AT_Socket_Tx_Msg.buff = buff;
	AT_Socket_Tx_Msg.busy = true;
	num_to_char(AT_Socket_Tx_Msg.clen, sizeof AT_Socket_Tx_Msg.clen, len);
	
	Main_AT_Status = AT_Socket_Send;
	
	return 0;
}

/**
 * @brief	msg_processed()
 * @note	Сообщение обработано. Снять флаг
 */
void msg_processed(void)
{
	AT_Socket_Rx_Msg.msg_received = false;
}

/**
 * @brief	gprs_recv_msg()
 * @note	Отдать полученное сообщение
 * @param	buff: буфер в который будет записано полученное сообщение
 * @return	1:нет принятых сообщений
 *		0:сообщение принято и записано в буфер
 */
bool gprs_recv_msg(U8* buff)
{
	if(!AT_Socket_Rx_Msg.msg_received) 
	{
		return 1; // Нет принятых сообщений
	}
	
	memcpy(buff, AT_Socket_Rx_Msg.buff, AT_Socket_Rx_Msg.len);
	
	return 0;	// Успех
}

/**
 * @brief	Main_AT_Check_Recv_Buff()
 * @note	Изучение приёмного буфера main канала на предмет сообщений от модема
 */
void Main_AT_Check_Recv_Buff(void)
{
	int res = 0;
	struct slre_cap caps = {0};
	char *ptmp_buf = (char*)USART3_Rx_buf;
	
//	char* ptmp_buf = (char*)malloc(strlen((const char*)USART3_Rx_buf));	// Выделить память для строки
//	if(ptmp_buf != NULL)	// Если хватает памяти
//		memcpy(ptmp_buf, USART3_Rx_buf, strlen((const char*)USART3_Rx_buf));	// Скопировать строку
	
	// Время ожидания ответа от модема
	if(Main_AT_Command.busy && localtime > Main_AT_Command.wait_time)
	{// Время вышло
		main_clean_rx_buf(); // Очистить приёмный буфер и флаги
		main_clear_stats(); // Очистить статусы
	}
	
	// Время приёма сообщения вышло
	if(Main_AT_Status == AT_Socket_Receive && localtime > tcp_recv_timeout)
	{
		Main_AT_Status = AT_Socket_Ready;
	}
	
	// Поиск ответа на АТ команду
	if(Main_AT_Command.busy)
	{
		res = slre_match("(OK|ERROR|>|SRECV)", ptmp_buf, strlen(ptmp_buf), &caps, 1, 0);
		if(res > 0)
		{
			if(strstr(ptmp_buf, "OK"))
			{
				Main_AT_Command_Handler(Main_AT_Command.status, Main_AT_Command.number, ptmp_buf);
			}
			else 
			if(strstr(ptmp_buf, "ERROR"))
			{
				if(++modem_err_cnt > MODEM_MAX_LINK_ERR)
				{
					change_sim();
				}
				else
				{
					Device_Status.on_line = false;
					main_clear_stats();
					Main_AT_Send((U8 *)"AT#ENHRST=1,0\r", NULL, NULL);	// Перезапустить модем
				}
			}
			else
			if(strstr(ptmp_buf, ">"))
			{
				Main_AT_Command_Handler(Main_AT_Status, AT_Socket_Send_Schedule, ptmp_buf);
			}
			else
			if(strstr(ptmp_buf, "SRECV"))
			{
				Main_AT_Command_Handler(Main_AT_Status, Main_AT_Command.number, ptmp_buf);
			}
			main_clean_rx_buf();
		}
	}
	else
	{
		res = slre_match("(SRING|NO CARRIER)", ptmp_buf, strlen(ptmp_buf), &caps, 1, 0);
		if(res > 0)
		{
			if(strstr(ptmp_buf, "SRING"))
			{
				Main_AT_Status = AT_Socket_Receive;
				tcp_recv_timeout = localtime + MODEM_WAIT_TIME;
				Main_AT_Command_Handler(Main_AT_Status, NULL, ptmp_buf);
				main_clean_rx_buf();
			}
			else
			if(strstr(ptmp_buf, "NO CARRIER"))
			{
//				if(++modem_err_cnt > MODEM_MAX_LINK_ERR)
//				{
//					change_sim();
//				}
//				else
//				{
//					Main_AT_Status = AT_Socket_Close;
//				}
			}
		}
	}
	
	//free(ptmp_buf); // Освободить память
}

/**
 * @brief	Aux_AT_Check_Recv_Buff()
 * @note	Изучение приёмного буфера aux канала на предмет сообщений от модема
 */
void Aux_AT_Check_Recv_Buff(void)
{
	int ires = 0;
	struct slre_cap caps = {0};
	char *ptmp_buf = (char*)USART1_Rx_buf;
	
//	char* ptmp_buf = (char*)malloc(strlen((const char*)USART1_Rx_buf));	// Выделить память для строки
//	if(ptmp_buf != NULL)	// Если хватает памяти
//		memcpy(ptmp_buf, USART1_Rx_buf, strlen((const char*)USART1_Rx_buf));	// Скопировать строку
	
	// Время ожидания ответа от модема
	if(Aux_AT_Command.busy && localtime > Aux_AT_Command.wait_time)
	{	// Время вышло
		aux_clean_rx_buf(); // Очистить приёмный буфер и флаги
		aux_clear_stats(); // Очистить статусы
	}
	
	// Обработка ответов от модема
	if(Aux_AT_Status == AT_Balance && AT_Balance_Schedule == AT_USSD_Receive)
	{
		Aux_AT_Command_Handler(Aux_AT_Status, AT_Balance_Schedule, ptmp_buf);
	}
	else
	{
		//Поиск ответа на АТ команду
		if(Aux_AT_Command.busy)
		{
			ires = slre_match("(OK|ERROR)", ptmp_buf, strlen(ptmp_buf), &caps, 1, 0);
			if(ires > 0)
			{
				if(strstr(ptmp_buf, "OK"))
				{
					Aux_AT_Command_Handler(Aux_AT_Command.status, Aux_AT_Command.number, ptmp_buf);
				}
				else if(strstr(ptmp_buf, "ERROR"))
				{
					
				}
				aux_clean_rx_buf();
			}
		}
	}
	
	//free(ptmp_buf); // Освободить память
}

/*-----------------------------------------------------------------------------------
 *        Task 't_Main_Modem_Handler' * Задача работает с main каналом модема *
 *----------------------------------------------------------------------------------*/
__task void t_Main_Modem_Handler(void)
{
	// Начать работу с SIM1
	SELECT_SIM1();
	
	for(;;)
	{		
		os_dly_wait(300);
		
		if(!Main_AT_Command.busy)
		{// Если не ждёт ответа
			switch(Main_AT_Status)
			{
				case AT_Modem_Start:		// Начало работы модема
					__AT_Modem_Start();
					break;
				case AT_Socket_Open:		// Открытие сокета
					__AT_Socket_Open();
					break;
				case AT_Socket_Ready:		// Сокет готов
					Device_Status.on_line = true;
					break;
				case AT_Socket_Send:		// Отправить сообщение через сокет
					__AT_Socket_Send();
					break;
				case AT_Socket_Close:		// Закрытие сокета
					__AT_Socket_Close();
					break;
				case AT_Socket_Receive:	// Принять сообщение через сокет
					__AT_Socket_Receive();
					break;
				default:
					break;
			}
		}
		
		// Проверить наличие новых данных во входном буфере
		Main_AT_Check_Recv_Buff();
	}
}

/*-----------------------------------------------------------------------------------
 *        Task 't_Aux_Modem_Handler' * Задача работает с aux каналом модема *
 *----------------------------------------------------------------------------------*/
__task void t_Aux_Modem_Handler(void)
{
	for(;;)
	{		
		os_dly_wait(300);
		
		if(!Modem_Stat.aux_on)
			continue;
		
		if(!Aux_AT_Command.busy)
		{// Если не ждёт ответа
			switch(Aux_AT_Status)
			{
				case AT_Modem_Poll:
					__AT_Modem_Poll();	// Периодические опросы модема
					break;
				case AT_Balance:			// Запрос баланса
					__AT_Balance();
					break;
				default:
					break;
			}
		}
		// Проверить наличие новых данных во входном буфере
		Aux_AT_Check_Recv_Buff();
	}
}




