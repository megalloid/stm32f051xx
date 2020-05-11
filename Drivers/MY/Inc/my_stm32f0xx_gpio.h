/**
 * @author  Andrey Zaostrovnykh
 * @email   megalloid@mail.ru
 * @website http://smarthouseautomatics.ru/stm32/stm32f0xx/gpio
 * @version v0.1
 * @ide     STM32CubeIDE
 * @brief   Библиотека для работы с GPIO (без EXTI)
 */

#ifndef MY_STM32F0xx_GPIO_H
	#define MY_STM32F0xx_GPIO_H 100

	/* C++ detection */
	#ifdef __cplusplus
		extern "C"
		{
	#endif

	/**
	 * @addtogroup MY_STM32F0xx_Libraries
	 * @{
	 */
		/**
		 * @defgroup MY_HAL_GPIO
		 * @brief    Библиотека GPIO для STM32F0xx
		 *
		 * GPIO библиотека обеспечивает легкую инициализацию и управление GPIO
		 * Может использоваться в качестве замены STD/HAL драйверов GPIO

		 ==============================================================================
								##### Особенности GPIO #####
		 ==============================================================================

		 (+) Каждый выход порта ввода\вывода general-purpose I/O (GPIO) может быть отдельно сконфигурирован
		 	 с помощью программного обеспечения. Поддерживаются следующие режимы:
			(++) Input mode
			(++) Analog mode
			(++) Output mode
			(++) Alternate function mode
			(++) External interrupt/event lines (используется в отдельной библиотеке EXTi)

		 (+) После сброса микроконтроллера альтернативные функции и линии внешних прерываний не доступны и
		 	 порты сконфигурированы в режиме Input floating.

		 (+) Все пины GPIO имеют встроенные pull-up и pull-down резисторы, которые могут быть включены.

		 (+) В режиме Output или Alternate, каждый вход\выход может быть сконфигурирован в режим Open-drain или push-pull
		     Скорость работы входа\выхода может быть выбрана в зависимости от напряжения VDD.

		 (+) Пины внутри микроконтроллера подключаются к периферии/модулям через мультиплексор, который
		     позволяет подключать только одну альтернативную функцию для работы с переферией (AF) в момент времени.
		     Это позволяет избежать конфликта при использовании периферии.

		 ==============================================================================
						##### Как использовать эту бибилиотеку #####
		 ==============================================================================

		 (#) Включаем таткирование на GPIO AHB используя макрос из библиотеки RCC: MY_RCC_GPIOx_CLK_ENABLE().

		 (#) Настроить пины GPIO используя MY_GPIO_Init() или MY_GPIO_StructInit()

			(++) Настроить IO mode используя элемент "Mode" из структуры MY_GPIO_Init_t

			(++) Включить Pull-up, Pull-down резистор используя элемент "Pull" из структуры GPIO_Init_t.

			(++) В случае если активируется режим Output или alternate function: скорость работы порта
				 настраивается через элемент "Speed" структуры MY_GPIO_Init_t.

			(++) В режиме alternate mode конкретая периферия подключается через указание значения
				 элемента "Alternate" структуры MY_GPIO_Init_t.

			(++) Аналоговый режим необходим для использования ADC или DAC.

		 (#) MY_GPIO_DeInit позволяет сбросить регистры к значению по умолчанию.
		  	 Это рекомендуется делать в случае если пин переключается в режим external interrupt или в event mode.

		 (#) Для получения текущего уровня - нужно использовать функцию MY_GPIO_ReadPin().

		 (#) Для изменения состояния GPIO пина в режиме output используйте MY_GPIO_WritePin()/MY_GPIO_TogglePin().

		 (#) Для блокировки конфигурации до следующего сброса используйте MY_GPIO_LockPin().

		 (#) Сразу после сброса альтернативные функции пинов не активны и пины GPIO сконфигурированы в режиме input floating (за исключением JTAG пинов).

		 (#) Пины осциллятора LSE OSC32_IN и OSC32_OUT могут использоваться как пины общего назначения (PC14 и PC15).
			 Режим LSE имеет приоритете над функциями GPIO.

		 (#) Пины осциллятора HSE OSC_IN/OSC_OUT могут использоваться как пины общего назначения (PF0 и PF1).
			 Режим HSE имеет приоритете над функциями GPIO.

		*
		* @{
		*/
			/* Подключаем основные файлы библиотек и настройки проекта*/
			#include "main.h"
			#include "my_stm32f0xx.h"
			#include "my_stm32f0xx_rcc.h"
			#include "my_stm32f0xx_utils.h"


			/**
			 * @defgroup MY_GPIO_Settings
			 * @brief    Библиотечные настройки
			 * 			 Данные настройки используются по умолчанию если они не переобъявлены в настройках проекта или в файле main.h
			 * @{
			 */

			/**
			 * @} MY_GPIO_Settings
			 */


			/**
			 * @defgroup MY_GPIO_Defines
			 * @brief    Библиотечные констатны
			 * @{
			 */

				/**
				 * @brief Базовые константы библиотеки
				 */
				#define GPIO_MODE             (0x00000003U)
				#define GPIO_OUTPUT_TYPE      (0x00000010U)

				#define GPIO_NUMBER           (16U)

				/**
				 * @brief Объявление GPIO пинов
				 * @note  Для совместимости с библиотекой HAL
				 */
				#define GPIO_Pin_0						((uint16_t)0x0001)
				#define GPIO_Pin_1						((uint16_t)0x0002)
				#define GPIO_Pin_2						((uint16_t)0x0004)
				#define GPIO_Pin_3						((uint16_t)0x0008)
				#define GPIO_Pin_4						((uint16_t)0x0010)
				#define GPIO_Pin_5						((uint16_t)0x0020)
				#define GPIO_Pin_6						((uint16_t)0x0040)
				#define GPIO_Pin_7						((uint16_t)0x0080)
				#define GPIO_Pin_8						((uint16_t)0x0100)
				#define GPIO_Pin_9						((uint16_t)0x0200)
				#define GPIO_Pin_10						((uint16_t)0x0400)
				#define GPIO_Pin_11						((uint16_t)0x0800)
				#define GPIO_Pin_12						((uint16_t)0x1000)
				#define GPIO_Pin_13						((uint16_t)0x2000)
				#define GPIO_Pin_14						((uint16_t)0x4000)
				#define GPIO_Pin_15						((uint16_t)0x8000)
				#define GPIO_Pin_ALL					((uint16_t)0xFFFF)

				/**
				 * @brief Объявление GPIO пинов
				 * @note  Для совместимости с библиотекой HAL
				 */
				#define GPIO_PIN_0						((uint16_t)0x0001)
				#define GPIO_PIN_1						((uint16_t)0x0002)
				#define GPIO_PIN_2						((uint16_t)0x0004)
				#define GPIO_PIN_3						((uint16_t)0x0008)
				#define GPIO_PIN_4						((uint16_t)0x0010)
				#define GPIO_PIN_5						((uint16_t)0x0020)
				#define GPIO_PIN_6						((uint16_t)0x0040)
				#define GPIO_PIN_7						((uint16_t)0x0080)
				#define GPIO_PIN_8						((uint16_t)0x0100)
				#define GPIO_PIN_9						((uint16_t)0x0200)
				#define GPIO_PIN_10						((uint16_t)0x0400)
				#define GPIO_PIN_11						((uint16_t)0x0800)
				#define GPIO_PIN_12						((uint16_t)0x1000)
				#define GPIO_PIN_13						((uint16_t)0x2000)
				#define GPIO_PIN_14						((uint16_t)0x4000)
				#define GPIO_PIN_15						((uint16_t)0x8000)
				#define GPIO_PIN_ALL					((uint16_t)0xFFFF)

				#define GPIO_PIN_MASK      				(0x0000FFFFU) 			/* PIN mask for assert test */


				/**
				 * @defgroup GPIO_mode GPIO mode
				 * @brief GPIO Configuration Mode
				 *        Elements values convention: 0xX0yz00YZ
				 *           - X  : GPIO mode or EXTI Mode
				 *           - y  : External IT or Event trigger detection
				 *           - z  : IO configuration on External IT or Event
				 *           - Y  : Output type (Push Pull or Open Drain)
				 *           - Z  : IO Direction mode (Input, Output, Alternate or Analog)
				 */
				#define  GPIO_MODE_INPUT                        (0x00000000U)   /*!< Input Floating Mode                   */
				#define  GPIO_MODE_OUTPUT_PP                    (0x00000001U)   /*!< Output Push Pull Mode                 */
				#define  GPIO_MODE_OUTPUT_OD                    (0x00000011U)   /*!< Output Open Drain Mode                */
				#define  GPIO_MODE_AF_PP                        (0x00000002U)   /*!< Alternate Function Push Pull Mode     */
				#define  GPIO_MODE_AF_OD                        (0x00000012U)   /*!< Alternate Function Open Drain Mode    */
				#define  GPIO_MODE_ANALOG                       (0x00000003U)   /*!< Analog Mode  */


				/**
				 * @defgroup GPIO_speed GPIO speed
				 * @brief GPIO Output Maximum frequency
				 */
				#define  GPIO_SPEED_FREQ_LOW      (0x00000000U)  /*!< range up to 2 MHz, please refer to the product datasheet */
				#define  GPIO_SPEED_FREQ_MEDIUM   (0x00000001U)  /*!< range  4 MHz to 10 MHz, please refer to the product datasheet */
				#define  GPIO_SPEED_FREQ_HIGH     (0x00000003U)  /*!< range 10 MHz to 50 MHz, please refer to the product datasheet */


				/**
				 * @defgroup GPIO_pull GPIO pull
				 * @brief GPIO Pull-Up or Pull-Down Activation
				 */
				#define  GPIO_NOPULL        (0x00000000U)   /*!< No Pull-up or Pull-down activation  */
				#define  GPIO_PULLUP        (0x00000001U)   /*!< Pull-up activation                  */
				#define  GPIO_PULLDOWN      (0x00000002U)   /*!< Pull-down activation                */

				/**
				 * @defgroup GPIOEx Alternate function selection
				 *
				 */
				#if defined (STM32F030x6)

					/*------------------------- STM32F030x6---------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1 Alternate Function mapping      */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_TIM3         ((uint8_t)0x00U)  /*!< AF0: TIM3 Alternate Function mapping      */

					/* AF 1 */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */

					/* AF 2 */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */

					/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */

					/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */
					#define GPIO_AF4_I2C1         ((uint8_t)0x04U)  /*!< AF4: I2C1 Alternate Function mapping      */

					/* AF 5 */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */

					/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

				#endif /* STM32F030x6 */


				#if defined (STM32F030x8)

					/*---------------------------------- STM32F030x8 -------------------------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1 Alternate Function mapping      */
					#define GPIO_AF0_SPI2         ((uint8_t)0x00U)  /*!< AF0: SPI2 Alternate Function mapping      */
					#define GPIO_AF0_TIM15        ((uint8_t)0x00U)  /*!< AF0: TIM15 Alternate Function mapping     */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_TIM3         ((uint8_t)0x00U)  /*!< AF0: TIM3 Alternate Function mapping      */

					/* AF 1 */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */
					#define GPIO_AF1_TIM15        ((uint8_t)0x01U)  /*!< AF1: TIM15 Alternate Function mapping     */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_USART2       ((uint8_t)0x01U)  /*!< AF1: USART2 Alternate Function mapping    */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_I2C2         ((uint8_t)0x01U)  /*!< AF1: I2C2 Alternate Function mapping      */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */

					/* AF 2 */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */

					/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */
					#define GPIO_AF3_TIM15        ((uint8_t)0x03U)  /*!< AF3: TIM15 Alternate Function mapping     */

					/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */

					/* AF 5 */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */

					/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

				#endif /* STM32F030x8 */


				#if defined (STM32F031x6) || \
					defined (STM32F038xx)

					/*--------------------------- STM32F031x6/STM32F038xx ---------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1/I2S1 Alternate Function mapping */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_SWDAT        ((uint8_t)0x00U)  /*!< AF0: SWDAT Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */

					/* AF 1 */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */

					/* AF 2 */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM2         ((uint8_t)0x02U)  /*!< AF2: TIM2 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */

					/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */

					/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */
					#define GPIO_AF4_I2C1         ((uint8_t)0x04U)  /*!< AF4: I2C1 Alternate Function mapping      */

					/* AF 5 */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */

					/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

				#endif /* STM32F031x6 || STM32F038xx */


				#if defined (STM32F051x8) || \
					defined (STM32F058xx)

					/*--------------------------- STM32F051x8/STM32F058xx---------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1/I2S1 Alternate Function mapping */
					#define GPIO_AF0_SPI2         ((uint8_t)0x00U)  /*!< AF0: SPI2 Alternate Function mapping      */
					#define GPIO_AF0_TIM15        ((uint8_t)0x00U)  /*!< AF0: TIM15 Alternate Function mapping     */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_CEC          ((uint8_t)0x00U)  /*!< AF0: CEC Alternate Function mapping       */

					/* AF 1 */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */
					#define GPIO_AF1_TIM15        ((uint8_t)0x01U)  /*!< AF1: TIM15 Alternate Function mapping     */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_USART2       ((uint8_t)0x01U)  /*!< AF1: USART2 Alternate Function mapping    */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_I2C2         ((uint8_t)0x01U)  /*!< AF1: I2C2 Alternate Function mapping      */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */
					#define GPIO_AF1_CEC          ((uint8_t)0x01U)  /*!< AF1: CEC Alternate Function mapping       */

					/* AF 2 */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM2         ((uint8_t)0x02U)  /*!< AF2: TIM2 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */

					/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */
					#define GPIO_AF3_TIM15        ((uint8_t)0x03U)  /*!< AF3: TIM15 Alternate Function mapping     */
					#define GPIO_AF3_TSC          ((uint8_t)0x03U)  /*!< AF3: TSC Alternate Function mapping       */

					/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */

					/* AF 5 */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */

					/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

					/* AF 7 */
					#define GPIO_AF7_COMP1        ((uint8_t)0x07U)  /*!< AF7: COMP1 Alternate Function mapping     */
					#define GPIO_AF7_COMP2        ((uint8_t)0x07U)  /*!< AF7: COMP2 Alternate Function mapping     */

				#endif /* STM32F051x8/STM32F058xx */


				#if defined (STM32F071xB)

					/*--------------------------- STM32F071xB ---------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: AEVENTOUT Alternate Function mapping */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_CEC          ((uint8_t)0x00U)  /*!< AF0: CEC Alternate Function mapping       */
					#define GPIO_AF0_CRS          ((uint8_t)0x00U)  /*!< AF0: CRS Alternate Function mapping       */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1/I2S1 Alternate Function mapping */
					#define GPIO_AF0_SPI2         ((uint8_t)0x00U)  /*!< AF0: SPI2/I2S2 Alternate Function mapping */
					#define GPIO_AF0_TIM1         ((uint8_t)0x00U)  /*!< AF0: TIM1 Alternate Function mapping      */
					#define GPIO_AF0_TIM3         ((uint8_t)0x00U)  /*!< AF0: TIM3 Alternate Function mapping      */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_TIM15        ((uint8_t)0x00U)  /*!< AF0: TIM15 Alternate Function mapping     */
					#define GPIO_AF0_TIM16        ((uint8_t)0x00U)  /*!< AF0: TIM16 Alternate Function mapping     */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_TSC          ((uint8_t)0x00U)  /*!< AF0: TSC Alternate Function mapping       */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */
					#define GPIO_AF0_USART2       ((uint8_t)0x00U)  /*!< AF0: USART2 Alternate Function mapping    */
					#define GPIO_AF0_USART3       ((uint8_t)0x00U)  /*!< AF0: USART3 Alternate Function mapping    */
					#define GPIO_AF0_USART4       ((uint8_t)0x00U)  /*!< AF0: USART4 Alternate Function mapping    */

					/* AF 1 */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */
					#define GPIO_AF1_TIM15        ((uint8_t)0x01U)  /*!< AF1: TIM15 Alternate Function mapping     */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_USART2       ((uint8_t)0x01U)  /*!< AF1: USART2 Alternate Function mapping    */
					#define GPIO_AF1_USART3       ((uint8_t)0x01U)  /*!< AF1: USART3 Alternate Function mapping    */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */
					#define GPIO_AF1_CEC          ((uint8_t)0x01U)  /*!< AF1: CEC Alternate Function mapping       */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_I2C2         ((uint8_t)0x01U)  /*!< AF1: I2C2 Alternate Function mapping      */
					#define GPIO_AF1_TSC          ((uint8_t)0x01U)  /*!< AF1: TSC Alternate Function mapping       */
					#define GPIO_AF1_SPI1         ((uint8_t)0x01U)  /*!< AF1: SPI1 Alternate Function mapping      */
					#define GPIO_AF1_SPI2         ((uint8_t)0x01U)  /*!< AF1: SPI2 Alternate Function mapping      */

					/* AF 2 */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM2         ((uint8_t)0x02U)  /*!< AF2: TIM2 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */

					/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_TSC          ((uint8_t)0x03U)  /*!< AF3: TSC Alternate Function mapping       */
					#define GPIO_AF3_TIM15        ((uint8_t)0x03U)  /*!< AF3: TIM15 Alternate Function mapping     */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */

					/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */
					#define GPIO_AF4_USART4       ((uint8_t)0x04U)  /*!< AF4: USART4 Alternate Function mapping    */
					#define GPIO_AF4_USART3       ((uint8_t)0x04U)  /*!< AF4: USART3 Alternate Function mapping    */
					#define GPIO_AF4_CRS          ((uint8_t)0x04U)  /*!< AF4: CRS Alternate Function mapping       */

					/* AF 5 */
					#define GPIO_AF5_TIM15        ((uint8_t)0x05U)  /*!< AF5: TIM15 Alternate Function mapping     */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */
					#define GPIO_AF5_SPI2         ((uint8_t)0x05U)  /*!< AF5: SPI2 Alternate Function mapping      */
					#define GPIO_AF5_I2C2         ((uint8_t)0x05U)  /*!< AF5: I2C2 Alternate Function mapping      */

					/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

					/* AF 7 */
					#define GPIO_AF7_COMP1        ((uint8_t)0x07U)  /*!< AF7: COMP1 Alternate Function mapping     */
					#define GPIO_AF7_COMP2        ((uint8_t)0x07U)  /*!< AF7: COMP2 Alternate Function mapping     */

				#endif /* STM32F071xB */


				#if defined(STM32F091xC) || \
					defined(STM32F098xx)

					/*--------------------------- STM32F091xC || STM32F098xx ------------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_CEC          ((uint8_t)0x00U)  /*!< AF0: CEC Alternate Function mapping       */
					#define GPIO_AF0_CRS          ((uint8_t)0x00U)  /*!< AF0: CRS Alternate Function mapping       */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1/I2S1 Alternate Function mapping */
					#define GPIO_AF0_SPI2         ((uint8_t)0x00U)  /*!< AF0: SPI2/I2S2 Alternate Function mapping */
					#define GPIO_AF0_TIM1         ((uint8_t)0x00U)  /*!< AF0: TIM1 Alternate Function mapping      */
					#define GPIO_AF0_TIM3         ((uint8_t)0x00U)  /*!< AF0: TIM3 Alternate Function mapping      */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_TIM15        ((uint8_t)0x00U)  /*!< AF0: TIM15 Alternate Function mapping     */
					#define GPIO_AF0_TIM16        ((uint8_t)0x00U)  /*!< AF0: TIM16 Alternate Function mapping     */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_TSC          ((uint8_t)0x00U)  /*!< AF0: TSC Alternate Function mapping       */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */
					#define GPIO_AF0_USART2       ((uint8_t)0x00U)  /*!< AF0: USART2 Alternate Function mapping    */
					#define GPIO_AF0_USART3       ((uint8_t)0x00U)  /*!< AF0: USART3 Alternate Function mapping    */
					#define GPIO_AF0_USART4       ((uint8_t)0x00U)  /*!< AF0: USART4 Alternate Function mapping    */
					#define GPIO_AF0_USART8       ((uint8_t)0x00U)  /*!< AF0: USART8 Alternate Function mapping    */
					#define GPIO_AF0_CAN          ((uint8_t)0x00U)  /*!< AF0: CAN Alternate Function mapping       */

					/* AF 1 */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */
					#define GPIO_AF1_TIM15        ((uint8_t)0x01U)  /*!< AF1: TIM15 Alternate Function mapping     */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_USART2       ((uint8_t)0x01U)  /*!< AF1: USART2 Alternate Function mapping    */
					#define GPIO_AF1_USART3       ((uint8_t)0x01U)  /*!< AF1: USART3 Alternate Function mapping    */
					#define GPIO_AF1_USART4       ((uint8_t)0x01U)  /*!< AF1: USART4 Alternate Function mapping    */
					#define GPIO_AF1_USART5       ((uint8_t)0x01U)  /*!< AF1: USART5 Alternate Function mapping    */
					#define GPIO_AF1_USART6       ((uint8_t)0x01U)  /*!< AF1: USART6 Alternate Function mapping    */
					#define GPIO_AF1_USART7       ((uint8_t)0x01U)  /*!< AF1: USART7 Alternate Function mapping    */
					#define GPIO_AF1_USART8       ((uint8_t)0x01U)  /*!< AF1: USART8 Alternate Function mapping    */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */
					#define GPIO_AF1_CEC          ((uint8_t)0x01U)  /*!< AF1: CEC Alternate Function mapping       */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_I2C2         ((uint8_t)0x01U)  /*!< AF1: I2C2 Alternate Function mapping      */
					#define GPIO_AF1_TSC          ((uint8_t)0x01U)  /*!< AF1: TSC Alternate Function mapping       */
					#define GPIO_AF1_SPI1         ((uint8_t)0x01U)  /*!< AF1: SPI1 Alternate Function mapping      */
					#define GPIO_AF1_SPI2         ((uint8_t)0x01U)  /*!< AF1: SPI2 Alternate Function mapping      */

					/* AF 2 */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM2         ((uint8_t)0x02U)  /*!< AF2: TIM2 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF2_USART5       ((uint8_t)0x02U)  /*!< AF2: USART5 Alternate Function mapping    */
					#define GPIO_AF2_USART6       ((uint8_t)0x02U)  /*!< AF2: USART6 Alternate Function mapping    */
					#define GPIO_AF2_USART7       ((uint8_t)0x02U)  /*!< AF2: USART7 Alternate Function mapping    */
					#define GPIO_AF2_USART8       ((uint8_t)0x02U)  /*!< AF2: USART8 Alternate Function mapping    */

					/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_TSC          ((uint8_t)0x03U)  /*!< AF3: TSC Alternate Function mapping       */
					#define GPIO_AF3_TIM15        ((uint8_t)0x03U)  /*!< AF3: TIM15 Alternate Function mapping     */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */

					/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */
					#define GPIO_AF4_USART4       ((uint8_t)0x04U)  /*!< AF4: USART4 Alternate Function mapping    */
					#define GPIO_AF4_USART3       ((uint8_t)0x04U)  /*!< AF4: USART3 Alternate Function mapping    */
					#define GPIO_AF4_CRS          ((uint8_t)0x04U)  /*!< AF4: CRS Alternate Function mapping       */
					#define GPIO_AF4_CAN          ((uint8_t)0x04U)  /*!< AF4: CAN Alternate Function mapping       */
					#define GPIO_AF4_I2C1         ((uint8_t)0x04U)  /*!< AF4: I2C1 Alternate Function mapping      */
					#define GPIO_AF4_USART5       ((uint8_t)0x04U)  /*!< AF4: USART5 Alternate Function mapping    */

					/* AF 5 */
					#define GPIO_AF5_TIM15        ((uint8_t)0x05U)  /*!< AF5: TIM15 Alternate Function mapping     */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */
					#define GPIO_AF5_SPI2         ((uint8_t)0x05U)  /*!< AF5: SPI2 Alternate Function mapping      */
					#define GPIO_AF5_I2C2         ((uint8_t)0x05U)  /*!< AF5: I2C2 Alternate Function mapping      */
					#define GPIO_AF5_MCO          ((uint8_t)0x05U)  /*!< AF5: MCO Alternate Function mapping       */
					#define GPIO_AF5_USART6       ((uint8_t)0x05U)  /*!< AF5: USART6 Alternate Function mapping    */

					/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

					/* AF 7 */
					#define GPIO_AF7_COMP1        ((uint8_t)0x07U)  /*!< AF7: COMP1 Alternate Function mapping     */
					#define GPIO_AF7_COMP2        ((uint8_t)0x07U)  /*!< AF7: COMP2 Alternate Function mapping     */

				#endif /* STM32F091xC  || STM32F098xx */


				#if defined(STM32F030xC)

					/*--------------------------- STM32F030xC ----------------------------------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1 Alternate Function mapping      */
					#define GPIO_AF0_SPI2         ((uint8_t)0x00U)  /*!< AF0: SPI2 Alternate Function mapping      */
					#define GPIO_AF0_TIM3         ((uint8_t)0x00U)  /*!< AF0: TIM3 Alternate Function mapping      */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_TIM15        ((uint8_t)0x00U)  /*!< AF0: TIM15 Alternate Function mapping     */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */
					#define GPIO_AF0_USART4       ((uint8_t)0x00U)  /*!< AF0: USART4 Alternate Function mapping    */

					/* AF 1 */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */
					#define GPIO_AF1_TIM15        ((uint8_t)0x01U)  /*!< AF1: TIM15 Alternate Function mapping     */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_USART2       ((uint8_t)0x01U)  /*!< AF1: USART2 Alternate Function mapping    */
					#define GPIO_AF1_USART3       ((uint8_t)0x01U)  /*!< AF1: USART3 Alternate Function mapping    */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_I2C2         ((uint8_t)0x01U)  /*!< AF1: I2C2 Alternate Function mapping      */
					#define GPIO_AF1_SPI2         ((uint8_t)0x01U)  /*!< AF1: SPI2 Alternate Function mapping      */

					/* AF 2 */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF2_USART5       ((uint8_t)0x02U)  /*!< AF2: USART5 Alternate Function mapping    */
					#define GPIO_AF2_USART6       ((uint8_t)0x02U)  /*!< AF2: USART6 Alternate Function mapping    */

					/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_TIM15        ((uint8_t)0x03U)  /*!< AF3: TIM15 Alternate Function mapping     */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */

					/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */
					#define GPIO_AF4_USART4       ((uint8_t)0x04U)  /*!< AF4: USART4 Alternate Function mapping    */
					#define GPIO_AF4_USART3       ((uint8_t)0x04U)  /*!< AF4: USART3 Alternate Function mapping    */
					#define GPIO_AF4_I2C1         ((uint8_t)0x04U)  /*!< AF4: I2C1 Alternate Function mapping      */
					#define GPIO_AF4_USART5       ((uint8_t)0x04U)  /*!< AF4: USART5 Alternate Function mapping    */

					/* AF 5 */
					#define GPIO_AF5_TIM15        ((uint8_t)0x05U)  /*!< AF5: TIM15 Alternate Function mapping     */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */
					#define GPIO_AF5_SPI2         ((uint8_t)0x05U)  /*!< AF5: SPI2 Alternate Function mapping      */
					#define GPIO_AF5_I2C2         ((uint8_t)0x05U)  /*!< AF5: I2C2 Alternate Function mapping      */
					#define GPIO_AF5_MCO          ((uint8_t)0x05U)  /*!< AF5: MCO Alternate Function mapping       */
					#define GPIO_AF5_USART6       ((uint8_t)0x05U)  /*!< AF5: USART6 Alternate Function mapping    */

					/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

				#endif /* STM32F030xC */


				#if defined (STM32F072xB) || \
					defined (STM32F078xx)

					/*--------------------------- STM32F072xB/STM32F078xx ---------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_CEC          ((uint8_t)0x00U)  /*!< AF0: CEC Alternate Function mapping       */
					#define GPIO_AF0_CRS          ((uint8_t)0x00U)  /*!< AF0: CRS Alternate Function mapping       */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1/I2S1 Alternate Function mapping */
					#define GPIO_AF0_SPI2         ((uint8_t)0x00U)  /*!< AF0: SPI2/I2S2 Alternate Function mapping */
					#define GPIO_AF0_TIM1         ((uint8_t)0x00U)  /*!< AF0: TIM1 Alternate Function mapping      */
					#define GPIO_AF0_TIM3         ((uint8_t)0x00U)  /*!< AF0: TIM3 Alternate Function mapping      */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_TIM15        ((uint8_t)0x00U)  /*!< AF0: TIM15 Alternate Function mapping     */
					#define GPIO_AF0_TIM16        ((uint8_t)0x00U)  /*!< AF0: TIM16 Alternate Function mapping     */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_TSC          ((uint8_t)0x00U)  /*!< AF0: TSC Alternate Function mapping       */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */
					#define GPIO_AF0_USART2       ((uint8_t)0x00U)  /*!< AF0: USART2 Alternate Function mapping    */
																			#define GPIO_AF0_USART3       ((uint8_t)0x00U)  /*!< AF0: USART2 Alternate Function mapping    */
					#define GPIO_AF0_USART4       ((uint8_t)0x00U)  /*!< AF0: USART4 Alternate Function mapping    */
					#define GPIO_AF0_CAN          ((uint8_t)0x00U)  /*!< AF0: CAN Alternate Function mapping       */

							/* AF 1 */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */
					#define GPIO_AF1_TIM15        ((uint8_t)0x01U)  /*!< AF1: TIM15 Alternate Function mapping     */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
																			#define GPIO_AF1_USART2       ((uint8_t)0x01U)  /*!< AF1: USART2 Alternate Function mapping    */
					#define GPIO_AF1_USART3       ((uint8_t)0x01U)  /*!< AF1: USART3 Alternate Function mapping    */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */
					#define GPIO_AF1_CEC          ((uint8_t)0x01U)  /*!< AF1: CEC Alternate Function mapping       */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_I2C2         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_TSC          ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_SPI1         ((uint8_t)0x01U)  /*!< AF1: SPI1 Alternate Function mapping      */
					#define GPIO_AF1_SPI2         ((uint8_t)0x01U)  /*!< AF1: SPI2 Alternate Function mapping      */

							/* AF 2 */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM2         ((uint8_t)0x02U)  /*!< AF2: TIM2 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF2_USB          ((uint8_t)0x02U)  /*!< AF2: USB Alternate Function mapping       */

							/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_TSC          ((uint8_t)0x03U)  /*!< AF3: TSC Alternate Function mapping       */
					#define GPIO_AF3_TIM15        ((uint8_t)0x03U)  /*!< AF3: TIM15 Alternate Function mapping     */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */

							/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */
					#define GPIO_AF4_USART4       ((uint8_t)0x04U)  /*!< AF4: USART4 Alternate Function mapping    */
					#define GPIO_AF4_USART3       ((uint8_t)0x04U)  /*!< AF4: USART3 Alternate Function mapping    */
					#define GPIO_AF4_CRS          ((uint8_t)0x04U)  /*!< AF4: CRS Alternate Function mapping       */
					#define GPIO_AF4_CAN          ((uint8_t)0x04U)  /*!< AF4: CAN Alternate Function mapping       */

							/* AF 5 */
					#define GPIO_AF5_TIM15        ((uint8_t)0x05U)  /*!< AF5: TIM15 Alternate Function mapping     */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */
					#define GPIO_AF5_SPI2         ((uint8_t)0x05U)  /*!< AF5: SPI2 Alternate Function mapping      */
					#define GPIO_AF5_I2C2         ((uint8_t)0x05U)  /*!< AF5: I2C2 Alternate Function mapping      */

							/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

							/* AF 7 */
					#define GPIO_AF7_COMP1        ((uint8_t)0x07U)  /*!< AF7: COMP1 Alternate Function mapping     */
					#define GPIO_AF7_COMP2        ((uint8_t)0x07U)  /*!< AF7: COMP2 Alternate Function mapping     */

				#endif /* STM32F072xB || STM32F078xx */


				#if defined (STM32F070xB)

							/*---------------------------------- STM32F070xB ---------------------------------------------*/
							/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1 Alternate Function mapping      */
					#define GPIO_AF0_SPI2         ((uint8_t)0x00U)  /*!< AF0: SPI2 Alternate Function mapping      */
					#define GPIO_AF0_TIM3         ((uint8_t)0x00U)  /*!< AF0: TIM3 Alternate Function mapping      */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_TIM15        ((uint8_t)0x00U)  /*!< AF0: TIM15 Alternate Function mapping     */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */
					#define GPIO_AF0_USART4       ((uint8_t)0x00U)  /*!< AF0: USART4 Alternate Function mapping    */

							/* AF 1 */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */
					#define GPIO_AF1_TIM15        ((uint8_t)0x01U)  /*!< AF1: TIM15 Alternate Function mapping     */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_USART2       ((uint8_t)0x01U)  /*!< AF1: USART2 Alternate Function mapping    */
					#define GPIO_AF1_USART3       ((uint8_t)0x01U)  /*!< AF1: USART4 Alternate Function mapping    */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_I2C2         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_SPI2         ((uint8_t)0x01U)  /*!< AF1: SPI2 Alternate Function mapping      */

							/* AF 2 */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF2_USB          ((uint8_t)0x02U)  /*!< AF2: USB Alternate Function mapping       */

							/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */
					#define GPIO_AF3_TIM15        ((uint8_t)0x03U)  /*!< AF3: TIM15 Alternate Function mapping     */

							/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */
					#define GPIO_AF4_USART4       ((uint8_t)0x04U)  /*!< AF4: USART4 Alternate Function mapping    */
					#define GPIO_AF4_USART3       ((uint8_t)0x04U)  /*!< AF4: USART3 Alternate Function mapping    */

							/* AF 5 */
					#define GPIO_AF5_TIM15        ((uint8_t)0x05U)  /*!< AF5: TIM15 Alternate Function mapping     */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */
					#define GPIO_AF5_SPI2         ((uint8_t)0x05U)  /*!< AF5: SPI2 Alternate Function mapping      */
					#define GPIO_AF5_I2C2         ((uint8_t)0x05U)  /*!< AF5: I2C2 Alternate Function mapping      */

							/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

				#endif /* STM32F070xB */


				#if defined (STM32F042x6) || \
					defined (STM32F048xx)

					/*--------------------------- STM32F042x6/STM32F048xx ---------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_CEC          ((uint8_t)0x00U)  /*!< AF0: CEC Alternate Function mapping       */
					#define GPIO_AF0_CRS          ((uint8_t)0x00U)  /*!< AF0: CRS Alternate Function mapping       */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1/I2S1 Alternate Function mapping */
					#define GPIO_AF0_SPI2         ((uint8_t)0x00U)  /*!< AF0: SPI2/I2S2 Alternate Function mapping */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */

					/* AF 1 */
					#define GPIO_AF1_CEC          ((uint8_t)0x01U)  /*!< AF1: CEC Alternate Function mapping       */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_USART2       ((uint8_t)0x01U)  /*!< AF1: USART2 Alternate Function mapping    */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */

					/* AF 2 */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM2         ((uint8_t)0x02U)  /*!< AF2: TIM2 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_USB          ((uint8_t)0x02U)  /*!< AF2: USB Alternate Function mapping       */

					/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */
					#define GPIO_AF3_TSC          ((uint8_t)0x03U)  /*!< AF3: TSC Alternate Function mapping       */

					/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */
					#define GPIO_AF4_CAN          ((uint8_t)0x04U)  /*!< AF4: CAN Alternate Function mapping       */
					#define GPIO_AF4_CRS          ((uint8_t)0x04U)  /*!< AF4: CRS Alternate Function mapping       */
					#define GPIO_AF4_I2C1         ((uint8_t)0x04U)  /*!< AF4: I2C1 Alternate Function mapping      */

					/* AF 5 */
					#define GPIO_AF5_MCO          ((uint8_t)0x05U)  /*!< AF5: MCO Alternate Function mapping       */
					#define GPIO_AF5_I2C1         ((uint8_t)0x05U)  /*!< AF5: I2C1 Alternate Function mapping      */
					#define GPIO_AF5_I2C2         ((uint8_t)0x05U)  /*!< AF5: I2C2 Alternate Function mapping      */
					#define GPIO_AF5_SPI2         ((uint8_t)0x05U)  /*!< AF5: SPI2 Alternate Function mapping      */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */
					#define GPIO_AF5_USB          ((uint8_t)0x05U)  /*!< AF5: USB Alternate Function mapping       */

					/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

				#endif /* STM32F042x6 || STM32F048xx */


				#if defined (STM32F070x6)

					/*--------------------------------------- STM32F070x6 ----------------------------------------*/
					/* AF 0 */
					#define GPIO_AF0_EVENTOUT     ((uint8_t)0x00U)  /*!< AF0: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF0_IR           ((uint8_t)0x00U)  /*!< AF0: IR Alternate Function mapping        */
					#define GPIO_AF0_MCO          ((uint8_t)0x00U)  /*!< AF0: MCO Alternate Function mapping       */
					#define GPIO_AF0_SPI1         ((uint8_t)0x00U)  /*!< AF0: SPI1 Alternate Function mapping      */
					#define GPIO_AF0_SWDIO        ((uint8_t)0x00U)  /*!< AF0: SWDIO Alternate Function mapping     */
					#define GPIO_AF0_SWCLK        ((uint8_t)0x00U)  /*!< AF0: SWCLK Alternate Function mapping     */
					#define GPIO_AF0_TIM14        ((uint8_t)0x00U)  /*!< AF0: TIM14 Alternate Function mapping     */
					#define GPIO_AF0_TIM17        ((uint8_t)0x00U)  /*!< AF0: TIM17 Alternate Function mapping     */
					#define GPIO_AF0_USART1       ((uint8_t)0x00U)  /*!< AF0: USART1 Alternate Function mapping    */

					/* AF 1 */
					#define GPIO_AF1_EVENTOUT     ((uint8_t)0x01U)  /*!< AF1: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF1_I2C1         ((uint8_t)0x01U)  /*!< AF1: I2C1 Alternate Function mapping      */
					#define GPIO_AF1_IR           ((uint8_t)0x01U)  /*!< AF1: IR Alternate Function mapping        */
					#define GPIO_AF1_USART1       ((uint8_t)0x01U)  /*!< AF1: USART1 Alternate Function mapping    */
					#define GPIO_AF1_USART2       ((uint8_t)0x01U)  /*!< AF1: USART2 Alternate Function mapping    */
					#define GPIO_AF1_TIM3         ((uint8_t)0x01U)  /*!< AF1: TIM3 Alternate Function mapping      */

					/* AF 2 */
					#define GPIO_AF2_EVENTOUT     ((uint8_t)0x02U)  /*!< AF2: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF2_TIM1         ((uint8_t)0x02U)  /*!< AF2: TIM1 Alternate Function mapping      */
					#define GPIO_AF2_TIM16        ((uint8_t)0x02U)  /*!< AF2: TIM16 Alternate Function mapping     */
					#define GPIO_AF2_TIM17        ((uint8_t)0x02U)  /*!< AF2: TIM17 Alternate Function mapping     */
					#define GPIO_AF2_USB          ((uint8_t)0x02U)  /*!< AF2: USB Alternate Function mapping       */

					/* AF 3 */
					#define GPIO_AF3_EVENTOUT     ((uint8_t)0x03U)  /*!< AF3: EVENTOUT Alternate Function mapping  */
					#define GPIO_AF3_I2C1         ((uint8_t)0x03U)  /*!< AF3: I2C1 Alternate Function mapping      */

					/* AF 4 */
					#define GPIO_AF4_TIM14        ((uint8_t)0x04U)  /*!< AF4: TIM14 Alternate Function mapping     */
					#define GPIO_AF4_I2C1         ((uint8_t)0x04U)  /*!< AF4: I2C1 Alternate Function mapping      */

					/* AF 5 */
					#define GPIO_AF5_MCO          ((uint8_t)0x05U)  /*!< AF5: MCO Alternate Function mapping       */
					#define GPIO_AF5_I2C1         ((uint8_t)0x05U)  /*!< AF5: I2C1 Alternate Function mapping      */
					#define GPIO_AF5_TIM16        ((uint8_t)0x05U)  /*!< AF5: TIM16 Alternate Function mapping     */
					#define GPIO_AF5_TIM17        ((uint8_t)0x05U)  /*!< AF5: TIM17 Alternate Function mapping     */
					#define GPIO_AF5_USB          ((uint8_t)0x05U)  /*!< AF5: USB Alternate Function mapping       */

					/* AF 6 */
					#define GPIO_AF6_EVENTOUT     ((uint8_t)0x06U)  /*!< AF6: EVENTOUT Alternate Function mapping  */

				#endif

			/**
			 * @} MY_GPIO_Defines
			 */


			/**
			 * @defgroup MY_GPIO_Macros
			 * @brief    Библиотечные макросы
			 * @{
			 */
				/**
				 * @defgroup GPIO Get Port Index
				 * @{
				 */

					#if defined(GPIOD) && defined(GPIOE)

						#define GPIO_GET_INDEX(__GPIOx)    (((__GPIOx) == (GPIOA))? 0U :\
															((__GPIOx) == (GPIOB))? 1U :\
															((__GPIOx) == (GPIOC))? 2U :\
															((__GPIOx) == (GPIOD))? 3U :\
															((__GPIOx) == (GPIOE))? 4U : 5U)
					#endif


					#if defined(GPIOD) && !defined(GPIOE)

						#define GPIO_GET_INDEX(__GPIOx)    (((__GPIOx) == (GPIOA))? 0U :\
															((__GPIOx) == (GPIOB))? 1U :\
															((__GPIOx) == (GPIOC))? 2U :\
															((__GPIOx) == (GPIOD))? 3U : 5U)
					#endif


					#if !defined(GPIOD) && defined(GPIOE)

						#define GPIO_GET_INDEX(__GPIOx)    (((__GPIOx) == (GPIOA))? 0U :\
															((__GPIOx) == (GPIOB))? 1U :\
															((__GPIOx) == (GPIOC))? 2U :\
															((__GPIOx) == (GPIOE))? 4U : 5U)
					#endif


					#if !defined(GPIOD) && !defined(GPIOE)

						#define GPIO_GET_INDEX(__GPIOx)    (((__GPIOx) == (GPIOA))? 0U :\
															((__GPIOx) == (GPIOB))? 1U :\
															((__GPIOx) == (GPIOC))? 2U : 5U)
					#endif

					/**
					 * @}
					 */

				/**
				 * @}  MY_GPIO_Macros
				 */


				/**
				 * @defgroup MY_GPIO_Typedefs
				 * @brief    Typedefs для библиотеки
				 * @{
				 */

					/**
					 * @brief  GPIO Bit SET and Bit RESET enumeration
					 */
					typedef enum
					{
						MY_GPIO_Pin_Reset = 0U,
						MY_GPIO_Pin_Set = 1U
					}
					MY_GPIO_PinState;


					/**
					 * @brief   GPIO Init structure definition
					 */
					typedef struct
					{
						uint32_t Pin;       /*!< Specifies the GPIO pins to be configured.
												 This parameter can be any value of @ref GPIO_pins */
						uint32_t Mode;      /*!< Specifies the operating mode for the selected pins.
												 This parameter can be a value of @ref GPIO_mode */
						uint32_t Pull;      /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
												 This parameter can be a value of @ref GPIO_pull */
						uint32_t Speed;     /*!< Specifies the speed for the selected pins.
												 This parameter can be a value of @ref GPIO_speed */
						uint32_t Alternate; /*!< Peripheral to be connected to the selected pins
												 This parameter can be a value of @ref GPIOEx_Alternate_function_selection */
					}
					MY_GPIO_Init_t;


					/**
					 * @brief GPIO Mode [GPIOx_MODER]
					 */
					typedef enum
					{
						MY_GPIO_Mode_In = 0x00,  				/*!< GPIO пин в режиме Input */
						MY_GPIO_Mode_Out = 0x01, 				/*!< GPIO пин в режиме Output */
						MY_GPIO_Mode_AF = 0x02,  				/*!< GPIO пин в режиме Alternate Function */
						MY_GPIO_Mode_Analog = 0x03,  				/*!< GPIO пин в режиме Analog input/output */
					}
					MY_GPIO_Mode_t;


					/**
					 * @brief GPIO Output type [GPIOx_OTYPER]
					 */
					typedef enum
					{
						MY_GPIO_OType_PP = 0x00, 				/*!< GPIO Output в режиме Push-Pull */
						MY_GPIO_OType_OD = 0x01  				/*!< GPIO Output в режиме Open-Drain */
					}
					MY_GPIO_OType_t;


					/**
					 * @brief  GPIO Speed [GPIOx_OSPEEDR]
					 */
					typedef enum
					{
						MY_GPIO_Speed_Low = 0x00,    		/*!< GPIO Speed Low */
						MY_GPIO_Speed_Medium = 0x01, 		/*!< GPIO Speed Medium */
						MY_GPIO_Speed_High = 0x03    		/*!< GPIO Speed High */
					}
					MY_GPIO_Speed_t;


					/**
					 * @brief GPIO pull resistors [GPIOx_PUPDR]
					 */
					typedef enum
					{
						MY_GPIO_PuPd_NoPull = 0x00, 		/*!< Без подтяжки */
						MY_GPIO_PuPd_Up = 0x01,     		/*!< Подтяжка в режиме pull-up */
						MY_GPIO_PuPd_Down = 0x02    		/*!< Подтяжка в режиме pull-down */
					}
					MY_GPIO_PuPd_t;

				/**
				 * @} MY_GPIO_Typedefs
				 */


				/**
				 * @defgroup MY_GPIO_Functions
				 * @brief    Функции используемые в библиотеке
				 * @{
				 */
					/**
					 * @brief  Инициализация GPIO пинов
					 * @note   Эта функция так же включает тактирование на целевом GPIO-порту
					 * @param  GPIOx: указатель на GPIOx порт который будет инициализироваться
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут инициализироваться. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @param  GPIO_Mode: Выбирает GPIO режим работы. Может быть выбран из @ref MY_GPIO_Mode_t
					 * @param  GPIO_OType: Выбирает GPIO режим работы Output. Может быть выбран из @ref MY_GPIO_OType_t
					 * @param  GPIO_PuPd: Выбирает GPIO режим подтяжки. Может быть выбран из @ref MY_GPIO_PuPd_t
					 * @param  GPIO_Speed: Выбирает GPIO скорость. Может быть выбран из @ref MY_GPIO_Speed_t
					 * @retval Нет
					 */
					void MY_GPIO_Init(GPIO_TypeDef* 	GPIOx, \
									  uint16_t 			GPIO_Pin, \
									  MY_GPIO_Mode_t 	GPIO_Mode, 		\
									  MY_GPIO_OType_t 	GPIO_OType, 	\
									  MY_GPIO_PuPd_t 	GPIO_PuPd, 		\
									  MY_GPIO_Speed_t 	GPIO_Speed);


					/**
					 * @brief  Initialize the GPIOx peripheral according to the specified parameters in the GPIO_Init.
					 * @param  GPIOx where x can be (A..F) to select the GPIO peripheral for STM32F0 family
					 * @param  GPIO_Init pointer to a GPIO_InitTypeDef structure that contains
					 *         the configuration information for the specified GPIO peripheral.
					 * @retval None
					 */
					void MY_GPIO_StructInit(GPIO_TypeDef* GPIOx, MY_GPIO_Init_t *GPIO_Init);


					/**
					 * @brief  Инициализация GPIO пинов в качестве альтернативной функции
					 * @note   Эта функция так же включает тактирование на целевом GPIO-порту
					 * @param  GPIOx: указатель на GPIOx порт который будет инициализироваться
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут инициализироваться. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @param  GPIO_OType: Выбирает GPIO режим работы Output. Может быть выбран из @ref MY_GPIO_OType_t
					 * @param  GPIO_PuPd: Выбирает GPIO режим подтяжки. Может быть выбран из @ref MY_GPIO_PuPd_t
					 * @param  GPIO_Speed: Выбирает GPIO скорость. Может быть выбран из @ref MY_GPIO_Speed_t
					 * @param  Alternate: альтернативная функция
					 * @retval Нет
					 */
					void MY_GPIO_InitAlternate(GPIO_TypeDef* 	GPIOx, \
											   uint16_t 	 	GPIO_Pin, 	\
											   MY_GPIO_OType_t 	GPIO_OType, \
											   MY_GPIO_PuPd_t 	GPIO_PuPd, 	\
											   MY_GPIO_Speed_t 	GPIO_Speed, \
											   uint8_t 			Alternate);


					/**
					 * @brief  Деинициализация GPIO-порта
					 * @note   Пины устанавливаются в аналоговый режим для энергосбережения
					 * @param  GPIOx: указатель на GPIOx порт который будет инициализироваться
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут деинициализированы. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @retval Нет
					 */
					void MY_GPIO_DeInit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);


					/**
					 * @brief  Включение тактирования на GPIO порт
					 * @note   Эта функция включает тактирование на целевом GPIO-порту.
					 * @param  GPIOx: указатель на GPIOx порт
					 */
					void MY_GPIO_EnableClock(GPIO_TypeDef* GPIOx);


					/**
					 * @brief  Выключение тактирования на GPIO порт
					 * @note   Эта функция выключает тактирование на целевом GPIO-порту.
					 * @param  GPIOx: указатель на GPIOx порт
					 */
					void MY_GPIO_DisableClock(GPIO_TypeDef* GPIOx);


					/**
					 * @brief  Вычисление порядкового номера GPIO-порта
					 * @param  GPIOx: указатель на GPIOx порт
					 * @retval Порядковый номер GPIO-порта
					 */
					uint16_t MY_GPIO_GetPortSource(GPIO_TypeDef* GPIOx);


					/**
					 * @brief  Получаем порядковый номер с желаемого GPIO пина
					 * @note   Предназначен для служебных целей.
					 * @param  GPIO_Pin: GPIO пин для подсчета порядкового номера
					 * @retval Подсчитанный порядковый номер GPIO пина
					 */
					uint16_t MY_GPIO_GetPinSource(uint16_t GPIO_Pin);


					/**
					 * @brief  Устанавливает пин(ы) как вход
					 * @note   Перед использованием функции необходимо предварительно их инициализировать через @ref MY_GPIO_Init() или @ref MY_GPIO_InitAlternate()
					 * @note   Это просто опция для быстрого переключения в режим input
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут изменены. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @retval Нет
					 */
					void MY_GPIO_SetPinAsInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);


					/**
					 * @brief  Устанавливает пин(ы) как выход
					 * @note   Перед использованием функции необходимо предварительно их инициализировать через @ref MY_GPIO_Init() или @ref MY_GPIO_InitAlternate()
					 * @note   Это просто опция для быстрого переключения в режим output
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут изменены. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @retval Нет
					 */
					void MY_GPIO_SetPinAsOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);


					/**
					 * @brief  Устанавливает пин(ы) как analog
					 * @note   Перед использованием функции необходимо предварительно их инициализировать через @ref MY_GPIO_Init() или @ref MY_GPIO_InitAlternate()
					 * @note   Это просто опция для быстрого переключения в режим analog
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут изменены. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @retval Нет
					 */
					void MY_GPIO_SetPinAsAnalog(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);


					/**
					 * @brief  Устанавливает пин(ы) как alternate function
					 * @note   Перед использованием функции необходимо предварительно их инициализировать через @ref MY_GPIO_InitAlternate().
					 *            Эта функция используется только для изменения режима GPIO
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут изменены. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @retval Нет
					 */
					void MY_GPIO_SetPinAsAlternate(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);


					/**
					 * @brief  Устанавливает настройки подтяжки в GPIO пинах
					 * @note   Перед использованием функции необходимо предварительно их инициализировать через @ref MY_GPIO_Init() или @ref MY_GPIO_InitAlternate()
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут изменены. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @param  GPIO_PuPd: Выбирает GPIO режим подтяжки. Может быть выбран из @ref MY_GPIO_PuPd_t
					 * @retval Нет
					 */
					void MY_GPIO_SetPullResistor(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, MY_GPIO_PuPd_t GPIO_PuPd);


					/**
					 * @brief  Устанавливает значение GPIO пина в ноль
					 * @note   Объявлен как макрос для получения максимальной скорости выполнения используя атомарную операцию
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут изменены. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @retval Нет
					 */
					#define MY_GPIO_SetPinLow(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (uint32_t)(((uint32_t)GPIO_Pin) << 16))


					/**
					 * @brief  Устанавливает значение GPIO пина в единицу
					 * @note   Объявлен как макрос для получения максимальной скорости выполнения используя атомарную операцию
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут изменены. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @retval Нет
					 */
					#define MY_GPIO_SetPinHigh(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (uint32_t)(GPIO_Pin))


					/**
					 * @brief  Устанавливает значение GPIO пина
					 * @note   Объявлен как макрос для получения максимальной скорости выполнения используя прямой доступ к регистрам
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут изменены. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @param  val: если параметр устанавливается в 0 тогда пин устанавливается в low, иначе high
					 * @retval Нет
					 */
					#define MY_GPIO_SetPinValue(GPIOx, GPIO_Pin, val)	((val) ? MY_GPIO_SetPinHigh(GPIOx, GPIO_Pin) : MY_GPIO_SetPinLow(GPIOx, GPIO_Pin))


					/**
					 * @brief  Инвертирует значение пина
					 * @note   Объявлен как макрос для получения максимальной скорости выполнения используя прямой доступ к регистрам
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  GPIO_Pin: GPIO пин(ы) которые будут изменены. Вы можете перечислить несколько пинов используя | (OR) оператор
					 * @retval Нет
					 */
					#define MY_GPIO_TogglePinValue(GPIOx, GPIO_Pin)		((GPIOx)->ODR ^= (GPIO_Pin))


					/**
					 * @brief  Устанавливает значение на ВЕСЬ порт GPIO в виде 32-битного значения
					 * @note   Объявлен как макрос для получения максимальной скорости выполнения используя прямой доступ к регистрам
					 * @param  GPIOx: указатель на GPIOx порт в котором будут производиться изменения
					 * @param  value: значение присваеваемое для GPIO OUTPUT
					 * @retval Нет
					 */
					#define MY_GPIO_SetPortValue(GPIOx, value)			((GPIOx)->ODR = (value))


					/**
					 * @brief  Получаем входные данные с GPIO-пина
					 * @note   Объявлен как макрос для получения максимальной скорости выполнения используя прямой доступ к регистрам
					 * @param  GPIOx: указатель на GPIOx порт из которого будут считываться данные
					 * @param  GPIO_Pin: GPIO пин(ы) в которых будут считывать значение
					 * @retval 1 если значение high, или 0 если low
					 */
					#define MY_GPIO_GetInputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->IDR & (GPIO_Pin)) == 0 ? 0 : 1)


					/**
					 * @brief  Получаем выходные данные с GPIO-пина
					 * @note   Объявлен как макрос для получения максимальной скорости выполнения используя прямой доступ к регистрам
					 * @param  GPIOx: указатель на GPIOx порт из которого будут считываться данные
					 * @param  GPIO_Pin: GPIO пин(ы) в которых будут считывать значение
					 * @retval 1 если значение high, или 0 если low
					 */
					#define MY_GPIO_GetOutputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->ODR & (GPIO_Pin)) == 0 ? 0 : 1)


					/**
					 * @brief  Получаем входные данные со ВСЕГО GPIO порта
					 * @note   Объявлен как макрос для получения максимальной скорости выполнения используя прямой доступ к регистрам
					 * @param  GPIOx: указатель на GPIOx порт из которого будут считываться данные
					 * @retval Значение всего регистра INPUT
					 */
					#define MY_GPIO_GetPortInputValue(GPIOx)			((GPIOx)->IDR)


					/**
					 * @brief  Получаем выходные данные со ВСЕГО GPIO порта
					 * @note   Объявлен как макрос для получения максимальной скорости выполнения используя прямой доступ к регистрам
					 * @param  GPIOx: указатель на GPIOx порт из которого будут считываться данные
					 * @retval Значение всего регистра OUTPUT
					 */
					#define MY_GPIO_GetPortOutputValue(GPIOx)			((GPIOx)->ODR)


					/**
					 * @brief  Получает матрицу свободных пинов которые используются и были инициализированы
					 * @param  GPIOx: указатель на GPIOx порт из которого будут считываться данные
					 * @retval Битовые значения матрицы задействованных пинов в GPIO-порту
					 */
					uint16_t MY_GPIO_GetUsedPins(GPIO_TypeDef* GPIOx);


					/**
					 * @brief  Получает матрицу свободных пинов которые не используются и не были инициализированы
					 * @param  GPIOx: указатель на GPIOx порт из которого будут считываться данные
					 * @retval Битовые значения матрицы свободных пинов в GPIO-порту
					 */
					uint16_t MY_GPIO_GetFreePins(GPIO_TypeDef* GPIOx);


					/**
					 * @brief  Блокирует GPIOx регистры пина для последующих изменений
					 * @note   Вы не сможете вносить изменения в конфигурационные регистры пинов GPIO пока MCU не будет перезагружен
					 * @param  GPIOx: указатель на GPIOx порт в котором будут блокироваться регистры
					 * @param  GPIO_Pin: GPIO пин в которо будут блокироваться регистры
					 * @retval Нет
					 */
					MY_Result_t MY_GPIO_Lock(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);


				/**
				 * @} MY_GPIO_Functions
				 */

			/**
			 * @} MY_HAL_GPIO
			 */

		/**
		 * @} MY_STM32F0xx_Libraries
		 */

		/* C++ detection */
		#ifdef __cplusplus
			}
		#endif

	#endif
