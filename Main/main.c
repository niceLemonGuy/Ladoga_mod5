/**
  ******************************************************************************
  * @file    main.c
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    10/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
	
/* Includes ------------------------------------------------------------------*/
#include "includes.h"


char Version_Num[] = {"5.1.0"};
#define Version_Mark 	0x28122018
	

/* Functions ---------------------------------------------------------------- */
/** 
	* @brief	Work_Start()
	* @note		Начало работы
	*/
void Work_Start(void)
{	
	// Записать субъективный номер версии
	Flash_Write_Version_Mark(Version_Mark);
	
	// Считать данные о приборе из FLASH памяти
	Flash_Read_Device_Data();
	
	// Считать информацию о подключённых УО из FLASH памяти
	Flash_Read_UO_Info();
	
	// Проверка авторизации
	ZA_Check_Auth();
	
	// Проверка событий во FLASH памяти
	ZA_Check_Flash_Events();
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *----------------------------------------------------------------------------*/
int main(void)	/* Program execution starts here ******************************/
{	
	Serial_Number();
  System_Setup();
	Work_Start();
	
  os_sys_init(t_Init);	/* Init and start with task 't_Init' ******************/
	
	/* We should never get here as control is now taken by the scheduler ********/

  /* Infinite loop */
  for(;;)
  {
		
  }
}

/* Task init stacks	***********************************************************/
U8 t_Brshs_recv_stack[1024];
U8 t_Time_Counter_stack[128];
U8 t_Touch_Memory_stack[512];
U8 t_Brshs_Handler_stack[4096];
U8 t_Device_Handler_stack[1024];
U8 t_Aux_Modem_Handler_stack[2048];
U8 t_Main_Modem_Handler_stack[4096];
U8 t_Indication_Manager_stack[1024];
U8 t_ZA_Protocol_Handler_stack[2048];

/*----------------------------------------------------------------------------
 *        Task 'Init': Initialize
 *----------------------------------------------------------------------------*/
__task void t_Init (void) 
{	
	// Тестовый режим
	Device_Test_Check();
	
	tid_Time_Counter				= os_tsk_create_user(t_Time_Counter, 2, t_Time_Counter_stack, sizeof t_Time_Counter_stack);												// Start task t_Time_Counter
	tid_Touch_Memory				= os_tsk_create_user(t_Touch_Memory, 8, t_Touch_Memory_stack, sizeof t_Touch_Memory_stack);												// Start task t_Touch_Memory
	tid_Brshs_Handler				= os_tsk_create_user(t_Brshs_Handler, 4, t_Brshs_Handler_stack, sizeof t_Brshs_Handler_stack);										// Start task t_Brshs_Handler
	tid_Device_Handler			= os_tsk_create_user(t_Device_Handler, 1, t_Device_Handler_stack, sizeof t_Device_Handler_stack);									// Start task t_Device_Handler
	tid_Aux_Modem_Handler		= os_tsk_create_user(t_Aux_Modem_Handler, 5, t_Aux_Modem_Handler_stack, sizeof t_Aux_Modem_Handler_stack);				// Start task t_Aux_Modem_Handler
	tid_Main_Modem_Handler	= os_tsk_create_user(t_Main_Modem_Handler, 7, t_Main_Modem_Handler_stack, sizeof t_Main_Modem_Handler_stack);			// Start task t_Main_Modem_Handler
	tid_Indication_Manager	= os_tsk_create_user(t_Indication_Manager, 3, t_Indication_Manager_stack, sizeof t_Indication_Manager_stack);			// Start task t_Indication_Manager
	tid_ZA_Protocol_Handler	= os_tsk_create_user(t_ZA_Protocol_Handler, 6, t_ZA_Protocol_Handler_stack, sizeof t_ZA_Protocol_Handler_stack);	// Start task t_ZA_Protocol_Handler
	
	Device_Test_Handler(Version_Num);
  
	os_tsk_delete_self ();	// Delete init task (no longer needed)
}



