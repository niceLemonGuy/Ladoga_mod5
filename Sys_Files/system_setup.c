/**
  ******************************************************************************
  * @file    system_setup.c
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    10/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */

/* Includes -------------------------------------------------------------------------*/
#include "includes.h"


/* Global variables -----------------------------------------------------------------*/
U16 ADC_buf[MAX_ADC_NUM];	// Буфер для измерений каналов АЦП



/* Private functions ----------------------------------------------------------------*/
/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	// 02.12.2010

	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 02.12.2010 

	/* Update the SysTick IRQ priority should be higher than the Ethernet IRQ */
	/* The Localtime should be updated during the Ethernet packets processing */
	NVIC_SetPriority (SysTick_IRQn, 2);  // 11.01.2011 

	// Enable the TIM2 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the TIM4 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the DMA1_Channel2 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the DMA1_Channel4 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the USART1 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the USART3 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the UART5 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Configure and enable ADC interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

  //-|   
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* Enable GPIOs clocks */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
	
	/** @start GPIO init @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	/** SIMSEL_PIN **/
	GPIO_InitStruct.GPIO_Pin = SIMSEL_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SIMSEL_PORT, &GPIO_InitStruct);
	
	/** TEST_PIN **/
	GPIO_InitStruct.GPIO_Pin = TEST_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(TEST_PORT, &GPIO_InitStruct);
	
	/** TAMPER_PIN **/
	GPIO_InitStruct.GPIO_Pin = TAMPER_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(TAMPER_PORT, &GPIO_InitStruct);
	
	/** @start USART1 GPIO init @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	/** USART1_TX **/
	GPIO_InitStruct.GPIO_Pin = USART1_TX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_TX_PORT, &GPIO_InitStruct);
	
	/** USART1_RX **/
	GPIO_InitStruct.GPIO_Pin = USART1_RX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1_RX_PORT, &GPIO_InitStruct);
	
	/** @start USART3 GPIO init @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	/** USART3_TX **/
	GPIO_InitStruct.GPIO_Pin = USART3_TX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART3_TX_PORT, &GPIO_InitStruct);
	
	/** USART3_RX **/
	GPIO_InitStruct.GPIO_Pin = USART3_RX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART3_RX_PORT, &GPIO_InitStruct);

	/** USART3_CTC **/
	GPIO_InitStruct.GPIO_Pin = USART3_CTC_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART3_CTC_PORT, &GPIO_InitStruct);

	/** USART3_RTS **/
	GPIO_InitStruct.GPIO_Pin = USART3_RTS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART3_RTS_PORT, &GPIO_InitStruct);
	
	/** @start UART5 GPIO init @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	/** UART5_TX **/
	GPIO_InitStruct.GPIO_Pin = UART5_TX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UART5_TX_PORT, &GPIO_InitStruct);
	
	/** UART5_RX **/
	GPIO_InitStruct.GPIO_Pin = UART5_RX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(UART5_RX_PORT, &GPIO_InitStruct);
	
	/** @start Touch Memory GPIO init @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	/** TM_ZUM **/
	GPIO_InitStruct.GPIO_Pin = TM_ZUM_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TM_ZUM_PORT, &GPIO_InitStruct);
	
	/** TM_VD **/
	GPIO_InitStruct.GPIO_Pin = TM_VD_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TM_VD_PORT, &GPIO_InitStruct);
	
	/** TM_High **/
	GPIO_InitStruct.GPIO_Pin = TM_HIGH_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TM_HIGH_PORT, &GPIO_InitStruct);
	
	/** TM_Low **/
	GPIO_InitStruct.GPIO_Pin = TM_LOW_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TM_LOW_PORT, &GPIO_InitStruct);
	
	/** TM_Read **/
	GPIO_InitStruct.GPIO_Pin = TM_READ_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TM_READ_PORT, &GPIO_InitStruct);
	
	/** @start Led GPIO init @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/	
	/** ACB_GREEN **/
	GPIO_InitStruct.GPIO_Pin = ACB_GREEN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ACB_GREEN_PORT, &GPIO_InitStruct);
	
	/** ACB_RED **/
	GPIO_InitStruct.GPIO_Pin = ACB_RED_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ACB_RED_PORT, &GPIO_InitStruct);
	
	/** V220_GREEN **/
	GPIO_InitStruct.GPIO_Pin = V220_GREEN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(V220_GREEN_PORT, &GPIO_InitStruct);
	
	/** V220_RED **/
	GPIO_InitStruct.GPIO_Pin = V220_RED_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(V220_RED_PORT, &GPIO_InitStruct);
	
	/** INS_GREEN **/
	GPIO_InitStruct.GPIO_Pin = INS_GREEN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(INS_GREEN_PORT, &GPIO_InitStruct);
	
	/** INS_RED **/
	GPIO_InitStruct.GPIO_Pin = INS_RED_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(INS_RED_PORT, &GPIO_InitStruct);
	
	/** GUARD_GREEN **/
	GPIO_InitStruct.GPIO_Pin = GUARD_GREEN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GUARD_GREEN_PORT, &GPIO_InitStruct);
	
	/** GUARD_RED **/
	GPIO_InitStruct.GPIO_Pin = GUARD_RED_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GUARD_RED_PORT, &GPIO_InitStruct);
	
	/** @start Set all led GPIO @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	ALL_LED_OFF();
}

/**
  * @brief  Initialize UARTs
  * @param  None
  * @retval None
  */
void UART_Configuration(void)
{
	
	// Создать структуру для инициализации USART	
	USART_InitTypeDef USART_InitStruct;
	
	/** @start USART1 init  @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	// Разрешить тактирование USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	// Настройка USART1
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No ;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// Инициализация USART1
	USART_Init(USART1, &USART_InitStruct);
	// Запуск USART1
	USART_Cmd(USART1, ENABLE);
	// Включить взаимодействие DMA с USART1
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	// Разрешить прерывания на приём
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/** @start USART3 init  @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	// Тактирование USART3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	// Переназначить пины USART3
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
	// Настройка USART3
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No ;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// Инициализация USART3
	USART_Init(USART3, &USART_InitStruct);
	// Запуск USART3
	USART_Cmd(USART3, ENABLE);	
	// Включить взаимодействие DMA с USART3
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
	// Разрешить прерывания на приём
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	/** @start UART5 init  @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	// Тактирование UART5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	// Настройка UART5
	USART_InitStruct.USART_BaudRate = 57600;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No ;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// Инициализация UART5
	USART_Init(UART5, &USART_InitStruct);
	// Запуск UART5
	USART_Cmd(UART5, ENABLE);	
	// Разрешить прерывания на приём
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
}

/**
  * @brief  Initialize DMA
  * @param  None
  * @retval None
  */
void DMA_Configuration(void)
{
	// Создать структуру для инициализации DMA
	DMA_InitTypeDef DMA_Init_Str;
	//Тактирование DMA1
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/** @start DMA1_Channel1	@vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	DMA_Init_Str.DMA_PeripheralBaseAddr = DR_ADDRESS;
	DMA_Init_Str.DMA_MemoryBaseAddr = (U32)ADC_buf;
	DMA_Init_Str.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_Init_Str.DMA_BufferSize = MAX_ADC_NUM;
	DMA_Init_Str.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Init_Str.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Init_Str.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_Init_Str.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_Init_Str.DMA_Mode = DMA_Mode_Circular;
	DMA_Init_Str.DMA_Priority = DMA_Priority_High;
	DMA_Init_Str.DMA_M2M = DMA_M2M_Disable;
	// Инициализация DMA_Init_Str
	DMA_Init(DMA1_Channel1, &DMA_Init_Str);
	// Enable DMA1 channel1
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/** @start DMA1_Channel2	@vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	DMA_Init_Str.DMA_PeripheralBaseAddr = (u32)&USART3->DR;
	DMA_Init_Str.DMA_MemoryBaseAddr = (u32)USART3_Tx_buf;
	DMA_Init_Str.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_Init_Str.DMA_BufferSize = sizeof USART3_Tx_buf;
	DMA_Init_Str.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Init_Str.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Init_Str.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_Init_Str.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Init_Str.DMA_Mode = DMA_Mode_Normal;
	DMA_Init_Str.DMA_Priority = DMA_Priority_Medium;
	DMA_Init_Str.DMA_M2M = DMA_M2M_Disable;
	// Инициализация DMA_Init_Str
	DMA_Init(DMA1_Channel2, &DMA_Init_Str);
	// Включить прерывания на передачу
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	
	/** @start DMA1_Channel4	@vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv**/
	DMA_Init_Str.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
	DMA_Init_Str.DMA_MemoryBaseAddr = (u32)USART1_Tx_buf;
	DMA_Init_Str.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_Init_Str.DMA_BufferSize = USART1_Tx_buf_SIZE;
	DMA_Init_Str.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Init_Str.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_Init_Str.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_Init_Str.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Init_Str.DMA_Mode = DMA_Mode_Normal;
	DMA_Init_Str.DMA_Priority = DMA_Priority_Medium;
	DMA_Init_Str.DMA_M2M = DMA_M2M_Disable;
	// Инициализация DMA_Init_Str
	DMA_Init(DMA1_Channel4, &DMA_Init_Str);
	// Включить прерывания на передачу
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
}

/**
  * @brief  Initialize TIMs
  * @param  None
  * @retval None
  */
void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	TIM_OCInitTypeDef         TIM_OCInitStructure;

	/** @brief TIM6 init @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv **/
	// Включить тактирование таймера
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	// Установка значений по умолчанию
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	// Инициализация таймера
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	/** @brief TIM3 init @vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv **/
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

	/* -----------------------------------------------------------------------
	TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
	TIM3CLK = 72 MHz, Prescaler = 0x0, TIM3 counter clock = 72 MHz
	TIM3 ARR Register = 28799 => TIM3 Frequency = TIM3 counter clock/(ARR + 1)
	TIM3 Frequency = 2.5 KHz.
	TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%  => CCR1_Val = 28799 * 0,5
	TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
	TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
	TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
	----------------------------------------------------------------------- */

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 28799;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 14400;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //оставляет вывод порта в "1"

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
}

/**
  * @brief  Initialize ADC channels
  * @param  None
  * @retval None
  */
void ADC_Configuration( void )
{
	ADC_InitTypeDef  ADC_InitStructure;

	//Enable peripheral clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // -реализован запуск по команде (ENABLE - непрерывное выполнение )
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = MAX_ADC_NUM;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  1, ADC_SampleTime_239Cycles5); // ADC7
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  2, ADC_SampleTime_239Cycles5); // ADC5
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6,  3, ADC_SampleTime_239Cycles5); // ADC4
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8,  4, ADC_SampleTime_239Cycles5); // ADC6

	ADC_DMACmd(ADC1, ENABLE); // Enable ADC1 DMA 

	ADC_Cmd(ADC1, ENABLE); // Enable ADC1 

	ADC_ResetCalibration(ADC1); // Enable ADC1 reset calibaration register

	while(ADC_GetResetCalibrationStatus(ADC1)); // Check the end of ADC1 reset calibration register

	ADC_StartCalibration(ADC1); // Start ADC1 calibaration 

	while(ADC_GetCalibrationStatus(ADC1)); // Check the end of ADC1 calibration

	ADC_SoftwareStartConvCmd(ADC1, ENABLE); // Start ADC1 Software Conversion

	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1)); // Test on Channel 1 DMA1_FLAG_TC flag 

	DMA_ClearFlag(DMA1_FLAG_TC1); // Clear Channel 1 DMA1_FLAG_TC flag 
}

/**
  * @brief  Setup STM32 system (clocks, Ethernet, GPIO, NVIC) and STM3210C-EVAL resources.
  * @param  None
  * @retval None
  */
void System_Setup(void)
{
	/* Setup STM32 clock, PLL and Flash configuration) */
	SystemInit();

	/* NVIC configuration */
	NVIC_Configuration();  

	/* Configure the GPIO ports */
	GPIO_Configuration();

	/* UART configuration */
	UART_Configuration();

	/* DMA configuration */
	DMA_Configuration();

	/* TIM configuration */
	TIM_Configuration();

	/* ADC configuration */
	ADC_Configuration();

	//Сброс через 6600 мс
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	// IWDG counter clock: 40KHz(LSI) / 64 = 0.625 KHz 
	IWDG_SetPrescaler(IWDG_Prescaler_64);

	// Set counter reload value to 0xfff
	IWDG_SetReload(0xfff);

	// Reload IWDG counter 
	IWDG_ReloadCounter();

	DBGMCU->CR = DBGMCU_CR_DBG_IWDG_STOP | DBGMCU_CR_DBG_I2C1_SMBUS_TIMEOUT;

	// Enable IWDG (the LSI oscillator will be enabled by hardware)
	IWDG_Enable();
}


