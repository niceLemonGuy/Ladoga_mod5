/**
  ******************************************************************************
  * @file    gpio_defines.h
  * @author  Roman_Yudin
  * @version V1.0.0
  * @date    10/10/2018
  * @brief   
  ******************************************************************************
  * @copy
  *
  *
  */
#ifndef __GPIO_DEFINES_H
#define __GPIO_DEFINES_H


/*
 * нельзя использовать следующие выводы т.к. они используются отладчиком:
 * PA13, PA14, PA15, PB3, PB4
 */


//GPIOA
#define ADC_7_PIN 								GPIO_Pin_4
#define ADC_7_PORT 								GPIOA
#define ADC_5_PIN 								GPIO_Pin_5
#define ADC_5_PORT								GPIOA
#define ADC_4_PIN 								GPIO_Pin_6
#define ADC_4_PORT 								GPIOA
#define USART1_TX_PIN							GPIO_Pin_9
#define USART1_TX_PORT						GPIOA
#define USART1_RX_PIN							GPIO_Pin_10
#define USART1_RX_PORT						GPIOA
#define USB_DM_PIN 								GPIO_Pin_11
#define USB_DM_PORT 							GPIOA
#define USB_DP_PIN 								GPIO_Pin_12
#define USB_DP_PORT 							GPIOA
#define SWDIO											GPIO_Pin_13
#define SWCLK											GPIO_Pin_14
#define TEST_PIN									GPIO_Pin_15
#define TEST_PORT									GPIOA

#define NOT_USE_PIN_PORT_A 				GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_7|GPIO_Pin_8

//GPIOB
#define ADC_6_PIN 								GPIO_Pin_0
#define ADC_6_PORT 								GPIOB
#define JT_DO											GPIO_Pin_3
#define JT_TRST										GPIO_Pin_4
#define SIMSEL_PIN								GPIO_Pin_5
#define SIMSEL_PORT 							GPIOB
#define I2C_SCL_PIN								GPIO_Pin_6
#define I2C_SCL_PORT							GPIOB
#define I2C_SDA_PIN								GPIO_Pin_7
#define I2C_SDA_PORT							GPIOB
#define V220_RED_PIN 							GPIO_Pin_9
#define V220_RED_PORT 						GPIOB
#define USART3_CTC_PIN						GPIO_Pin_13
#define USART3_CTC_PORT						GPIOB
#define USART3_RTS_PIN						GPIO_Pin_14
#define USART3_RTS_PORT						GPIOB
#define INS_RED_PIN 							GPIO_Pin_15
#define INS_RED_PORT 							GPIOB

#define NOT_USE_PIN_PORT_B 				GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12

//GPIOC
#define V_REF_ON_PIN 							GPIO_Pin_3
#define V_REF_ON_PORT 						GPIOC
#define INS_GREEN_PIN 						GPIO_Pin_9
#define INS_GREEN_PORT 						GPIOC
#define USART3_TX_PIN							GPIO_Pin_10
#define USART3_TX_PORT						GPIOC
#define USART3_RX_PIN							GPIO_Pin_11
#define USART3_RX_PORT						GPIOC
#define UART5_TX_PIN							GPIO_Pin_12
#define UART5_TX_PORT							GPIOC

#define NOT_USE_PIN_PORT_C 				GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15

//GPIOD
#define GUARD_RED_PIN 						GPIO_Pin_0
#define GUARD_RED_PORT 						GPIOD
#define MCLR_PIN 									GPIO_Pin_1	// Сброс БРШС
#define MCLR_PORT 								GPIOD
#define UART5_RX_PIN							GPIO_Pin_2
#define UART5_RX_PORT							GPIOD
#define USART2_TX_PIN							GPIO_Pin_5
#define USART2_TX_PORT						GPIOD
#define USART2_RX_PIN							GPIO_Pin_6
#define USART2_RX_PORT						GPIOD
#define RESET3V3_PIN 							GPIO_Pin_7
#define RESET3V3_PORT 						GPIOD
#define USB_EN_PIN 								GPIO_Pin_10
#define USB_EN_PORT 							GPIOD
#define GUARD_GREEN_PIN						GPIO_Pin_13
#define GUARD_GREEN_PORT 					GPIOD

#define NOT_USE_PIN_PORT_D				GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15

//GPIOE
#define V220_GREEN_PIN 						GPIO_Pin_0
#define V220_GREEN_PORT 					GPIOE
#define TAMPER_PIN 								GPIO_Pin_1
#define TAMPER_PORT 							GPIOE
#define WP_PIN 										GPIO_Pin_2
#define WP_PORT 									GPIOE
#define ACB_RED_PIN 							GPIO_Pin_3
#define ACB_RED_PORT 							GPIOE
#define ACB_GREEN_PIN 						GPIO_Pin_4
#define ACB_GREEN_PORT 						GPIOE
#define TM_ZUM_PIN 								GPIO_Pin_10
#define TM_ZUM_PORT 							GPIOE
#define TM_VD_PIN 								GPIO_Pin_11
#define TM_VD_PORT 								GPIOE
#define TM_READ_PIN 							GPIO_Pin_12
#define TM_READ_PORT 							GPIOE
#define TM_HIGH_PIN 							GPIO_Pin_13
#define TM_HIGH_PORT 							GPIOE
#define TM_LOW_PIN 								GPIO_Pin_14
#define TM_LOW_PORT 							GPIOE

#define NOT_USE_PIN_PORT_E				GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_15





#endif // __GPIO_DEFINES_H


