/**
  ******************************************************************************
  * @file    k_bsp.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    26-December-2014
  * @brief   Header for k_bsp.c file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __K_BSP_H
#define __K_BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f429i_WireTester.h"
#include "main.h"
#include "cmsis_os.h"	 
#include "k_rtc.h"	 
#include "GUI.h"
#include "DIALOG.h"	 
#include "wt_bsp_file.h"
#include "wt_bsp_key_led.h"
#include "wt_bsp_io.h"
#include "wt_bsp_usart.h"
#include "wt_bsp_store.h"
#include "wt_bsp_ad.h"
#include "stm32f429i_wt_eeprom.h"
#include "wt_task_wireselfcheck.h"
#include "wt_uart_COM1.h"
#include "wt_uart_COM2.h"
#include "wt_uart_24GHz.h"
#include "wt_uart_Audio.h"
#include "wt_bsp_infrared.h"
	 


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
void k_BspInit(void);
void k_BspAudioInit(void);
void k_TouchUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /*__K_BSP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
