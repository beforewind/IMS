/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    26-February-2014
  * @brief   This file provides main program functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "k_bsp.h" 
#include "k_log.h"  
#include "verision.h"
//#include "k_calibration.h"
//#include "k_storage.h" 
#include "k_mem.h" 
#include "wt_task_gui.h"
#include "stm32429i_wt_sd.h"
#include "wt_task_wiretest.h"
#include "wt_task_wirestudy.h"
#include "wt_task_wireselfcheck.h"
#include "wt_task_wirefindpoint.h"


extern void wt_StartUp(void);
extern void wt_Logo_StartUp(void);
extern void Menu_NextICO(void);
extern void Menu_OpenLinkICO(void);
extern uint8_t WT_Config_Read(void);


/** @defgroup MAIN_Private_FunctionPrototypes
* @{
*/ 
static void SystemClock_Config(void);
static void StartThread(void const * argument);
static void TimerCallback(void const *n);


/* */
WT_TesterStatusTypedef TesterStatus;	//


/** @defgroup MAIN_Private_Functions
* @{
*/ 
SystemSettingsTypeDef 	settings;
osMessageQId 						WireTestEvent;
osMessageQId 						WireStudyEvent;
osMessageQId 						WireSelfCheckEvent;
osMessageQId 						WireFindPointEvent;
osMessageQId 						UartCOM1Event;
osMessageQId 						UartCOM2Event;
osMessageQId 						UartAudioEvent;
osMessageQId 						Uart24GHzEvent;
osMessageQId 						UartInfrEvent;
WT_ConfigTypedef				WT_Config;

/**
* @brief  Main program
* @param  None
* @retval int
*/
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
  */
	
	//__set_MSP(0x08000000);
	//NVIC_SetVectorTable(0x08000000,0x200);
	
  HAL_Init();
 
  /* Configure the system clock @ 168 Mhz */
  SystemClock_Config();
  
	/*Initialize memory pools */
  k_MemInit();
	
  /* Create Start task */
  osThreadDef(Kernel_Thread, StartThread, osPriorityHigh, 0, 2 * configMINIMAL_STACK_SIZE);
  osThreadCreate (osThread(Kernel_Thread), NULL);
  
  /* Start scheduler */
  osKernelStart ();

  /* We should never get here as control is now taken by the scheduler */
  for( ;; );
}

/**
  * @brief  Start task
  * @param  argument: pointer that is passed to the thread function as start argument.
  * @retval None
  */
static void StartThread(void const * argument)
{
//  osTimerId lcd_timer;
	
  /* Initialize Joystick, Touch screen and Leds */
  k_BspInit();
  k_LogInit();
  WT_Config_Read();
		
  /* Initialize GUI */
  GUI_Init();
	LISTVIEW_SetDefaultGridColor(GUI_GRAY);
	LCD_BackLight_SetPWM(WT_Config.BackLight);
	WM_MULTIBUF_Enable(1);
  GUI_SelectLayer(1);
	GUI_UC_SetEncodeUTF8();
	
	//show logo
	wt_Logo_StartUp();
	
  /* Initialize RTC */
  k_CalendarBkupInit();
  
  /* Initialize Storage Units */
  k_StorageInit();
  WT_StoreFiles_Init();
	WT_StudyFiles_Init();
	//WT_PrintFile_Init();
	
  /* Create GUI task */
  osThreadDef(GUI_Thread, GUIThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 8);	//1 kB
  osThreadCreate (osThread(GUI_Thread), NULL); 

	/* Create WireTest task */
  osThreadDef(WireTest_Thread, WIRETESTThread, osPriorityNormal, 0, 5 * configMINIMAL_STACK_SIZE);	//15
  osThreadCreate (osThread(WireTest_Thread), NULL); 
	
	/* Create Auto Study task */
  osThreadDef(WireStudy_Thread, WIRESTUDYThread, osPriorityNormal, 0, 5 * configMINIMAL_STACK_SIZE);	//15
  osThreadCreate (osThread(WireStudy_Thread), NULL); 
	
	/* Create Self Check task */
  osThreadDef(WireSelfCheck_Thread, WIRESELFCHECKThread, osPriorityNormal, 0, 2 * configMINIMAL_STACK_SIZE);	//15
  osThreadCreate (osThread(WireSelfCheck_Thread), NULL); 
	
	/* Create Find Point Check task */
  osThreadDef(WireFindPoint_Thread, WIREFINDPOINTThread, osPriorityNormal, 0, 2 * configMINIMAL_STACK_SIZE);	//15
  osThreadCreate (osThread(WireFindPoint_Thread), NULL); 
	
	/* Create KEY_LED task */
  osThreadDef(KEY_LED_Thread, TimerCallback, osPriorityLow, 0, 2 * configMINIMAL_STACK_SIZE);	//15
  osThreadCreate (osThread(KEY_LED_Thread), NULL); 
	
	/* Create Uart_COM1 task */
	osThreadDef(UART_COM1_Thread, UARTCOM1Thread, osPriorityAboveNormal, 0, 2 * configMINIMAL_STACK_SIZE);	//15
  osThreadCreate (osThread(UART_COM1_Thread), NULL); 
	
	/* Create Uart_INFR task */
//	osThreadDef(UART_INFR_Thread, UARTINFRThread, osPriorityAboveNormal, 0, 2 * configMINIMAL_STACK_SIZE);	//15
//  osThreadCreate (osThread(UART_INFR_Thread), NULL); 
	/* Create Uart_24GHZ task */
	#ifndef HD_VERSION1
	osThreadDef(UART_24GHZ_Thread, UART24GHZThread, osPriorityAboveNormal, 0, 6 * configMINIMAL_STACK_SIZE);	//15
  osThreadCreate (osThread(UART_24GHZ_Thread), NULL); 
	#endif
	
	/* Create Uart_COM2 task */
	osThreadDef(UART_COM2_Thread, UARTCOM2Thread, osPriorityAboveNormal, 0, 2 * configMINIMAL_STACK_SIZE);	//15
  osThreadCreate (osThread(UART_COM2_Thread), NULL); 
	
	/* Create Uart_Audio task */
	osThreadDef(UART_Audio_Thread, UARTAudioThread, osPriorityAboveNormal, 0, 2 * configMINIMAL_STACK_SIZE);	//15
  osThreadCreate (osThread(UART_Audio_Thread), NULL); 
	
	/* Create Test Message Queue */
  osMessageQDef(osqueue_test, 1, uint16_t);
  WireTestEvent = osMessageCreate (osMessageQ(osqueue_test), NULL);
	
	/* Create auto study Message Queue */
  osMessageQDef(osqueue_study, 1, uint16_t);
  WireStudyEvent = osMessageCreate (osMessageQ(osqueue_study), NULL);
	
	/* Create Self check Message Queue */
  osMessageQDef(osqueue_selfcheck, 1, uint16_t);
  WireSelfCheckEvent = osMessageCreate (osMessageQ(osqueue_selfcheck), NULL);
	
	/* Create find point Message Queue */
  osMessageQDef(osqueue_findpoint, 1, uint16_t);
  WireFindPointEvent = osMessageCreate (osMessageQ(osqueue_findpoint), NULL);

	/* Create Uart COM1 Message Queue */
  osMessageQDef(osqueue_uart_com1, 1, uint16_t);
  UartCOM1Event = osMessageCreate (osMessageQ(osqueue_uart_com1), NULL);
	
	/* Create Uart INFR Message Queue */
//  osMessageQDef(osqueue_uart_infr, 1, uint16_t);
//  UartInfrEvent = osMessageCreate (osMessageQ(osqueue_uart_infr), NULL);
	
	/* Create Uart COM2 Message Queue */
  osMessageQDef(osqueue_uart_com2, 1, uint16_t);
  UartCOM2Event = osMessageCreate (osMessageQ(osqueue_uart_com2), NULL);
	
	/* Create Uart 24G Message Queue */
  osMessageQDef(osqueue_uart_24G, 1, uint16_t);
  Uart24GHzEvent = osMessageCreate (osMessageQ(osqueue_uart_24G), NULL);
	
	/* Create Uart Audio Message Queue */
  osMessageQDef(osqueue_uart_aduio, 1, uint16_t);
  UartAudioEvent = osMessageCreate (osMessageQ(osqueue_uart_aduio), NULL);
	
//  /* Create Touch screen Timer */
//  osTimerDef(TS_Timer, TimerCallback);
//  lcd_timer =  osTimerCreate(osTimer(TS_Timer), osTimerPeriodic, (void *)0);
//  osTimerStart(lcd_timer, 20);	/* Start the TS Timer */
  
	
  for( ;; )
  {
		LED_RUN_ON;   
//		LED1_ON_G();
		osDelay(500);
		
		LED_RUN_OFF;   
		LED1_OFF();
		osDelay(500);
  }
}



/**
  * @brief  Timer callbacsk 
  * @param  n: Timer index 
  * @retval None
  */
#define TIMES_KEY_LED		10
static void TimerCallback(void const *n)
{  
	uint8_t cnt = 0;
	
	while(1)
	{
		BSP_Update_KEY_LED();	//read 595 Key
		BSP_Update_Input();		//read Din
		
		// 1S for update Input & Output
		cnt++;
		if(cnt > (1000 / TIMES_KEY_LED))
		{
			cnt = 0;
			BSP_Update_Output();	//update output
		}
		
		//os delay
		osDelay(TIMES_KEY_LED);
	}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}



//Is_UART_COM1_Rx_Come

/**
  * @brief  converter num to str.
  * @param  num(1-256)
  * @retval None
  */
void num_converter(uint16_t num, char * str)
{
	char tmp_str[20];
	memset(tmp_str,0,20);
	if(num <=64) sprintf(tmp_str,"A%d",num);
	else if(num >64 && num <=128) sprintf(tmp_str,"B%d",num-64);
	else if(num >128 && num <=192) sprintf(tmp_str,"C%d",num-128);
	else if(num >192 && num <=256) sprintf(tmp_str,"D%d",num-192);
	else sprintf(tmp_str,"E%d",num);
	strcpy(str,tmp_str);
	
}

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
  number,ex: printf("Wrong parameters value: file %s on line %d\r\n", 
  file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}

#endif


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
