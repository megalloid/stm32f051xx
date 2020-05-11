/**
 * @author  Andrey Zaostrovnykh
 * @email   megalloid@mail.ru
 * @link    http://smarthouseautomatics.ru/stm32/stm32f0xx/rcc/
 * @version v0.1
 * @ide     STM32CubeIDE
 * @brief   Библиотека RCC для STM32F0xx
 */

#include "my_stm32f0xx_rcc.h"

extern uint32_t SystemCoreClock;

MY_Result_t MY_RCC_System_Init(void)
{
	/* Структуры для настроек */
	MY_RCC_Clock_Init_t  RCC_Clock_InitStruct;
	MY_RCC_Osc_Init_t 	 RCC_Osc_InitStruct;

	/* Устанавливаем из настроек источник тактирования */
	RCC_Osc_InitStruct.OscillatorType = RCC_OSCILLATORTYPE;

	/* Устанавливаем параметры осциллятора HSE или HSI*/
	if(RCC_Osc_InitStruct.OscillatorType == RCC_OSCILLATORTYPE_HSE)
	{
		RCC_Osc_InitStruct.HSE_State = RCC_HSE_ON;
		RCC_Osc_InitStruct.HSI_State = RCC_HSI_OFF;
		RCC_Osc_InitStruct.PLL.PLL_State = RCC_PLL_NONE;
	}
	else if(RCC_Osc_InitStruct.OscillatorType == RCC_OSCILLATORTYPE_HSI)
	{
		RCC_Osc_InitStruct.HSE_State = RCC_HSE_OFF;
		RCC_Osc_InitStruct.HSI_State = RCC_HSI_ON;
		RCC_Osc_InitStruct.PLL.PLL_State = RCC_PLL_NONE;
		RCC_Osc_InitStruct.HSI_CalibrationValue = RCC_HSICALIBRATION_VALUE;
	}

	/* Если используется PLL в качестве источника тактового сигнала для SYSCLK */
	if(RCC_SYSCLK_SOURCE == RCC_SYSCLK_SOURCE_PLL)
	{
		RCC_Osc_InitStruct.PLL.PLL_Source = RCC_PLLSOURCE;
		RCC_Osc_InitStruct.PLL.PLL_PREDIV = RCC_PLL_PREDIV;
		RCC_Osc_InitStruct.PLL.PLL_MUL = RCC_PLL_MUL;
		RCC_Osc_InitStruct.PLL.PLL_State = RCC_PLL_ON;
	}

	/* Сбрасываем настройки тактирования к дефолту */
	MY_RCC_System_DeInit();

	/* Производим настройку осциллятора */
	if(MY_RCC_Osc_Config(&RCC_Osc_InitStruct)!= MY_Result_Ok)
	{
		return MY_Result_Error;
	}

	/* Настраиваем инициализируемые шины CPU, AHB и APB*/
	RCC_Clock_InitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_Clock_InitStruct.SYSCLK_Source = RCC_SYSCLK_SOURCE;
	RCC_Clock_InitStruct.AHBCLK_Divider = RCC_AHB_DIV;
	RCC_Clock_InitStruct.APB1CLK_Divider = RCC_APB1_DIV;

	/* Инициализируем тактирование шин CPU, AHB и APB */
	if(MY_RCC_Clock_Config(&RCC_Clock_InitStruct, FLASH_LATENCY) != MY_Result_Ok)
	{
		return MY_Result_Error;
	}

	return MY_Result_Ok;

}


void MY_RCC_System_DeInit(void)
{
	/* Устанавливаем бит HSION, HSITRIM[4:0] к значениям устанавливаемым при сбросе */
	SET_BIT(RCC->CR, RCC_CR_HSION | RCC_CR_HSITRIM_4);

	/* Сбрасываем биты SW[1:0], HPRE[3:0], PPRE[2:0] и MCOSEL[2:0] */
	CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW | RCC_CFGR_HPRE | RCC_CFGR_PPRE | RCC_CFGR_MCO);

	/* Сбрасываем биты HSEON, CSSON, PLLON */
	CLEAR_BIT(RCC->CR, RCC_CR_PLLON | RCC_CR_CSSON | RCC_CR_HSEON);

	/* Сбрасываем бит HSEBYP */
	CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

	/* Сбрасываем регистр CFGR */
	CLEAR_REG(RCC->CFGR);

	/* Сбрасываем регистр CFGR2 */
	CLEAR_REG(RCC->CFGR2);

	/* Сбрасываем регистр CFGR3 */
	CLEAR_REG(RCC->CFGR3);

	/* Отключаем все прерывания */
	CLEAR_REG(RCC->CIR);

	/* Обновление глобальной переменной SystemCoreClock */
	SystemCoreClock = HSI_VALUE;
}


MY_Result_t	MY_RCC_Osc_Config(MY_RCC_Osc_Init_t *RCC_Osc_InitStruct)
{
	/* Переменная используемая для отсчёта таймаутов */
	uint32_t tickstart = 0U;

	/*------------------------------- Настройка HSE -------------------------------*/
	/* Если для настройки выбран осциллятор HSE */
	if(((RCC_Osc_InitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE)
	{
		/* Когда HSE используется в качестве системного источника тактирования или источником тактового сигнала для PLL когда он используется в качестве основного осциллятора */
		if((MY_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSE) || \
		  ((MY_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && (MY_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSE)))
		{
			/* Если он используется и до этого не был включен */
			if((MY_RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET) && (RCC_Osc_InitStruct->HSE_State == RCC_HSE_OFF))
			{
				return MY_Result_Error;
			}
		}
		else
		{
			/* Устанавливаем новое состояние для HSE */
			MY_RCC_HSE_CONFIG(RCC_Osc_InitStruct->HSE_State);

			/* Проверяем состояние HSE в структуре */
			if(RCC_Osc_InitStruct->HSE_State != RCC_HSE_OFF)
			{
				/* Помещаем значение для отчёта таймаута */
				tickstart = MY_SysTick_GetTick();

				/* Ждём пока RCC взведёт флаг HSE ready */
				while(MY_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
				{
					/* Если истёк таймаут - выходим с ошибкой */
					if((MY_SysTick_GetTick() - tickstart) > HSE_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}
			}
			else
			{
				/* Помещаем значение для отчёта таймаута */
				tickstart = MY_SysTick_GetTick();

				/* Ждём пока RCC снимет флаг HSE ready */
				while(MY_RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET)
				{
					/* Если истёк таймаут - выходим с ошибкой */
					if((MY_SysTick_GetTick() - tickstart) > HSE_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}
			}
		}
	}

	/*----------------------------- Настройка HSI -----------------------------*/
	/* Если для настройки выбран внутренний осциллятор HSI */
	if(((RCC_Osc_InitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI)
	{
		/* Когда HSI используется в качестве системного источника тактирования или источником тактового сигнала для PLL когда он используется в качестве основного осциллятора */
		if((MY_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSI) ||
		  ((MY_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && (MY_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI)))
		{
			/* В этом случае запрещено его выключать и дальнейшая настройка не возможна */
			if((MY_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET) && (RCC_Osc_InitStruct->HSI_State != RCC_HSI_ON))
			{
				return MY_Result_Error;
			}
			/* Иначе доступна только калибровка */
			else
			{
				/* Подстраиваем значение калибровки High Speed oscillator (HSI) */
				MY_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_Osc_InitStruct->HSI_CalibrationValue);
			}
		}
		else
		{
			/* Проверяем состояние HSI в структуре */
			if(RCC_Osc_InitStruct->HSI_State != RCC_HSI_OFF)
			{
				/* Включаем внутренний High Speed oscillator (HSI) */
				MY_RCC_HSI_ENABLE();

				/* Помещаем значение для отчёта таймаута */
				tickstart = MY_SysTick_GetTick();

				/* Ждём пока RCC не взведёт флаг готовности HSI */
				while(MY_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
				{
					/* Если истёк таймаут - выходим с ошибкой */
					if((MY_SysTick_GetTick() - tickstart ) > HSI_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}

				/* Подстраиваем значение калибровки High Speed oscillator (HSI) */
				MY_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_Osc_InitStruct->HSI_CalibrationValue);
			}
			else
			{
				/* Отключаем High Speed oscillator (HSI) */
				MY_RCC_HSI_DISABLE();

				/* Помещаем значение для отчёта таймаута */
				tickstart = MY_SysTick_GetTick();

				/* Ждём пока RCC не снимет флаг готовности HSI */
				while(MY_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET)
				{
					/* Если истёк таймаут - выходим с ошибкой */
					if((MY_SysTick_GetTick() - tickstart ) > HSI_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}
			}
		}
	}


	/*------------------------------ Конфигурация LSI ------------------------------*/
	/* Если для настройки выбран осциллятор LSI */
	if(((RCC_Osc_InitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI)
	{
		/* Проверяем текущее состояние LSI в настройках */
		if(RCC_Osc_InitStruct->LSI_State != RCC_LSI_OFF)
		{
			/* Включаем встроенный Low Speed oscillator (LSI) */
			MY_RCC_LSI_ENABLE();

			/* Помещаем значение для отчёта таймаута */
			tickstart = MY_SysTick_GetTick();

			/* Ждём пока RCC не взведёт флаг готовности LSI */
			while(MY_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
			{
				/* Если истёк таймаут - выходим с ошибкой */
				if((MY_SysTick_GetTick() - tickstart ) > LSI_TIMEOUT_VALUE)
				{
					return MY_Result_Timeout;
				}
			}
		}
		else
		{
			/* Отключаем встроенный Low Speed oscillator (LSI) */
			MY_RCC_LSI_DISABLE();

			/* Помещаем значение для отчёта таймаута */
			tickstart = MY_SysTick_GetTick();

			/* Ждём пока RCC не снимет флаг готовности LSI */
			while(MY_RCC_GET_FLAG(RCC_FLAG_LSIRDY) != RESET)
			{
				/* Если истёк таймаут - выходим с ошибкой */
				if((MY_SysTick_GetTick() - tickstart ) > LSI_TIMEOUT_VALUE)
				{
					return MY_Result_Timeout;
				}
			}
		}
	}


	/*------------------------------ Конфигурация LSE ------------------------------*/
	/* Если для настройки выбран осциллятор LSE */
	if(((RCC_Osc_InitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE)
	{
		/* Статус сброса по питанию */
		FlagStatus pwrclkchanged = RESET;

		/* Обновляем конфигурацию LSE в регистре Backup Domain control */
		/* Это необходимо для включения доступа на запись в Backup Domain */
		if(MY_RCC_PWR_IS_CLK_DISABLED())
		{
			MY_RCC_PWR_CLK_ENABLE();
			pwrclkchanged = SET;
		}

		/* Если бит Disable Backup Domain write protection сброшен*/
		if(IS_BIT_CLR(PWR->CR, PWR_CR_DBP))
		{
			/* Разрешаем запись в Backup domain */
			SET_BIT(PWR->CR, PWR_CR_DBP);

			/* Помещаем значение для отчёта таймаута */
			tickstart = MY_SysTick_GetTick();

			/* Ждём когда защита от записи в Backup domain будет снята */
			while((((PWR->CR) & (PWR_CR_DBP)) == RESET))
			{
				/* Если истёк таймаут - выходим с ошибкой */
				if((MY_SysTick_GetTick() - tickstart) > RCC_DBP_TIMEOUT_VALUE)
				{
					return MY_Result_Timeout;
				}
			}
		}

		/* Устанавливаем новое состояние LSE */
		MY_RCC_LSE_CONFIG(RCC_Osc_InitStruct->LSE_State);

		/* Проверяем новое состояние LSE */
		if(RCC_Osc_InitStruct->LSE_State != RCC_LSE_OFF)
		{
			/* Помещаем значение для отчёта таймаута */
			tickstart = MY_SysTick_GetTick();

			/* Ждём когда не будет взведен флаг готовности LSE */
			while(MY_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
			{
				/* Если истёк таймаут - выходим с ошибкой */
				if((MY_SysTick_GetTick() - tickstart ) > RCC_LSE_TIMEOUT_VALUE)
				{
					return MY_Result_Timeout;
				}
			}
		}
		else
		{
			/* Помещаем значение для отчёта таймаута */
			tickstart = MY_SysTick_GetTick();

			/* Ждём когда не будет взведен флаг готовности LSE */
			while(MY_RCC_GET_FLAG(RCC_FLAG_LSERDY) != RESET)
			{
				/* Если истёк таймаут - выходим с ошибкой */
				if((MY_SysTick_GetTick() - tickstart ) > RCC_LSE_TIMEOUT_VALUE)
				{
					return MY_Result_Timeout;
				}
			}
		}

		/* Требуется отключить тактирование power clock */
		if(pwrclkchanged == SET)
		{
			MY_RCC_PWR_CLK_DISABLE();
		}
	}


	/*----------------------------- Настройка HSI14 -----------------------------*/
	/* Если для настройки выбран осциллятор HSI14 */
	if(((RCC_Osc_InitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI14) == RCC_OSCILLATORTYPE_HSI14)
	{
		/* Проверка текущего состояния HSI14 в настройках */
		if(RCC_Osc_InitStruct->HSI14_State == RCC_HSI14_ON)
		{
			/* Отключаем ADC control для HSI14 */
			MY_RCC_HSI14ADC_DISABLE();

			/* Включаем HSI14 */
			MY_RCC_HSI14_ENABLE();

			/* Помещаем значение для отчёта таймаута */
			tickstart = MY_SysTick_GetTick();

			/* Ждём когда не будет взведен флаг готовности HSI14 */
			while(MY_RCC_GET_FLAG(RCC_FLAG_HSI14RDY) == RESET)
			{
				/* Если истёк таймаут - выходим с ошибкой */
				if((MY_SysTick_GetTick() - tickstart) > HSI14_TIMEOUT_VALUE)
				{
					return MY_Result_Timeout;
				}
			}

			/* Подстраиваем калибровочное значение для Internal High Speed oscillator 14Mhz (HSI14) */
			MY_RCC_HSI14_CALIBRATIONVALUE_ADJUST(RCC_Osc_InitStruct->HSI14_CalibrationValue);
		}
		else if(RCC_Osc_InitStruct->HSI14_State == RCC_HSI14_ADC_CONTROL)
		{
			/* Включаем ADC для Internal High Speed oscillator HSI14 */
			MY_RCC_HSI14ADC_ENABLE();

			/* Подстраиваем калибровочное значение для Internal High Speed oscillator 14Mhz (HSI14) */
			MY_RCC_HSI14_CALIBRATIONVALUE_ADJUST(RCC_Osc_InitStruct->HSI14_CalibrationValue);
		}
		else
		{
			/* Отключаем ADC control для HSI14 */
			MY_RCC_HSI14ADC_DISABLE();

			/* Отключаем HSI14 */
			MY_RCC_HSI14_DISABLE();

			/* Помещаем значение для отчёта таймаута */
			tickstart = MY_SysTick_GetTick();

			/* Ждём когда не будет снят флаг готовности HSI14 */
			while(MY_RCC_GET_FLAG(RCC_FLAG_HSI14RDY) != RESET)
			{
				/* Если истёк таймаут - выходим с ошибкой */
				if((MY_SysTick_GetTick() - tickstart) > HSI14_TIMEOUT_VALUE)
				{
					return MY_Result_Timeout;
				}
			}
		}
	}

	#if defined(RCC_HSI48_SUPPORT)

	/*----------------------------- Настройка HSI48 -----------------------------*/
	/* Если необходимо настроить HSI48 */
	if(((RCC_Osc_InitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI48) == RCC_OSCILLATORTYPE_HSI48)
	{
		/* Когда используется HSI48 в качестве источника системного тактирования он не может быть отключен */
		if((MY_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSI48) ||
		  ((MY_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && (MY_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI48)))
		{
			/* Если флаг готовности взведен но не включен в настройках структуры - возвращаем ошибку */
			if((MY_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) != RESET) && (RCC_Osc_InitStruct->HSI48_State != RCC_HSI48_ON))
			{
				return MY_Result_Error;
			}
		}
		else
		{
			/* Проверяем состояние HSI48 в настройках структуры */
			if(RCC_Osc_InitStruct->HSI48_State != RCC_HSI48_OFF)
			{
				/* Включаем HSI48 */
				MY_RCC_HSI48_ENABLE();

				/* Помещаем значение для отчёта таймаута */
				tickstart = MY_SysTick_GetTick();

				/* Ждём когда не будет взведен флаг готовности HSI48 */
				while(MY_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) == RESET)
				{
					/* Если истёк таймаут - выходим с ошибкой */
					if((MY_SysTick_GetTick() - tickstart) > HSI48_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}
			}
			else
			{
				/* Отключаем HSI48 */
				MY_RCC_HSI48_DISABLE();

				/* Помещаем значение для отчёта таймаута */
				tickstart = MY_SysTick_GetTick();

				/* Ждём когда не будет снят флаг готовности HSI48 */
				while(MY_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) != RESET)
				{
					/* Если истёк таймаут - выходим с ошибкой */
					if((MY_SysTick_GetTick() - tickstart) > HSI48_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}
			}
		}
	}
	#endif

	/*-------------------------------- Настройка PLL --------------------------------*/
	/* Если в качестве настраиваемого осциллятора выбран PLL */
	if ((RCC_Osc_InitStruct->PLL.PLL_State) != RCC_PLL_NONE)
	{
		/* Проверяем, является ли PLL источником системного тактирования или нет*/
		if(MY_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
		{
			/* Если в структуре PLL запущен */
			if((RCC_Osc_InitStruct->PLL.PLL_State) == RCC_PLL_ON)
			{
				/* Сначана отключаем PLL */
				MY_RCC_PLL_DISABLE();

				/* Помещаем значение для отчёта таймаута */
				tickstart = MY_SysTick_GetTick();

				/* Ждём когда не будет снят флаг готовности PLL */
				while(MY_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  != RESET)
				{
					/* Если истёк таймаут - выходим с ошибкой */
					if((MY_SysTick_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}

				/* Настраиваем тактирование PLL, предделитель и умножитель */
				MY_RCC_PLL_CONFIG(RCC_Osc_InitStruct->PLL.PLL_Source, RCC_Osc_InitStruct->PLL.PLL_PREDIV, RCC_Osc_InitStruct->PLL.PLL_MUL);

				/* Включаем PLL обратно */
				MY_RCC_PLL_ENABLE();

				/* Помещаем значение для отчёта таймаута */
				tickstart = MY_SysTick_GetTick();

				/* Ждём когда не будет взведен флаг готовности PLL */
				while(MY_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
				{
					/* Если истёк таймаут - выходим с ошибкой */
					if((MY_SysTick_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}
			}
			else
			{
				/* Отключаем PLL */
				MY_RCC_PLL_DISABLE();

				/* Помещаем значение для отчёта таймаута */
				tickstart = MY_SysTick_GetTick();

				/* Ждём когда не будет снят флаг готовности PLL */
				while(MY_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  != RESET)
				{
					/* Если истёк таймаут - выходим с ошибкой */
					if((MY_SysTick_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}
			}
		}
		else
		{
			return MY_Result_Error;
		}
	}

	return MY_Result_Ok;
}


MY_Result_t MY_RCC_Clock_Config(MY_RCC_Clock_Init_t *RCC_Clock_InitStruct, uint32_t FlashLatency)
{
	/* Переменная используемая для отсчёта таймаутов */
	uint32_t tickstart = 0U;

	/* Для корректного чтения данных с FLASH памяти, число периодов ожидания (LATENCY)
	   должно быть корректно установлено в соответствии с частотой CPU (HCLK) на используемом МК.
	   Увеличивается количество периодов ожидания при использовании высокой частоты CPU (от 24MHz) */
	if(FLASH_LATENCY > (FLASH->ACR & FLASH_ACR_LATENCY))
	{
		/* Записываем новое количество периодов ожидания в бит LATENCY в регистре FLASH_ACR */
		FLASH->ACR = (FLASH->ACR&(~FLASH_ACR_LATENCY)) | (FlashLatency);

		/* Проверяем установилось ли значение в поле LATENCY в регистре FLASH_ACR */
		if((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_LATENCY)
		{
			return MY_Result_Error;
		}
	}


	/*-------------------------- Настройка HCLK --------------------------*/
	if(((RCC_Clock_InitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
	{
		MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_Clock_InitStruct->AHBCLK_Divider);
	}


	/*------------------------- Настройка SYSCLK ---------------------------*/
	if(((RCC_Clock_InitStruct->ClockType) & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
	{
		/* Если HSE выбран как System Clock Source */
		if(RCC_Clock_InitStruct->SYSCLK_Source == RCC_SYSCLK_SOURCE_HSE)
		{
			/* Проверяем установлен ли флаг HSE ready */
			if(MY_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
			{
				return MY_Result_Error;
			}
		}
		/* Если PLL выбран как System Clock Source */
		else if(RCC_Clock_InitStruct->SYSCLK_Source == RCC_SYSCLK_SOURCE_PLL)
		{
			/* Проверяем установлен ли флаг PLL ready */
			if(MY_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
			{
				return MY_Result_Error;
			}
		}

		/* Если в системе есть HSI48 */
		#if defined(RCC_CFGR_SWS_HSI48)

			/* Если HSI48 выбран как System Clock Source */
		else if(RCC_Clock_InitStruct->SYSCLK_Source == RCC_SYSCLKSOURCE_HSI48)
		{
			/* Проверяем установлен ли HSI48 ready */
			if(MY_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) == RESET)
			{
				return MY_Result_Error;
			}
		}

		#endif

		/* Если HSI выбран как System Clock Source */
		else
		{
			/* Проверяем установлен ли HSI ready */
			if(MY_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
			{
				return MY_Result_Error;
			}
		}

		/* Устанавливаем выбранный источник System Clock Source */
		MY_RCC_SYSCLK_CONFIG(RCC_Clock_InitStruct->SYSCLK_Source);


		/* Получаем значение для вычисления таймаута на выполнение */
		tickstart = MY_SysTick_GetTick();

		/* Проверяем, установилось ли значение в регистре */
		if(RCC_Clock_InitStruct->SYSCLK_Source == RCC_SYSCLK_SOURCE_HSE)
		{
			while (MY_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSE)
			{
				if((MY_SysTick_GetTick() - tickstart ) > (5000U))
				{
					return MY_Result_Timeout;
				}
			}
		}
		else if(RCC_Clock_InitStruct->SYSCLK_Source == RCC_SYSCLK_SOURCE_PLL)
		{
			while (MY_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
			{
				if((MY_SysTick_GetTick() - tickstart ) > (5000U))
				{
					return MY_Result_Timeout;
				}
			}
		}

		#if defined(RCC_CFGR_SWS_HSI48)

		else if(RCC_Clock_InitStruct->SYSCLK_Source == RCC_SYSCLKSOURCE_HSI48)
		{
			while (MY_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSI48)
			{
				if((MY_SysTick_GetTick() - tickstart) > CLOCKSWITCH_TIMEOUT_VALUE)
				{
					return MY_Result_Timeout;
				}
			}
		}

		#endif

		else
		{
			while (MY_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSI)
			{
				if((MY_SysTick_GetTick() - tickstart ) > (5000U))
				{
					return MY_Result_Timeout;
				}
			}
		}
	}

	/* Если используется частота <24МГц - записываем значение предвыборки */
	if(FLASH_LATENCY < (FLASH->ACR & FLASH_ACR_LATENCY))
	{
		/* Записываем новое значение LATENCY в регистр FLASH_ACR */
		FLASH->ACR = (FLASH->ACR&(~FLASH_ACR_LATENCY)) | (FLASH_LATENCY);

		/* Проверяем установилось ли значение в регистре */
		if((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_LATENCY)
		{
			return MY_Result_Error;
		}
	}


	/*-------------------------- Настройка PCLK1  ---------------------------*/
	if(((RCC_Clock_InitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
	{
		MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE, RCC_Clock_InitStruct->APB1CLK_Divider);
	}

	/* Обновляем значение глобальной переменной SystemCoreClock */
	SystemCoreClock = MY_RCC_SysClock_GetFreq() >> AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE)>> RCC_CFGR_HPRE_BITNUMBER];

	/* Устанавливаем значение для SysTick таймера, чтобы получить ровно 1 мс. */
	MY_SysTick_Init(SystemCoreClock/1000U, 0U);

	return MY_Result_Ok;

}


MY_Result_t MY_RCC_PeriphClock_Config(MY_RCC_PeriphClock_Init_t *RCC_PeriphClock_InitStruct)
{
	uint32_t tickstart = 0U;
	uint32_t temp_reg = 0U;

	/*---------------------------- Настройка RTC -------------------------------*/
	if(((RCC_PeriphClock_InitStruct->PeriphClockSelection) & RCC_PERIPHCLK_RTC) == (RCC_PERIPHCLK_RTC))
	{
		FlagStatus pwrclkchanged = RESET;

		/* As soon as function is called to change RTC clock source, activation of the power domain is done. */
		/* Requires to enable write access to Backup Domain of necessary */
		if(MY_RCC_PWR_IS_CLK_DISABLED())
		{
			MY_RCC_PWR_CLK_ENABLE();
			pwrclkchanged = SET;
		}

		if(IS_BIT_CLR(PWR->CR, PWR_CR_DBP))
		{
			/* Enable write access to Backup domain */
			SET_BIT(PWR->CR, PWR_CR_DBP);

			/* Wait for Backup domain Write protection disable */
			tickstart = MY_SysTick_GetTick();

			while(IS_BIT_CLR(PWR->CR, PWR_CR_DBP))
			{
				if((MY_SysTick_GetTick() - tickstart) > RCC_DBP_TIMEOUT_VALUE)
				{
					return MY_Result_Timeout;
				}
			}
		}

		/* Reset the Backup domain only if the RTC Clock source selection is modified from reset value */
		temp_reg = (RCC->BDCR & RCC_BDCR_RTCSEL);

		if((temp_reg != 0x00000000U) && (temp_reg != (RCC_PeriphClock_InitStruct->RTC_ClockSelection & RCC_BDCR_RTCSEL)))
		{
			/* Store the content of BDCR register before the reset of Backup Domain */
			temp_reg = (RCC->BDCR & ~(RCC_BDCR_RTCSEL));

			/* RTC Clock selection can be changed only if the Backup Domain is reset */
			MY_RCC_BACKUPRESET_FORCE();
			MY_RCC_BACKUPRESET_RELEASE();

			/* Restore the Content of BDCR register */
			RCC->BDCR = temp_reg;

			/* Wait for LSERDY if LSE was enabled */
			if (IS_BIT_SET(temp_reg, RCC_BDCR_LSEON))
			{
				/* Get Start Tick */
				tickstart = MY_SysTick_GetTick();

				/* Wait till LSE is ready */
				while(MY_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
				{
					if((MY_SysTick_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
					{
						return MY_Result_Timeout;
					}
				}
			}
		}

		MY_RCC_RTC_CONFIG(RCC_PeriphClock_InitStruct->RTC_ClockSelection);

		/* Require to disable power clock if necessary */
		if(pwrclkchanged == SET)
		{
			MY_RCC_PWR_CLK_DISABLE();
		}
	}

	/*------------------------------- USART1 Configuration ------------------------*/
	if(((RCC_PeriphClock_InitStruct->PeriphClockSelection) & RCC_PERIPHCLK_USART1) == RCC_PERIPHCLK_USART1)
	{
		/* Configure the USART1 clock source */
		MY_RCC_USART1_CONFIG(RCC_PeriphClock_InitStruct->USART1_ClockSelection);
	}


	#if defined(STM32F071xB) || \
		defined(STM32F072xB) || \
		defined(STM32F078xx) || \
		defined(STM32F091xC) || \
		defined(STM32F098xx)

	/*----------------------------- USART2 Configuration --------------------------*/
	if(((RCC_PeriphClock_InitStruct->PeriphClockSelection) & RCC_PERIPHCLK_USART2) == RCC_PERIPHCLK_USART2)
	{
		/* Configure the USART2 clock source */
		MY_RCC_USART2_CONFIG(RCC_PeriphClock_InitStruct->USART2_ClockSelection);
	}

	#endif


	#if defined(STM32F091xC) || \
		defined(STM32F098xx)

	/*----------------------------- USART3 Configuration --------------------------*/
	if(((RCC_PeriphClock_InitStruct->PeriphClockSelection) & RCC_PERIPHCLK_USART3) == RCC_PERIPHCLK_USART3)
	{
		/* Configure the USART3 clock source */
		MY_RCC_USART3_CONFIG(RCC_PeriphClock_InitStruct->USART3_ClockSelection);
	}

	#endif


	/*------------------------------ I2C1 Configuration ------------------------*/
	if(((RCC_PeriphClock_InitStruct->PeriphClockSelection) & RCC_PERIPHCLK_I2C1) == RCC_PERIPHCLK_I2C1)
	{
		/* Configure the I2C1 clock source */
		MY_RCC_I2C1_CONFIG(RCC_PeriphClock_InitStruct->I2C1_ClockSelection);
	}


	#if defined(STM32F042x6) || \
		defined(STM32F048xx) || \
		defined(STM32F072xB) || \
		defined(STM32F078xx) || \
		defined(STM32F070xB) || \
		defined(STM32F070x6)

	/*------------------------------ USB Configuration ------------------------*/
	if(((RCC_PeriphClock_InitStruct->PeriphClockSelection) & RCC_PERIPHCLK_USB) == RCC_PERIPHCLK_USB)
	{
		/* Configure the USB clock source */
		MY_RCC_USB_CONFIG(RCC_PeriphClock_InitStruct->USB_ClockSelection);
	}

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

	/*------------------------------ CEC clock Configuration -------------------*/
	if(((RCC_PeriphClock_InitStruct->PeriphClockSelection) & RCC_PERIPHCLK_CEC) == RCC_PERIPHCLK_CEC)
	{
		/* Configure the CEC clock source */
		MY_RCC_CEC_CONFIG(RCC_PeriphClock_InitStruct->CEC_ClockSelection);
	}

	#endif

	return MY_Result_Ok;
}


void MY_RCC_PeriphClock_GetConfig(MY_RCC_PeriphClock_Init_t *RCC_PeriphClock_InitStruct)
{
	/* Set all possible values for the extended clock type parameter------------*/
	/* Common part first */
	RCC_PeriphClock_InitStruct->PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1   | RCC_PERIPHCLK_RTC;

	/* Get the RTC configuration --------------------------------------------*/
	RCC_PeriphClock_InitStruct->RTC_ClockSelection = MY_RCC_GET_RTC_SOURCE();

	/* Get the USART1 clock configuration --------------------------------------------*/
	RCC_PeriphClock_InitStruct->USART1_ClockSelection = MY_RCC_GET_USART1_SOURCE();

	/* Get the I2C1 clock source -----------------------------------------------*/
	RCC_PeriphClock_InitStruct->I2C1_ClockSelection = MY_RCC_GET_I2C1_SOURCE();


	#if defined(STM32F071xB) || \
		defined(STM32F072xB) || \
		defined(STM32F078xx) || \
		defined(STM32F091xC) || \
		defined(STM32F098xx)

		RCC_PeriphClock_InitStruct->PeriphClockSelection |= RCC_PERIPHCLK_USART2;

		/* Get the USART2 clock source ---------------------------------------------*/
		RCC_PeriphClock_InitStruct->USART2_ClockSelection = MY_RCC_GET_USART2_SOURCE();

	#endif


	#if defined(STM32F091xC) || \
		defined(STM32F098xx)

		RCC_PeriphClock_InitStruct->PeriphClockSelection |= RCC_PERIPHCLK_USART3;

		/* Get the USART3 clock source ---------------------------------------------*/
		RCC_PeriphClock_InitStruct->USART3_ClockSelection = MY_RCC_GET_USART3_SOURCE();

	#endif


	#if defined(STM32F042x6) || \
		defined(STM32F048xx) || \
		defined(STM32F072xB) || \
		defined(STM32F078xx) || \
		defined(STM32F070xB) || \
		defined(STM32F070x6)

		RCC_PeriphClock_InitStruct->PeriphClockSelection |= RCC_PERIPHCLK_USB;

		/* Get the USB clock source ---------------------------------------------*/
		RCC_PeriphClock_InitStruct->USB_ClockSelection = MY_RCC_GET_USB_SOURCE();

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

		RCC_PeriphClock_InitStruct->PeriphClockSelection |= RCC_PERIPHCLK_CEC;

		/* Get the CEC clock source ------------------------------------------------*/
		RCC_PeriphClock_InitStruct->CEC_ClockSelection = MY_RCC_GET_CEC_SOURCE();

	#endif

}


uint32_t MY_RCC_PeriphClock_GetFreq(uint32_t PeriphClock)
{
	/* frequency == 0 : means that no available frequency for the peripheral */
	uint32_t frequency = 0U;
	uint32_t srcclk = 0U;

	#if defined(USB)
		uint32_t pllmull = 0U, pllsource = 0U, predivfactor = 0U;
	#endif

	switch (PeriphClock)
	{
		case RCC_PERIPHCLK_RTC:
		{
			/* Get the current RTC source */
			srcclk = MY_RCC_GET_RTC_SOURCE();

			/* Check if LSE is ready and if RTC clock selection is LSE */
			if ((srcclk == RCC_RTCCLKSOURCE_LSE) && (IS_BIT_SET(RCC->BDCR, RCC_BDCR_LSERDY)))
			{
				frequency = LSE_VALUE;
			}
			/* Check if LSI is ready and if RTC clock selection is LSI */
			else if ((srcclk == RCC_RTCCLKSOURCE_LSI) && (IS_BIT_SET(RCC->CSR, RCC_CSR_LSIRDY)))
			{
				frequency = LSI_VALUE;
			}
			/* Check if HSE is ready  and if RTC clock selection is HSI_DIV32*/
			else if ((srcclk == RCC_RTCCLKSOURCE_HSE_DIV32) && (IS_BIT_SET(RCC->CR, RCC_CR_HSERDY)))
			{
				frequency = HSE_VALUE / 32U;
			}
			break;
		}
		case RCC_PERIPHCLK_USART1:
		{
			/* Get the current USART1 source */
			srcclk = MY_RCC_GET_USART1_SOURCE();

			/* Check if USART1 clock selection is PCLK1 */
			if (srcclk == RCC_USART1CLKSOURCE_PCLK1)
			{
				frequency = MY_RCC_PCLK1_GetFreq();
			}
			/* Check if HSI is ready and if USART1 clock selection is HSI */
			else if ((srcclk == RCC_USART1CLKSOURCE_HSI) && (IS_BIT_SET(RCC->CR, RCC_CR_HSIRDY)))
			{
				frequency = HSI_VALUE;
			}
			/* Check if USART1 clock selection is SYSCLK */
			else if (srcclk == RCC_USART1CLKSOURCE_SYSCLK)
			{
				frequency = MY_RCC_SysClock_GetFreq();
			}
			/* Check if LSE is ready  and if USART1 clock selection is LSE */
			else if ((srcclk == RCC_USART1CLKSOURCE_LSE) && (IS_BIT_SET(RCC->BDCR, RCC_BDCR_LSERDY)))
			{
				frequency = LSE_VALUE;
			}
			break;
		}

		#if defined(RCC_CFGR3_USART2SW)

		case RCC_PERIPHCLK_USART2:
		{
			/* Get the current USART2 source */
			srcclk = MY_RCC_GET_USART2_SOURCE();

			/* Check if USART2 clock selection is PCLK1 */
			if (srcclk == RCC_USART2CLKSOURCE_PCLK1)
			{
				frequency = MY_RCC_PCLK1_GetFreq();
			}
			/* Check if HSI is ready and if USART2 clock selection is HSI */
			else if ((srcclk == RCC_USART2CLKSOURCE_HSI) && (IS_BIT_SET(RCC->CR, RCC_CR_HSIRDY)))
			{
				frequency = HSI_VALUE;
			}
			/* Check if USART2 clock selection is SYSCLK */
			else if (srcclk == RCC_USART2CLKSOURCE_SYSCLK)
			{
				frequency = MY_RCC_SysClock_GetFreq();
			}
			/* Check if LSE is ready  and if USART2 clock selection is LSE */
			else if ((srcclk == RCC_USART2CLKSOURCE_LSE) && (IS_BIT_SET(RCC->BDCR, RCC_BDCR_LSERDY)))
			{
				frequency = LSE_VALUE;
			}
			break;
		}

		#endif


		#if defined(RCC_CFGR3_USART3SW)

		case RCC_PERIPHCLK_USART3:
		{
			/* Get the current USART3 source */
			srcclk = MY_RCC_GET_USART3_SOURCE();

			/* Check if USART3 clock selection is PCLK1 */
			if (srcclk == RCC_USART3CLKSOURCE_PCLK1)
			{
				frequency = MY_RCC_PCLK1_GetFreq();
			}
			/* Check if HSI is ready and if USART3 clock selection is HSI */
			else if ((srcclk == RCC_USART3CLKSOURCE_HSI) && (IS_BIT_SET(RCC->CR, RCC_CR_HSIRDY)))
			{
				frequency = HSI_VALUE;
			}
			/* Check if USART3 clock selection is SYSCLK */
			else if (srcclk == RCC_USART3CLKSOURCE_SYSCLK)
			{
				frequency = MY_RCC_SysClock_GetFreq();
			}
			/* Check if LSE is ready  and if USART3 clock selection is LSE */
			else if ((srcclk == RCC_USART3CLKSOURCE_LSE) && (IS_BIT_SET(RCC->BDCR, RCC_BDCR_LSERDY)))
			{
				frequency = LSE_VALUE;
			}
			break;
		}

		#endif

		case RCC_PERIPHCLK_I2C1:
		{
			/* Get the current I2C1 source */
			srcclk = MY_RCC_GET_I2C1_SOURCE();

			/* Check if HSI is ready and if I2C1 clock selection is HSI */
			if ((srcclk == RCC_I2C1CLKSOURCE_HSI) && (IS_BIT_SET(RCC->CR, RCC_CR_HSIRDY)))
			{
				frequency = HSI_VALUE;
			}
			/* Check if I2C1 clock selection is SYSCLK */
			else if (srcclk == RCC_I2C1CLKSOURCE_SYSCLK)
			{
				frequency = MY_RCC_SysClock_GetFreq();
			}
			break;
		}

		#if defined(USB)

		case RCC_PERIPHCLK_USB:
		{
			/* Get the current USB source */
			srcclk = MY_RCC_GET_USB_SOURCE();

			/* Check if PLL is ready and if USB clock selection is PLL */
			if ((srcclk == RCC_USBCLKSOURCE_PLL) && (IS_BIT_SET(RCC->CR, RCC_CR_PLLRDY)))
			{
				/* Get PLL clock source and multiplication factor ----------------------*/
				pllmull      = RCC->CFGR & RCC_CFGR_PLLMUL;
				pllsource    = RCC->CFGR & RCC_CFGR_PLLSRC;
				pllmull      = (pllmull >> RCC_CFGR_PLLMUL_BITNUMBER) + 2U;
				predivfactor = (RCC->CFGR2 & RCC_CFGR2_PREDIV) + 1U;

				if (pllsource == RCC_CFGR_PLLSRC_HSE_PREDIV)
				{
					/* HSE used as PLL clock source : frequency = HSE/PREDIV * PLLMUL */
					frequency = (HSE_VALUE/predivfactor) * pllmull;
				}

				#if defined(RCC_CR2_HSI48ON)

				else if (pllsource == RCC_CFGR_PLLSRC_HSI48_PREDIV)
				{
					/* HSI48 used as PLL clock source : frequency = HSI48/PREDIV * PLLMUL */
					frequency = (HSI48_VALUE / predivfactor) * pllmull;
				}

				#endif

				else
				{
					#if defined(STM32F042x6) || \
						defined(STM32F048xx) || \
						defined(STM32F078xx) || \
						defined(STM32F072xB) || \
						defined(STM32F070xB)

						/* HSI used as PLL clock source : frequency = HSI/PREDIV * PLLMUL */
						frequency = (HSI_VALUE / predivfactor) * pllmull;

					#else

						/* HSI used as PLL clock source : frequency = HSI/2U * PLLMUL */
						frequency = (HSI_VALUE >> 1U) * pllmull;

					#endif
				}
			}

			#if defined(RCC_CR2_HSI48ON)

			/* Check if HSI48 is ready and if USB clock selection is HSI48 */
			else if ((srcclk == RCC_USBCLKSOURCE_HSI48) && (IS_BIT_SET(RCC->CR2, RCC_CR2_HSI48RDY)))
			{
				frequency = HSI48_VALUE;
			}

			#endif

			break;

		}

		#endif


		#if defined(CEC)

		case RCC_PERIPHCLK_CEC:
		{
			/* Get the current CEC source */
			srcclk = MY_RCC_GET_CEC_SOURCE();

			/* Check if HSI is ready and if CEC clock selection is HSI */
			if ((srcclk == RCC_CECCLKSOURCE_HSI) && (IS_BIT_SET(RCC->CR, RCC_CR_HSIRDY)))
			{
				frequency = HSI_VALUE;
			}
			/* Check if LSE is ready  and if CEC clock selection is LSE */
			else if ((srcclk == RCC_CECCLKSOURCE_LSE) && (IS_BIT_SET(RCC->BDCR, RCC_BDCR_LSERDY)))
			{
				frequency = LSE_VALUE;
			}
			break;
		}

		#endif

		default:
		{
			break;
		}
	}

	return(frequency);

}


uint32_t MY_RCC_SysClock_GetFreq(void)
{
	const uint8_t PLLMULFactorTable[16] = { 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 16U};
	const uint8_t PLLPREDIVFactorTable[16] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U,  10U, 11U, 12U, 13U, 14U, 15U, 16U};

	uint32_t tmpreg = 0U, prediv = 0U, pllclk = 0U, pllmul = 0U;
	uint32_t sysclockfreq = 0U;

	tmpreg = RCC->CFGR;

	/* Get SYSCLK source -------------------------------------------------------*/
	switch (tmpreg & RCC_CFGR_SWS)
	{
		case RCC_SYSCLKSOURCE_STATUS_HSE:  /* HSE used as system clock */
		{
			sysclockfreq = HSE_VALUE;
			break;
		}
		case RCC_SYSCLKSOURCE_STATUS_PLLCLK:  /* PLL used as system clock */
		{
			pllmul = PLLMULFactorTable[(uint32_t)(tmpreg & RCC_CFGR_PLLMUL) >> RCC_CFGR_PLLMUL_BITNUMBER];
			prediv = PLLPREDIVFactorTable[(uint32_t)(RCC->CFGR2 & RCC_CFGR2_PREDIV) >> RCC_CFGR2_PREDIV_BITNUMBER];

			if ((tmpreg & RCC_CFGR_PLLSRC) == RCC_PLLSOURCE_HSE)
			{
				/* HSE used as PLL clock source : PLLCLK = HSE/PREDIV * PLLMUL */
				pllclk = (HSE_VALUE / prediv) * pllmul;
			}

			#if defined(RCC_CFGR_PLLSRC_HSI48_PREDIV)

			else if ((tmpreg & RCC_CFGR_PLLSRC) == RCC_PLLSOURCE_HSI48)
			{
				/* HSI48 used as PLL clock source : PLLCLK = HSI48/PREDIV * PLLMUL */
				pllclk = (HSI48_VALUE / prediv) * pllmul;
			}

			#endif

			else
			{

				#if defined(STM32F042x6) || \
					defined(STM32F048xx) || \
					defined(STM32F070x6) || \
					defined(STM32F071xB) || \
					defined(STM32F072xB) || \
					defined(STM32F078xx) || \
					defined(STM32F070xB) || \
					defined(STM32F091xC) || \
					defined(STM32F098xx) || \
					defined(STM32F030xC)

					/* HSI used as PLL clock source : PLLCLK = HSI/PREDIV * PLLMUL */
					pllclk = (HSI_VALUE / prediv) * pllmul;

				#else

					/* HSI used as PLL clock source : PLLCLK = HSI/2 * PLLMUL */
					pllclk = (uint32_t)((HSI_VALUE >> 1U) * pllmul);

				#endif

			}

			sysclockfreq = pllclk;

			break;
		}


		#if defined(RCC_CFGR_SWS_HSI48)

		case RCC_SYSCLKSOURCE_STATUS_HSI48:    /* HSI48 used as system clock source */
		{
			sysclockfreq = HSI48_VALUE;
			break;
		}

		#endif


		case RCC_SYSCLKSOURCE_STATUS_HSI:  /* HSI used as system clock source */


		default: /* HSI used as system clock */
		{
			sysclockfreq = HSI_VALUE;
			break;
		}
	}

	return sysclockfreq;
}


uint32_t MY_RCC_HCLK_GetFreq(void)
{
	return SystemCoreClock;
}


uint32_t MY_RCC_PCLK1_GetFreq(void)
{
	/* Get HCLK source and Compute PCLK1 frequency */
	return (MY_RCC_HCLK_GetFreq() >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE) >> RCC_CFGR_PPRE_BITNUMBER]);
}


void MY_RCC_Osc_GetConfig(MY_RCC_Osc_Init_t  *RCC_Osc_InitStruct)
{
	/* Set all possible values for the Oscillator type parameter ---------------*/
	RCC_Osc_InitStruct->OscillatorType = 	RCC_OSCILLATORTYPE_HSE | \
											RCC_OSCILLATORTYPE_HSI | \
											RCC_OSCILLATORTYPE_LSE | \
											RCC_OSCILLATORTYPE_LSI | \
											RCC_OSCILLATORTYPE_HSI14;
	#if defined(RCC_HSI48_SUPPORT)

	RCC_OscInitStruct->OscillatorType |= RCC_OSCILLATORTYPE_HSI48;

	#endif


	/* Get the HSE configuration -----------------------------------------------*/
	if((RCC->CR & RCC_CR_HSEBYP) == RCC_CR_HSEBYP)
	{
		RCC_Osc_InitStruct->HSE_State = RCC_HSE_BYPASS;
	}
	else if((RCC->CR &RCC_CR_HSEON) == RCC_CR_HSEON)
	{
		RCC_Osc_InitStruct->HSE_State = RCC_HSE_ON;
	}
	else
	{
		RCC_Osc_InitStruct->HSE_State = RCC_HSE_OFF;
	}


	/* Get the HSI configuration -----------------------------------------------*/
	if((RCC->CR & RCC_CR_HSION) == RCC_CR_HSION)
	{
		RCC_Osc_InitStruct->HSI_State = RCC_HSI_ON;
	}
	else
	{
		RCC_Osc_InitStruct->HSI_State = RCC_HSI_OFF;
	}

	RCC_Osc_InitStruct->HSI_CalibrationValue = (uint32_t)((RCC->CR &RCC_CR_HSITRIM) >> RCC_CR_HSITRIM_BITNUMBER);


	/* Get the LSE configuration -----------------------------------------------*/
	if((RCC->BDCR &RCC_BDCR_LSEBYP) == RCC_BDCR_LSEBYP)
	{
		RCC_Osc_InitStruct->LSE_State = RCC_LSE_BYPASS;
	}
	else if((RCC->BDCR &RCC_BDCR_LSEON) == RCC_BDCR_LSEON)
	{
		RCC_Osc_InitStruct->LSE_State = RCC_LSE_ON;
	}
	else
	{
		RCC_Osc_InitStruct->LSE_State = RCC_LSE_OFF;
	}


	/* Get the LSI configuration -----------------------------------------------*/
	if((RCC->CSR &RCC_CSR_LSION) == RCC_CSR_LSION)
	{
		RCC_Osc_InitStruct->LSI_State = RCC_LSI_ON;
	}
	else
	{
		RCC_Osc_InitStruct->LSI_State = RCC_LSI_OFF;
	}


	/* Get the HSI14 configuration -----------------------------------------------*/
	if((RCC->CR2 & RCC_CR2_HSI14ON) == RCC_CR2_HSI14ON)
	{
		RCC_Osc_InitStruct->HSI14_State = RCC_HSI_ON;
	}
	else
	{
		RCC_Osc_InitStruct->HSI14_State = RCC_HSI_OFF;
	}


	RCC_Osc_InitStruct->HSI14_CalibrationValue = (uint32_t)((RCC->CR2 & RCC_CR2_HSI14TRIM) >> RCC_HSI14TRIM_BITNUMBER);


	#if defined(RCC_HSI48_SUPPORT)

	/* Get the HSI48 configuration if any-----------------------------------------*/
	RCC_Osc_InitStruct->HSI48_State = MY_RCC_GET_HSI48_STATE();

	#endif /* RCC_HSI48_SUPPORT */


	/* Get the PLL configuration -----------------------------------------------*/
	if((RCC->CR &RCC_CR_PLLON) == RCC_CR_PLLON)
	{
		RCC_Osc_InitStruct->PLL.PLL_State = RCC_PLL_ON;
	}
	else
	{
		RCC_Osc_InitStruct->PLL.PLL_State = RCC_PLL_OFF;
	}

	RCC_Osc_InitStruct->PLL.PLL_Source = (uint32_t)(RCC->CFGR & RCC_CFGR_PLLSRC);
	RCC_Osc_InitStruct->PLL.PLL_MUL = (uint32_t)(RCC->CFGR & RCC_CFGR_PLLMUL);
	RCC_Osc_InitStruct->PLL.PLL_PREDIV = (uint32_t)(RCC->CFGR2 & RCC_CFGR2_PREDIV);

}


void MY_RCC_Clock_GetConfig(MY_RCC_Clock_Init_t  *RCC_Clock_InitStruct, uint32_t *pFlashLatency)
{
	/* Set all possible values for the Clock type parameter --------------------*/
	RCC_Clock_InitStruct->ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;

	/* Get the SYSCLK configuration --------------------------------------------*/
	RCC_Clock_InitStruct->SYSCLK_Source = (uint32_t)(RCC->CFGR & RCC_CFGR_SW);

	/* Get the HCLK configuration ----------------------------------------------*/
	RCC_Clock_InitStruct->AHBCLK_Divider = (uint32_t)(RCC->CFGR & RCC_CFGR_HPRE);

	/* Get the APB1 configuration ----------------------------------------------*/
	RCC_Clock_InitStruct->APB1CLK_Divider = (uint32_t)(RCC->CFGR & RCC_CFGR_PPRE);

	/* Get the Flash Wait State (Latency) configuration ------------------------*/
	*pFlashLatency = (uint32_t)(FLASH->ACR & FLASH_ACR_LATENCY);
}


void MY_RCC_MCO_Config(uint32_t RCC_MCO_Source, uint32_t RCC_MCO_Div)
{
	/* Инициализация пина GPIO */
	MY_GPIO_Init_t		 MCO_GPIO;

	MCO_GPIO.Mode      = GPIO_MODE_AF_PP;
	MCO_GPIO.Speed     = GPIO_SPEED_FREQ_HIGH;
	MCO_GPIO.Pull      = GPIO_NOPULL;
	MCO_GPIO.Pin       = MCO1_PIN;
	MCO_GPIO.Alternate = GPIO_AF0_MCO;

	MY_GPIO_StructInit(MCO1_GPIO_PORT, &MCO_GPIO);

	/* Настройка MCO */
	MY_RCC_MCO_CONFIG(RCC_MCO_Source, RCC_MCO_Div);
}


void MY_RCC_CSS_Enable(void)
{
	SET_BIT(RCC->CR, RCC_CR_CSSON);
}


void MY_RCC_CSS_Disable(void)
{
	CLEAR_BIT(RCC->CR, RCC_CR_CSSON);
}


#if defined(STM32F042x6) || \
	defined(STM32F048xx) || \
	defined(STM32F071xB) || \
	defined(STM32F072xB) || \
	defined(STM32F078xx) || \
	defined(STM32F091xC) || \
	defined(STM32F098xx)

void MY_RCC_CRS_Config(MY_RCC_CRS_Init_t *pInit)
{
	uint32_t value = 0U;

	/* CONFIGURATION */

	/* Before configuration, reset CRS registers to their default values*/
	MY_RCC_CRS_FORCE_RESET();
	MY_RCC_CRS_RELEASE_RESET();

	/* Set the SYNCDIV[2:0] bits according to Prescaler value */
	/* Set the SYNCSRC[1:0] bits according to Source value */
	/* Set the SYNCSPOL bit according to Polarity value */
	value = (pInit->Prescaler | pInit->Source | pInit->Polarity);

	/* Set the RELOAD[15:0] bits according to ReloadValue value */
	value |= pInit->ReloadValue;

	/* Set the FELIM[7:0] bits according to ErrorLimitValue value */
	value |= (pInit->ErrorLimitValue << CRS_CFGR_FELIM_BITNUMBER);

	WRITE_REG(CRS->CFGR, value);

	/* Adjust HSI48 oscillator smooth trimming */
	/* Set the TRIM[5:0] bits according to RCC_CRS_HSI48CalibrationValue value */
	MODIFY_REG(CRS->CR, CRS_CR_TRIM, (pInit->HSI48_CalibrationValue << CRS_CR_TRIM_BITNUMBER));

	/* START AUTOMATIC SYNCHRONIZATION*/

	/* Enable Automatic trimming & Frequency error counter */
	SET_BIT(CRS->CR, CRS_CR_AUTOTRIMEN | CRS_CR_CEN);
}


void MY_RCC_CRS_SoftwareSyncGenerate(void)
{
	SET_BIT(CRS->CR, CRS_CR_SWSYNC);
}


void MY_RCC_CRS_GetSyncInfo(RCC_CRS_Sync_Info_t *pSyncInfo)
{
	/* Get the reload value */
	pSyncInfo->ReloadValue = (uint32_t)(READ_BIT(CRS->CFGR, CRS_CFGR_RELOAD));

	/* Get HSI48 oscillator smooth trimming */
	pSyncInfo->HSI48CalibrationValue = (uint32_t)(READ_BIT(CRS->CR, CRS_CR_TRIM) >> CRS_CR_TRIM_BITNUMBER);

	/* Get Frequency error capture */
	pSyncInfo->FreqErrorCapture = (uint32_t)(READ_BIT(CRS->ISR, CRS_ISR_FECAP) >> CRS_ISR_FECAP_BITNUMBER);

	/* Get Frequency error direction */
	pSyncInfo->FreqErrorDirection = (uint32_t)(READ_BIT(CRS->ISR, CRS_ISR_FEDIR));
}


uint32_t MY_RCC_CRS_WaitSync(uint32_t Timeout)
{
	uint32_t crsstatus = RCC_CRS_NONE;
	uint32_t tickstart = 0U;

	/* Get timeout */
	tickstart = MY_SysTick_GetTick();

	/* Wait for CRS flag or timeout detection */
	do
	{
		if(Timeout != MAX_DELAY)
		{
			if((Timeout == 0U) || ((MY_SysTick_GetTick() - tickstart) > Timeout))
			{
				crsstatus = RCC_CRS_TIMEOUT;
			}
		}


		/* Check CRS SYNCOK flag  */
		if(MY_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_SYNCOK))
		{
			/* CRS SYNC event OK */
			crsstatus |= RCC_CRS_SYNCOK;

			/* Clear CRS SYNC event OK bit */
			MY_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_SYNCOK);
		}


		/* Check CRS SYNCWARN flag  */
		if(MY_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_SYNCWARN))
		{
			/* CRS SYNC warning */
			crsstatus |= RCC_CRS_SYNCWARN;

			/* Clear CRS SYNCWARN bit */
			MY_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_SYNCWARN);
		}


		/* Check CRS TRIM overflow flag  */
		if(MY_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_TRIMOVF))
		{
			/* CRS SYNC Error */
			crsstatus |= RCC_CRS_TRIMOVF;

			/* Clear CRS Error bit */
			MY_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_TRIMOVF);
		}


		/* Check CRS Error flag  */
		if(MY_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_SYNCERR))
		{
			/* CRS SYNC Error */
			crsstatus |= RCC_CRS_SYNCERR;

			/* Clear CRS Error bit */
			MY_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_SYNCERR);
		}


		/* Check CRS SYNC Missed flag  */
		if(MY_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_SYNCMISS))
		{
			/* CRS SYNC Missed */
			crsstatus |= RCC_CRS_SYNCMISS;

			/* Clear CRS SYNC Missed bit */
			MY_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_SYNCMISS);
		}


		/* Check CRS Expected SYNC flag  */
		if(MY_RCC_CRS_GET_FLAG(RCC_CRS_FLAG_ESYNC))
		{
			/* frequency error counter reached a zero value */
			MY_RCC_CRS_CLEAR_FLAG(RCC_CRS_FLAG_ESYNC);
		}
	}
	while(RCC_CRS_NONE == crsstatus);

	return crsstatus;
}


void MY_RCC_CRS_IRQHandler(void)
{
	uint32_t crserror = RCC_CRS_NONE;

	/* Get current IT flags and IT sources values */
	uint32_t itflags = READ_REG(CRS->ISR);
	uint32_t itsources = READ_REG(CRS->CR);

	/* Check CRS SYNCOK flag  */
	if(((itflags & RCC_CRS_FLAG_SYNCOK) != RESET) && ((itsources & RCC_CRS_IT_SYNCOK) != RESET))
	{
		/* Clear CRS SYNC event OK flag */
		WRITE_REG(CRS->ICR, CRS_ICR_SYNCOKC);

		/* user callback */
		MY_RCC_CRS_SyncOkCallback();
	}
	/* Check CRS SYNCWARN flag  */
	else if(((itflags & RCC_CRS_FLAG_SYNCWARN) != RESET) && ((itsources & RCC_CRS_IT_SYNCWARN) != RESET))
	{
		/* Clear CRS SYNCWARN flag */
		WRITE_REG(CRS->ICR, CRS_ICR_SYNCWARNC);

		/* user callback */
		MY_RCC_CRS_SyncWarnCallback();
	}
	/* Check CRS Expected SYNC flag  */
	else if(((itflags & RCC_CRS_FLAG_ESYNC) != RESET) && ((itsources & RCC_CRS_IT_ESYNC) != RESET))
	{
		/* frequency error counter reached a zero value */
		WRITE_REG(CRS->ICR, CRS_ICR_ESYNCC);

		/* user callback */
		MY_RCC_CRS_ExpectedSyncCallback();
	}
	/* Check CRS Error flags  */
	else
	{
		if(((itflags & RCC_CRS_FLAG_ERR) != RESET) && ((itsources & RCC_CRS_IT_ERR) != RESET))
		{
			if((itflags & RCC_CRS_FLAG_SYNCERR) != RESET)
			{
				crserror |= RCC_CRS_SYNCERR;
			}

			if((itflags & RCC_CRS_FLAG_SYNCMISS) != RESET)
			{
				crserror |= RCC_CRS_SYNCMISS;
			}

			if((itflags & RCC_CRS_FLAG_TRIMOVF) != RESET)
			{
				crserror |= RCC_CRS_TRIMOVF;
			}

			/* Clear CRS Error flags */
			WRITE_REG(CRS->ICR, CRS_ICR_ERRC);

			/* user error callback */
			MY_RCC_CRS_ErrorCallback(crserror);
		}
	}
}


_weak void MY_RCC_CRS_SyncOkCallback(void)
{
	/* NOTE : This function should not be modified, when the callback is needed, the @ref MY_RCC_CRS_SyncOkCallback should be implemented in the user file */
}


__weak void MY_RCC_CRS_SyncWarnCallback(void)
{
	/* NOTE : This function should not be modified, when the callback is needed, the @ref MY_RCC_CRS_SyncWarnCallback should be implemented in the user file */
}


__weak void MY_RCC_CRS_ExpectedSyncCallback(void)
{
	/* NOTE : This function should not be modified, when the callback is needed, the @ref MY_RCC_CRS_ExpectedSyncCallback should be implemented in the user file */
}


__weak void MY_RCC_CRS_ErrorCallback(uint32_t Error)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(Error);

	/* NOTE : This function should not be modified, when the callback is needed, the @ref MY_RCC_CRS_ErrorCallback should be implemented in the user file */
}

#endif
