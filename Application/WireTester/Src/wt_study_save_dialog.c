/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.22                          *
*        Compiled Jul  4 2013, 15:16:01                              *
*        (c) 2013 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END
#define WT_WINDOWS_TEST_GLOBALS

#include "DIALOG.h"
#include "k_storage.h"
#include "wt_task_gui.h"
#include "wt_bsp_key_led.h"
#include "k_rtc.h"
#include "wt_bsp_file.h"
#include "wt_task_wirestudy.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0     				(GUI_ID_USER + 0x00)
#define ID_TEXT_0     					(GUI_ID_USER + 0x02)
#define ID_EDIT_TILEPATH    		(GUI_ID_USER + 0x03)
#define ID_BUTTON_OK     				(GUI_ID_USER + 0x04)
#define ID_BUTTON_CANCEL     		(GUI_ID_USER + 0x05)
#define ID_PROGBAR0							(GUI_ID_USER + 0x06)
#define ID_TEXT_1     					(GUI_ID_USER + 0x07)
#define ID_TEXT_2    						(GUI_ID_USER + 0x08)
#define ID_TEXT_3    						(GUI_ID_USER + 0x09)
#define ID_TEXT_WARNING  				(GUI_ID_USER + 0x0A)
#define ID_TEXT_INFO  					(GUI_ID_USER + 0x0B)
#define ID_DROPDOWN_0    				(GUI_ID_USER + 0x0C)

#pragma diag_suppress 870 

// USER START (Optionally insert additional defines)
// USER END
static char input_char[67]={'0','1','2','3','4','5','6','7','8','9',
														'.','-','_','&','@','a','b','c','d','e',
	                          'f','g','h','i','j','k','l','m','n','o',
														'p','q','r','s','t','u','v','w','x','y',
														'z','A','B','C','D','E','F','G','H','I',
														'J','K','L','M','N','O','P','Q','R','S',
														'T','U','V','W','X','Y','Z',};

static int cursorindex=0;
static WM_HWIN  hItemwarn;	
static WM_HWIN  hIteminfo;		

extern uint8_t WT_StudyFiles_Write(uint8_t * path);
														
/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 480, 222, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "Dialog_text", ID_TEXT_0, 0, 35, 480, 20, 0, 0x0, 0 },
	
	{ TEXT_CreateIndirect, "Pathname_text", ID_TEXT_3, 10, 80, 90, 30, 0, 0x0, 0 },
	{ DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_0, 90,  75,  80,  30, 0, 0x0, 0 },
	
	{ TEXT_CreateIndirect, "Filename_text", ID_TEXT_1, 10, 112, 90, 30, 0, 0x0, 0 },
	{ EDIT_CreateIndirect, "Filename", ID_EDIT_TILEPATH, 90, 107, 360, 35, 0, 0x0, 0 },
	
	{ TEXT_CreateIndirect, "Filetype", ID_TEXT_2, 10, 145, 120, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "warning", ID_TEXT_WARNING, 10, 170, 220, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "info", ID_TEXT_INFO, 10, 190, 220, 20, 0, 0x0, 0 },
//	{ BUTTON_CreateIndirect, "确定", ID_BUTTON_OK, 37, 160, 100, 20, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "取消", ID_BUTTON_CANCEL, 152, 160, 80, 20, 0, 0x0, 0 },
	{	BUTTON_CreateIndirect, "确定", ID_BUTTON_OK, 140, 200, 100, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "取消", ID_BUTTON_CANCEL, 240, 200, 100, 20, 0, 0x0, 0 },

  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN                hItem;

	int cursorpos=0;
  int index=0;
	uint8_t res=0;
	uint8_t sel=0;
	char str[60];
	uint8_t filename[110];
  uint8_t NumChars=0;
	hItem = pMsg->hWin;
	WINDOW_SetBkColor(hItem, GUI_LIGHTGRAY);
	
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		TEXT_SetTextAlign(hItem, TEXT_CF_HCENTER | TEXT_CF_VCENTER);
    TEXT_SetText(hItem, (char *)"保存自学习文件");  
	
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		TEXT_SetText(hItem, (char *)"文件名称："); 
		
	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		TEXT_SetText(hItem, (char *)"文件类型：WTR"); 
	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		TEXT_SetText(hItem, (char *)"保存路径："); 
	
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
		DROPDOWN_SetFont(hItem,&GUI_FontHZ_Song_16);
    DROPDOWN_AddString(hItem, (char *)"U盘");
    DROPDOWN_AddString(hItem, (char *)"SD卡");
		if(k_StorageGetStatus(MSD_DISK_UNIT) != 0)
		DROPDOWN_SetSel(hItem,1);
		else DROPDOWN_SetSel(hItem,0);
	
		hItemwarn = WM_GetDialogItem(pMsg->hWin, ID_TEXT_WARNING);
    TEXT_SetFont(hItemwarn,&GUI_FontHZ_Song_12);
		TEXT_SetTextColor(hItemwarn,GUI_RED);
		TEXT_SetText(hItemwarn, (char *)"警告：文件名不能为空！");
		WM_HideWindow(hItemwarn);
	
		hIteminfo = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFO);
    TEXT_SetFont(hIteminfo,&GUI_FontHZ_Song_12);
		TEXT_SetTextColor(hIteminfo,GUI_RED);
		TEXT_SetText(hIteminfo, (char *)"文件保存成功！");
		WM_HideWindow(hIteminfo);
		
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TILEPATH);
		//EDIT_SetDecMode(hItem, 24, 1,256, 0, 0);
		EDIT_SetFont(hItem,GUI_FONT_32_1);
		EDIT_SetMaxLen(hItem, 50);
		EDIT_EnableBlink(hItem, 600, 1);
		EDIT_SetInsertMode(hItem,1);
		//strcpy ((char*)str,(char *)path_studyfile);
		//strcat ((char*)str,"/");
		//strcat ((char*)str,"A.wtr");
    EDIT_SetText(hItem,"A");
		cursorindex=EDIT_GetCursorCharPos(hItem);
		WM_SetFocus(hItem);
    
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetFont(hItem,&GUI_FontHZ_Song_12);
		BUTTON_SetSkinClassic(hItem);
		BUTTON_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		BUTTON_SetBkColor(hItem,BUTTON_CI_UNPRESSED, GUI_GREEN);
		BUTTON_SetFocussable(hItem,0);//不接收焦点
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
		BUTTON_SetFont(hItem,&GUI_FontHZ_Song_12);
		BUTTON_SetSkinClassic(hItem);
		BUTTON_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		BUTTON_SetBkColor(hItem,BUTTON_CI_UNPRESSED, GUI_RED);
		BUTTON_SetFocussable(hItem,0);//不接收焦点
		
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
	case WM_KEY:
		//GUI_SendKeyMsg(GUI_KEY_TAB, 1);//改变输入焦点
	
		break;
	case MY_MESSAGE_CLICK:
		GUI_SendKeyMsg(GUI_KEY_TAB, 1);//改变输入焦点
		break;
	case MY_MESSAGE_OK:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TILEPATH);
		if(EDIT_GetNumChars(hItem)==0)
		{
			hItemwarn = WM_GetDialogItem(pMsg->hWin, ID_TEXT_WARNING);
			WM_ShowWindow(hItemwarn);
			break;
		}
		else 
		{
			hItemwarn = WM_GetDialogItem(pMsg->hWin, ID_TEXT_WARNING);
			WM_HideWindow(hItemwarn);		
		}
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
		store_dev=DROPDOWN_GetSel(hItem);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TILEPATH);
		EDIT_GetText(hItem, str, 51);
		if(store_dev == 0)//usb
		{
			if(sizeof(path_studyfile) < 50) strcpy ((char*)filename,(char *)path_studyfile);
			else														strcpy ((char*)filename,(char *)path_Default);
		}
		if(store_dev == 1)//sd
		{
			if(sizeof(path_studyfile) < 50) strcpy ((char*)filename,(char *)path_studyfile_sd);
			else														strcpy ((char*)filename,(char *)path_Default_sd);
		}
		strcat ((char*)filename,"/");
		strcat ((char*)filename,str);
		strcat ((char*)filename,".wtr");
		hIteminfo = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFO);
		TEXT_SetText(hIteminfo, "正在保存自学习文件！");
		WM_ShowWindow(hIteminfo);
		GUI_Exec();
		res=WT_StudyFiles_Write(filename);//0-ok, 1-hardware error, 2-file operate error, 
		if(res==0)
		{
			hIteminfo = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFO);
			TEXT_SetText(hIteminfo, (char *)"文件保存成功！");
			WM_ShowWindow(hIteminfo);
		}
		else
		{
			hIteminfo = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFO);
			TEXT_SetText(hIteminfo, "警告：保存失败，未检测到存储设备！");
			WM_ShowWindow(hIteminfo);
			break;
		}

		GUI_EndDialog(pMsg->hWin,0);

		break;
	case MY_MESSAGE_DOWN://向右移动光标
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
		if(WM_HasFocus(hItem))//选择存储路径
		{
			sel=DROPDOWN_GetSel(hItem);
			if(sel<1)
			{
				DROPDOWN_IncSel(hItem);
			}					
			else //sel>=2 
			{
				DROPDOWN_SetSel(hItem,0);		
			}						
		}
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TILEPATH);	
		cursorpos=EDIT_GetCursorCharPos(hItem);
		NumChars = EDIT_GetNumChars(hItem);
		if(cursorpos <= (NumChars-1) )
		{
			EDIT_SetCursorAtChar(hItem,cursorpos+1);
			EDIT_SetSel(hItem,cursorpos+1,cursorpos+1);
			cursorindex=EDIT_GetCursorCharPos(hItem);
		}
		//KeyLed_State.wheel=0;
		break;
	case MY_MESSAGE_UP://向左移动光标
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TILEPATH);
		cursorpos=EDIT_GetCursorCharPos(hItem);
		EDIT_SetCursorAtChar(hItem,cursorpos-1);
		EDIT_SetSel(hItem,cursorpos-1,cursorpos-1);
		cursorindex=EDIT_GetCursorCharPos(hItem);
		break;
	case	MY_MESSAGE_WHEEL://处理滚轮事件
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TILEPATH);
		EDIT_SetCursorAtChar(hItem,cursorindex);
		index=KeyLed_State.wheel;

		if(KeyLed_State.wheel>=67 && KeyLed_State.wheel <134) index=KeyLed_State.wheel-67;
		if(KeyLed_State.wheel>=134 && KeyLed_State.wheel <201) index=KeyLed_State.wheel-134;
		if(KeyLed_State.wheel>=201) index=66;
		if(KeyLed_State.wheel<0 && KeyLed_State.wheel >= -67) index=KeyLed_State.wheel+67;
		if(KeyLed_State.wheel<-67 && KeyLed_State.wheel >= -134) index=KeyLed_State.wheel+134;
		if(KeyLed_State.wheel<-134) index=0;
		if(cursorindex==EDIT_GetCursorCharPos(hItem))
		{
			EDIT_AddKey(hItem,GUI_KEY_DELETE);
			EDIT_AddKey(hItem,input_char[index]);
			cursorpos=EDIT_GetCursorCharPos(hItem);
			EDIT_SetSel(hItem,cursorpos-1,cursorpos-1);
		}

		break;
	case MY_MESSAGE_BUTTONDELETE://删除字符
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TILEPATH);
		if(WM_HasFocus(hItem))
		{
			EDIT_AddKey(hItem,GUI_KEY_BACKSPACE);
			cursorindex=EDIT_GetCursorCharPos(hItem);
		}
		break;
	case WM_PAINT://绘制标题栏
		GUI_SetColor(GUI_DARKBLUE);
		GUI_FillRect(0,0,480,22);
		GUI_SetColor(GUI_DARKGRAY);
		GUI_SetPenSize(6);
		GUI_DrawRect(0,0,480-2,222-2);
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateWindow
*/
WM_HWIN Create_StudySaveDlgWindow( WM_HWIN hWin_para);
WM_HWIN Create_StudySaveDlgWindow( WM_HWIN hWin_para) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, hWin_para, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/