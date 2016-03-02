/**
  ******************************************************************************
  * @file    wt_ad_app.h
  * @author  zhang manxin
  * @version V1.0.0
  * @date    2014-7-18
  * @brief   This file contains all the functions prototypes for the E2PROM driver.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WT_AD_APP_H
#define __WT_AD_APP_H

#ifdef __cplusplus
 extern "C" {
#endif   
   
/* Includes ------------------------------------------------------------------*/
#include <stdint.h> 
#include "stm32f429i_WireTester.h"
#include "wt_bsp_ad.h"
#include "cmsis_os.h"
#include "wt_bsp_key_led.h"
#include "wt_bsp_file.h"

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   WT_AD_APP_GLOBALS
#define  WT_AD_APP_EXT
#else
#define  WT_AD_APP_EXT  extern
#endif



#define		WT_WZK_MOD_27R		0		// U4.5V,  P0.1, 5.5us
#define		WT_WZK_MOD_365R		1		// U4.5V,  P1.0, 8.5us



//typedef struct{
//  uint8_t TestMode_W;
//	uint8_t TestMode_Z;
//	uint8_t TestMode_K;
//}
//WT_TestConfigTypedef;


///* variables ---------------------------------------------------------*/

//WT_AD_APP_EXT WT_TestConfigTypedef 	WT_TestConfig;








///* Exported functions --------------------------------------------------------*/
//  

uint8_t WireTest_Process(void);
uint8_t WireStudy_Process(uint16_t 	NumStudyPort);
uint8_t WireTest_WaitForNext(void);	//0:all remove, 1:file errror, 3-user cancel; 
uint8_t StudyFile_Optimize(void);
uint8_t Update_Loopinfo(void);


#ifdef __cplusplus
}
#endif
#endif /* __WT_AD_APP_H */

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */       
/************************ (C) COPYRIGHT CNDZ *****END OF FILE****/
