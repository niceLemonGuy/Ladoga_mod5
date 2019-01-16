/**
  ******************************************************************************
  * @file    defines.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    10/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
#ifndef __DEFINES_H
#define __DEFINES_H

// Максимальное количество подключаемых оконечных устройств
#define MAX_UO_NUM							32

// Максимальное количество зон
#define MAX_ZONE_NUM						16

// Максимальный размер пакета передаваемого или принимаемого по TCP
#define MAX_TCP_PKT_LEN					1024

// Допустимое кол-во ошибок связи
#define MAX_LINK_ERR						3

// Максимальное количество событий в очереди
#define EVENT_QUEUE_MAX					MAX_ZONE_NUM * 2

// Время ожидания ответа по TCP [сек]
#define TCP_WAIT_ANSWER_TIME		10

// Периодический опрос [сек]
#define PING_SEND_PERIOD				10

// Период отправки событий
#define GUARD_SEND_PERIOD				5

// Кол-во общих опросов перед индивидуальным опросом каждого УО
#define MAX_UO_GLOBAL_POLL			5




#endif	// __DEFINES_H
	
	


