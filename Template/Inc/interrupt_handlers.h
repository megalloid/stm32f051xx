/**
 * @author  Андрей Заостровных
 * @email   megalloid@mail.ru
 * @website http://smarthouseautomatics.ru/stm32/stm32f0xx/
 * @version v0.1
 * @ide     STM32CubeIDE
 * @brief   Этот файл содержит заголовки функций для обработчиков прерываний
 */

#ifndef 	INTERRUPTS_HANDLER_H
	#define INTERRUPTS_HANDLER_H

	#ifdef __cplusplus
		extern "C" {
	#endif


	#include "main.h"
	#include "my_stm32f0xx.h"
	#include "my_stm32f0xx_rcc.h"
	#include "my_stm32f0xx_cortex.h"

	/******************************************************************************/
	/*            Cortex-M0 Processor Exceptions Handlers                         */
	/******************************************************************************/

	/**
	 * @brief  This function handles NMI exception.
	 * @param  Нет
	 * @retval Нет
	 */
	void NMI_Handler(void);


	/**
	 * @brief  This function handles Hard Fault exception.
	 * @param  Нет
	 * @retval Нет
	 */
	void HardFault_Handler(void);


	/**
	 * @brief  This function handles SVCall exception.
	 * @param  Нет
	 * @retval Нет
	 */
	void SVC_Handler(void);


	/**
	 * @brief  This function handles PendSVC exception.
	 * @param  Нет
	 * @retval Нет
	 */
	void PendSV_Handler(void);


	/**
	 * @brief  This function handles SysTick Handler.
	 * @param  Нет
	 * @retval Нет
	 */
	void SysTick_Handler(void);


	/**
	 * @brief  This function handles SysTick Handler.
	 * @param  Нет
	 * @retval Нет
	 */
	void FLASH_IRQHandler(void);


	/******************************************************************************/
	/*                 STM32F0xx Peripherals Interrupt Handlers                   */
	/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
	/*  available peripheral interrupt handler's name please refer to the startup */
	/*  file (startup_stm32f0xx.s).                                               */
	/******************************************************************************/

	/**
	 * @brief  This function handles ...
	 * @param  Нет
	 * @retval Нет
	 */



	#ifdef __cplusplus
		}
	#endif

#endif
