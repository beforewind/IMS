/**
  ******************************************************************************
  * @file    wt_task_gui.h
  * @author  zhang manxin
  * @version V1.0.0
  * @date    2014-7-2
  * @brief   This file contains all the functions prototypes for the E2PROM driver.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WT_WINDOWS_GUI_H
#define __WT_WINDOWS_GUI_H

#ifdef __cplusplus
 extern "C" {
#endif   
   
/* Includes ------------------------------------------------------------------*/
#include <stdint.h> 
  

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   WT_WINDOWS_GUI_GLOBALS
#define  WT_WINDOWS_GUI_EXT
#else
#define  WT_WINDOWS_GUI_EXT  extern
#endif


/* Exported constants --------------------------------------------------------*/
  
/** @defgroup Exported_Constants
  * @{
  */ 

    


/* variables ---------------------------------------------------------*/

/** @defgroup WireTester Variables
  * @{
  */
WT_WINDOWS_GUI_EXT uint8_t Number_Windos;	//当前显示窗口编号
//WT_WINDOWS_GUI_EXT SystemSettingsTypeDef    settings;




/* Exported functions --------------------------------------------------------*/
  
/** @defgroup Exported_Functions
  * @{
  */

/** 
  * @brief functions
  */
	
void GUIThread(void const * argument);
void wt_SetText_Title(const char * pTitle);
void wt_SetText_Menu(const char * pMenu);
void wt_SetText_Status(const char * pStatus);


#ifdef __cplusplus
}
#endif
#endif /* __WT_WINDOWS_GUI_H */

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
