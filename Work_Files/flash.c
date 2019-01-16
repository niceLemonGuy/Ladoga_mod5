/**
  ******************************************************************************
  * @file    flash.c
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    03/12/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"


/* Types for flash -----------------------------------------------------------*/
static Flash_GCP_Queue_u Flash_GCP_Queue;
static Flash_Device_Data_u Flash_Device_Data;
static Flash_UO_Info_u Flash_UO_Info;


/* Variables -----------------------------------------------------------------*/
U8 Flash_GCP_Queue_Len = 0, Flash_GCP_Queue_Head = 0; 


/* Functions -----------------------------------------------------------------*/

//pageAddress - любой адрес, принадлежащий стираемой странице
void Internal_Flash_Erase(U32 pageAddress) {
	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = pageAddress;
	FLASH->CR |= FLASH_CR_STRT;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
}

//data - указатель на записываемые данные
//address - адрес во flash
//count - количество записываемых байт, должно быть кратно 2
void Internal_Flash_Write(U8 *data, U32 address, U32 count) {
	U32 i;

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i < count; i += 2) {
		*(volatile U8 *)(address + i) = (((U8)data[i + 1]) << 8) + data[i];
		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}

/**	
	* @brief	FLASH_Program_Array()
  * @note		Write array in internal Flash Memory. На одной странице Flash Momory должен быть только один такой массив
  * @param	adr: specifies the address to be programmed
  * @param	data: specifies pointer to the data to be programmed
  * @param	len: specifies size of Array on half word to be programmed
  * @retval	result
  */
bool FLASH_Program_Array(U32 adr, U16 *data, U16 len)
{
	bool res = false;
	U16 read_data = 0, cnt = 0;
	FLASH_Status F_Status;

  FLASH_Unlock();
	
  while(!res && (cnt < len))
  {	
		read_data = *((uint32_t *)(adr + cnt * 2));
    if (read_data == 0xffff)
			cnt++;
		else
			res = true;
  }
	
  if(res)
  {// Указанное место на странице не пустое
    F_Status = FLASH_GetStatus();
    if (F_Status == FLASH_ERROR_PG)
      FLASH_ClearFlag(FLASH_FLAG_PGERR);
    //F_Status = FLASH_ErasePage(adr);  
  }

  F_Status = FLASH_GetStatus();
	
  if(F_Status == FLASH_ERROR_PG)
		FLASH_ClearFlag(FLASH_FLAG_PGERR);
	
  res = false; 
	cnt = 0;
	
  while (!res && (cnt < len))
  {
    F_Status = FLASH_ProgramHalfWord(adr + (cnt * 2), *(data + cnt));
		if (F_Status == FLASH_COMPLETE)
		{ 
			read_data = *((U32 *)(adr + (cnt * 2)));
			if (read_data == *(data + cnt))
				cnt++;
			else
				res = true;
		}
		else
      res = true;
  }

  return res;
}

/**	
	* @brief	FLASH_Read_Array()
  * @note		Read array from internal Flash Memory
  * @param	adr: specifies the address to be read
  * @param	data: specifies pointer to the data to be read
  * @param	len: specifies size of Array on half word to be read
  * @retval	None
  */
void FLASH_Read_Array(U32 adr, U16 *data, U16 len)
{
  U16 i;

  for(i=0; i<len; i++)
    *(data + i) = *((U32 *)(adr + (i * 2)));
}

/**	
	* @brief	FLASH_Erase_Page()
  * @note		Delete array from internal Flash Memory
  * @param	adr: specifies the address to be read
  * @retval	None
  */
void FLASH_Erase_Page(U32 adr)
{
  FLASH_Status F_Status;

  FLASH_Unlock();
  F_Status = FLASH_ErasePage(adr);  
  if (F_Status == FLASH_ERROR_PG)
    FLASH_ClearFlag(FLASH_FLAG_PGERR);
}

/**	
	* @brief	Flash_Write_Version_Mark()
  * @note		Записать субъективный номер версии
  */
void Flash_Write_Version_Mark(U32 v_mark)
{
	FLASH_Program_Array(FLASH_VERSION_MARK, (U16 *)&v_mark, 2);
}

/**	
	* @brief	Flash_Write_Device_Data()
  * @note		Записать структуру Device_Data во FLASH память
  * @retval	Результат записи
  */
bool Flash_Write_Device_Data(void)
{	
	if(sizeof(Device_Data_t) > FLASH_PAGE_SIZE)
  {// Если сл событие вылезает за пределы страницы - начать заполнять очередь с начала
    return 0;
  }
  memcpy(&Flash_Device_Data.data, &Device_Data, sizeof(Device_Data_t));
  return FLASH_Program_Array(FLASH_DEVICE_DATA_PAGE, Flash_Device_Data.Flash_HalfWords, sizeof(Flash_Device_Data_u)/2);
}

/**	
	* @brief	Flash_Read_Device_Data()
  * @note		Прочитать структуру Device_Data из FLASH памяти
  */
void Flash_Read_Device_Data(void)
{
	FLASH_Read_Array(FLASH_DEVICE_DATA_PAGE, (U16 *)Flash_Device_Data.Flash_HalfWords, sizeof(Flash_Device_Data_u)/2);
	memcpy(&Device_Data, &Flash_Device_Data.data, sizeof(Device_Data_t));
}

/**	
	* @brief	Flash_Erase_Device_Data()
  * @note		Стереть структуру Device_Data из FLASH памяти
  */
void Flash_Erase_Device_Data(void)
{
	Internal_Flash_Erase(FLASH_DEVICE_DATA_PAGE);
	
//	memset(&Flash_Device_Data, 0xff, sizeof(Flash_Device_Data_u));
//  FLASH_Program_Array(FLASH_UO_INFO_PAGE, (U16 *)Flash_UO_Info.Flash_HalfWords, sizeof(Flash_Device_Data_u)/2);
}

/**	
	* @brief	Flash_Write_UO_Info()
  * @note		Записать структуру UO_Info во FLASH память
  * @param	data: данные для записи
	* @param	num: порядковый номер структуры во FLASH памяти
	* @retval	Результат записи
  */
bool Flash_Write_UO_Info(RF_UO_Info_t *info, U8 num)
{ 
	Flash_UO_Info.flash_str.stat = (U8)BUSY_SECTOR;
  memcpy(&Flash_UO_Info.flash_str.info, info, sizeof(RF_UO_Info_t));
  return FLASH_Program_Array(FLASH_UO_INFO_PAGE + (sizeof(Flash_UO_Info_u) * num), (U16 *)Flash_UO_Info.Flash_HalfWords, sizeof(Flash_UO_Info_u)/2);
}

/**	
	* @brief	Flash_Read_UO_Info()
  * @note		Прочитать структуры UO_Info из FLASH памяти
  */
void Flash_Read_UO_Info(void)
{
	U8 i;
	for(i = 0; i < MAX_UO_NUM; i++)
	{
		FLASH_Read_Array(FLASH_UO_INFO_PAGE + (sizeof(Flash_UO_Info_u) * i), (U16 *)Flash_UO_Info.Flash_HalfWords, sizeof(Flash_UO_Info_u)/2);
		if(Flash_UO_Info.flash_str.stat == (U8)BUSY_SECTOR)
			memcpy(&RF_UO_Data[i].Info, &Flash_UO_Info.flash_str.info, sizeof(RF_UO_Info_t));
	}
}

/**	
	* @brief	Flash_Erase_UO_Info()
  * @note		Стереть структуру UO_Info из FLASH памяти
	* @param	str_num: номер структуры, которую нужно удалить из FLASH памяти
	* @retval	Результат записи
  */
bool Flash_Erase_UO_Info(U8 num)
{
	U16 Flash_Page[FLASH_PAGE_SIZE/2];
	
	FLASH_Read_Array(FLASH_UO_INFO_PAGE, Flash_Page, FLASH_PAGE_SIZE/2);
	memset((U8 *)Flash_Page + (sizeof(Flash_UO_Info_u) * num), 0xff, sizeof(Flash_UO_Info_u));
	Internal_Flash_Erase(FLASH_UO_INFO_PAGE);
	return FLASH_Program_Array(FLASH_UO_INFO_PAGE, (U16 *)Flash_Page, sizeof(Flash_Page));
}

/**	
	* @brief	Flash_Write_GCP_Event()
  * @note		Записать событие во FLASH память
  * @param	event: событие для записи
  * @retval	Результат записи
  */
bool Flash_Write_GCP_Event(ZA_GCP_Event_t *event)
{  
  if((Flash_GCP_Queue_Len + 1) * sizeof(Flash_GCP_Queue_t) > FLASH_PAGE_SIZE)
  {// Если сл событие вылезает за пределы страницы - начать заполнять очередь с начала
    Flash_GCP_Queue_Len = 0; 
		Flash_GCP_Queue_Head = 0;
  }
	Flash_GCP_Queue.flash_str.stat = BUSY_SECTOR;
  memcpy(&Flash_GCP_Queue.flash_str.event, event, sizeof(ZA_GCP_Event_t));
  return FLASH_Program_Array(FLASH_GCP_QUEUE_PAGE + (sizeof(Flash_GCP_Queue_u) * Flash_GCP_Queue_Len), (U16 *)Flash_GCP_Queue.Flash_HalfWords, sizeof(Flash_GCP_Queue_u)/2);
}

/**	
	* @brief	Flash_Read_Events()
  * @note		Прочитать все события из FLASH памяти и сохранить в очередь
	* @param	queue: указатель на очередь событий
  */
void Flash_Read_Events(Event_Queue_t *queue)
{
	U8 i;
	for(i = 0; i < EVENT_QUEUE_MAX; i++)
	{
		FLASH_Read_Array(FLASH_GCP_QUEUE_PAGE + (sizeof(Flash_GCP_Queue_u) * i), (U16 *)Flash_GCP_Queue.Flash_HalfWords, sizeof(Flash_GCP_Queue_u)/2);
		if(Flash_GCP_Queue.flash_str.stat == BUSY_SECTOR)
		{
			memcpy(&queue->event, &Flash_GCP_Queue.flash_str.event, sizeof(ZA_GCP_Event_t));
			queue->len++;
		}
	}
}

/**	
	* @brief	Flash_Erase_Event()
  * @note		Удалить событие из FLASH памяти
	* @param	event_num: номер события, которое нужно удалить из FLASH памяти
  */
void Flash_Erase_Event(U8 event_num)
{
	U16 Flash_Page[FLASH_PAGE_SIZE/2];
	
	FLASH_Read_Array(FLASH_GCP_QUEUE_PAGE, Flash_Page, FLASH_PAGE_SIZE/2);
	memset((U8 *)Flash_Page + (sizeof(Flash_GCP_Queue_u) * event_num), 0xff, sizeof(Flash_GCP_Queue_u));
	FLASH_Erase_Page(FLASH_GCP_QUEUE_PAGE);
	FLASH_Program_Array(FLASH_GCP_QUEUE_PAGE, (U16 *)Flash_Page, sizeof(Flash_Page));
	
//	memset(&Flash_GCP_Queue, 0xff, sizeof(Flash_GCP_Queue_u));
//  FLASH_Program_Array(FLASH_GCP_QUEUE_PAGE + (sizeof(Flash_GCP_Queue_u) * event_num), (U16 *)Flash_GCP_Queue.Flash_HalfWords, sizeof(Flash_GCP_Queue_u)/2);
}

