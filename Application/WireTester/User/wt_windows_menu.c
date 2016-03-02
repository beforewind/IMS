/**
  ******************************************************************************
  * @progect LZY Wire Cube Tester
	* @file    wt_windows_menu.c
  * @author  LZY Zhang Manxin
  * @version V1.0.0
  * @date    2014-06-12
  * @brief   This file provides the startup functions
  ******************************************************************************
  */
#include "wt_bsp_file.h"
#include "k_bsp.h" 
/* Includes ------------------------------------------------------------------*/
#include "DIALOG.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dialog.h"
#include "messagebox.h"
#include "k_rtc.h"
#include "k_log.h"
#include "k_storage.h"
#include "k_module.h"
#include "cpu_utils.h"
#include "wt_bsp_ad.h"
#include "..\..\Core\Src\k_res.c"
#include "wt_task_gui.h"
#include "wt_bsp_key_led.h"
#include "wt_bsp_usart.h"
#include "wt_task_wirefindpoint.h"


#pragma diag_suppress 870 

extern void WT_Test(void);


/* External variables --------------------------------------------------------*/
//extern GUI_CONST_STORAGE GUI_BITMAP bm_BackGround480x272A;
//extern GUI_CONST_STORAGE GUI_BITMAP bmbmLZYLogo40x20;

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define ID_ICONVIEW_MENU  (GUI_ID_USER + 0x02)

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static ICONVIEW_Handle hIcon = 0;
int32_t wheel_value=0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) {
//  
//  MENU_MSG_DATA* pData;
//  uint32_t NCode, Id;
//  static uint8_t sel = 0;
//  
  switch (pMsg->MsgId) 
  {

		case WM_PAINT:
			GUI_SetBkColor(GUI_TRANSPARENT);
			GUI_Clear();
			if(hIcon)
			{
				WM_BringToBottom(hIcon);
			}
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  Initializes the menu.
  * @param  None.
  * @retval None.
  */
void k_InitMenu(void) 
{
  uint8_t i = 0;
	char temp[50];

  GUI_SetLayerVisEx (0, 1);
  GUI_SelectLayer(1);
  GUI_Clear();
  WM_SetCallback(WM_HBKWIN, _cbBk);
  
	if(hIcon == 0)
	{
		hIcon = ICONVIEW_CreateEx(0, 
															25, 
															LCD_GetXSize(), 
															LCD_GetYSize()- 50, 
															WM_HBKWIN, 
															WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_BGND ,
															ICONVIEW_CF_AUTOSCROLLBAR_V,
															ID_ICONVIEW_MENU, 
															115, 
															90);
		
		//ICONVIEW_SetFont(hIcon, &GUI_Font13B_ASCII);
		ICONVIEW_SetFont(hIcon, &GUI_FontHZ_Song_16);
		
		ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_LIGHTBLUE |GUI_TRANSPARENT);
		
		ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, 10);
		
		ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 10);
			
		for (i = 0; i < k_ModuleGetNumber(); i++)
		{
			ICONVIEW_AddBitmapItem(hIcon,module_prop[i].module->icon, (char *)module_prop[i].module->name);
		}
	}
	else
	{
		WM_ShowWindow(hIcon);
	}
	WM_SetFocus(hIcon);
	//update title & menu
	strcpy(temp, DEF_WT_NAME);
	strcat(temp, " ");
	strcat(temp, DEF_WT_MODEL);
	wt_SetText_Title(temp);
	wt_SetText_Menu("主菜单");
}


/**
  * @brief  Initializes the menu.
  * @param  None.
  * @retval None.
  */
static void Menu_NextICO(void) 
{
//	WM_MESSAGE Msg;
//	WM_KEY_INFO Info;
//	
//	Info.Key = GUI_KEY_RIGHT;
//	Info.PressedCnt = 1;
//	Msg.MsgId = WM_NOTIFICATION_SEL_CHANGED;
//	Msg.Data.p = &Info;
//	
//	WM_SendMessage(hIcon,&Msg);
	
	static int id;
	id=ICONVIEW_GetSel(hIcon);
	id++;
	
	if(id > (ICONVIEW_GetNumItems(hIcon)-1)) id=0;
	ICONVIEW_SetSel(hIcon, id);
	wheel_value=KeyLed_State.wheel;
	switch(id)
	{
		case 0:
			wt_SetText_Status("运行线束检测程序"); 	
			break;
		case 1:
			wt_SetText_Status("自动学习测试规则"); 
			break;
		case 2:
			wt_SetText_Status("运行自动找点程序"); 
			break;
		case 3:
			wt_SetText_Status("编辑测试规则文件"); 
			break;
		case 4:
			wt_SetText_Status("运行设备自检程序"); 
			break;
		case 5:
			wt_SetText_Status("进入系统设置菜单"); 
			break;
		case 6:
			wt_SetText_Status("显示系统信息");
		case 7:
			wt_SetText_Status("系统打印");
			break;
		default:
			wt_SetText_Status("");
			break;
	}
}

/**
  * @brief  Initializes the menu.
  * @param  None.
  * @retval None.
  */
static void Menu_ForwardICO(void) 
{
	static int id;
	id=ICONVIEW_GetSel(hIcon);
	id--;
	if(id <0) id=ICONVIEW_GetNumItems(hIcon)-1;
	ICONVIEW_SetSel(hIcon, id);
	wheel_value=KeyLed_State.wheel;
	switch(id)
	{
		case 0:
			wt_SetText_Status("运行线束检测程序"); 	
			break;
		case 1:
			wt_SetText_Status("自动学习测试规则"); 
			break;
		case 2:
			wt_SetText_Status("运行自动找点程序"); 
			break;
		case 3:
			wt_SetText_Status("编辑测试规则文件"); 
			break;
		case 4:
			wt_SetText_Status("运行设备自检程序"); 
			break;
		case 5:
			wt_SetText_Status("进入系统设置菜单"); 
			break;
		case 6:
			wt_SetText_Status("显示系统信息");
			break;
		case 7:
			wt_SetText_Status("运行打印程序");
			break;
		default:
			wt_SetText_Status("");
			break;
	}
}


/**
  * @brief  Open the ICO Item.
  * @param  None.
  * @retval None.
  */
//static void Menu_OpenLinkICO(void) 
//{
//	uint8_t sel;
//	//static uint8_t state = 0;
//	
//	//if(state != 0) return;
//	
//	//state++;
//	sel = ICONVIEW_GetSel(hIcon);
//	if(sel < k_ModuleGetNumber())
//	{
//		//WM_HideWindow(hIcon);
//		WM_DeleteWindow(hIcon);
//		Number_Windos = 1;
//	}
//	
//}

/**
  * @brief  Open the ICO Item.
  * @param  None.
  * @retval None.
  */
void WT_Windows_Menu(uint32_t *ID_Select)
{
	//uint32_t wheel_value=0;
	uint8_t buf8=0;
	char temp[50];
	/* Show the main menu */
  k_InitMenu();
 
  /* Gui background Task */
  while(1)
  {
		//key detect
//		#define	KeyUp				0		//Up
//		#define	KeyDown			3		//Down
//		#define	KeyOK				1		//OK
//		#define	KeyCancle		4		//Cancle

		if(BSP_GetKEY_State(KeyUp) == 1)
		{
			while(BSP_GetKEY_State(KeyUp) == 1)
			{			
				osDelay(50); 
			}
			
			Menu_ForwardICO();
			//WT_StudyFiles_Write("0:/LZY_WireTester/StudyFiles/www.wtr");
		}
		
		if(BSP_GetKEY_State(KeyDown) == 1)
		{
			while(BSP_GetKEY_State(KeyDown) == 1)
			{
				osDelay(50); 
			}
			Menu_NextICO();
			//osMessagePut(UartAudioEvent, UartAudio_TX_Event, 0);	//收到数据帧
			//UartAudio_SendCommand(0);
		}
		
		if(BSP_GetKEY_State(KeyOK) == 1 || BSP_GetKEY_State(KeyWheel)==1 )
		{
			while(BSP_GetKEY_State(KeyOK) == 1 || BSP_GetKEY_State(KeyWheel)==1)
			{
				osDelay(50); 
			}
			//
			//Menu_OpenLinkICO();
			*ID_Select = ICONVIEW_GetSel(hIcon);
			//WM_DeleteWindow(hIcon);
			WM_HideWindow(hIcon);
			Number_Windos = 1;
			return;
		}
		if(wheel_value!=KeyLed_State.wheel)
		{
			if(wheel_value < KeyLed_State.wheel)
			{
				osDelay(50); 
				Menu_NextICO();
				wheel_value=KeyLed_State.wheel;
			}
			else
			{
				osDelay(50); 
				Menu_ForwardICO();
				wheel_value=KeyLed_State.wheel;
			}
			osDelay(100); 
		}
		if(BSP_GetLock_State() == 1 && WT_Config.Lockstat == 0xFE)
		{
			buf8 = 10;
			while(BSP_GetLock_State() == 1)
			{
				osDelay(20);
				buf8--;
				if(buf8 == 0) break;
			}
			wt_SetText_Title("设备锁定，请解锁！");
		}
		if(BSP_GetLock_State() == 0)
		{
			buf8 = 10;
			while(BSP_GetLock_State() == 1)
			{
				osDelay(20);
				buf8--;
				if(buf8 == 0) break;
			}
			WT_Config.Lockstat = 0;
			strcpy(temp, DEF_WT_NAME);
			strcat(temp, " ");
			strcat(temp, DEF_WT_MODEL);
			wt_SetText_Title(temp);
		}
		
		
		//delay
		osDelay(100); 
		GUI_Exec();
	}
}



/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
