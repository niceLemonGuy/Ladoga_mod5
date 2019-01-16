/**
  ******************************************************************************
  * @file    touch_memory.c
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    10/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
	

/* Includes ----------------------------------------------------------------------*/
#include "includes.h"
#include "keyboard.h"


/* Task ID -----------------------------------------------------------------------*/
OS_TID tid_Touch_Memory;	// ID задачи t_Touch_Memory
	
	
/* Global variables --------------------------------------------------------------*/
U8 TM_Buf[TM_BUF_LEN];	// Буфер приема ТМ


/* Private variables -------------------------------------------------------------*/
const U8 READ_KEYBOARD	= 0x93; // Команда чтения клавиатуры
const U8 READ_PZU	= 0x33;	// Команда чтения ПЗУ ТМ


/* Private defines ---------------------------------------------------------------*/
#define DS1961S			(U8)0x33	// Код семейства
#define DS1963S 		(U8)0x18	// Код семейства
#define FAMILY_CODE		(U8)0   	// Адрес кода семейства в буфере ТМ
#define SAZE_MASS_TM		(U8)80		// Количество кодов ключей в ЕЕПРОМ
#define KOD_KL_6		(U8)6		// Длина кода ключа


/* Functions ---------------------------------------------------------------------*/
/**
 * @brief	delay_ms()
 * @note	Миллисекундная задержка. Использовать только в задаче Touch_Memory. 
 *		Для других задач os_dly_tsk()
 */
void delay_ms(uint16_t delay)
{
	TIM6->PSC = (uint16_t)(SYSTEM_CORE_CLOCK / 10000) - 1;	// Устанавливаем предделитель 7200
	TIM6->ARR = delay * 10;	// Устанавливаем значение переполнения таймера, умножается на 10, потому что предделитель 7200, а не 72000
	TIM6->EGR |= TIM_EGR_UG;	// Генерируем Событие обновления для записи данных в регистры PSC и ARR
	TIM6->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM;	// Запускаем таймер записью бита CEN и устанавливаем режим Одного прохода установкой бита OPM
	while((TIM6->CR1 & TIM_CR1_CEN) != 0);
}

/**
 * @brief	delay_us()
 * @note	Микросекундная задержка. Использовать только в задаче Touch_Memory. 
 *		Для других задач os_dly_tsk()
 */
void delay_us(uint16_t delay)
{ 	
	TIM6->PSC = (uint16_t)(SYSTEM_CORE_CLOCK / 1000000) - 1;	// Устанавливаем предделитель 72
	TIM6->ARR = delay;	// Устанавливаем значение переполнения таймера
	TIM6->EGR |= TIM_EGR_UG;	// Генерируем Событие обновления для записи данных в регистры PSC и ARR
	TIM6->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM;	// Запускаем таймер записью бита CEN и устанавливаем режим Одного прохода установкой бита OPM
	while((TIM6->CR1 & TIM_CR1_CEN) != 0);
}
	
/**
 * @brief	Процедура инициализации интерфейса 1-WIRE.
 * @note	Процедура длиться минимум 960мкс. 
 *			1. МК передаёт импульс RESET. Осуществляется подачей высокого уровня на TM_LOW_PIN на 700мкс.
 *			2. МК находится в состоянии приёма, т.е. слушает TM_READ_PIN.
 *			3. Устройство передаёт импульс PRESENCE - низкий уровень на TM_READ_PIN около 100мкс.
 * @return	Ответ от устройства:
 *			0: получен
 *			1: не получен
 */
int reset_pulse(void)
{
	uint8_t tmp = 0;
	volatile uint32_t count;

	// 700мкс импульс
	GPIO_SetBits(TM_LOW_PORT, TM_LOW_PIN);
	delay_us(700);
	GPIO_ResetBits(TM_LOW_PORT, TM_LOW_PIN);
	
	// Ожидание возврата шины на высокий уровень - 1
	count = 100;
	while ((!(GPIOE->IDR & TM_READ_PIN))|(!count)) {GPIOE->BSRR |= TM_HIGH_PIN; count--;}
  	GPIOE->BRR |= TM_HIGH_PIN;
	if(!count) return 1; // Сброс передачи, если высокого уровня так и не было
	
	delay_us(8);
	
	// Ожидание импульса присутствия TM_READ_PIN
	for(count = 0; count < 28; count++)
	{
		// Чтение пина TM_READ_PIN в течении 300мкс каждые 10мкс
		if(!(GPIOE->IDR & TM_READ_PIN)) tmp++; 
		delay_us(10);
	}
	
	// Ожидание возврата шины на высокий уровень - 2
	count = 100;
	while ((!(GPIOE->IDR & TM_READ_PIN))|(!count)) { GPIOE->BSRR |= TM_HIGH_PIN; count--;}
	GPIOE->BRR |= TM_HIGH_PIN;
	if(!count) return 1; // Сброс передачи, если высокого уровня так и не было
	
	// Если больше 6-ти раз прочитан низкий уровень, значит это импульс PRESENCE от устройства(предохранитель от ложных срабатываний)
	if(tmp >= 4 && tmp < 25)
	{
		// Ожидание возврата шины на высокий уровень - 3
		count = 100;
		while ((!(GPIOE->IDR & TM_READ_PIN))|(!count)) { GPIOE->BSRR |= TM_HIGH_PIN; count--;}
		GPIOE->BRR |= TM_HIGH_PIN;
		
		// Включить индикацию нажатия клавиши
		Indication_mod_stat = Ind_TM_Pressed_key;
		
		return 0; // Успешное завершение
	}
	else return 1; // Завершение с ошибкой
}

/**
 * @brief	Передача одного байта команды ТМ
 * @note   	Процедура передачи осуществляется тайм-слотом. Тайм-слот - это лимитированная по веремени 
 *		последовательность смены уровней сигнала. Между отдельными тайм-слотами предусмотрен интервал не менее 1мкс.
 *		Тайм-слот начинает МК путём перевода шины 1-WIRE на низкий логический уровень. Длительность тайм-слота
 *		находится в пределах от 60 до 120мкс
 * @param	data: байт команды на передачу
 */
void prd_TM(uint8_t data)
{
	int i;
	volatile uint32_t count;
	
	for(i=0; i<8; i++) // Колличество повторов в цикле соответствует кол-ву бит
	{
		if(data & 0x01) // Передача 1
		{
			// Перевод шины на низкий логический уровень. Начало тайм-слота. 
			GPIO_SetBits(TM_LOW_PORT, TM_LOW_PIN);
			delay_us(6);
			GPIO_ResetBits(TM_LOW_PORT, TM_LOW_PIN);
			
			// Ожидание возврата шины на высокий уровень - 1
			count = 100;
			while ((!(GPIOE->IDR & TM_READ_PIN))|(!count)) {GPIOE->BSRR |= TM_HIGH_PIN; count--;}
			GPIOE->BRR |= TM_HIGH_PIN;
			if(!count) break;	// Сброс передачи, если высокого уровня так и не было

			// Передача 1 в течении 50мск
			delay_us(50);
		}
		else	// Передача 0
		{
			// Передача 0 в течении 68мск
			GPIO_SetBits(TM_LOW_PORT, TM_LOW_PIN);
			delay_us(60);
			GPIO_ResetBits(TM_LOW_PORT, TM_LOW_PIN);
			
			// Ожидание возврата шины на высокий уровень - 2
			count = 100;
			while ((!(GPIOE->IDR & TM_READ_PIN))|(!count)) {GPIOE->BSRR |= TM_HIGH_PIN; count--;}
			GPIOE->BRR |= TM_HIGH_PIN;
			if(!count) break; // Сброс передачи, если высокого уровня так и не было
		}
		// При удачной передаче сдвигаем data на бит вправо для передачи следующего бита
		data >>= 1;
		delay_us(8); // Задержка между тайм-слотами 8мкс
	}
}

/**
 * @brief	Прием одного байта ТМ
 * @return	Принятый байт
 */
uint8_t	prm_TM(void)
{
	uint8_t	i, data = 0;
  	uint8_t  bit_0, bit_1;
	volatile uint32_t count;
	
	for(i=0;i<8;i++)
	{
		bit_0 = 0;
		bit_1 = 0;
		
		GPIO_SetBits(TM_LOW_PORT, TM_LOW_PIN);
		delay_us(6);
		GPIO_ResetBits(TM_LOW_PORT, TM_LOW_PIN);
		
		GPIO_SetBits(TM_HIGH_PORT, TM_HIGH_PIN);
		delay_us(1);
		GPIO_ResetBits(TM_HIGH_PORT, TM_HIGH_PIN);
		
		count = 70;                // Trelease = 15 мкс
		while(count)  
		{ count--;
		  if(!(GPIOE->IDR & TM_READ_PIN)) bit_0++;
		  else bit_1++;
		}
		
		if(bit_0 > bit_1)	data >>= 1;
  		else data = ((data >> 1) | 0x80);
		delay_us(25);
	}
	return data;
}



/*----------------------------------------------------------------------------
 *        Task 'Touch_Memory'
 *---------------------------------------------------------------------------*/
__task void t_Touch_Memory(void)
{
	int i;
	
	for(;;)
	{
		os_dly_wait(30);
		
		if(!reset_pulse())
		{
			prd_TM(READ_PZU);
			for(i = 0; i < TM_BUF_LEN; i++)
			{
				TM_Buf[i] = prm_TM();
				
				if(TM_Buf[0] == 0xc1)
				{
					keyboard_handler(TM_Buf);
					memset(TM_Buf, 0, TM_BUF_LEN);
					Indication_mod_stat = Ind_TM_Cmnd_recv;
				}
			}
			os_dly_wait(100);
		}
	}
}

	
	
	
	
	
	
