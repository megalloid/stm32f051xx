/**
 * @author  Andrey Zaostrovnykh
 * @email   megalloid@mail.ru
 * @link    http://smarthouseautomatics.ru/stm32/stm32f0xx/rcc/
 * @version v0.1
 * @ide     STM32CubeIDE
 * @brief   Библиотека RCC для STM32F0xx
 */


#ifndef MY_STM32F0xx_RCC_H
	#define MY_STM32F0xx_RCC_H 100

	/* C++ detection */
	#ifdef __cplusplus
		extern "C" {
	#endif

	/**
	 * @addtogroup MY_STM32F0xx_Libraries
	 * @{
	 */
		/**
		 * @defgroup MY_HAL_RCC
		 * @brief    Библиотека RCC для STM32F0xx
		 * @{
		 *
		 * RCC библиотека обеспечивает инициализацию тактирования.
		 * Функция @ref MY_RCC_InitSystem должна быть вызвана в начале @ref функции main.
		 *
		 *
		==============================================================================
							##### Особенности RCC #####
		==============================================================================

		После старта устройства для тактирования используется внутернний Internal High Speed осциллятор
		(HSI 8MHz) с Flash 0 wait state, включенным буфером предвыборки Flash prefetch buffer,
		и вся периферия	кроме внутренней SRAM, Flash и JTAG.

		(+) На высокоскоростных (AHB) и низкоскоростных (APB) шинах нет предделителя;
			все периферийные устройства, подключенные к этим шинам, работают на скорости HSI.

		(+) Тактирование отключено для всей периферии кроме SRAM и FLASH.

		(+) Все порты GPIO установлены в input floating режим, исключая выводы JTAG которые используются для отладки.
			После запуска устройства, в приложении необходимо:

		(+) Настроить источник тактирования, который будет использоваться в качестве системного
			(если нужна более высокая частота / производительность)

		(+) Настроить частоту тактирования System clock и настройки Flash

		(+) Настроить предделители шин AHB и APB

		(+) Включить тактирование для используемой периферии

		(+) Настроить источники тактирования для периферии которая не тактируется
			с системной шины System clock (RTC, ADC, I2C, USART, TIM, USB FS, etc..)


		==============================================================================
							##### Ограничения RCC #####
		==============================================================================

		Должна быть принята во внимание задержка между включением тактирования периферии и
		включением самой периферии и рекомендуется делать проверку установки битов чтением/записью из/в регистры.

		(+) Эта задержка зависит от расположения периферийного блока.
			(++) Для AHB и APB периферии, 1 чтение необходимо как минимум
			(++) Для задержки используется установка битов с помощью функции MY_UTILS_SetBitWithRead()

		Workarounds:
		(#) Для AHB и APB периферии, используется MY_UTILS_SetBitWithRead() для фиктивной задержки при включении тактирования


		===============================================================================
							##### Функции инициализации и деинициализации #####
		===============================================================================

		В данном разделе описано как обеспечить настройку внешних или внутренникх осцилляторов
		(HSE, HSI, HSI14, HSI48, LSE, LSI, PLL, CSS и MCO) и системные шины тактирования (SYSCLK, AHB и APB1)
		которые возможно использовать в STM32F0xx-семействе контроллеров

		Настройка внешнего/внутреннего источника тактирования и PLL

		(#) HSI (high-speed internal), устанавливаемая при производстве 8 MHz RC-цепь используемая напрямую
			или через PLL в качестве источника системного тактирования.
			HSI может использоваться также в качестве тактирования USART и I2C.

		(#) HSI14 (high-speed internal), устанавливаемая при производстве 14 MHz RC-цепь
			для прямого тактирования ADC.

		(#) LSI (low-speed internal), энергоэффективная ~40 KHz RC-цепь используемая
			в качестве источника тактирования для IWDG и/или RTC

		(#) HSE (high-speed external), внешний кварцевый осциллятор номиналом от 4 до 32 MHz
			может использоваться напрямую или через PLL в качестве источника системного тактирования.
			Может так же использоваться в качестве источника тактирования для RTC.

		(#) LSE (low-speed external), осциллятор 32 KHz который используется для тактирования RTC.

		(#) PLL (может тактироваться от HSI, HSI48 или HSE), так же содержит несколько выходных тактовых сигналов:

			(++) Первый выход используется для генерации высокоскоростного тактового сигнала для системного тактирования (до 48 MHz)
			(++) Второй выход используется для генерации тактового сигнала для USB FS (48 MHz)
			(++) Третий выход может использоваться для генерации тактового сигнала для TIM, I2C и USART (до 48 MHz)

		(#) CSS (Clock security system), включается единоразово через вызов макроса MY_RCC_CSS_ENABLE()
			и если HSE тактирование нарушится (если HSE использовано напрямую или через PLL как источник системного тактирования),
			то системное тактировани автоматически переключится на HSI и сгенерирует прерывание.
			Данное прерывание генерируется в качестве немаскированного (Cortex-M0 Non-Maskable Interrupt exception vector).

		(#) MCO (microcontroller clock output), используется для вывода тактовой частоты
			с SYSCLK, HSI, HSE, LSI, LSE или PLL (деленый на 2) на GPIO-ножку (используется PA8).


		Настройка системного тактирования и шин AHB и APB

		(#) Может использоваться несколько источников тактироваиня дл яSystem clock (SYSCLK): HSI, HSE и PLL.
			Тактирование шины AHB (HCLK) обеспечивается с системной шины через настраиваемый
			предделитель и используется для тактирования CPU, памяти и периферии которая находится на шине AHB (DMA, GPIO и т.д.).
			Тактовый сигнал для APB1 (PCLK1) используется из AHB шины через конфигурируемый предделитель
			и используется для тактирования периферии расположенной на этой шине.
			Вы можеет использовать функцию @ref MY_RCC_GetSysClockFreq() для получения текущего значения частоты.

		(#) Все тактирование периферии обеспечивается с системной шины(SYSCLK) за исключением:

			(++) Программирование/очистка FLASH памяти тактируется всегда с HSI 8MHz.
			(++) Тактирование USB 48 MHz всегда обеспечивается через PLL VCO.
			(++) Тактирование USART может быть обеспечено в т.ч. с HSI 8MHz, либо с LSI или LSE.
			(++) Тактирование I2C может обеспечиваться так же через HSI 8MHz.
			(++) Тактирование ADC может быть обеспечено через PLL.
			(++) Тактирование RTC может быть обеспечено с LSE, LSI или 1 MHz HSE_RTC
				(частота с HSE делится на программно устанавливаемый предделитель). Частота системного тактирования(SYSCLK)
				должна быть больше или равной частоте тактирования RTC.
			(++) Тактирование IWDG всегда обеспечивается через LSI.

		(#) Для серии STM32F0xx максимальная частотат SYSCLK, HCLK и PCLK1 равна 48 MHz,
			В зависимости от частоты SYSCLK, задержка выборки при чтении (flash latency) должна быть
			установлены в соответствии с системной частотой.

		(#) После сброса для тактирования системной шины всегда используется HSI (8 MHz) с 0 WS и отключеной предвыборкой (prefetch).

			Таблица установки задержки в соответствии с частотой:

			+-----------------------------------------------+
			| Задержка      | SYSCLK clock frequency (MHz)  |
			|---------------|-------------------------------|
			|0WS(1CPU cycle)|       0 < SYSCLK <= 24        |
			|---------------|-------------------------------|
			|1WS(2CPU cycle)|      24 < SYSCLK <= 48        |
			+-----------------------------------------------+

		===============================================================================
					##### Функции Extended Clock Recovery System Control #####
		===============================================================================

		Для устройств с поддержкой системы Clock Recovery System (CRS), библиотека RCC может быть использована для:

		(#) В конфигурации системного тактирования, если нужно задействовать HSI48

		(#) Включение тактирования CRS в инициализации встроенной периферии которая использует функции CRS

		(#) Вызов функций работы с CRS, такие как:

			(##) Подготовка синхронизации важной для калибровки HSI48:

				(+++) Значения по умолчанию могут быть установлены для частоты Error Measurement (лимит перезагрузки и ошибки)
					  и также для сглаживания частоты осцилляции HSI48.

				(+++) Макрос MY_RCC_CRS_RELOADVALUE_CALCULATE может быть так же использован для вычисления
					  значения перезагрузки с целевыми значениями и частотами синхронизации

			(##) Вызов функции MY_RCC_CRSConfig которая:

				(+++) Сброс регистров CRS к значениям по умолчанию

				(+++) Настройка регистров CRS с настройками синхронизации

				(+++) Включение автоматической калибровки частоты и подсчёта количества данных ошибок

				Примечание: Когда используется USB LPM (Link Power Management) и устройство находится в Sleep mode,
				периодический пакет USB SOF не генерируется хостом. Таким образом, сигнал синхронизации не будет подаватьсяв CRS
				для калибровки HSI48 на ходу. Чтобы гарантировать необходимую точность синхронизации после выхода из спящего режима,
				в качестве сигнала синхронизации следует использовать LSE или опорный источник тактирования на GPIO.

			(##) Предусмотрена функция опроса для ожидания полной синхронизации:

				(+++) Вызовом функции MY_RCC_CRSWaitSynchronization()

				(+++) В соответствии со статусом CRS, пользователь может повторно провести калибровку или продолжить
					  выполнение если синхронизация в норме.

		(#) Пользователь может получить информацию о синхронизации вызвав функцию MY_RCC_CRSGetSynchronizationInfo()

		(#) Что касается статуса синхронизации и информации о синхронизации, пользователь может попробовать обновленную
			калибровку изменив конфигурацию синхронизации и повторно вызвав MY_RCC_CRSConfig.

			Примечание: Когда событие синхронизации обнаружено во время фазы нисходящего счета (до достижения нулевого значения),
			это означает, что фактическая частота ниже целевой (и поэтому значение обрезки должно быть увеличено),
			в то время как когда оно обнаружено во время фазы восходящего счета, это означает, что фактическая частота
			выше (и что значение подстройки должно быть уменьшено).

		(#) В режиме прерываний, пользователь может использовать макрос (MY_RCC_CRS_XXX_IT).
			Прерывание пройдет через CRS Handler (RCC_IRQn/RCC_IRQHandler).
			Осуществляется это следующим образом:

			(++) Вызов функции MY_RCC_CRSConfig()
			(++) Включение RCC_IRQn (через NVIC функции)
			(++) Включение CRS прерывания (@ref MY_RCC_CRS_ENABLE_IT)
			(++) Реализация усправления статусом CRS по следующим вызовам callback-функций вызываемых из MY_RCC_CRS_IRQHandler():

				(+++) MY_RCC_CRS_SyncOkCallback()
				(+++) MY_RCC_CRS_SyncWarnCallback()
				(+++) MY_RCC_CRS_ExpectedSyncCallback()
				(+++) MY_RCC_CRS_ErrorCallback()

		(#) Чтобы вручную вызвать SYNC EVENT, пользователь может использовать функцию MY_RCC_CRSSoftwareSynchronizationGenerate().
			Эта функция может быть вызвана перед вызовом MY_RCC_CRSConfig (например в обработчике прерывания от Systick)
		*/

		#include "main.h"
		#include "my_stm32f0xx.h"
		#include "my_stm32f0xx_cortex.h"
		#include "my_stm32f0xx_utils.h"
		#include "my_stm32f0xx_gpio.h"


		/**
		 * @defgroup MY_Settings
		 * @brief    Библиотечные настройки
		 * Данные настройки используются по умолчанию если они не переобъявлены в настройках проекта или в файле main.h
		 * @{
		 */
			/**
			 * @brief Укажите значение внешнего высокоскоростного кварцевого осциллятора (HSE) используемого в приложении.
			 *        Это значение используется так же для рассчета системной частоты в функциях библиотеки.
			 *        (когда HSE используется в качестве источника системного тактирования, напрямую или через PLL).
			 */
			#if !defined (HSE_VALUE)
				#define   HSE_VALUE            							8000000U  	/*!< Значение в герцах */
			#endif


			/**
			 * @brief Значение таймаута старта HSE
			 */
			#if !defined (HSE_STARTUP_TIMEOUT)
				#define   HSE_STARTUP_TIMEOUT  							100U      	/*!< Время старта HSE в мс. */
			#endif


			/**
			 * @brief Включение или отключение HSE bypass
			 */
			#ifndef 	RCC_HSE_BYPASS_STATE
				#define RCC_HSE_BYPASS_STATE							0U
			#endif


			/**
			 * @brief Укажите значение внутреннего высокоскоростного осциллятора (HSI) используемого в приложении.
			 *        Это значение используется так же для рассчета системной частоты в функциях библиотеки.
			 *        (когда HSE используется в качестве источника системного тактирования, напрямую или через PLL).
			 */
			#if !defined (HSI_VALUE)
				#define   HSI_VALUE            							8000000U  	/*!< Значение в герцах */
			#endif


			/**
			 * @brief Значение таймаута старта HSI
			 */
			#if !defined (HSI_STARTUP_TIMEOUT)
				#define   HSI_STARTUP_TIMEOUT  							5000U     	/*!< Время старта HSI в мс. */
			#endif


			/**
			 * @brief Частота внутреннего высокоскоростного осциллятора используемого для ADC (HSI14)
			 */
			#if !defined (HSI14_VALUE)
				#define   HSI14_VALUE          							14000000U 	/*!< Значение в герцах. Реальное значение может меняться в зависимости от питающего напряжения и температуры */
			#endif


			/**
			 * @brief Значение калибровки для ADC (HSI14)
			 */
			#if !defined (RCC_HSI14CALIBRATION_VALUE)
				#define   RCC_HSI14CALIBRATION_VALUE        			0U 		/*!< Значение должно быть в диапазоне от 0 до 0x1F */
			#endif


			/**
			 * @brief Частота внутреннего высокоскоростного осциллятора используемого для USB (HSI48)
			 */
			#if !defined (HSI48_VALUE)
				#define   HSI48_VALUE          							48000000U 	/*!< Значение в герцах. Реальное значение может меняться в зависимости от питающего напряжения и температуры */
			#endif


			/**
			 * @brief Частота внутреннего низкоскоростного осциллятора (LSI)
			 */
			#if !defined (LSI_VALUE)
				#define   LSI_VALUE            							40000U   		/*!< Значение в герцах. Реальное значение может меняться в зависимости от питающего напряжения и температуры */
			#endif


			/**
			 * @brief Частота внешнего низкоскоростного осциллятора (LSE)
			 */
			#if !defined (LSE_VALUE)
				#define   LSE_VALUE            							32768U    	/*!< Значение в герцах. */
			#endif


			/**
			 * @brief Значение таймаута старта LSE
			 */
			#if !defined (LSE_STARTUP_TIMEOUT)
				#define   LSE_STARTUP_TIMEOUT  							5000U     	/*!< Время для старта LSE в мс. */
			#endif


			/**
			 * @brief Включение или отключение LSE bypass
			 */
			#ifndef 		RCC_LSE_BYPASS_STATE
				#define RCC_LSE_BYPASS_STATE								0U
			#endif


			/**
			 * @} MY_Settings
			 */

			/**
			 * @defgroup MY_RCC_Defines
			 * @brief    RCC библиотечные константы
			 * @{
			 */
				/**
				 * @brief Регистры и нумерации битов в RCC
				 * @{
				 */
					/* Смещение регистра в карте памяти относительного базового адреса */
					#define RCC_OFFSET                						(RCC_BASE - PERIPH_BASE)

					/* Смещения регистров отдельных регистров модуля RCC */
					#define RCC_CR_OFFSET             						0x00
					#define RCC_CFGR_OFFSET           						0x04
					#define RCC_CIR_OFFSET            						0x08
					#define RCC_BDCR_OFFSET           						0x20
					#define RCC_CSR_OFFSET            						0x24

					/* Прямое объявление адресов регистров RCC */
					#define REG_RCC_CR         								(RCC_BASE + 0x00000000)	/*!< RCC clock control */
					#define REG_RCC_CFGR       								(RCC_BASE + 0x00000004)	/*!< RCC clock configuration */
					#define REG_RCC_CIR        								(RCC_BASE + 0x00000008)	/*!< RCC clock interrupt */
					#define REG_RCC_APB2RSTR   								(RCC_BASE + 0x0000000C)	/*!< RCC APB2 peripheral reset */
					#define REG_RCC_APB1RSTR   								(RCC_BASE + 0x00000010)	/*!< RCC APB1 peripheral reset */
					#define REG_RCC_AHBENR     								(RCC_BASE + 0x00000014)	/*!< RCC AHB peripheral clock */
					#define REG_RCC_APB2ENR    								(RCC_BASE + 0x00000018)	/*!< RCC APB2 peripheral clock enable */
					#define REG_RCC_APB1ENR    								(RCC_BASE + 0x0000001C)	/*!< RCC APB1 peripheral clock enable */
					#define REG_RCC_BDCR       								(RCC_BASE + 0x00000020)	/*!< RCC Backup domain control */
					#define REG_RCC_CSR        								(RCC_BASE + 0x00000024)	/*!< RCC clock control & status */
					#define REG_RCC_AHBRSTR 								(RCC_BASE + 0x00000028)	/*!< RCC AHB peripheral reset */
					#define REG_RCC_CFGR2      								(RCC_BASE + 0x0000002C)	/*!< RCC clock configuration 2 */
					#define REG_RCC_CFGR3      								(RCC_BASE + 0x00000030)	/*!< RCC clock configuration 3 */
					#define REG_RCC_CR2        								(RCC_BASE + 0x00000034)	/*!< RCC clock control 2 */


					/* Байт 2 регистра CR (биты [23:16]) относительно базового адреса RCC */
					#define RCC_CR_BYTE2_ADDRESS      						((uint32_t)(RCC_BASE + RCC_CR_OFFSET + 0x02U))

					/* Байт 1 регистра CIR (биты [15:8]) относительно базового адреса RCC */
					#define RCC_CIR_BYTE1_ADDRESS     						((uint32_t)(RCC_BASE + RCC_CIR_OFFSET + 0x01U))

					/* Байт 2 регистра CIR (биты [23:16]) относительно базового адреса RCC */
					#define RCC_CIR_BYTE2_ADDRESS     						((uint32_t)(RCC_BASE + RCC_CIR_OFFSET + 0x02U))

					/* Константы используемые для объявления флагов */
					#define CR_REG_INDEX              						((uint8_t)1U)
					#define CR2_REG_INDEX             						((uint8_t)2U)
					#define BDCR_REG_INDEX            						((uint8_t)3U)
					#define CSR_REG_INDEX            	 					((uint8_t)4U)

					/* Позиции битов в CFGR регистре */
					#define RCC_CFGR_PLLMUL_BITNUMBER         				18U
					#define RCC_CFGR_HPRE_BITNUMBER           				4U
					#define RCC_CFGR_PPRE_BITNUMBER           				8U

					/* Флаги в CFGR2 регистре */
					#define RCC_CFGR2_PREDIV_BITNUMBER        				0

					/* Флаги в CR регистре */
					#define RCC_CR_HSIRDY_BITNUMBER           				1
					#define RCC_CR_HSERDY_BITNUMBER           				17
					#define RCC_CR_PLLRDY_BITNUMBER           				25

					/* Флаги в CR2 регистре */
					#define RCC_CR2_HSI14RDY_BITNUMBER        				1
					#define RCC_CR2_HSI14TRIM_BITNUMBER         			3
					#define RCC_CR2_HSI48RDY_BITNUMBER       				16

					/* Флаги в BDCR регистре */
					#define RCC_BDCR_LSERDY_BITNUMBER         				1

					/* Флаги в CSR регистре */
					#define RCC_CSR_LSIRDY_BITNUMBER          				1
					#define RCC_CSR_V18PWRRSTF_BITNUMBER      				23
					#define RCC_CSR_RMVF_BITNUMBER            				24
					#define RCC_CSR_OBLRSTF_BITNUMBER         				25
					#define RCC_CSR_PINRSTF_BITNUMBER         				26
					#define RCC_CSR_PORRSTF_BITNUMBER         				27
					#define RCC_CSR_SFTRSTF_BITNUMBER         				28
					#define RCC_CSR_IWDGRSTF_BITNUMBER        				29
					#define RCC_CSR_WWDGRSTF_BITNUMBER        				30
					#define RCC_CSR_LPWRRSTF_BITNUMBER        				31

					/* Флаги в HSITRIM регистре */
					#define RCC_CR_HSITRIM_BITNUMBER          				3
					#define RCC_HSI14TRIM_BITNUMBER           				3

					#define RCC_FLAG_MASK                    				((uint8_t)0x1FU)

					/* Позиции битов CRS в регистрах */
					#define CRS_CFGR_FELIM_BITNUMBER    					16
					#define CRS_CR_TRIM_BITNUMBER       					8
					#define CRS_ISR_FECAP_BITNUMBER     					16

				/**
				 * @}
				 */


				/**
				 * @brief Тип используемого осциллятора
				 * @{
				 */
					#define RCC_OSCILLATORTYPE_NONE            				((uint32_t)0x00000000U)
					#define RCC_OSCILLATORTYPE_HSE             				((uint32_t)0x00000001U)
					#define RCC_OSCILLATORTYPE_HSI             				((uint32_t)0x00000002U)
					#define RCC_OSCILLATORTYPE_LSE             				((uint32_t)0x00000004U)
					#define RCC_OSCILLATORTYPE_LSI             				((uint32_t)0x00000008U)
					#define RCC_OSCILLATORTYPE_HSI14           				((uint32_t)0x00000010U)

					#if defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx)

						#define RCC_OSCILLATORTYPE_HSI48   					((uint32_t)0x00000020)

					#endif

				/**
				 * @}
				 */


				/**
				 * @brief Источник тактирования SYSCLK
				 * @{
				 */
					#define RCC_SYSCLK_SOURCE_HSE             				RCC_CFGR_SW_HSE
					#define RCC_SYSCLK_SOURCE_HSI             				RCC_CFGR_SW_HSI
					#define RCC_SYSCLK_SOURCE_PLL             				RCC_CFGR_SW_PLL

					#if defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx)

						#define RCC_SYSCLKSOURCE_HSI48      				RCC_CFGR_SW_HSI48

					#endif

				/**
				 * @}
				 */


				/**
				 * @brief Предделитель для шины AHB (SYSCLK)
				 * @{
				 */
					#define RCC_AHB_DIV1                        		RCC_CFGR_HPRE_DIV1
					#define RCC_AHB_DIV2                        		RCC_CFGR_HPRE_DIV2
					#define RCC_AHB_DIV4                        		RCC_CFGR_HPRE_DIV4
					#define RCC_AHB_DIV8                        		RCC_CFGR_HPRE_DIV8
					#define RCC_AHB_DIV16                      			RCC_CFGR_HPRE_DIV16
					#define RCC_AHB_DIV64                       		RCC_CFGR_HPRE_DIV64
					#define RCC_AHB_DIV128                      		RCC_CFGR_HPRE_DIV128
					#define RCC_AHB_DIV256                      		RCC_CFGR_HPRE_DIV256
					#define RCC_AHB_DIV512                      		RCC_CFGR_HPRE_DIV512

				/**
				 * @}
				 */


				/**
				 * @brief Делитель шины APB1 (HCLK)
				 * @{
				 */
					#define RCC_APB1_DIV1                      			RCC_CFGR_PPRE_DIV1
					#define RCC_APB1_DIV2                       		RCC_CFGR_PPRE_DIV2
					#define RCC_APB1_DIV4                       		RCC_CFGR_PPRE_DIV4
					#define RCC_APB1_DIV8                       		RCC_CFGR_PPRE_DIV8
					#define RCC_APB1_DIV16                      		RCC_CFGR_PPRE_DIV16

				/**
				 * @}
				 */


				/**
				 * @brief Шины системного тактирования
				 * @{
				 */
					#define RCC_CLOCKTYPE_SYSCLK                		((uint32_t)0x00000001U)
					#define RCC_CLOCKTYPE_HCLK                  		((uint32_t)0x00000002U)
					#define RCC_CLOCKTYPE_PCLK1                 		((uint32_t)0x00000004U)

				/**
				 * @}
				 */


				/**
				 * @brief Переферия с отдельными источниками тактирования
				 * @{
				 */
					#if defined(STM32F030x6) || \
						defined(STM32F030x8) || \
						defined(STM32F031x6) || \
						defined(STM32F038xx) || \
						defined(STM32F030xC)

						#define RCC_PERIPHCLK_USART1        			((uint32_t)0x00000001U)
						#define RCC_PERIPHCLK_I2C1          			((uint32_t)0x00000020U)
						#define RCC_PERIPHCLK_RTC           			((uint32_t)0x00010000U)

					#elif defined(STM32F070x6) || \
						  defined(STM32F070xB)

						#define RCC_PERIPHCLK_USART1          			((uint32_t)0x00000001U)
						#define RCC_PERIPHCLK_I2C1            			((uint32_t)0x00000020U)
						#define RCC_PERIPHCLK_RTC             			((uint32_t)0x00010000U)
						#define RCC_PERIPHCLK_USB             			((uint32_t)0x00020000U)

					#elif defined(STM32F042x6) || \
						  defined(STM32F048xx)

						#define RCC_PERIPHCLK_USART1          			((uint32_t)0x00000001U)
						#define RCC_PERIPHCLK_I2C1            			((uint32_t)0x00000020U)
						#define RCC_PERIPHCLK_CEC             			((uint32_t)0x00000400U)
						#define RCC_PERIPHCLK_RTC             			((uint32_t)0x00010000U)
						#define RCC_PERIPHCLK_USB             			((uint32_t)0x00020000U)

					#elif defined(STM32F051x8) || \
						  defined(STM32F058xx)

						#define RCC_PERIPHCLK_USART1          			((uint32_t)0x00000001U)
						#define RCC_PERIPHCLK_I2C1            			((uint32_t)0x00000020U)
						#define RCC_PERIPHCLK_CEC             			((uint32_t)0x00000400U)
						#define RCC_PERIPHCLK_RTC             			((uint32_t)0x00010000U)

					#elif defined(STM32F071xB)

						#define RCC_PERIPHCLK_USART1          			((uint32_t)0x00000001U)
						#define RCC_PERIPHCLK_USART2          			((uint32_t)0x00000002U)
						#define RCC_PERIPHCLK_I2C1            			((uint32_t)0x00000020U)
						#define RCC_PERIPHCLK_CEC             			((uint32_t)0x00000400U)
						#define RCC_PERIPHCLK_RTC             			((uint32_t)0x00010000U)

					#elif defined(STM32F072xB) || \
						  defined(STM32F078xx)

						#define RCC_PERIPHCLK_USART1          			((uint32_t)0x00000001U)
						#define RCC_PERIPHCLK_USART2          			((uint32_t)0x00000002U)
						#define RCC_PERIPHCLK_I2C1            			((uint32_t)0x00000020U)
						#define RCC_PERIPHCLK_CEC             			((uint32_t)0x00000400U)
						#define RCC_PERIPHCLK_RTC             			((uint32_t)0x00010000U)
						#define RCC_PERIPHCLK_USB             			((uint32_t)0x00020000U)

					#elif defined(STM32F091xC) || \
						  defined(STM32F098xx)

						#define RCC_PERIPHCLK_USART1          			((uint32_t)0x00000001U)
						#define RCC_PERIPHCLK_USART2          			((uint32_t)0x00000002U)
						#define RCC_PERIPHCLK_I2C1            			((uint32_t)0x00000020U)
						#define RCC_PERIPHCLK_CEC             			((uint32_t)0x00000400U)
						#define RCC_PERIPHCLK_RTC             			((uint32_t)0x00010000U)
						#define RCC_PERIPHCLK_USART3          			((uint32_t)0x00040000U)

					#endif

				/**
				 * @}
				 */


				/**
				 * @brief Таймауты RCC
				 * @{
				 */
					#define HSE_TIMEOUT_VALUE                   		((uint32_t)100U)	/*!< Таймаут для старта HSE, в 100 ms */
					#define HSI_TIMEOUT_VALUE                   		((uint8_t)2U)      	/*!< Таймаут для старта HSI, 2 ms (Tick + 1U) */
					#define LSI_TIMEOUT_VALUE                   		((uint8_t)2U)     	/*!< Таймаут для старта LSI, 2 ms (Tick + 1U) */
					#define PLL_TIMEOUT_VALUE                   		((uint8_t)2U)      	/*!< Таймаут для старта PLL, 2 ms (Tick + 1U) */
					#define HSI14_TIMEOUT_VALUE                 		((uint8_t)2U)      	/*!< Таймаут для старта HSI14, 2 ms (Tick + 1U) */

					#define RCC_DBP_TIMEOUT_VALUE      					(100U)       		/*!< Таймаут смены состояние защиты от записи в Disable Backup domain */
					#define RCC_LSE_TIMEOUT_VALUE      					LSE_STARTUP_TIMEOUT	/*!< Таймаут смены статуса LSE */

					#define CLOCKSWITCH_TIMEOUT_VALUE  					(5000U)  			/*!< Таймаут для смены источника тактировани 5 s */

					#if defined(RCC_HSI48_SUPPORT)
						#define HSI48_TIMEOUT_VALUE        				((uint8_t)2U)      	/*!< Таймаут для HSI48,  2 ms (minimum Tick + 1U) */
					#endif

				/**
				 * @}
				 */


				/**
				 * @brief Константы для MCO
				 * @{
				 */
					/* Используемые порт и пин для MCO */
					#define MCO1_GPIO_PORT        						GPIOA
					#define MCO1_PIN              						GPIO_PIN_8

				/**
				 * @}
				 */


				/**
				 * @brief Константы для Clock Recovery System (HSI48)
				 * @{
				 */
					#if defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx)

						/* RCC CRS Status */
						#define RCC_CRS_NONE      						((uint32_t)0x00000000U)
						#define RCC_CRS_TIMEOUT   						((uint32_t)0x00000001U)
						#define RCC_CRS_SYNCOK    						((uint32_t)0x00000002U)
						#define RCC_CRS_SYNCWARM  						((uint32_t)0x00000004U)
						#define RCC_CRS_SYNCERR   						((uint32_t)0x00000008U)
						#define RCC_CRS_SYNCMISS  						((uint32_t)0x00000010U)
						#define RCC_CRS_TRIMOV    						((uint32_t)0x00000020U)


						/* RCC CRS SynchroSource */
						#define RCC_CRS_SYNC_SOURCE_GPIO       			((uint32_t)0x00000000U) /*!< Synchro Signal soucre GPIO */
						#define RCC_CRS_SYNC_SOURCE_LSE        			CRS_CFGR_SYNCSRC_0      /*!< Synchro Signal source LSE */
						#define RCC_CRS_SYNC_SOURCE_USB        			CRS_CFGR_SYNCSRC_1      /*!< Synchro Signal source USB SOF (default)*/


						/* RCC CRS SynchroDivider */
						#define RCC_CRS_SYNC_DIV1        				((uint32_t)0x00000000U) /*!< Synchro Signal not divided (default) */
						#define RCC_CRS_SYNC_DIV2        				CRS_CFGR_SYNCDIV_0      /*!< Synchro Signal divided by 2 */
						#define RCC_CRS_SYNC_DIV4        				CRS_CFGR_SYNCDIV_1                        /*!< Synchro Signal divided by 4 */
						#define RCC_CRS_SYNC_DIV8        			 	(CRS_CFGR_SYNCDIV_1 | CRS_CFGR_SYNCDIV_0) /*!< Synchro Signal divided by 8 */
						#define RCC_CRS_SYNC_DIV16       				CRS_CFGR_SYNCDIV_2                        /*!< Synchro Signal divided by 16 */
						#define RCC_CRS_SYNC_DIV32       				(CRS_CFGR_SYNCDIV_2 | CRS_CFGR_SYNCDIV_0) /*!< Synchro Signal divided by 32 */
						#define RCC_CRS_SYNC_DIV64       			 	(CRS_CFGR_SYNCDIV_2 | CRS_CFGR_SYNCDIV_1) /*!< Synchro Signal divided by 64 */
						#define RCC_CRS_SYNC_DIV128      				CRS_CFGR_SYNCDIV                          /*!< Synchro Signal divided by 128 */


						/* RCC CRS SynchroPolarity */
						#define RCC_CRS_SYNC_POLARITY_RISING        	((uint32_t)0x00000000U) /*!< Synchro Active on rising edge (default) */
						#define RCC_CRS_SYNC_POLARITY_FALLING       	CRS_CFGR_SYNCPOL      	/*!< Synchro Active on falling edge */


						/* RCC CRS ReloadValueDefault */
						#define RCC_CRS_RELOADVALUE_DEFAULT         	((uint32_t)0x0000BB7FU) /*!< The reset value of the RELOAD field corresponds
																									 to a target frequency of 48 MHz and a synchronization
																																																			 signal frequency of 1 kHz (SOF signal from USB). */

						/* RCC CRS ErrorLimitDefault */
						#define RCC_CRS_ERRORLIMIT_DEFAULT          	((uint32_t)0x00000022U) /*!< Default Frequency error limit */


						/* RCC CRS HSI48CalibrationDefault */
						#define RCC_CRS_HSI48CALIBRATION_DEFAULT    	((uint32_t)0x00000020U) /*!< The default value is 32, which corresponds to the middle of the trimming interval.
																																																			The trimming step is around 67 kHz between two consecutive TRIM steps. A higher TRIM value
																																																			corresponds to a higher output frequency */
						/* RCC CRS FreqErrorDirection*/
						#define RCC_CRS_FREQERRORDIR_UP             	((uint32_t)0x00000000U)   /*!< Upcounting direction, the actual frequency is above the target */
						#define RCC_CRS_FREQERRORDIR_DOWN           	((uint32_t)CRS_ISR_FEDIR) /*!< Downcounting direction, the actual frequency is below the target */

					#endif

				/**
				 * @} CRS
				 */

			/**
			 * @} MY_RCC_Defines
			 */


			/**
			 * @defgroup MY_RCC_Macros
			 * @brief    RCC библиотечные макросы
			 * @{
			 */
				/**
				 * @brief	Быстрые макросы для включения/выключения и проверки тактирования на шине AHB
				 * @note   	После сброса тактирование периферии (используемая в т.ч. для чтения/записи регистров) отключается,
				 *			и перед использованием периферии тактирование должно включаться вручную.
				 * @{
				 */
					#define MY_RCC_AHB_FORCE_RESET()     				(RCC->AHBRSTR = 0xFFFFFFFFU)
					#define MY_RCC_AHB_RELEASE_RESET()   				(RCC->AHBRSTR = 0x00000000U)

					/* GPIOA */
					#define MY_RCC_GPIOA_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_GPIOAEN);
					#define MY_RCC_GPIOA_CLK_DISABLE()  				(RCC->AHBENR &= ~(RCC_AHBENR_GPIOAEN))

					#define MY_RCC_GPIOA_IS_CLK_ENABLED() 				((RCC->AHBENR & (RCC_AHBENR_GPIOAEN)) != RESET)
					#define MY_RCC_GPIOA_IS_CLK_DISABLED()  			((RCC->AHBENR & (RCC_AHBENR_GPIOAEN)) == RESET)

					#define MY_RCC_GPIOA_FORCE_RESET()   				(RCC->AHBRSTR |= (RCC_AHBRSTR_GPIOARST))
					#define MY_RCC_GPIOA_RELEASE_RESET() 				(RCC->AHBRSTR &= ~(RCC_AHBRSTR_GPIOARST))

					/* GPIOB */
					#define MY_RCC_GPIOB_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_GPIOBEN);
					#define MY_RCC_GPIOB_CLK_DISABLE()    			 	(RCC->AHBENR &= ~(RCC_AHBENR_GPIOBEN))

					#define MY_RCC_GPIOB_IS_CLK_ENABLED()     		 	((RCC->AHBENR & (RCC_AHBENR_GPIOBEN)) != RESET)
					#define MY_RCC_GPIOB_IS_CLK_DISABLED()    		 	((RCC->AHBENR & (RCC_AHBENR_GPIOBEN)) == RESET)

					#define MY_RCC_GPIOB_FORCE_RESET()   				(RCC->AHBRSTR |= (RCC_AHBRSTR_GPIOBRST))
					#define MY_RCC_GPIOB_RELEASE_RESET() 				(RCC->AHBRSTR &= ~(RCC_AHBRSTR_GPIOBRST))

					/* GPIOC */
					#define MY_RCC_GPIOC_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_GPIOCEN);
					#define MY_RCC_GPIOC_CLK_DISABLE()    			 	(RCC->AHBENR &= ~(RCC_AHBENR_GPIOCEN))

					#define MY_RCC_GPIOC_IS_CLK_ENABLED()     		 	((RCC->AHBENR & (RCC_AHBENR_GPIOCEN)) != RESET)
					#define MY_RCC_GPIOC_IS_CLK_DISABLED()    		 	((RCC->AHBENR & (RCC_AHBENR_GPIOCEN)) == RESET)

					#define MY_RCC_GPIOC_FORCE_RESET()   				(RCC->AHBRSTR |= (RCC_AHBRSTR_GPIOCRST))
					#define MY_RCC_GPIOC_RELEASE_RESET() 				(RCC->AHBRSTR &= ~(RCC_AHBRSTR_GPIOCRST))


					#if defined(STM32F030x6) || \
						defined(STM32F030x8) || \
						defined(STM32F051x8) || \
						defined(STM32F058xx) || \
						defined(STM32F070xB) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* GPIOD */
						#define MY_RCC_GPIOD_CLK_ENABLE()   			MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_GPIODEN);
						#define MY_RCC_GPIOD_CLK_DISABLE()       		(RCC->AHBENR &= ~(RCC_AHBENR_GPIODEN))

						#define MY_RCC_GPIOD_IS_CLK_ENABLED()      		((RCC->AHBENR & (RCC_AHBENR_GPIODEN)) != RESET)
						#define MY_RCC_GPIOD_IS_CLK_DISABLED()     		((RCC->AHBENR & (RCC_AHBENR_GPIODEN)) == RESET)

						#define MY_RCC_GPIOD_FORCE_RESET()   			(RCC->AHBRSTR |= 	(RCC_AHBRSTR_GPIODRST))
						#define MY_RCC_GPIOD_RELEASE_RESET() 			(RCC->AHBRSTR &= ~(RCC_AHBRSTR_GPIODRST))

					#endif


					#if defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F070xB) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* GPIOE */
						#define MY_RCC_GPIOE_CLK_ENABLE()   			MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_GPIOEEN);
						#define MY_RCC_GPIOE_CLK_DISABLE()        		(RCC->AHBENR &= ~(RCC_AHBENR_GPIOEEN))

						#define MY_RCC_GPIOE_IS_CLK_ENABLED()      		((RCC->AHBENR & (RCC_AHBENR_GPIOEEN)) != RESET)
						#define MY_RCC_GPIOE_IS_CLK_DISABLED()     		((RCC->AHBENR & (RCC_AHBENR_GPIOEEN)) == RESET)

						#define MY_RCC_GPIOE_FORCE_RESET()   			(RCC->AHBRSTR |= 	(RCC_AHBRSTR_GPIOERST))
						#define MY_RCC_GPIOE_RELEASE_RESET() 			(RCC->AHBRSTR &= ~(RCC_AHBRSTR_GPIOERST))

					#endif

					/* GPIOF */
					#define MY_RCC_GPIOF_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_GPIOFEN);
					#define MY_RCC_GPIOF_CLK_DISABLE()    			 	(RCC->AHBENR &= ~(RCC_AHBENR_GPIOFEN))

					#define MY_RCC_GPIOF_IS_CLK_ENABLED()     		 	((RCC->AHBENR & (RCC_AHBENR_GPIOFEN)) != RESET)
					#define MY_RCC_GPIOF_IS_CLK_DISABLED() 				((RCC->AHBENR & (RCC_AHBENR_GPIOFEN)) == RESET)

					#define MY_RCC_GPIOF_FORCE_RESET()   				(RCC->AHBRSTR |= (RCC_AHBRSTR_GPIOFRST))
					#define MY_RCC_GPIOF_RELEASE_RESET() 				(RCC->AHBRSTR &= ~(RCC_AHBRSTR_GPIOFRST))


					/* CRC */
					#define MY_RCC_CRC_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_CRCEN);
					#define MY_RCC_CRC_CLK_DISABLE()      			 	(RCC->AHBENR &= ~(RCC_AHBENR_CRCEN))

					#define MY_RCC_CRC_IS_CLK_ENABLED()       		 	((RCC->AHBENR & (RCC_AHBENR_CRCEN)) != RESET)
					#define MY_RCC_CRC_IS_CLK_DISABLED()      		 	((RCC->AHBENR & (RCC_AHBENR_CRCEN)) == RESET)


					/* DMA1 */
					#define MY_RCC_DMA1_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_DMA1EN);
					#define MY_RCC_DMA1_CLK_DISABLE()     			 	(RCC->AHBENR &= ~(RCC_AHBENR_DMA1EN))

					#define MY_RCC_DMA1_IS_CLK_ENABLED()      		 	((RCC->AHBENR & (RCC_AHBENR_DMA1EN)) != RESET)
					#define MY_RCC_DMA1_IS_CLK_DISABLED()     		 	((RCC->AHBENR & (RCC_AHBENR_DMA1EN)) == RESET)


					#if defined(STM32F091xC) || \
						defined(STM32F098xx)

						/* DMA2 */
						#define MY_RCC_DMA2_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_DMA2EN);
						#define MY_RCC_DMA2_CLK_DISABLE()    			(RCC->AHBENR &= ~(RCC_AHBENR_DMA2EN))

						#define MY_RCC_DMA2_IS_CLK_ENABLED()       		((RCC->AHBENR & (RCC_AHBENR_DMA2EN)) != RESET)
						#define MY_RCC_DMA2_IS_CLK_DISABLED()      		((RCC->AHBENR & (RCC_AHBENR_DMA2EN)) == RESET)

					#endif


					/* SRAM */
					#define MY_RCC_SRAM_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_SRAMEN);
					#define MY_RCC_SRAM_CLK_DISABLE()     			 	(RCC->AHBENR &= ~(RCC_AHBENR_SRAMEN))

					#define MY_RCC_SRAM_IS_CLK_ENABLED()      		 	((RCC->AHBENR & (RCC_AHBENR_SRAMEN)) != RESET)
					#define MY_RCC_SRAM_IS_CLK_DISABLED()     		 	((RCC->AHBENR & (RCC_AHBENR_SRAMEN)) == RESET)

					/* FLITF */
					#define MY_RCC_FLITF_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_FLITFEN);
					#define MY_RCC_FLITF_CLK_DISABLE()    			 	(RCC->AHBENR &= ~(RCC_AHBENR_FLITFEN))

					#define MY_RCC_FLITF_IS_CLK_ENABLED()     		 	((RCC->AHBENR & (RCC_AHBENR_FLITFEN)) != RESET)
					#define MY_RCC_FLITF_IS_CLK_DISABLED()    		 	((RCC->AHBENR & (RCC_AHBENR_FLITFEN)) == RESET)


					#if defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F051x8) || \
						defined(STM32F058xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx)

						/* TSC */
						#define MY_RCC_TSC_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->AHBENR, RCC_AHBENR_TSCEN);
						#define MY_RCC_TSC_CLK_DISABLE()        		(RCC->AHBENR &= ~(RCC_AHBENR_TSCEN))

						#define MY_RCC_TSC_IS_CLK_ENABLED()        		((RCC->AHBENR & (RCC_AHBENR_TSCEN)) != RESET)
						#define MY_RCC_TSC_IS_CLK_DISABLED()       		((RCC->AHBENR & (RCC_AHBENR_TSCEN)) == RESET)

						#define MY_RCC_TSC_FORCE_RESET()     			(RCC->AHBRSTR |= (RCC_AHBRSTR_TSCRST))
						#define MY_RCC_TSC_RELEASE_RESET()   			(RCC->AHBRSTR &= ~(RCC_AHBRSTR_TSCRST))

					#endif

				/**
				 * @} AHB
				 */


				/**
				 * @brief	Быстрые макросы для включения/выключения и проверки тактирования на шине APB1
				 * @note   	После сброса тактирование периферии (используемая в т.ч. для чтения/записи регистров) отключается,
			     *			и перед использованием периферии тактирование должно включаться вручную.
			     * @{
			     */
					#define MY_RCC_APB1_FORCE_RESET()     				(RCC->APB1RSTR = 0xFFFFFFFFU)
					#define MY_RCC_APB1_RELEASE_RESET()   				(RCC->APB1RSTR = 0x00000000U)


					#if defined(STM32F031x6) || \
						defined(STM32F038xx) || \
						defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F051x8) || \
						defined(STM32F058xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx)

						/* TIM2 */
						#define MY_RCC_TIM2_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
						#define MY_RCC_TIM2_CLK_DISABLE()   			(RCC->APB1ENR &= ~(RCC_APB1ENR_TIM2EN))

						#define MY_RCC_TIM2_IS_CLK_ENABLED()       		((RCC->APB1ENR & (RCC_APB1ENR_TIM2EN)) != RESET)
						#define MY_RCC_TIM2_IS_CLK_DISABLED()      		((RCC->APB1ENR & (RCC_APB1ENR_TIM2EN)) == RESET)

						#define MY_RCC_TIM2_FORCE_RESET()     			(RCC->APB1RSTR |= (RCC_APB1RSTR_TIM2RST))
						#define MY_RCC_TIM2_RELEASE_RESET()   			(RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST))

					#endif


					/* TIM3 */
					#define MY_RCC_TIM3_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_TIM3EN);
					#define MY_RCC_TIM3_CLK_DISABLE()   				(RCC->APB1ENR &= ~(RCC_APB1ENR_TIM3EN))

					#define MY_RCC_TIM3_IS_CLK_ENABLED()   				((RCC->APB1ENR & (RCC_APB1ENR_TIM3EN)) != RESET)
					#define MY_RCC_TIM3_IS_CLK_DISABLED()  				((RCC->APB1ENR & (RCC_APB1ENR_TIM3EN)) == RESET)

					#define MY_RCC_TIM3_FORCE_RESET()     			 	(RCC->APB1RSTR |= (RCC_APB1RSTR_TIM3RST))
					#define MY_RCC_TIM3_RELEASE_RESET()   			 	(RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST))


					#if defined(STM32F030x8) || \
						defined(STM32F051x8) || \
						defined(STM32F058xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F070xB) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* TIM6 */
						#define MY_RCC_TIM6_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_TIM6EN);
						#define MY_RCC_TIM6_CLK_DISABLE()   			(RCC->APB1ENR &= ~(RCC_APB1ENR_TIM6EN))

						#define MY_RCC_TIM6_IS_CLK_ENABLED()       		((RCC->APB1ENR & (RCC_APB1ENR_TIM6EN)) != RESET)
						#define MY_RCC_TIM6_IS_CLK_DISABLED()      		((RCC->APB1ENR & (RCC_APB1ENR_TIM6EN)) == RESET)

						#define MY_RCC_TIM6_FORCE_RESET()     			(RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM6RST))
						#define MY_RCC_TIM6_RELEASE_RESET()   		 	(RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM6RST))


						/* I2C2 */
						#define MY_RCC_I2C2_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_I2C2EN);
						#define MY_RCC_I2C2_CLK_DISABLE()   			(RCC->APB1ENR &= ~(RCC_APB1ENR_I2C2EN))

						#define MY_RCC_I2C2_IS_CLK_ENABLED()       		((RCC->APB1ENR & (RCC_APB1ENR_I2C2EN)) != RESET)
						#define MY_RCC_I2C2_IS_CLK_DISABLED()      		((RCC->APB1ENR & (RCC_APB1ENR_I2C2EN)) == RESET)

						#define MY_RCC_I2C2_FORCE_RESET()     			(RCC->APB1RSTR |= (RCC_APB1RSTR_I2C2RST))
						#define MY_RCC_I2C2_RELEASE_RESET()   			(RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C2RST))

					#endif


					/* TIM14 */
					#define MY_RCC_TIM14_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_TIM14EN);
					#define MY_RCC_TIM14_CLK_DISABLE()  				(RCC->APB1ENR &= ~(RCC_APB1ENR_TIM14EN))

					#define MY_RCC_TIM14_IS_CLK_ENABLED()  				((RCC->APB1ENR & (RCC_APB1ENR_TIM14EN)) != RESET)
					#define MY_RCC_TIM14_IS_CLK_DISABLED() 				((RCC->APB1ENR & (RCC_APB1ENR_TIM14EN)) == RESET)

					#define MY_RCC_TIM14_FORCE_RESET()    			 	(RCC->APB1RSTR |= (RCC_APB1RSTR_TIM14RST))
					#define MY_RCC_TIM14_RELEASE_RESET()  			 	(RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM14RST))


					/* WWDG */
					#define MY_RCC_WWDG_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_WWDGEN);
					#define MY_RCC_WWDG_CLK_DISABLE()   				(RCC->APB1ENR &= ~(RCC_APB1ENR_WWDGEN))

					#define MY_RCC_WWDG_IS_CLK_ENABLED()   				((RCC->APB1ENR & (RCC_APB1ENR_WWDGEN)) != RESET)
					#define MY_RCC_WWDG_IS_CLK_DISABLED()  				((RCC->APB1ENR & (RCC_APB1ENR_WWDGEN)) == RESET)

					#define MY_RCC_WWDG_FORCE_RESET()     			 	(RCC->APB1RSTR |= (RCC_APB1RSTR_WWDGRST))
					#define MY_RCC_WWDG_RELEASE_RESET()   			 	(RCC->APB1RSTR &= ~(RCC_APB1RSTR_WWDGRST))


					/* I2C1 */
					#define MY_RCC_I2C1_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
					#define MY_RCC_I2C1_CLK_DISABLE()   				(RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN))

					#define MY_RCC_I2C1_IS_CLK_ENABLED()   				((RCC->APB1ENR & (RCC_APB1ENR_I2C1EN)) != RESET)
					#define MY_RCC_I2C1_IS_CLK_DISABLED()  				((RCC->APB1ENR & (RCC_APB1ENR_I2C1EN)) == RESET)

					#define MY_RCC_I2C1_FORCE_RESET()     			 	(RCC->APB1RSTR |= (RCC_APB1RSTR_I2C1RST))
					#define MY_RCC_I2C1_RELEASE_RESET()   			 	(RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST))


					/* PWR */
					#define MY_RCC_PWR_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_PWREN);
					#define MY_RCC_PWR_CLK_DISABLE()    				(RCC->APB1ENR &= ~(RCC_APB1ENR_PWREN))

					#define MY_RCC_PWR_IS_CLK_ENABLED()    				((RCC->APB1ENR & (RCC_APB1ENR_PWREN)) != RESET)
					#define MY_RCC_PWR_IS_CLK_DISABLED()   				((RCC->APB1ENR & (RCC_APB1ENR_PWREN)) == RESET)

					#define MY_RCC_PWR_FORCE_RESET()      			 	(RCC->APB1RSTR |= (RCC_APB1RSTR_PWRRST))
					#define MY_RCC_PWR_RELEASE_RESET()    			 	(RCC->APB1RSTR &= ~(RCC_APB1RSTR_PWRRST))


					#if defined(STM32F030x8) || \
						defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F070x6) || \
						defined(STM32F051x8) || \
						defined(STM32F058xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F070xB) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* USART2 */
						#define MY_RCC_USART2_CLK_ENABLE()   			MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_USART2EN);
						#define MY_RCC_USART2_CLK_DISABLE() 			(RCC->APB1ENR &= ~(RCC_APB1ENR_USART2EN))

						#define MY_RCC_USART2_IS_CLK_ENABLED()     		((RCC->APB1ENR & (RCC_APB1ENR_USART2EN)) != RESET)
						#define MY_RCC_USART2_IS_CLK_DISABLED()    		((RCC->APB1ENR & (RCC_APB1ENR_USART2EN)) == RESET)

						#define MY_RCC_USART2_FORCE_RESET()   			(RCC->APB1RSTR |= (RCC_APB1RSTR_USART2RST))
						#define MY_RCC_USART2_RELEASE_RESET() 			(RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART2RST))

					#endif


					#if defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F070xB) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* USART3 */
						#define MY_RCC_USART3_CLK_ENABLE()   			MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_USART3EN);
						#define MY_RCC_USART3_CLK_DISABLE() 			(RCC->APB1ENR &= ~(RCC_APB1ENR_USART3EN))

						#define MY_RCC_USART3_IS_CLK_ENABLED()     		((RCC->APB1ENR & (RCC_APB1ENR_USART3EN)) != RESET)
						#define MY_RCC_USART3_IS_CLK_DISABLED()    		((RCC->APB1ENR & (RCC_APB1ENR_USART3EN)) == RESET)

						#define MY_RCC_USART3_FORCE_RESET()   			(RCC->APB1RSTR |=  (RCC_APB1RSTR_USART3RST))
						#define MY_RCC_USART3_RELEASE_RESET() 			(RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART3RST))


						/* USART4 */
						#define MY_RCC_USART4_CLK_ENABLE()    			MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_USART4EN);
						#define MY_RCC_USART4_CLK_DISABLE() 			(RCC->APB1ENR &= ~(RCC_APB1ENR_USART4EN))

						#define MY_RCC_USART4_IS_CLK_ENABLED()     		((RCC->APB1ENR & (RCC_APB1ENR_USART4EN)) != RESET)
						#define MY_RCC_USART4_IS_CLK_DISABLED()    		((RCC->APB1ENR & (RCC_APB1ENR_USART4EN)) == RESET)

						#define MY_RCC_USART4_FORCE_RESET()   			(RCC->APB1RSTR |= (RCC_APB1RSTR_USART4RST))
						#define MY_RCC_USART4_RELEASE_RESET() 			(RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART4RST))

					#endif


					#if defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* USART5 */
						#define MY_RCC_USART5_CLK_ENABLE()  			MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_USART5EN);
						#define MY_RCC_USART5_CLK_DISABLE()      		(RCC->APB1ENR &= ~(RCC_APB1ENR_USART5EN))

						#define MY_RCC_USART5_IS_CLK_ENABLED()     		((RCC->APB1ENR & (RCC_APB1ENR_USART5EN)) != RESET)
						#define MY_RCC_USART5_IS_CLK_DISABLED()    		((RCC->APB1ENR & (RCC_APB1ENR_USART5EN)) == RESET)

						#define MY_RCC_USART5_FORCE_RESET()    			(RCC->APB1RSTR |= (RCC_APB1RSTR_USART5RST))
						#define MY_RCC_USART5_RELEASE_RESET()  			(RCC->APB1RSTR &= ~(RCC_APB1RSTR_USART5RST))

					#endif


					#if defined(STM32F030x8) || \
						defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F051x8) || \
						defined(STM32F058xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F070xB) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* SPI2 */
						#define MY_RCC_SPI2_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_SPI2EN);
						#define MY_RCC_SPI2_CLK_DISABLE()   			(RCC->APB1ENR &= ~(RCC_APB1ENR_SPI2EN))

						#define MY_RCC_SPI2_IS_CLK_ENABLED()       		((RCC->APB1ENR & (RCC_APB1ENR_SPI2EN)) != RESET)
						#define MY_RCC_SPI2_IS_CLK_DISABLED()      		((RCC->APB1ENR & (RCC_APB1ENR_SPI2EN)) == RESET)

						#define MY_RCC_SPI2_FORCE_RESET()     			(RCC->APB1RSTR |= (RCC_APB1RSTR_SPI2RST))
						#define MY_RCC_SPI2_RELEASE_RESET()   			(RCC->APB1RSTR &= ~(RCC_APB1RSTR_SPI2RST))

					#endif


					#if defined(STM32F051x8) || \
						defined(STM32F058xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx)

						/* DAC1 */
						#define MY_RCC_DAC1_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_DACEN);
						#define MY_RCC_DAC1_CLK_DISABLE()   				(RCC->APB1ENR &= ~(RCC_APB1ENR_DACEN))

						#define MY_RCC_DAC1_IS_CLK_ENABLED()       			((RCC->APB1ENR & (RCC_APB1ENR_DAC1EN)) != RESET)
						#define MY_RCC_DAC1_IS_CLK_DISABLED()      			((RCC->APB1ENR & (RCC_APB1ENR_DAC1EN)) == RESET)

						#define MY_RCC_DAC1_FORCE_RESET()     				(RCC->APB1RSTR |= (RCC_APB1RSTR_DACRST))
						#define MY_RCC_DAC1_RELEASE_RESET()   		 		(RCC->APB1RSTR &= ~(RCC_APB1RSTR_DACRST))

					#endif


					#if defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F051x8) || \
						defined(STM32F058xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx)

						/* HDMI CEC */
						#define MY_RCC_CEC_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_CECEN);
						#define MY_RCC_CEC_CLK_DISABLE()    				(RCC->APB1ENR &= ~(RCC_APB1ENR_CECEN))

						#define MY_RCC_CEC_IS_CLK_ENABLED()        			((RCC->APB1ENR & (RCC_APB1ENR_CECEN)) != RESET)
						#define MY_RCC_CEC_IS_CLK_DISABLED()       			((RCC->APB1ENR & (RCC_APB1ENR_CECEN)) == RESET)

						#define MY_RCC_CEC_FORCE_RESET()      				(RCC->APB1RSTR |= (RCC_APB1RSTR_CECRST))
						#define MY_RCC_CEC_RELEASE_RESET()    				(RCC->APB1RSTR &= ~(RCC_APB1RSTR_CECRST))

					#endif


					#if defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F070xB) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* TIM7 */
						#define MY_RCC_TIM7_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_TIM7EN);
						#define MY_RCC_TIM7_CLK_DISABLE()   				(RCC->APB1ENR &= ~(RCC_APB1ENR_TIM7EN))

						#define MY_RCC_TIM7_IS_CLK_ENABLED()       			((RCC->APB1ENR & (RCC_APB1ENR_TIM7EN)) != RESET)
						#define MY_RCC_TIM7_IS_CLK_DISABLED()      			((RCC->APB1ENR & (RCC_APB1ENR_TIM7EN)) == RESET)

						#define MY_RCC_TIM7_FORCE_RESET()     				(RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM7RST))
						#define MY_RCC_TIM7_RELEASE_RESET()   				(RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM7RST))

					#endif


					#if defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F070x6) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F070xB)

						/* USB */
						#define MY_RCC_USB_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_USBEN);
						#define MY_RCC_USB_CLK_DISABLE()    				(RCC->APB1ENR &= ~(RCC_APB1ENR_USBEN))

						#define MY_RCC_USB_IS_CLK_ENABLED()        			((RCC->APB1ENR & (RCC_APB1ENR_USBEN)) != RESET)
						#define MY_RCC_USB_IS_CLK_DISABLED()       			((RCC->APB1ENR & (RCC_APB1ENR_USBEN)) == RESET)

						#define MY_RCC_USB_FORCE_RESET()      		  		(RCC->APB1RSTR |= (RCC_APB1RSTR_USBRST))
						#define MY_RCC_USB_RELEASE_RESET()    				(RCC->APB1RSTR &= ~(RCC_APB1RSTR_USBRST))

					#endif


					#if defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F072xB) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx)

						/* CAN1 */
						#define MY_RCC_CAN1_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_CANEN);
						#define MY_RCC_CAN1_CLK_DISABLE()    				(RCC->APB1ENR &= ~(RCC_APB1ENR_CANEN))

						#define MY_RCC_CAN1_IS_CLK_ENABLED()       			((RCC->APB1ENR & (RCC_APB1ENR_CAN1EN)) != RESET)
						#define MY_RCC_CAN1_IS_CLK_DISABLED()      			((RCC->APB1ENR & (RCC_APB1ENR_CAN1EN)) == RESET)

						#define MY_RCC_CAN1_FORCE_RESET()       			(RCC->APB1RSTR |= (RCC_APB1RSTR_CANRST))
						#define MY_RCC_CAN1_RELEASE_RESET()     			(RCC->APB1RSTR &= ~(RCC_APB1RSTR_CANRST))

					#endif


					#if defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx)

						/* CRS */
						#define MY_RCC_CRS_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB1ENR, RCC_APB1ENR_CRSEN);
						#define MY_RCC_CRS_CLK_DISABLE()    				(RCC->APB1ENR &= ~(RCC_APB1ENR_CRSEN))

						#define MY_RCC_CRS_IS_CLK_ENABLED()        			((RCC->APB1ENR & (RCC_APB1ENR_CRSEN)) != RESET)
						#define MY_RCC_CRS_IS_CLK_DISABLED()       			((RCC->APB1ENR & (RCC_APB1ENR_CRSEN)) == RESET)

						#define MY_RCC_CRS_FORCE_RESET()      		  		(RCC->APB1RSTR |= (RCC_APB1RSTR_CRSRST))
						#define MY_RCC_CRS_RELEASE_RESET()    				(RCC->APB1RSTR &= ~(RCC_APB1RSTR_CRSRST))

					#endif

				/**
				 * @} APB1
				 */


				/**
				 * @brief 	Быстрые макросы для включения/выключения и проверки тактирования на шине APB2
				 * @note   	После сброса тактирование периферии (используемая в т.ч. для чтения/записи регистров) отключается,
				 *			и перед использованием периферии тактирование должно включаться вручную.
				 * @{
				 */
					#define MY_RCC_APB2_FORCE_RESET()     					(RCC->APB2RSTR = 0xFFFFFFFFU)
					#define MY_RCC_APB2_RELEASE_RESET()   					(RCC->APB2RSTR = 0x00000000U)

					/* SYSCFG */
					#define MY_RCC_SYSCFG_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
					#define MY_RCC_SYSCFG_CLK_DISABLE() 				 	(RCC->APB2ENR &= ~(RCC_APB2ENR_SYSCFGEN))

					#define MY_RCC_SYSCFG_IS_CLK_ENABLED()     		 		((RCC->APB2ENR & (RCC_APB2ENR_SYSCFGEN)) != RESET)
					#define MY_RCC_SYSCFG_IS_CLK_DISABLED()    		 		((RCC->APB2ENR & (RCC_APB2ENR_SYSCFGEN)) == RESET)

					#define MY_RCC_SYSCFG_FORCE_RESET()   		   			(RCC->APB2RSTR |= (RCC_APB2RSTR_SYSCFGRST))
					#define MY_RCC_SYSCFG_RELEASE_RESET() 		   			(RCC->APB2RSTR &= ~(RCC_APB2RSTR_SYSCFGRST))


					/* ADC1 */
					#define MY_RCC_ADC1_CLK_ENABLE()   						MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_ADC1EN);
					#define MY_RCC_ADC1_CLK_DISABLE()   				 	(RCC->APB2ENR &= ~(RCC_APB2ENR_ADC1EN))

					#define MY_RCC_ADC1_IS_CLK_ENABLED()        	 		((RCC->APB2ENR & (RCC_APB2ENR_ADC1EN)) != RESET)
					#define MY_RCC_ADC1_IS_CLK_DISABLED()       	 		((RCC->APB2ENR & (RCC_APB2ENR_ADC1EN)) == RESET)

					#define MY_RCC_ADC1_FORCE_RESET()     			 	  	(RCC->APB2RSTR |= (RCC_APB2RSTR_ADC1RST))
					#define MY_RCC_ADC1_RELEASE_RESET()   			 	  	(RCC->APB2RSTR &= ~(RCC_APB2RSTR_ADC1RST))


					/* TIM1 */
					#define MY_RCC_TIM1_CLK_ENABLE()  						MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_TIM1EN);
					#define MY_RCC_TIM1_CLK_DISABLE()   				 	(RCC->APB2ENR &= ~(RCC_APB2ENR_TIM1EN))

					#define MY_RCC_TIM1_IS_CLK_ENABLED()        	 		((RCC->APB2ENR & (RCC_APB2ENR_TIM1EN)) != RESET)
					#define MY_RCC_TIM1_IS_CLK_DISABLED()       	 		((RCC->APB2ENR & (RCC_APB2ENR_TIM1EN)) == RESET)

					#define MY_RCC_TIM1_FORCE_RESET()     			 		(RCC->APB2RSTR |= (RCC_APB2RSTR_TIM1RST))
					#define MY_RCC_TIM1_RELEASE_RESET()   			 		(RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM1RST))


					/* SPI1 */
					#define MY_RCC_SPI1_CLK_ENABLE()   						MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_SPI1EN);
					#define MY_RCC_SPI1_CLK_DISABLE()   				 	(RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))

					#define MY_RCC_SPI1_IS_CLK_ENABLED()        	 		((RCC->APB2ENR & (RCC_APB2ENR_SPI1EN)) != RESET)
					#define MY_RCC_SPI1_IS_CLK_DISABLED()       	 		((RCC->APB2ENR & (RCC_APB2ENR_SPI1EN)) == RESET)

					#define MY_RCC_SPI1_FORCE_RESET()     			 	  	(RCC->APB2RSTR |= (RCC_APB2RSTR_SPI1RST))
					#define MY_RCC_SPI1_RELEASE_RESET()   			 		(RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI1RST))


					#if defined(STM32F030x8) || \
						defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F070x6) || \
						defined(STM32F051x8) || \
						defined(STM32F058xx) || \
						defined(STM32F071xB) || \
						defined(STM32F072xB) || \
						defined(STM32F078xx) || \
						defined(STM32F070xB) || \
						defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* TIM15 */
						#define MY_RCC_TIM15_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_TIM15EN);
						#define MY_RCC_TIM15_CLK_DISABLE()   				(RCC->APB2ENR &= ~(RCC_APB2ENR_TIM15EN))

						#define MY_RCC_TIM15_IS_CLK_ENABLED()      			((RCC->APB2ENR & (RCC_APB2ENR_TIM15EN)) != RESET)
						#define MY_RCC_TIM15_IS_CLK_DISABLED()     			((RCC->APB2ENR & (RCC_APB2ENR_TIM15EN)) == RESET)

						#define MY_RCC_TIM15_FORCE_RESET()    				(RCC->APB2RSTR |= (RCC_APB2RSTR_TIM15RST))
						#define MY_RCC_TIM15_RELEASE_RESET()  				(RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM15RST))

					#endif


					/* TIM16 */
					#define MY_RCC_TIM16_CLK_ENABLE()  						MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_TIM16EN);
					#define MY_RCC_TIM16_CLK_DISABLE()  				 	(RCC->APB2ENR &= ~(RCC_APB2ENR_TIM16EN))

					#define MY_RCC_TIM16_IS_CLK_ENABLED()       	 		((RCC->APB2ENR & (RCC_APB2ENR_TIM16EN)) != RESET)
					#define MY_RCC_TIM16_IS_CLK_DISABLED()      	 		((RCC->APB2ENR & (RCC_APB2ENR_TIM16EN)) == RESET)

					#define MY_RCC_TIM16_FORCE_RESET()    			 		(RCC->APB2RSTR |= (RCC_APB2RSTR_TIM16RST))
					#define MY_RCC_TIM16_RELEASE_RESET()  			 		(RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM16RST))


					/* TIM17 */
					#define MY_RCC_TIM17_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_TIM17EN);
					#define MY_RCC_TIM17_CLK_DISABLE()  				 	(RCC->APB2ENR &= ~(RCC_APB2ENR_TIM17EN))

					#define MY_RCC_TIM17_IS_CLK_ENABLED()      		 		((RCC->APB2ENR & (RCC_APB2ENR_TIM17EN)) != RESET)
					#define MY_RCC_TIM17_IS_CLK_DISABLED()     		 		((RCC->APB2ENR & (RCC_APB2ENR_TIM17EN)) == RESET)

					#define MY_RCC_TIM17_FORCE_RESET()    			 		(RCC->APB2RSTR |= (RCC_APB2RSTR_TIM17RST))
					#define MY_RCC_TIM17_RELEASE_RESET()  			 		(RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM17RST))


					/* USART1 */
					#define MY_RCC_USART1_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_USART1EN);
					#define MY_RCC_USART1_CLK_DISABLE() 				 	(RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN))

					#define MY_RCC_USART1_IS_CLK_ENABLED()      	 		((RCC->APB2ENR & (RCC_APB2ENR_USART1EN)) != RESET)
					#define MY_RCC_USART1_IS_CLK_DISABLED()     	 		((RCC->APB2ENR & (RCC_APB2ENR_USART1EN)) == RESET)

					#define MY_RCC_USART1_FORCE_RESET()   		 	 	  	(RCC->APB2RSTR |= (RCC_APB2RSTR_USART1RST))
					#define MY_RCC_USART1_RELEASE_RESET() 			 		(RCC->APB2RSTR &= ~(RCC_APB2RSTR_USART1RST))


					#if defined(STM32F091xC) || \
						defined(STM32F098xx) || \
						defined(STM32F030xC)

						/* USART6 */
						#define MY_RCC_USART6_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_USART6EN);
						#define MY_RCC_USART6_CLK_DISABLE()      			(RCC->APB2ENR &= ~(RCC_APB2ENR_USART6EN))

						#define MY_RCC_USART6_IS_CLK_ENABLED()     			((RCC->APB2ENR & (RCC_APB2ENR_USART6EN)) != RESET)
						#define MY_RCC_USART6_IS_CLK_DISABLED()    			((RCC->APB2ENR & (RCC_APB2ENR_USART6EN)) == RESET)

						#define MY_RCC_USART6_FORCE_RESET()    				(RCC->APB2RSTR |= (RCC_APB2RSTR_USART6RST))
						#define MY_RCC_USART6_RELEASE_RESET()  				(RCC->APB2RSTR &= ~(RCC_APB2RSTR_USART6RST))

					#endif


					#if defined(STM32F091xC) || \
						defined(STM32F098xx)

						/* USART7 */
						#define MY_RCC_USART7_CLK_ENABLE()   				MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_USART7EN);
						#define MY_RCC_USART7_CLK_DISABLE()  			 	(RCC->APB2ENR &= ~(RCC_APB2ENR_USART7EN))

						#define MY_RCC_USART7_IS_CLK_ENABLED()     			((RCC->APB2ENR & (RCC_APB2ENR_USART7EN)) != RESET)
						#define MY_RCC_USART7_IS_CLK_DISABLED()    			((RCC->APB2ENR & (RCC_APB2ENR_USART7EN)) == RESET)

						#define MY_RCC_USART7_FORCE_RESET()    				(RCC->APB2RSTR |= (RCC_APB2RSTR_USART7RST))
						#define MY_RCC_USART7_RELEASE_RESET()  				(RCC->APB2RSTR &= ~(RCC_APB2RSTR_USART7RST))


						/* USART8 */
						#define MY_RCC_USART8_CLK_ENABLE()  				MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_USART8EN);
						#define MY_RCC_USART8_CLK_DISABLE()      			(RCC->APB2ENR &= ~(RCC_APB2ENR_USART8EN))

						#define MY_RCC_USART8_IS_CLK_ENABLED()     			((RCC->APB2ENR & (RCC_APB2ENR_USART8EN)) != RESET)
						#define MY_RCC_USART8_IS_CLK_DISABLED()    			((RCC->APB2ENR & (RCC_APB2ENR_USART8EN)) == RESET)

						#define MY_RCC_USART8_FORCE_RESET()    				(RCC->APB2RSTR |= (RCC_APB2RSTR_USART8RST))
						#define MY_RCC_USART8_RELEASE_RESET()  		  		(RCC->APB2RSTR &= ~(RCC_APB2RSTR_USART8RST))

					#endif


					/* DBGMCU */
					#define MY_RCC_DBGMCU_CLK_ENABLE()   					MY_UTILS_SetBitWithRead(&RCC->APB2ENR, RCC_APB2ENR_DBGMCUEN);
					#define MY_RCC_DBGMCU_CLK_DISABLE() 				 	(RCC->APB2ENR &= ~(RCC_APB2ENR_DBGMCUEN))

					#define MY_RCC_DBGMCU_IS_CLK_ENABLED()      	 		((RCC->APB2ENR & (RCC_APB2ENR_DBGMCUEN)) != RESET)
					#define MY_RCC_DBGMCU_IS_CLK_DISABLED()     	 		((RCC->APB2ENR & (RCC_APB2ENR_DBGMCUEN)) == RESET)

					#define MY_RCC_DBGMCU_FORCE_RESET()   			 		(RCC->APB2RSTR |= (RCC_APB2RSTR_DBGMCURST))
					#define MY_RCC_DBGMCU_RELEASE_RESET() 			 		(RCC->APB2RSTR &= ~(RCC_APB2RSTR_DBGMCURST))

				/**
				 * @} APB2
				 */


				/**
				 * @brief 	Макросы для включения или отключения внутреннего высокоскоростного генератора (HSI).
				 * @note   	HSI остановлен при входе в режимы STOP и STANDBY.
				 * @note	HSI не может быть остановлен если используется в качестве источника системного тактирования.
				 * 			В случае когда нужно остановить HSI - нужно переключиться на другой источник тактирования - HSE или PLL.
				 * @note	После включения HSI необходимо обязательно дожидаться установки флага HSIRDY.
				 * @note	После отключения HSI, флаг HSIRDY снимается только через 6 тактов генератора HSI.
				 */
				#define RCC_HSI_OFF                             ((uint32_t)0x00000000U)         /*!< Отключение HSI осциллятора */
				#define RCC_HSI_ON                              RCC_CR_HSION           			/*!< Включение HSI осциллятора */

				#define MY_RCC_HSI_ENABLE()  		 			SET_BIT(RCC->CR, RCC_CR_HSION)
				#define MY_RCC_HSI_DISABLE() 					CLEAR_BIT(RCC->CR, RCC_CR_HSION)


				/**
				 * @defgroup Макросы для калибровки частоты внутреннего высокоскоростного генератора (HSI).
				 * @note   	Калибровка используется для компенсации изменений напряжения и температуры, которые влияют на частоту внутреннего HSI RC.
				 * @note			_HSICALIBRATIONVALUE_: указывает значение обрезки калибровки. (по умолчанию RCC_HSICALIBRATION_DEFAULT)
				 */
				#define RCC_HSICALIBRATION_DEFAULT              ((uint8_t)0x00000010U)         		/*!< Значение по умолчанию для калибровки HSI (calibration trimming value) */

				#define MY_RCC_HSI_CALIBRATIONVALUE_ADJUST(__RCC_HSICALIBRATION_VALUE__) \
																							MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, (uint32_t)(__RCC_HSICALIBRATION_VALUE__) << RCC_CR_HSITRIM_BITNUMBER)


				/**
				 * @brief  Макрос для включения или выключения тактирования HSI48
				 * @note   Тактирование HSI48 останавливается аппаратно если происходит переход в режим STOP или STANDBY.
				 * @note   HSI48 не может быть остановлен если используется в качестве источника системного тактирования.
				 *		   В этом случае необходимо выбрать другой источник тактирования и только потом останавливать HSI48.
				 * @note   После включения HSI48 необходимо дожидаться флага HSI48RDY который указывает что HSI48 стабилен и может использоваться
				 *		   в качестве системного тактирования. Это не важно если используется функция инициализации MY_RCC_OscConfig().
				 * @note   Когда HSI48 остановлен, HSI48RDY флаг снимается только через 6 HSI48 циклов тактирования.
				 */
				#if defined(STM32F042x6) || \
					defined(STM32F048xx) || \
					defined(STM32F071xB) || \
					defined(STM32F072xB) || \
					defined(STM32F078xx) || \
					defined(STM32F091xC) || \
					defined(STM32F098xx)

					/** @brief  Макросы для установки и запроса состояния HSI48
					 * @retval  Может быть двух значений:
					 *          @arg RCC_HSI48_ON:  HSI48 включен
					 *          @arg RCC_HSI48_OFF: HSI48 выключен
					 */
					#define RCC_HSI48_OFF                 		((uint8_t)0x00)
					#define RCC_HSI48_ON                  		((uint8_t)0x01

					#define MY_RCC_HSI48_ENABLE()  				SET_BIT(RCC->CR2, RCC_CR2_HSI48ON)
					#define MY_RCC_HSI48_DISABLE() 				CLEAR_BIT(RCC->CR2, RCC_CR2_HSI48ON)

					#define MY_RCC_GET_HSI48_STATE()          	(((uint32_t)(READ_BIT(RCC->CFGR3, RCC_CR2_HSI48ON)) != RESET) ?  RCC_HSI48_ON : RCC_HSI48_OFF)

				#else

					/* HSI48 Настройка */
					#define RCC_HSI48_OFF                    	((uint8_t)0x00)

					#define MY_RCC_GET_HSI48_STATE()   			RCC_HSI_OFF

				#endif


				/**
				 * @brief  Макрос для настройки внешнего высокоскоростного генератора (HSE).
				 * @note   Переход от HSE Bypass к HSE On/HSE On к HSE Bypass не поддерживается в этом макросе.
				 *         Пользователь должен сначала перейти к HSE Off, а потом HSE On или HSE Bypass.
				 * @note   После включения HSE (RCC_HSE_ON или RCC_HSE_Bypass), необходимо программно дождаться
				 *         установки флага HSERDY, указывающего, что тактирование стабильно и может использоваться для PLL или/и системного тактирования.
				 * @note   Состояние HSE не может быть изменено пока используется в качестве системного тактирования.
				 *         В этом случае, вы должны переключиться на другой источник тактирование если хотите изменить его состояние (напр.отключить его).
				 * @note   Тактирование HSE останавливается аппаратно когда МК входит в режим STOP и STANDBY.
				 * @note   Эта функция сбрасывает бит Clock Security System ON, если CSS была включена ранее,
				 *         нужно включить её вручную после вызова этой функции.
				 * @param  STATE - устанавливает новое состояние HSE.
				 *         Этот параметр может быть следующих значения:
				 *            @arg @ref RCC_HSE_OFF    - выключает HSE осциллятор, HSERDY флаг снимается после 6 циклов осциллятора.
				 *            @arg @ref RCC_HSE_ON     - включает HSE осциллятор.
				 *            @arg @ref RCC_HSE_BYPASS - включает обход HSE осциллятора при использовании внешнего генератора
				 */

				#define RCC_HSE_OFF                             (0x00000000U)       /*!< HSE отключен */
				#define RCC_HSE_ON                              (0x00000001U)       /*!< HSE включен */
				#define RCC_HSE_BYPASS                          (0x00000005U)       /*!< Внешний источник тактирования для HSE */

				#define MY_RCC_HSE_CONFIG(__STATE__)            do { \
																	if ((__STATE__) == RCC_HSE_ON)                        \
																	{                                                     \
																		SET_BIT(RCC->CR, RCC_CR_HSEON);                   \
																	}                                                     \
																	else if ((__STATE__) == RCC_HSE_OFF)                  \
																	{                                                     \
																		CLEAR_BIT(RCC->CR, RCC_CR_HSEON);                 \
																		CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);                \
																	}                                                     \
																	else if ((__STATE__) == RCC_HSE_BYPASS)               \
																	{                                                     \
																		SET_BIT(RCC->CR, RCC_CR_HSEBYP);                  \
																		SET_BIT(RCC->CR, RCC_CR_HSEON);                   \
																	}                                                     \
																	else                                                  \
																	{                                                     \
																		CLEAR_BIT(RCC->CR, RCC_CR_HSEON);                 \
																		CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);                \
																	}                                                     \
																} \
																while(0U)


				/** @brief  Макрос для включения/выключения PLL
				 * @note   	После включения основного PLL прикладная программа должна дождаться установки флага PLLRDY,
				 *			указывающего, что тактирование PLL стабильно и может использоваться в качестве источника системного тактирования.
				 * @note   Основной PLL отключается аппаратно при входе в режимы STOP и STANDBY.
				 */

				#define RCC_PLL_NONE                         		(0x00000000U)  				/*!< PLL не задействован */
				#define RCC_PLL_OFF                          		(0x00000001U)  				/*!< PLL отключен */
				#define RCC_PLL_ON                           		(0x00000002U)  				/*!< PLL включен */

				#define MY_RCC_PLL_ENABLE() 						SET_BIT(RCC->CR, RCC_CR_PLLON)
				#define MY_RCC_PLL_DISABLE() 						CLEAR_BIT(RCC->CR, RCC_CR_PLLON)


				/** @brief  Макрос для конфигурирования источника тактирования для PLL, установки уможителей и делителей.
				 * @note   Эта функция должна выполняться только когда PLL отключен.
				 * @param  PLLSOURCE - устанавливает значение источника тактирования для PLL.
				 *         Этот параметр может принимать следующие значение:
				 *            @arg @ref RCC_PLLSOURCE_HSI - HSI
				 *            @arg @ref RCC_PLLSOURCE_HSE - HSE
				 * @param  PLLMUL - устанавливает умножитель для PLL VCO
				 *         Этот параметр может принимать следующие значение:
				 *          	@arg @ref RCC_PLL_MUL2 и RCC_PLL_MUL16.
				 * @param  PREDIV - устанавливает предделитель для PLL VCO
				 *         Этот параметр может принимать следующие значение
				 * 					@arg @ref RCC_PREDIV_DIV1 и RCC_PREDIV_DIV16
				 *
				 */
				#define RCC_PLLSOURCE_HSE           				RCC_CFGR_PLLSRC_HSE_PREDIV /*!< Тактовая частота HSE */


				/* Если используется HSI */
				#if defined(STM32F042x6) || \
					defined(STM32F048xx) || \
					defined(STM32F071xB) || \
					defined(STM32F072xB) || \
					defined(STM32F078xx) || \
					defined(STM32F091xC) || \
					defined(STM32F098xx)

					#define RCC_PLLSOURCE_HSI                		RCC_CFGR_PLLSRC_HSI_PREDIV
					#define RCC_PLLSOURCE_HSI48              		RCC_CFGR_PLLSRC_HSI48_PREDIV

				#else

					#if defined(STM32F070xB) || \
						defined(STM32F070x6) || \
						defined(STM32F030xC)

						#define RCC_PLLSOURCE_HSI              		RCC_CFGR_PLLSRC_HSI_PREDIV

					#else

						#define RCC_PLLSOURCE_HSI              		RCC_CFGR_PLLSRC_HSI_DIV2

					#endif

				#endif


				#define RCC_PLL_PREDIV_DIV1                  		RCC_CFGR2_PREDIV_DIV1
				#define RCC_PLL_PREDIV_DIV2                  		RCC_CFGR2_PREDIV_DIV2
				#define RCC_PLL_PREDIV_DIV3                 		RCC_CFGR2_PREDIV_DIV3
				#define RCC_PLL_PREDIV_DIV4                  		RCC_CFGR2_PREDIV_DIV4
				#define RCC_PLL_PREDIV_DIV5                  		RCC_CFGR2_PREDIV_DIV5
				#define RCC_PLL_PREDIV_DIV6                  		RCC_CFGR2_PREDIV_DIV6
				#define RCC_PLL_PREDIV_DIV7                  		RCC_CFGR2_PREDIV_DIV7
				#define RCC_PLL_PREDIV_DIV8                  		RCC_CFGR2_PREDIV_DIV8
				#define RCC_PLL_PREDIV_DIV9                  		RCC_CFGR2_PREDIV_DIV9
				#define RCC_PLL_PREDIV_DIV10                 		RCC_CFGR2_PREDIV_DIV10
				#define RCC_PLL_PREDIV_DIV11                 		RCC_CFGR2_PREDIV_DIV11
				#define RCC_PLL_PREDIV_DIV12                 		RCC_CFGR2_PREDIV_DIV12
				#define RCC_PLL_PREDIV_DIV13                 		RCC_CFGR2_PREDIV_DIV13
				#define RCC_PLL_PREDIV_DIV14                 		RCC_CFGR2_PREDIV_DIV14
				#define RCC_PLL_PREDIV_DIV15                 		RCC_CFGR2_PREDIV_DIV15
				#define RCC_PLL_PREDIV_DIV16                 		RCC_CFGR2_PREDIV_DIV16


				#define RCC_PLL_MUL2                        	 	RCC_CFGR_PLLMUL2
				#define RCC_PLL_MUL3                         		RCC_CFGR_PLLMUL3
				#define RCC_PLL_MUL4                         		RCC_CFGR_PLLMUL4
				#define RCC_PLL_MUL5                         		RCC_CFGR_PLLMUL5
				#define RCC_PLL_MUL6                        		RCC_CFGR_PLLMUL6
				#define RCC_PLL_MUL7                         		RCC_CFGR_PLLMUL7
				#define RCC_PLL_MUL8                         		RCC_CFGR_PLLMUL8
				#define RCC_PLL_MUL9                         		RCC_CFGR_PLLMUL9
				#define RCC_PLL_MUL10                        		RCC_CFGR_PLLMUL10
				#define RCC_PLL_MUL11                        		RCC_CFGR_PLLMUL11
				#define RCC_PLL_MUL12                        		RCC_CFGR_PLLMUL12
				#define RCC_PLL_MUL13                        		RCC_CFGR_PLLMUL13
				#define RCC_PLL_MUL14                        		RCC_CFGR_PLLMUL14
				#define RCC_PLL_MUL15                        		RCC_CFGR_PLLMUL15
				#define RCC_PLL_MUL16                        		RCC_CFGR_PLLMUL16


				#define MY_RCC_PLL_CONFIG(__RCC_PLLSOURCE__, __RCC_PLL_PREDIV__, __RCC_PLL_MUL__) \
																									do { \
																										MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PREDIV, (__RCC_PLL_PREDIV__)); \
																										MODIFY_REG(RCC->CFGR,  RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC, (uint32_t)((__RCC_PLL_MUL__)|(__RCC_PLLSOURCE__))); \
																									} \
																									while(0U)

				#define MY_RCC_GET_PLL_OSCSOURCE() 					((uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_PLLSRC)))


				/**
				 * @brief  Отдельный макрос для настройки предделителя для HSE при использовании PLL - PLL PREDIV HSE
				 * @note   Коэффициент предделителя не может измениться если PLL используется как источник системного тактирования.
				 *         В этом случае нужно выбрать другой источник системного тактирования, отключить PLL и
				 *         тогда можете изменить предделитель HSE.
				 * @param  HSE_PREDIV_VALUE: устанавливаемое значение в качестве предделителя HSE.
				 *         Это параметр может быть установлен следующих значений между RCC_PLL_PREDIV_DIV1 и RCC_PLL_PREDIV_DIV16
				 */
				#define MY_RCC_HSE_PREDIV_CONFIG(__HSE_PREDIV_VALUE__)  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PREDIV, (uint32_t)(__HSE_PREDIV_VALUE__))


				/** @brief  Макрос для получения источника тактирования используемого для системного тактирования.
				 * @retval  Источник тактирования SYSCLK. Возвращаемое значиение может быть одно из перечисленных:
				 *			@arg @ref RCC_SYSCLKSOURCE_STATUS_MSI    - MSI
				 *          @arg @ref RCC_SYSCLKSOURCE_STATUS_HSI    - HSI
				 *          @arg @ref RCC_SYSCLKSOURCE_STATUS_HSE    - HSE
				 *          @arg @ref RCC_SYSCLKSOURCE_STATUS_PLLCLK - PLL
				 */
				#define RCC_SYSCLKSOURCE_STATUS_HSI             		RCC_CFGR_SWS_HSI
				#define RCC_SYSCLKSOURCE_STATUS_HSE             		RCC_CFGR_SWS_HSE
				#define RCC_SYSCLKSOURCE_STATUS_PLLCLK          		RCC_CFGR_SWS_PLL


				#if defined(STM32F042x6) || \
					defined(STM32F048xx) || \
					defined(STM32F071xB) || \
					defined(STM32F072xB) || \
					defined(STM32F078xx) || \
					defined(STM32F091xC) || \
					defined(STM32F098xx)

					#define RCC_SYSCLKSOURCE_STATUS_HSI48   			RCC_CFGR_SWS_HSI48

				#endif


				#define MY_RCC_SYSCLK_CONFIG(__SYSCLKSOURCE__)  		MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, (__SYSCLKSOURCE__))
				#define MY_RCC_GET_SYSCLK_SOURCE() 				      	((uint32_t)(READ_BIT(RCC->CFGR, RCC_CFGR_SWS)))


				/** @brief  Макроысы для включения или отключения внутреннего низкоскоростного генератора (LSI).
				 * @note   После включения LSI, необходимо обязательно дожидаться установки флага LSIRDY и его можно использовать для IWDG и/или RTC.
				 * @note   LSI нельзя отключить, если работает IWDG.
				 * @note   Когда LSI остановлен, флаг LSIRDY снимается после 6 тактов генератора LSI.
				 */
				#define RCC_LSI_OFF                             		(0x00000000U)   	/*!< Выключение LSI */
				#define RCC_LSI_ON                              		RCC_CSR_LSION   	/*!< Включение LSI */

				#define MY_RCC_LSI_ENABLE() 							SET_BIT(RCC->CSR, RCC_CSR_LSION)
				#define MY_RCC_LSI_DISABLE() 							CLEAR_BIT(RCC->CSR, RCC_CSR_LSION)


				/**
				 * @brief  Макрос для настройке внешнего низкоскоростного осциллятора (LSE).
				 * @note   Переход с LSE Bypass к LSE On и от LSE On к LSE Bypass не поддерживается в этом макросе.
				 * @note   Поскольку LSE находится в Backup domain и доступ к записи запрещен после сброса данного домена
				 *         вам нужно включить возможность записи используя функцию @ref MY_PWR_EnableBkUpAccess() перед конфигурированием LSE
				 *         (выполняется один раз после сброса).
				 * @note   После включения LSE (RCC_LSE_ON или RCC_LSE_BYPASS), пользовательское приложение
				 *         должно дождаться флаг LSERDY который показывает что LSE тактирование стабильно и может быть использовано
				 *         для тактирования RTC.
				 * @param  STATE - устанавливает новое состояние LSE.
				 *         Этот параметр может принимать следующие значения:
				 *            @arg @ref RCC_LSE_OFF    - отключаем LSE осциллятор, флаг LSERDY снимается после 6 тактов осциллятора LSE.
				 *            @arg @ref RCC_LSE_ON     - включаем LSE осциллятор.
				 *            @arg @ref RCC_LSE_BYPASS - LSE осциллятор использует внешний источник тактирования.
				 */

				#define RCC_LSE_OFF                      				(0x00000000U)       /*!< Отключение LSE */
				#define RCC_LSE_ON                       				(0x00000001U)       /*!< Включение LSE*/
				#define RCC_LSE_BYPASS                   				(0x00000005U)       /*!< Внешний источник тактирования для LSE */

				#define MY_RCC_LSE_CONFIG(__STATE__)            do {                                                  \
																	if ((__STATE__) == RCC_LSE_ON)                    \
																	{                                                 \
																		SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);           \
																	}                                                 \
																	else if ((__STATE__) == RCC_LSE_OFF)              \
																	{                                                 \
																		CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);         \
																		CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);        \
																	}                                                 \
																	else if ((__STATE__) == RCC_LSE_BYPASS)           \
																	{                                                 \
																		SET_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);          \
																		SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);           \
																	}                                                 \
																	else                                              \
																	{                                                 \
																		CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);         \
																		CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);        \
																	}                                                 \
																}                                                     \
																while(0U)


				/** @brief  Макрос для включение или отключение внутреннего 14Mhz High Speed осциллятора (HSI14).
				 * @note   HSI14 аппаратно останавливается при переходе в режиме STOP и STANDBY.
				 * @note   HSI14 не может быть остановлен, если используется в качестве источника тактирования.
				 *         В этом случае, необходимо выбрать другой источник тактирования и тогда остановить HSI14.
				 * @note   После включения HSI14 с MY_RCC_HSI14_ENABLE(), в пользовательской программе необходимо
				 *         дождаться установки флага HSI14RDY, что в свою очередь сигнализирует о том что частота стабильная и
				 *         его можно использовать в качестве источника тактирования (напр. ADC).
				 *					В вызове данной функции нет необходимости если используется MY_RCC_OscConfig().
				 * @note   Когда HSI14 остановлен, флаг HSI14RDY снимается после 6 тактов HSI14 осциллятора.
				 */
				#define RCC_HSI14_OFF                        	(0x00000000U)
				#define RCC_HSI14_ON                         	RCC_CR2_HSI14ON

				#define MY_RCC_HSI14_ENABLE()  	 			  	SET_BIT(RCC->CR2,   RCC_CR2_HSI14ON)
				#define MY_RCC_HSI14_DISABLE() 	 			  	CLEAR_BIT(RCC->CR2, RCC_CR2_HSI14ON)


				/** @brief  Макрос для включения или отключения HSI14 при использовании ADC.
				 */
				#define RCC_HSI14_ADC_CONTROL                	(~RCC_CR2_HSI14DIS)

				#define MY_RCC_HSI14ADC_ENABLE()  				CLEAR_BIT(RCC->CR2, RCC_CR2_HSI14DIS)
				#define MY_RCC_HSI14ADC_DISABLE() 				SET_BIT(RCC->CR2,   RCC_CR2_HSI14DIS)


				/** @brief  Макрос для калибровки HSI14.
				 * @note   Калибровка используется для компенсации изменений напряжения и температуры, которые влияют на частоту внутреннего HSI14 RC.
				 * @param  HSI14CalibrationValue: указывает значение для калибровки, (по умолчанию, RCC_HSI14CALIBRATION_DEFAULT).
				 *         Этот параметр должен быть числом от 0 до 0x1F.
				 */
				#define RCC_HSI14CALIBRATION_DEFAULT         	(0x10U)   /*!< Значение по умолчанию для калибровки HSI14 (calibration trimming value) */

				#define MY_RCC_HSI14_CALIBRATIONVALUE_ADJUST(__HSI14_CALIBRATION_VALUE__) \
																MODIFY_REG(RCC->CR2, RCC_CR2_HSI14TRIM, (uint32_t)(__HSI14_CALIBRATION_VALUE__) << RCC_CR2_HSI14TRIM_BITNUMBER)


				/** @brief  Макрос для конфигурации источника тактирования USART1 (USART1CLK).
				 * @param  USART1CLKSource: указывает источник тактирования USART1.
				 *         Этот параметр может быть одним из:
				 *            @arg RCC_USART1CLKSOURCE_PCLK1  - PCLK1
				 *            @arg RCC_USART1CLKSOURCE_HSI    - HSI
				 *            @arg RCC_USART1CLKSOURCE_SYSCLK - System Clock
				 *            @arg RCC_USART1CLKSOURCE_LSE    - LSE
				 */
				#define RCC_USART1CLKSOURCE_PCLK1            	RCC_CFGR3_USART1SW_PCLK
				#define RCC_USART1CLKSOURCE_SYSCLK           	RCC_CFGR3_USART1SW_SYSCLK
				#define RCC_USART1CLKSOURCE_LSE              	RCC_CFGR3_USART1SW_LSE
				#define RCC_USART1CLKSOURCE_HSI              	RCC_CFGR3_USART1SW_HSI

				#define MY_RCC_USART1_CONFIG(__USART1_CLKSOURCE__) \
																MODIFY_REG(RCC->CFGR3, RCC_CFGR3_USART1SW, (uint32_t)(__USART1_CLKSOURCE__))

				#define MY_RCC_GET_USART1_SOURCE() 				((uint32_t)(READ_BIT(RCC->CFGR3, RCC_CFGR3_USART1SW)))


				/** @brief  Макрос для конфигурации источника тактирования USART2 (USART2CLK).
				 * @param  USART1CLKSource: указывает источник тактирования USART2.
				 *         Этот параметр может быть одним из:
				 *            @arg RCC_USART2CLKSOURCE_PCLK1  - PCLK1
				 *            @arg RCC_USART2CLKSOURCE_HSI    - HSI
				 *            @arg RCC_USART2CLKSOURCE_SYSCLK - System Clock
				 *            @arg RCC_USART2CLKSOURCE_LSE    - LSE
				 */
				#if defined(STM32F071xB) || \
					defined(STM32F072xB) || \
					defined(STM32F078xx) || \
					defined(STM32F091xC) || \
					defined(STM32F098xx)

					#define RCC_USART2CLKSOURCE_PCLK1       	RCC_CFGR3_USART2SW_PCLK
					#define RCC_USART2CLKSOURCE_SYSCLK       	RCC_CFGR3_USART2SW_SYSCLK
					#define RCC_USART2CLKSOURCE_LSE          	RCC_CFGR3_USART2SW_LSE
					#define RCC_USART2CLKSOURCE_HSI          	RCC_CFGR3_USART2SW_HSI

					#define MY_RCC_USART2_CONFIG(__USART2_CLKSOURCE__)   \
																MODIFY_REG(RCC->CFGR3, RCC_CFGR3_USART2SW, (uint32_t)(__USART2_CLKSOURCE__))

					#define MY_RCC_GET_USART2_SOURCE() 			((uint32_t)(READ_BIT(RCC->CFGR3, RCC_CFGR3_USART2SW)))

				#endif


				/** @brief  Макрос для конфигурации источника тактирования USART2 (USART2CLK).
				 * @param  USART1CLKSource: указывает источник тактирования USART2.
				 *         Этот параметр может быть одним из:
				 *            @arg RCC_USART2CLKSOURCE_PCLK1  - PCLK1
				 *            @arg RCC_USART2CLKSOURCE_HSI    - HSI
				 *            @arg RCC_USART2CLKSOURCE_SYSCLK - System Clock
				 *            @arg RCC_USART2CLKSOURCE_LSE    - LSE
				 */
				#if defined(STM32F091xC) || \
					defined(STM32F098xx)

					#define RCC_USART3CLKSOURCE_PCLK1        	RCC_CFGR3_USART3SW_PCLK
					#define RCC_USART3CLKSOURCE_SYSCLK       	RCC_CFGR3_USART3SW_SYSCLK
					#define RCC_USART3CLKSOURCE_LSE          	RCC_CFGR3_USART3SW_LSE
					#define RCC_USART3CLKSOURCE_HSI          	RCC_CFGR3_USART3SW_HSI

					#define MY_RCC_USART3_CONFIG(__USART3_CLKSOURCE__) \
																MODIFY_REG(RCC->CFGR3, RCC_CFGR3_USART3SW, (uint32_t)(__USART3_CLKSOURCE__))

					#define MY_RCC_GET_USART3_SOURCE() 			((uint32_t)(READ_BIT(RCC->CFGR3, RCC_CFGR3_USART3SW)))

				#endif


				/** @brief  Макрос для настройки источника тактирования I2C1 (I2C1CLK).
				 * @param  I2C1CLKSource: указывает источник тактирования I2C1.
				 *         Этот параметр может быть одним из:
				 *            @arg RCC_I2C1CLKSOURCE_HSI: 		- HSI
				 *            @arg RCC_I2C1CLKSOURCE_SYSCLK: - System Clock
				 */
				#define RCC_I2C1CLKSOURCE_HSI                	RCC_CFGR3_I2C1SW_HSI
				#define RCC_I2C1CLKSOURCE_SYSCLK             	RCC_CFGR3_I2C1SW_SYSCLK

				#define MY_RCC_I2C1_CONFIG(__I2C1_CLKSOURCE__)  MODIFY_REG(RCC->CFGR3, RCC_CFGR3_I2C1SW, (uint32_t)(__I2C1_CLKSOURCE__))

				#define MY_RCC_GET_I2C1_SOURCE() 				((uint32_t)(READ_BIT(RCC->CFGR3, RCC_CFGR3_I2C1SW)))


				/** @brief  Макрос для настройки тактирования USB (USBCLK).
				 * @param  __USB_CLKSOURCE__: устанавливает источник тактирования
				 *         Этот параметр может быть одним из:
				 *            @arg RCC_USBCLKSOURCE_HSI48:  HSI48 для USB (не доступен на STM32F070x6 & STM32F070xB)
				 *            @arg RCC_USBCLKSOURCE_PLLCLK: PLL
				 */
				#if defined(STM32F042x6) || \
					defined(STM32F048xx) || \
					defined(STM32F072xB) || \
					defined(STM32F078xx)

					#define RCC_USBCLKSOURCE_HSI48           	RCC_CFGR3_USBSW_HSI48
					#define RCC_USBCLKSOURCE_PLLCLK          	RCC_CFGR3_USBSW_PLLCLK

				#elif defined(STM32F070x6) || \
					defined(STM32F070xB)

					#define RCC_USBCLKSOURCE_PLLCLK          	RCC_CFGR3_USBSW_PLLCLK

				#endif


				#if defined(STM32F042x6) || \
					defined(STM32F048xx) || \
					defined(STM32F072xB) || \
					defined(STM32F078xx) || \
					defined(STM32F070x6) || \
					defined(STM32F070xB)

					#define MY_RCC_USB_CONFIG(__USB_CLKSOURCE__)   	MODIFY_REG(RCC->CFGR3, RCC_CFGR3_USBSW, (uint32_t)(__USB_CLKSOURCE__))

					#define MY_RCC_GET_USB_SOURCE() 				((uint32_t)(READ_BIT(RCC->CFGR3, RCC_CFGR3_USBSW)))

				#endif


				/** @brief 	Макрос для настройки тактирования RTC (RTCCLK).
				 * @note   Поскольку биты конфигурации часов RTC находятся в резервном домене,
				 *					и доступ к записи в этом домене после сброса запрещен, необходимо разрешить доступ на запись
				 *					с помощью макроса Power Backup Access, прежде чем настраивать источник часов RTC (это необходимо сделать один раз после сброса).
				 *
				 * @note   Как только тактирование RTC настроено - его нельзя изменить, пока
				 *					не будет сброшен резервный домен с помощью макроса MY_RCC_BACKUPRESET_FORCE() или с помощью сброса при включении питания (POR).
				 *
				 * @param  RTC_CLKSOURCE: указывает источник тактирования RTC
				 *         Этот параметр может быть одним из:
				 *             @arg RCC_RTCCLKSOURCE_NO_CLK: 		- без тактирования
				 *             @arg RCC_RTCCLKSOURCE_LSE: 				- LSE
				 *             @arg RCC_RTCCLKSOURCE_LSI: 				- LSI
				 *             @arg RCC_RTCCLKSOURCE_HSE_DIV32: 	- HSE деленый на 32
				 * @note   Если LSE или LSI использует в качестве источника тактирования RTC,
				 *					RTC продолжит работать в STOP и STANDBY режимах, и может использоваться в качестве источника wakeup.
				 *         Однако, когда LSI тактирование и HSE деленый на 32 используется в качестве источника тактирования RTC,
				 *         тогда RTC не может быть использован в STOP и STANDBY режимах.
				 * @note   Система всегда должна быть сконфигурирована так, чтобы получить частоту PCLK, большую или равную частоте RTCCLK для правильной работы RTC.
				 */
				#define RCC_RTCCLKSOURCE_NO_CLK              		(0x00000000U)          /*!< Без тактирования */
				#define RCC_RTCCLKSOURCE_LSE                 		RCC_BDCR_RTCSEL_LSE    /*!< RTC тактирование идёт с осциллятора LSE */
				#define RCC_RTCCLKSOURCE_LSI                 		RCC_BDCR_RTCSEL_LSI    /*!< RTC тактирование идёт с осциллятора LSI */
				#define RCC_RTCCLKSOURCE_HSE_DIV32          		RCC_BDCR_RTCSEL_HSE    /*!< RTC тактирование идёт с осциллятора HSE деленого на 32 */

				#define MY_RCC_RTC_CONFIG(__RTC_CLKSOURCE__) 		MODIFY_REG(RCC->BDCR, RCC_BDCR_RTCSEL, (__RTC_CLKSOURCE__))

				#define MY_RCC_GET_RTC_SOURCE() 					READ_BIT(RCC->BDCR, RCC_BDCR_RTCSEL)


			/** @brief  Макрос для включения/выключения тактирования для RTC.
			 * @note   Этот макрос может быть использован только после установки источника тактирования для RTC.
			 */
			#define MY_RCC_RTC_ENABLE() 							SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN)
			#define MY_RCC_RTC_DISABLE() 							CLEAR_BIT(RCC->BDCR, RCC_BDCR_RTCEN)


			/** @brief  Макрос для сброса Backup domain.
			 * @note   Эта функция сбрасывает периферию RTC (включая backup-регистры)
			 *         и выбор тактирования для RTC в RCC_BDCR регистре.
			 */
			#define MY_RCC_BACKUPRESET_FORCE()  					SET_BIT(RCC->BDCR, RCC_BDCR_BDRST)
			#define MY_RCC_BACKUPRESET_RELEASE() 					CLEAR_BIT(RCC->BDCR, RCC_BDCR_BDRST)


			/** @brief  Макрос для конфигурации тактирования CEC.
			 * @param  __CECCLKSource__: указывается источник тактирования CEC .
			 *         Этот параметр может быть одним из:
			 *            @arg RCC_CECCLKSOURCE_HSI - HSI
			 *            @arg RCC_CECCLKSOURCE_LSE - LSE
			 */
			#if defined(STM32F042x6) || \
				defined(STM32F048xx) || \
				defined(STM32F051x8) || \
				defined(STM32F058xx) || \
				defined(STM32F071xB) || \
				defined(STM32F072xB) || \
				defined(STM32F078xx) || \
				defined(STM32F091xC) || \
				defined(STM32F098xx)

				#define RCC_CECCLKSOURCE_HSI                		RCC_CFGR3_CECSW_HSI_DIV244
				#define RCC_CECCLKSOURCE_LSE                		RCC_CFGR3_CECSW_LSE


				#define MY_RCC_CEC_CONFIG(__RCC_CECCLKSOURCE__) 	MODIFY_REG(RCC->CFGR3, RCC_CFGR3_CECSW, (uint32_t)(__RCC_CECCLKSOURCE__))

				#define MY_RCC_GET_CEC_SOURCE() 					((uint32_t)(READ_BIT(RCC->CFGR3, RCC_CFGR3_CECSW)))

			#endif


			/** @brief  Включение/выключение прерываний RCC.
			 * @param  INTERRUPT: устанавливает разрешенные прерывания от RCC.
			 *         Этот параметр может быть одним из:
			 *            @arg RCC_IT_LSIRDY - Флаг прерывания LSI ready
			 *            @arg RCC_IT_LSERDY - Флаг прерывания LSE ready
			 *            @arg RCC_IT_HSIRDY - Флаг прерывания HSI ready
			 *            @arg RCC_IT_HSERDY - Флаг прерывания HSE ready
			 *            @arg RCC_IT_PLLRDY - Флаг прерывания PLL ready
			 *            @arg RCC_IT_HSI14RDY - Флаг прерывания HSI14 ready
			 *            @arg RCC_IT_HSI48RDY - Флаг прерывания HSI48 ready (только на STM32F0X2 с поддержкой USB)
			 */

			#define RCC_IT_LSIRDY                        		((uint8_t)RCC_CIR_LSIRDYF)   /*!< Флаг прерывания LSI Ready */
			#define RCC_IT_LSERDY                        		((uint8_t)RCC_CIR_LSERDYF)   /*!< Флаг прерывания LSE Ready */
			#define RCC_IT_HSIRDY                        		((uint8_t)RCC_CIR_HSIRDYF)   /*!< Флаг прерывания HSI Ready */
			#define RCC_IT_HSERDY                        		((uint8_t)RCC_CIR_HSERDYF)   /*!< Флаг прерывания HSE Ready */
			#define RCC_IT_PLLRDY                        		((uint8_t)RCC_CIR_PLLRDYF)   /*!< Флаг прерывания PLL Ready */
			#define RCC_IT_HSI14RDY                      		((uint8_t)RCC_CIR_HSI14RDYF) /*!< Флаг прерывания HSI14 Ready */
			#define RCC_IT_CSS                           		((uint8_t)RCC_CIR_CSSF)      /*!< Флаг прерывания Clock Security System */


			#if defined(STM32F042x6) || \
				defined(STM32F048xx) || \
				defined(STM32F071xB) || \
				defined(STM32F072xB) || \
				defined(STM32F078xx) || \
				defined(STM32F091xC) || \
				defined(STM32F098xx)

				#define RCC_IT_HSI48                 			RCC_CIR_HSI48RDYF /*!< HSI48 Ready Interrupt flag */

			#endif


			#define MY_RCC_GET_IT(__INTERRUPT__) 				((RCC->CIR & (__INTERRUPT__)) == (__INTERRUPT__))
			#define MY_RCC_ENABLE_IT(__INTERRUPT__) 			(*(__IO uint8_t *) RCC_CIR_BYTE1_ADDRESS |= (__INTERRUPT__))
			#define MY_RCC_DISABLE_IT(__INTERRUPT__) 			(*(__IO uint8_t *) RCC_CIR_BYTE1_ADDRESS &= ~(__INTERRUPT__))
			#define MY_RCC_CLEAR_IT(__INTERRUPT__) 				(*(__IO uint8_t *) RCC_CIR_BYTE2_ADDRESS = (__INTERRUPT__))


			/** @brief  Проверка установлен тот или иной флаг в RCC
			 * @param  RCC_FLAG - параметр для установки того, какой флаг проверять.
			 *         Может принимать одно из значений:
			 *            @arg @ref RCC_FLAG_HSIRDY - HSI осциллятор готов
			 *            @arg @ref RCC_FLAG_HSERDY - HSE осциллятор готов
			 *            @arg @ref RCC_FLAG_PLLRDY - PLL готов
			 *            @arg @ref RCC_FLAG_HSI14RDY - HSI14 осциллятор готов
			 *            @arg @ref RCC_FLAG_LSERDY - LSE осциллятор готов
			 *            @arg @ref RCC_FLAG_LSIRDY - LSI осциллятор готов
			 *            @arg @ref RCC_FLAG_OBLRST - Сброс через Option Byte Load
			 *            @arg @ref RCC_FLAG_PINRST - Сброс через Reset Pin
			 *            @arg @ref RCC_FLAG_PORRST - Сброс через POR/PDR
			 *            @arg @ref RCC_FLAG_SFTRST - Программный сброс
			 *            @arg @ref RCC_FLAG_IWDGRST - Сброс через внешний независимый Watchdog
			 *            @arg @ref RCC_FLAG_WWDGRST - Сброс через оконный сторожевой Watchdog
			 *            @arg @ref RCC_FLAG_LPWRRST - Сброс от контроллера Low Power
			 * @retval Состояние флага, установлен ли он (TRUE или FALSE).
			 */

			#define RCC_FLAG_HSIRDY                      		((uint8_t)((CR_REG_INDEX << 5U) | RCC_CR_HSIRDY_BITNUMBER))
			#define RCC_FLAG_HSERDY                      		((uint8_t)((CR_REG_INDEX << 5U) | RCC_CR_HSERDY_BITNUMBER))
			#define RCC_FLAG_PLLRDY                      		((uint8_t)((CR_REG_INDEX << 5U) | RCC_CR_PLLRDY_BITNUMBER))
			#define RCC_FLAG_LSERDY                      		((uint8_t)((BDCR_REG_INDEX << 5U) | RCC_BDCR_LSERDY_BITNUMBER))
			#define RCC_FLAG_HSI14RDY                    		((uint8_t)((CR2_REG_INDEX << 5U) | RCC_CR2_HSI14RDY_BITNUMBER))
			#define RCC_FLAG_LSIRDY                      		((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_LSIRDY_BITNUMBER))
			#define RCC_FLAG_OBLRST                      		((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_OBLRSTF_BITNUMBER))
			#define RCC_FLAG_PINRST                      		((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_PINRSTF_BITNUMBER))
			#define RCC_FLAG_PORRST                      		((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_PORRSTF_BITNUMBER))
			#define RCC_FLAG_SFTRST                      		((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_SFTRSTF_BITNUMBER))
			#define RCC_FLAG_IWDGRST                     		((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_IWDGRSTF_BITNUMBER))
			#define RCC_FLAG_WWDGRST                     		((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_WWDGRSTF_BITNUMBER))
			#define RCC_FLAG_LPWRRST                     		((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_LPWRRSTF_BITNUMBER))


			#if defined(STM32F042x6) || \
				defined(STM32F048xx) || \
				defined(STM32F071xB) || \
				defined(STM32F072xB) || \
				defined(STM32F078xx) || \
				defined(STM32F091xC) || \
				defined(STM32F098xx)

				#define RCC_FLAG_HSI48RDY            			((uint8_t)((CR2_REG_INDEX << 5) | RCC_CR2_HSI48RDY_BITNUMBER))

			#endif


			#if defined(RCC_CSR_V18PWRRSTF)
				#define RCC_FLAG_V18PWRRST               		((uint8_t)((CSR_REG_INDEX << 5U) | RCC_CSR_V18PWRRSTF_BITNUMBER))	 /*!< Флаг сброса в домене питания V1.8 */
			#endif


			#define MY_RCC_GET_FLAG(__RCC_FLAG__) 			 	(((((__RCC_FLAG__) >> 5U) == CR_REG_INDEX   ) ? RCC->CR   :  \
																  (((__RCC_FLAG__) >> 5U) == CR2_REG_INDEX  ) ? RCC->CR2  :  \
																  (((__RCC_FLAG__) >> 5U) == BDCR_REG_INDEX ) ? RCC->BDCR :  \
																	RCC->CSR) & (1U << ((__RCC_FLAG__) & RCC_FLAG_MASK)))

			#define MY_RCC_CLEAR_RESET_FLAGS() 					(RCC->CSR |= RCC_CSR_RMVF)


			/** @brief  Макросы для конфигурирования пина MCO
			 * @param  __MCO_CLKSOURCE__: устанавливает источник тактирования
			 *         Параметр, в зависимости от используемого МК может быть следующим:
			 *            @arg RCC_MCOSOURCE_HSI - HSI
			 *            @arg RCC_MCOSOURCE_HSE - HSE
			 *            @arg RCC_MCOSOURCE_LSI - LSI
			 *            @arg RCC_MCOSOURCE_LSE - LSE
			 *            @arg RCC_MCOSOURCE_PLLCLK_NODIV - PLLCLK (проверяйте в документации поддерживается ли Вашим МК)
			 *            @arg RCC_MCOSOURCE_PLLCLK_DIV2 - PLLCLK деленый на 2
			 *            @arg RCC_MCOSOURCE_SYSCLK - System Clock
			 *            @arg RCC_MCOSOURCE_HSI14 - HSI14
			 *            @arg RCC_MCOSOURCE_HSI48 - HSI48
			 * @param  __MCO_DIV__: устанавливает предделитель для MCO (проверяйте в документации поддерживается ли Вашим МК)
			 *         Параметр, в зависимости от используемого МК может быть следующим:
			 *						 @arg RCC_MCODIV_1 -  делится на 1 в сериях STM32F030x8, STM32F051x8 и STM32F058xx
			 *            @arg RCC_MCO_DIV1 - делится на 1
			 *            @arg RCC_MCO_DIV2 - делится на 2
			 *            @arg RCC_MCO_DIV4 - делится на 4
			 *            @arg RCC_MCO_DIV8 - делится на 8
			 *            @arg RCC_MCO_DIV16 - делится на 16
			 *            @arg RCC_MCO_DIV32 - делится на 32
			 *            @arg RCC_MCO_DIV64 - делится на 64
			 *            @arg RCC_MCO_DIV128 - делится на 128
			 */

			/* Источники тактирования для вывода на MCO */
			#define RCC_MCO1SOURCE_NOCLOCK                		RCC_CFGR_MCO_NOCLOCK
			#define RCC_MCO1SOURCE_LSI                    		RCC_CFGR_MCO_LSI
			#define RCC_MCO1SOURCE_LSE                    		RCC_CFGR_MCO_LSE
			#define RCC_MCO1SOURCE_SYSCLK                 		RCC_CFGR_MCO_SYSCLK
			#define RCC_MCO1SOURCE_HSI                    		RCC_CFGR_MCO_HSI
			#define RCC_MCO1SOURCE_HSE                    		RCC_CFGR_MCO_HSE
			#define RCC_MCO1SOURCE_PLLCLK_DIV2            		RCC_CFGR_MCO_PLL
			#define RCC_MCO1SOURCE_HSI14                  		RCC_CFGR_MCO_HSI14


			#if defined(STM32F030xC) || \
				defined(STM32F030x6) || \
				defined(STM32F031x6) || \
				defined(STM32F038xx) || \
				defined(STM32F070x6) || \
				defined(STM32F070xB)

				#define RCC_MCOSOURCE_PLLCLK_NODIV   			(RCC_CFGR_MCO_PLL | RCC_CFGR_PLLNODIV)

			#elif defined(STM32F042x6) || \
				defined(STM32F048xx) || \
				defined(STM32F071xB) || \
				defined(STM32F072xB) || \
				defined(STM32F078xx) || \
				defined(STM32F091xC) || \
				defined(STM32F098xx)

				#define RCC_MCOSOURCE_HSI48          			RCC_CFGR_MCO_HSI48
				#define RCC_MCOSOURCE_PLLCLK_NODIV   			(RCC_CFGR_MCO_PLL | RCC_CFGR_PLLNODIV)

			#endif


			/* Возможные варианты делителей для MCO */
			#if defined(STM32F030x8) || \
				defined(STM32F051x8) || \
				defined(STM32F058xx)

				#define RCC_MCODIV_1                    		((uint32_t)0x00000000)

			#elif defined(STM32F030x6) || \
				defined(STM32F031x6) || \
				defined(STM32F038xx) || \
				defined(STM32F070x6) || \
				defined(STM32F042x6) || \
				defined(STM32F048xx) || \
				defined(STM32F071xB) || \
				defined(STM32F070xB) || \
				defined(STM32F072xB) || \
				defined(STM32F078xx) || \
				defined(STM32F091xC) || \
				defined(STM32F098xx) || \
				defined(STM32F030xC)

				#define RCC_MCO_DIV1                     		((uint32_t)0x00000000)
				#define RCC_MCO_DIV2                     		((uint32_t)0x10000000)
				#define RCC_MCO_DIV4                     		((uint32_t)0x20000000)
				#define RCC_MCO_DIV8                     		((uint32_t)0x30000000)
				#define RCC_MCO_DIV16                    		((uint32_t)0x40000000)
				#define RCC_MCO_DIV32                    		((uint32_t)0x50000000)
				#define RCC_MCO_DIV64                    		((uint32_t)0x60000000)
				#define RCC_MCO_DIV128                   		((uint32_t)0x70000000)

			#endif


			/* Макрос конфигурации MCO */
			#if defined(STM32F030xC) || \
				defined(STM32F030x6) || \
				defined(STM32F031x6) || \
				defined(STM32F038xx) || \
				defined(STM32F042x6) || \
				defined(STM32F048xx) || \
				defined(STM32F070x6) || \
				defined(STM32F070xB) || \
				defined(STM32F071xB) || \
				defined(STM32F072xB) || \
				defined(STM32F078xx) || \
				defined(STM32F091xC) || \
				defined(STM32F098xx)

				#define MY_RCC_MCO_CONFIG(__MCO_CLKSOURCE__, __MCO_DIV__) \
																MODIFY_REG(RCC->CFGR, (RCC_CFGR_MCO | RCC_CFGR_MCOPRE), ((__MCO_CLKSOURCE__) | (__MCO_DIV__)))

			#else

				#define MY_RCC_MCO_CONFIG(__MCO_CLKSOURCE__, __MCO_DIV__)     MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO, __MCO_CLKSOURCE__)

			#endif


			#if defined(STM32F042x6) || \
				defined(STM32F048xx) || \
				defined(STM32F071xB) || \
				defined(STM32F072xB) || \
				defined(STM32F078xx) || \
				defined(STM32F091xC) || \
				defined(STM32F098xx)

				/** @defgroup Источники прерывания CRS
				 * @{
				 */
					#define RCC_CRS_IT_SYNCOK            			CRS_ISR_SYNCOKF    /*!< SYNC event OK */
					#define RCC_CRS_IT_SYNCWARN           			CRS_ISR_SYNCWARNF  /*!< SYNC warning */
					#define RCC_CRS_IT_ERR                			CRS_ISR_ERRF       /*!< error */
					#define RCC_CRS_IT_ESYNC              			CRS_ISR_ESYNCF     /*!< Expected SYNC */
					#define RCC_CRS_IT_TRIMOVF            			CRS_ISR_TRIMOVF    /*!< Trimming overflow or underflow */
					#define RCC_CRS_IT_SYNCERR            			CRS_ISR_SYNCERR    /*!< SYNC error */
					#define RCC_CRS_IT_SYNCMISS           			CRS_ISR_SYNCMISS   /*!< SYNC missed*/

					#define RCC_CRS_IT_ERROR_MASK             		((uint32_t)(RCC_CRS_IT_TRIMOVF | RCC_CRS_IT_SYNCERR | RCC_CRS_IT_SYNCMISS))

					#define MY_RCC_CRS_ENABLE_IT(__INTERRUPT__)   	(CRS->CR |=  (__INTERRUPT__))
					#define MY_RCC_CRS_DISABLE_IT(__INTERRUPT__)  	(CRS->CR &= ~(__INTERRUPT__))
					#define MY_RCC_CRS_GET_IT_SOURCE(__INTERRUPT__) ((CRS->CR &   (__INTERRUPT__))? SET : RESET)
					#define MY_RCC_CRS_CLEAR_IT(__INTERRUPT__)  	((((__INTERRUPT__) &  RCC_CRS_IT_ERROR_MASK)!= 0) ? \
																		(CRS->ICR |= CRS_ICR_ERRC) : \
																		(CRS->ICR |= (__INTERRUPT__)))
				/**
				 * @}
				 */


				/** @defgroup RCC CRS флаги
				 * @{
				 */
					#define RCC_CRS_FLAG_SYNCOK             		CRS_ISR_SYNCOKF     /* SYNC event OK flag     */
					#define RCC_CRS_FLAG_SYNCWARN           		CRS_ISR_SYNCWARNF   /* SYNC warning flag      */
					#define RCC_CRS_FLAG_ERR               			CRS_ISR_ERRF        /* Error flag        */
					#define RCC_CRS_FLAG_ESYNC              		CRS_ISR_ESYNCF      /* Expected SYNC flag     */
					#define RCC_CRS_FLAG_TRIMOVF            		CRS_ISR_TRIMOVF     /*!< Trimming overflow or underflow */
					#define RCC_CRS_FLAG_SYNCERR            		CRS_ISR_SYNCERR     /*!< SYNC error */
					#define RCC_CRS_FLAG_SYNCMISS           		CRS_ISR_SYNCMISS    /*!< SYNC missed*/

					#define RCC_CRS_FLAG_ERROR_MASK           		((uint32_t)(RCC_CRS_FLAG_TRIMOVF | RCC_CRS_FLAG_SYNCERR | RCC_CRS_FLAG_SYNCMISS))

					#define MY_RCC_CRS_GET_FLAG(__FLAG__)  			((CRS->ISR  & (__FLAG__)) == (__FLAG__))
					#define MY_RCC_CRS_CLEAR_FLAG(__FLAG__)   		((((__FLAG__) & RCC_CRS_FLAG_ERROR_MASK)!= 0) ? \
																		(CRS->ICR |= CRS_ICR_ERRC) : \
																		(CRS->ICR |= (__FLAG)))
				/**
				 * @}
				 */


				/**
				 * @brief  Включает тактирование для счетчика ошибок частоты
				 * @note   Когда бит CEN установлен, регистр CRS_CFGR становится защищенным от записи
				 * @retval Нет
				 */
				#define MY_RCC_CRS_ENABLE_FREQ_ERROR_COUNTER() 		(CRS->CR |= CRS_CR_CEN)


				/**
				 * @brief  Выключает тактирование для счетчика ошибок частоты
				 * @retval Нет
				 */
				#define MY_RCC_CRS_DISABLE_FREQ_ERROR_COUNTER()  	(CRS->CR &= ~CRS_CR_CEN)


				/**
				 * @brief  Включает аппаратную подстройку TRIM-битов.
				 * @note   При установке бита AUTOTRIMEN регистр CRS_CFGR становится защищенным от записи.
				 * @retval Нет
				 */
				#define MY_RCC_CRS_ENABLE_AUTOMATIC_CALIB()  		(CRS->CR |= CRS_CR_AUTOTRIMEN)


				/**
				 * @brief  Отключает аппаратную подстройку TRIM-битов.
				 * @retval Нет
				 */
				#define MY_RCC_CRS_DISABLE_AUTOMATIC_CALIB()  		(CRS->CR &= ~CRS_CR_AUTOTRIMEN)


				/**
				 * @brief  Макрос для вычисления значения перезагрузки, устанавливаемого в регистре CRS в соответствии с целевыми и синхронизирующими частотами
				 * @note   Значение перезагрузки следует выбирать в соответствии с соотношением между целевой частотой и частотой источника синхронизации
				 *		   после предварительного масштабирования. Он уменьшается на единицу, чтобы достичь ожидаемой синхронизации при нулевом значении.
				 *		   Формула следующая:
				 *         RELOAD = (fTARGET / fSYNC) -1
				 * @param  _FTARGET_ целевая частота (в Hz)
				 * @param  _FSYNC_ частота сигнала синхронизации (в Hz)
				 * @retval None
				 */
				#define MY_RCC_CRS_CALCULATE_RELOADVALUE(__FTARGET__, __FSYNC__)  	\
																	(((__FTARGET__) / (__FSYNC__)) - 1)

			#endif

		/**
		 * @} MY_RCC_Macros
		 */


		/**
		 * @defgroup MY_RCC_Typedefs
		 * @brief    RCC Typedefs используемые в библиотеке RCC для инициализации тактирования
		 * @{
		 */
			/**
			 * @brief  Структура для настройки и инициализации PLL
			 */
			typedef struct
			{
				uint32_t PLL_State;      			/*!< Состояние PLL */
				uint32_t PLL_Source;     			/*!< Источник тактирования для PLL */
				uint32_t PLL_MUL;        			/*!< Коэфициент умножения для PLL */
				uint32_t PLL_PREDIV;        		/*!< Предделитель для PLL */
			}
			MY_RCC_PLL_Init_t;


			/**
			 * @brief  Структура для хранения конфигурации внутренних/внешних осцилляторов (HSE, HSI, LSE and LSI)
			 */
			typedef struct
			{
				uint32_t OscillatorType;        	/*!< Выбранный осциллятор */
				uint32_t HSE_State;             	/*!< Состояние HSE */
				uint32_t LSE_State;             	/*!< Состояние LSE */
				uint32_t HSI_State;             	/*!< Состояние HSI */
				uint32_t HSI_CalibrationValue;  	/*!< Значение бита калибрации HSI (default is RCC_HSICALIBRATION_DEFAULT).
													 	Значение должно быть в диапазоне Min_Data = 0x00 и Max_Data = 0x1FU */
				uint32_t HSI14_State;          	 	/*!< Состояние HSI14  */
				uint32_t HSI14_CalibrationValue;  	/*!< Значение бита калибрации HSI14 (default is RCC_HSI14CALIBRATION_DEFAULT).
														Значение должно быть в диапазоне Min_Data = 0x00 и Max_Data = 0x1FU */

				uint32_t HSI48_State;              	/*!< Новое состояние HSI48 (применимо только STM32F07x, STM32F0x2 и STM32F09x)
														Значение должно быть @ref RCCEx_HSI48_Config */

				uint32_t LSI_State;              	/*!< Состояние LSI */
				MY_RCC_PLL_Init_t PLL;       		/*!< Параметры PLL */
			}
			MY_RCC_Osc_Init_t;


			/**
			 * @brief  Структура для хранения конфигурации системы тактирования, шин тактирования AHB и APB
			 */
			typedef struct
			{
				uint32_t ClockType;             	/*!< Настройки типа тактирования */
				uint32_t SYSCLK_Source;          	/*!< Источник тактирования для SYSCLK */
				uint32_t AHBCLK_Divider;         	/*!< Делитель шины AHB (HCLK) */
				uint32_t APB1CLK_Divider;       	/*!< Делитель шины APB1 (PCLK1) */
			}
			MY_RCC_Clock_Init_t;


			/**
			 * @brief  Расширенные настройки тактирования для периферии
			 */
			#if defined(STM32F030x6) || \
				defined(STM32F030x8) || \
				defined(STM32F031x6) || \
				defined(STM32F038xx) || \
				defined(STM32F030xC)

				typedef struct
				{
					uint32_t PeriphClockSelection; 	/*!< Конфигурация тактирования для периферии */
					uint32_t RTC_ClockSelection;    /*!< Устанавливает источник тактирования для RTC */
					uint32_t USART1_ClockSelection; /*!< Источник тактирования для USART1 */
					uint32_t I2C1_ClockSelection;   /*!< Источник тактирования для I2C1 */
				}
				MY_RCC_PeriphClock_Init_t;

			#elif defined(STM32F070x6) || \
				defined(STM32F070xB)

				typedef struct
				{
					uint32_t PeriphClockSelection; 	/*!< Конфигурация тактирования для периферии */
					uint32_t RTC_ClockSelection;   	/*!< Устанавливает источник тактирования для RTC */
					uint32_t USART1_ClockSelection; /*!< Источник тактирования для USART1 */
					uint32_t I2C1_ClockSelection;   /*!< Источник тактирования для I2C1 */
					uint32_t USB_ClockSelection;    /*!< Источник тактирования для USB */
				}
				MY_RCC_PeriphClock_Init_t;

			#elif defined(STM32F042x6) || \
				defined(STM32F048xx)

				typedef struct
				{
					uint32_t PeriphClockSelection; 	/*!< Конфигурация тактирования для периферии */
					uint32_t RTC_ClockSelection;    /*!< Устанавливает источник тактирования для RTC */
					uint32_t USART1_ClockSelection; /*!< Источник тактирования для USART1 */
					uint32_t I2C1_ClockSelection;   /*!< Источник тактирования для I2C1 */
					uint32_t CEC_ClockSelection;    /*!< Источник тактирования для HDMI CEC */
					uint32_t USB_ClockSelection;    /*!< Источник тактирования для USB */
				}
				MY_RCC_PeriphClock_Init_t;

			#elif defined(STM32F051x8) || \
				defined(STM32F058xx)

				typedef struct
				{
					uint32_t PeriphClockSelection; 	/*!< Конфигурация тактирования для периферии */
					uint32_t RTC_ClockSelection;    /*!< Устанавливает источник тактирования для RTC */
					uint32_t USART1_ClockSelection; /*!< Источник тактирования для USART1 */
					uint32_t I2C1_ClockSelection;   /*!< Источник тактирования для I2C1 */
					uint32_t CEC_ClockSelection;    /*!< Источник тактирования для HDMI CEC */
				}
				MY_RCC_PeriphClock_Init_t;

			#elif defined(STM32F071xB)

				typedef struct
				{
					uint32_t PeriphClockSelection; 	/*!< Конфигурация тактирования для периферии */
					uint32_t RTC_ClockSelection;    /*!< Устанавливает источник тактирования для RTC */
					uint32_t USART1_ClockSelection; /*!< Источник тактирования для USART1 */
					uint32_t USART2_ClockSelection; /*!< Источник тактирования для USART2 */
					uint32_t I2C1_ClockSelection;   /*!< Источник тактирования для I2C1 */
					uint32_t CEC_ClockSelection;    /*!< Источник тактирования для HDMI CEC */
				}
				MY_RCC_PeriphClock_Init_t;

			#elif defined(STM32F072xB) || \
				defined(STM32F078xx)

				typedef struct
				{
					uint32_t PeriphClockSelection; 	/*!< Конфигурация тактирования для периферии */
					uint32_t RTC_ClockSelection;    /*!< Устанавливает источник тактирования для RTC */
					uint32_t USART1_ClockSelection; /*!< Источник тактирования для USART1 */
					uint32_t USART2_ClockSelection; /*!< Источник тактирования для USART2 */
					uint32_t I2C1_ClockSelection;   /*!< Источник тактирования для I2C1 */
					uint32_t CEC_ClockSelection;    /*!< Источник тактирования для HDMI CEC */
					uint32_t USB_ClockSelection;    /*!< Источник тактирования для USB */
				}
				MY_RCC_PeriphClock_Init_t;

			#elif defined(STM32F091xC) || \
				defined(STM32F098xx)

				typedef struct
				{
					uint32_t PeriphClockSelection; /*!< Конфигурация тактирования для периферии */
					uint32_t RTC_ClockSelection;    /*!< Устанавливает источник тактирования для RTC */
					uint32_t USART1_ClockSelection; /*!< Источник тактирования для USART1 */
					uint32_t USART2_ClockSelection; /*!< Источник тактирования для USART2 */
					uint32_t USART3_ClockSelection; /*!< Источник тактирования для USART3 */
					uint32_t I2C1_ClockSelection;   /*!< Источник тактирования для I2C1 */
					uint32_t CEC_ClockSelection;    /*!< Источник тактирования для HDMI CEC */
				}
				MY_RCC_PeriphClock_Init_t;

			#endif


			#if defined(STM32F042x6) || \
				defined(STM32F048xx) || \
				defined(STM32F071xB) || \
				defined(STM32F072xB) || \
				defined(STM32F078xx) || \
				defined(STM32F091xC) || \
				defined(STM32F098xx)

				/**
				 * @brief RCC Clock Recovery System (CRS) Init structure definition
				 */
				typedef struct
				{
					uint32_t Prescaler;             /*!< Specifies the division factor of the SYNC signal.
														 This parameter can be a value of @ref RCCEx_CRS_SynchroDivider */
					uint32_t Source;                /*!< Specifies the SYNC signal source.
														 This parameter can be a value of @ref RCCEx_CRS_SynchroSource */
					uint32_t Polarity;              /*!< Specifies the input polarity for the SYNC signal source.
														 This parameter can be a value of @ref RCCEx_CRS_SynchroPolarity */
					uint32_t ReloadValue;           /*!< Specifies the value to be loaded in the frequency error counter with each SYNC event.
														 It can be calculated in using macro MY_RCC_CRS_CALCULATE_RELOADVALUE(_FTARGET_, _FSYNC_)
														 This parameter must be a number between 0 and 0xFFFF or a value of @ref RCCEx_CRS_ReloadValueDefault .*/
					uint32_t ErrorLimitValue;       /*!< Specifies the value to be used to evaluate the captured frequency error value.
														 This parameter must be a number between 0 and 0xFF or a value of @ref RCCEx_CRS_ErrorLimitDefault */
					uint32_t HSI48CalibrationValue; /*!< Specifies a user-programmable trimming value to the HSI48 oscillator.
														 This parameter must be a number between 0 and 0x3F or a value of @ref RCCEx_CRS_HSI48CalibrationDefault */
				}
				MY_RCC_CRS_Init_t;


				/**
				 * @brief RCC_CRS Synchronization structure definition
				 */
				typedef struct
				{
					uint32_t ReloadValue;           /*!< Specifies the value loaded in the Counter reload value.
														 This parameter must be a number between 0 and 0xFFFF */
					uint32_t HSI48CalibrationValue; /*!< Specifies value loaded in HSI48 oscillator smooth trimming.
														 This parameter must be a number between 0 and 0x3F */
					uint32_t FreqErrorCapture;      /*!< Specifies the value loaded in the .FECAP, the frequency error counter value latched in the time of the last SYNC event.
														 This parameter must be a number between 0 and 0xFFFF */
					uint32_t FreqErrorDirection;    /*!< Specifies the value loaded in the .FEDIR, the counting direction of the
														 frequency error counter latched in the time of the last SYNC event.
														 It shows whether the actual frequency is below or above the target.
														 This parameter must be a value of @ref RCCEx_CRS_FreqErrorDirection */
				}
				MY_RCC_CRS_Sync_Info_t;

			#endif /* CRS */

		/**
		 * @} MY_RCC_Typedefs
		 */


		/**
		 * @defgroup MY_RCC_Functions
		 * @brief    RCC функции
		 * @{
		 */
			/**
			 * @brief  Инициализация системного тактирования
			 * @note   Функция @ref MY_RCC_System_Init должна вызываться для иницализации тактирования при старте
			 * 				В ходе выполнения функции:
			 *						- сброс к дефолтным значениям настроек системы тактирования
			 *						- выбор осциллятора в качестве основного
			 *						- если используется PLL - то происходит его настройка
			 *						- настраивается задержка Flash
			 *						- делители HCLK, PCLK1
			 *						- настраивается источник тактирования SYSCLK
			 *						- обновляется значение SystemCoreClock
			 *						- инициализируется SysTick
			 * @param  Нет
			 * @retval MY_RCC_Result_t
			 */
			MY_Result_t MY_RCC_System_Init(void);


			/**
			 * @brief  Сбрасывает настройки RCC к значениям по умолчанию
			 * @note   Значения по умолчанию следующие:
			 *           - HSI ON и используется как источник системного тактирования
			 *           - HSE и PLL OFF
			 *           - AHB, APB1 предделитель устанавливается на 1.
			 *           - CSS и MCO1 OFF
			 *           - Все прерывания выключены
			 * @note   Эта функция не изменяет настройки:
			 *           - Периферии
			 *           - LSI, LSE и RTC
			 * @param  Нет
			 * @retval MY_RCC_Result_t
			 */
			void MY_RCC_System_DeInit(void);


			/**
			 * @brief  Initializes the RCC Oscillators according to the specified parameters in the
			 *         RCC_OscInitTypeDef.
			 * @param  RCC_OscInitStruct pointer to an RCC_OscInitTypeDef structure that
			 *         contains the configuration information for the RCC Oscillators.
			 * @note   The PLL is not disabled when used as system clock.
			 * @note   Transitions LSE Bypass to LSE On and LSE On to LSE Bypass are not
			 *         supported by this macro. User should request a transition to LSE Off
			 *         first and then LSE On or LSE Bypass.
			 * @note   Transition HSE Bypass to HSE On and HSE On to HSE Bypass are not
			 *         supported by this macro. User should request a transition to HSE Off
			 *         first and then HSE On or HSE Bypass.
			 * @retval HAL status
			 */
			MY_Result_t MY_RCC_Osc_Config(MY_RCC_Osc_Init_t *RCC_Osc_InitStruct);


			/**
			 * @brief  Initializes the CPU, AHB and APB buses clocks according to the specified
			 *         parameters in the RCC_ClkInitStruct.
			 * @param  RCC_ClkInitStruct pointer to an RCC_OscInitTypeDef structure that
			 *         contains the configuration information for the RCC peripheral.
			 * @param  FLatency FLASH Latency
			 *          The value of this parameter depend on device used within the same series
			 * @note   The SystemCoreClock CMSIS variable is used to store System Clock Frequency
			 *         and updated by @ref MY_RCC_GetHCLKFreq() function called within this function
			 *
			 * @note   The HSI is used (enabled by hardware) as system clock source after
			 *         start-up from Reset, wake-up from STOP and STANDBY mode, or in case
			 *         of failure of the HSE used directly or indirectly as system clock
			 *         (if the Clock Security System CSS is enabled).
			 *
			 * @note   A switch from one clock source to another occurs only if the target
			 *         clock source is ready (clock stable after start-up delay or PLL locked).
			 *         If a clock source which is not yet ready is selected, the switch will
			 *         occur when the clock source will be ready.
			 *         You can use @ref MY_RCC_GetClockConfig() function to know which clock is
			 *         currently used as system clock source.
			 * @retval HAL status
			 */
			MY_Result_t MY_RCC_Clock_Config(MY_RCC_Clock_Init_t *RCC_Clock_InitStruct, uint32_t FlashLatency);


			/**
			 * @brief  Initializes the RCC extended peripherals clocks according to the specified
			 *         parameters in the RCC_PeriphClock_InitStructTypeDef.
			 * @param  PeriphClock_InitStruct pointer to an RCC_PeriphClock_InitStructTypeDef structure that
			 *         contains the configuration information for the Extended Peripherals clocks
			 *         (USART, RTC, I2C, CEC and USB).
			 *
			 * @note   Care must be taken when @ref HAL_RCCEx_PeriphCLKConfig() is used to select
			 *         the RTC clock source; in this case the Backup domain will be reset in
			 *         order to modify the RTC Clock source, as consequence RTC registers (including
			 *         the backup registers) and RCC_BDCR register are set to their reset values.
			 *
			 * @retval HAL status
			 */
			MY_Result_t MY_RCC_PeriphClock_Config(MY_RCC_PeriphClock_Init_t *RCC_PeriphClock_InitStruct);


			/**
			 * @brief  Get the RCC_ClkInitStruct according to the internal
			 * RCC configuration registers.
			 * @param  PeriphClock_InitStruct pointer to an RCC_PeriphClock_InitStructTypeDef structure that
			 *         returns the configuration information for the Extended Peripherals clocks
			 *         (USART, RTC, I2C, CEC and USB).
			 * @retval None
			 */
			void MY_RCC_PeriphClock_GetConfig(MY_RCC_PeriphClock_Init_t *RCC_PeriphClock_InitStruct);


			/**
			 * @brief  Returns the peripheral clock frequency
			 * @note   Returns 0 if peripheral clock is unknown
			 * @param  PeriphClk Peripheral clock identifier
			 *         This parameter can be one of the following values:
			 *            @arg @ref RCC_PERIPHCLK_RTC     RTC peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_USART1  USART1 peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_I2C1    I2C1 peripheral clock
			 *
			 *			@if STM32F042x6
			 *            @arg @ref RCC_PERIPHCLK_USB     USB peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock
			 *			@endif
			 *
			 *			@if STM32F048xx
			 *            @arg @ref RCC_PERIPHCLK_USB     USB peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock
			 *          @endif
			 *
			 *          @if STM32F051x8
			 *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock
			 *          @endif
			 *
			 *          @if STM32F058xx
			 *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock
			 *           @endif
			 *
			 *          @if STM32F070x6
			 *            @arg @ref RCC_PERIPHCLK_USB     USB peripheral clock
			 *          @endif
			 *
			 *          @if STM32F070xB
			 *            @arg @ref RCC_PERIPHCLK_USB     USB peripheral clock
			 *          @endif
			 *
			 *          @if STM32F071xB
			 *            @arg @ref RCC_PERIPHCLK_USART2  USART2 peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock
			 *          @endif
			 *
			 *          @if STM32F072xB
			 *            @arg @ref RCC_PERIPHCLK_USART2  USART2 peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_USB     USB peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock
			 *          @endif
			 *
			 *        	@if STM32F078xx
			 *            @arg @ref RCC_PERIPHCLK_USART2  USART2 peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_USB     USB peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock
			 *          @endif
			 *
			 *          @if STM32F091xC
			 *            @arg @ref RCC_PERIPHCLK_USART2  USART2 peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_USART3  USART2 peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock
			 *          @endif
			 *
			 *          @if STM32F098xx
			 *            @arg @ref RCC_PERIPHCLK_USART2  USART2 peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_USART3  USART2 peripheral clock
			 *            @arg @ref RCC_PERIPHCLK_CEC     CEC peripheral clock
			 *          @endif
			 *
			 * @retval Frequency in Hz (0: means that no available frequency for the peripheral)
			 */
			uint32_t MY_RCC_PeriphClock_GetFreq(uint32_t PeriphClock);


			/**
			 * @brief  Returns the SYSCLK frequency
			 * @note   The system frequency computed by this function is not the real
			 *         frequency in the chip. It is calculated based on the predefined
			 *         constant and the selected clock source:
			 * @note     If SYSCLK source is HSI, function returns values based on HSI_VALUE(*)
			 * @note     If SYSCLK source is HSE, function returns a value based on HSE_VALUE
			 *           divided by PREDIV factor(**)
			 * @note     If SYSCLK source is PLL, function returns a value based on HSE_VALUE
			 *           divided by PREDIV factor(**) or depending on STM32F0xxxx devices either a value based
			 *           on HSI_VALUE divided by 2 or HSI_VALUE divided by PREDIV factor(*) multiplied by the
			 *           PLL factor.
			 * @note     (*) HSI_VALUE is a constant defined in stm32f0xx_hal_conf.h file (default value
			 *               8 MHz) but the real value may vary depending on the variations
			 *               in voltage and temperature.
			 * @note     (**) HSE_VALUE is a constant defined in stm32f0xx_hal_conf.h file (default value
			 *                8 MHz), user has to ensure that HSE_VALUE is same as the real
			 *                frequency of the crystal used. Otherwise, this function may
			 *                have wrong result.
			 *
			 * @note   The result of this function could be not correct when using fractional
			 *         value for HSE crystal.
			 *
			 * @note   This function can be used by the user application to compute the
			 *         baud-rate for the communication peripherals or configure other parameters.
			 *
			 * @note   Each time SYSCLK changes, this function must be called to update the
			 *         right SYSCLK value. Otherwise, any configuration based on this function will be incorrect.
			 *
			 * @retval SYSCLK frequency
			 */
			uint32_t MY_RCC_SysClock_GetFreq(void);


			/**
			 * @brief  Возвращает текущую частоту HCLK
			 * @note   Each time HCLK changes, this function must be called to update the
			 *         right HCLK value. Otherwise, any configuration based on this function will be incorrect.
			 *
			 * @note   The SystemCoreClock CMSIS variable is used to store System Clock Frequency
			 *         and updated within this function
			 * @retval HCLK frequency
			 */
			uint32_t MY_RCC_HCLK_GetFreq(void);


			/**
			 * @brief  Возвращает текущую частоту PCLK1
			 * @note   Each time PCLK1 changes, this function must be called to update the right PCLK1 value.
			 * 		   Otherwise, any configuration based on this function will be incorrect.
			 * @retval PCLK1 frequency
			 */
			uint32_t MY_RCC_PCLK1_GetFreq(void);


			/**
			 * @brief  Configures the RCC_OscInitStruct according to the internal RCC configuration registers.
			 * @param  RCC_OscInitStruct pointer to an RCC_OscInitTypeDef structure that will be configured.
			 * @retval None
			 */
			void MY_RCC_Osc_GetConfig(MY_RCC_Osc_Init_t *RCC_Osc_InitStruct);


			/**
			 * @brief  Get the RCC_ClkInitStruct according to the internal RCC configuration registers.
			 * @param  RCC_ClkInitStruct pointer to an RCC_ClkInitTypeDef structure that contains the current clock configuration.
			 * @param  pFLatency Pointer on the Flash Latency.
			 * @retval None
			 */
			void MY_RCC_Clock_GetConfig(MY_RCC_Clock_Init_t *RCC_Clk_InitStruct, uint32_t *pFLatency);


			/**
			 * @brief  Selects the clock source to output on MCO pin.
			 * @note   MCO pin should be configured in alternate function mode.
			 * @param  RCC_MCO_Source specifies the clock source to output.
			 *          This parameter can be one of the following values:
			 *            @arg @ref RCC_MCO1SOURCE_NOCLOCK     No clock selected as MCO clock
			 *            @arg @ref RCC_MCO1SOURCE_SYSCLK      System clock selected as MCO clock
			 *            @arg @ref RCC_MCO1SOURCE_HSI         HSI selected as MCO clock
			 *            @arg @ref RCC_MCO1SOURCE_HSE         HSE selected as MCO clock
			 *            @arg @ref RCC_MCO1SOURCE_LSI         LSI selected as MCO clock
			 *            @arg @ref RCC_MCO1SOURCE_LSE         LSE selected as MCO clock
			 *            @arg @ref RCC_MCO1SOURCE_HSI14       HSI14 selected as MCO clock
			 *            @arg @ref RCC_MCO1SOURCE_PLLCLK_DIV2 PLLCLK Divided by 2 selected as MCO clock
			 * @param  RCC_MCO_Div specifies the MCO DIV.
			 *          This parameter can be one of the following values:
			 *            @arg @ref RCC_MCODIV_1 no division applied to MCO clock
			 * @retval None
			 */
			void MY_RCC_MCO_Config(uint32_t RCC_MCO_Source, uint32_t RCC_MCO_Div);


			/**
			 * @brief  Enables the Clock Security System.
			 * @note   If a failure is detected on the HSE oscillator clock, this oscillator
			 *         is automatically disabled and an interrupt is generated to inform the
			 *         software about the failure (Clock Security System Interrupt, CSSI),
			 *         allowing the MCU to perform rescue operations. The CSSI is linked to
			 *         the Cortex-M0 NMI (Non-Maskable Interrupt) exception vector.
			 * @retval None
			 */
			void MY_RCC_CSS_Enable(void);


			/**
			 * @brief  Disables the Clock Security System.
			 * @retval None
			 */
			void MY_RCC_CSS_Disable(void);


			#if defined(STM32F042x6) || \
				defined(STM32F048xx) || \
				defined(STM32F071xB) || \
				defined(STM32F072xB) || \
				defined(STM32F078xx) || \
				defined(STM32F091xC) || \
				defined(STM32F098xx)

				/**
				 * @brief  Start automatic synchronization for polling mode
				 * @param  pInit Pointer on RCC_CRSInitTypeDef structure
				 * @retval None
				 */
				void MY_RCC_CRS_Config(MY_RCC_CRS_Init_t *pInit);


				/**
				 * @brief  Generate the software synchronization event
				 * @retval None
				 */
				void MY_RCC_CRS_SoftwareSyncGenerate(void);


				/**
				 * @brief  Return synchronization info
				 * @param  pSynchroInfo Pointer on RCC_CRSSynchroInfoTypeDef structure
				 * @retval None
				 */
				void MY_RCC_CRS_GetSyncInfo(RCC_CRSSyncInfo_t *pSyncInfo);


				/**
				 * @brief Wait for CRS Synchronization status.
				 * @param Timeout  Duration of the timeout
				 * @note  Timeout is based on the maximum time to receive a SYNC event based on synchronization frequency.
				 * @note  If Timeout set to HAL_MAX_DELAY, HAL_TIMEOUT will be never returned.
				 * @retval Combination of Synchronization status
				 *          This parameter can be a combination of the following values:
				 *            @arg @ref RCC_CRS_TIMEOUT
				 *            @arg @ref RCC_CRS_SYNCOK
				 *            @arg @ref RCC_CRS_SYNCWARN
				 *            @arg @ref RCC_CRS_SYNCERR
				 *            @arg @ref RCC_CRS_SYNCMISS
				 *            @arg @ref RCC_CRS_TRIMOVF
				 */
				uint32_t MY_RCC_CRS_WaitSync(uint32_t Timeout);


				/**
				 * @brief Handle the Clock Recovery System interrupt request.
				 * @retval None
				 */
				void MY_RCC_CRS_IRQHandler(void);


				/**
				 * @brief  RCC Clock Recovery System SYNCOK interrupt callback.
				 * @retval none
				 */
				void MY_RCC_CRS_SyncOkCallback(void);


				/**
				 * @brief  RCC Clock Recovery System SYNCWARN interrupt callback.
				 * @retval none
				 */
				void MY_RCC_CRS_SyncWarnCallback(void);


				/**
				 * @brief  RCCEx Clock Recovery System Expected SYNC interrupt callback.
				 * @retval none
				 */
				void MY_RCC_CRS_ExpectedSyncCallback(void);


				/**
				 * @brief  RCCEx Clock Recovery System Error interrupt callback.
				 * @param  Error Combination of Error status.
				 *         This parameter can be a combination of the following values:
				 *           @arg @ref RCC_CRS_SYNCERR
				 *           @arg @ref RCC_CRS_SYNCMISS
				 *           @arg @ref RCC_CRS_TRIMOVF
				 * @retval none
				 */
				void MY_RCC_CRS_ErrorCallback(uint32_t Error);

			#endif

			/**
			 * @} MY_RCC_Functions
			 */

		/**
		 * @}
		 */

	/**
	 * @}
	 */

	/* C++ detection */
	#ifdef __cplusplus
		}
	#endif

#endif
