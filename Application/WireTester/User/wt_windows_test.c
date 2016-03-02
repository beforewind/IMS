/**
  ******************************************************************************
  * @progect LZY Wire Cube Tester
	* @file    wt_windows_test.c
  * @author  LZY Zhang Manxin
  * @version V1.0.0
  * @date    2014-7-3
  * @brief   This file provides the startup functions
  ******************************************************************************
  */

#define WT_WINDOWS_TEST_GLOBALS


/* Includes ------------------------------------------------------------------*/
#include "dialog.h"
#include "k_rtc.h"
#include "k_storage.h"
#include "wt_task_gui.h"
#include "wt_windows_test.h"
#include "wt_bsp_key_led.h"
#include "wt_bsp_file.h"
#include "wt_task_wiretest.h"
#include "wt_bsp_io.h"
#include "stm32f429i_wt_eeprom.h"
#include "wt_bsp_infrared.h"
#include "wt_uart_24GHz.h"

#pragma diag_suppress 870 

extern GUI_CONST_STORAGE GUI_BITMAP bm_ICO_test;
extern GUI_CONST_STORAGE GUI_BITMAP bmbm_test_ok;
extern GUI_CONST_STORAGE GUI_BITMAP bmbm_test_fail;
extern GUI_CONST_STORAGE GUI_BITMAP bmbm_test_run;

extern WM_HWIN Create_TestDlgWindow(WM_HWIN hWin_para);
extern WM_HWIN CreateMessageBox(WM_HWIN hWin_para);
extern WM_HWIN CreateWarningBox(WM_HWIN hWin_para);
//extern WM_HWIN CreateTestOKBox(WM_HWIN hWin_para);
extern uint8_t WireTest_Process(void);
//extern void WT_Test(void);

extern K_ModuleItem_Typedef  wt_find;

uint8_t NG_point1 = 1;
uint8_t NG_point2 = 2;
uint8_t NG_points[8];
uint8_t NG_type = '0'; //0-断路  1-错位  2-短路

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0     		(GUI_ID_USER + 0x00)
#define ID_BUTTON_OK     		(GUI_ID_USER + 0x01)
#define ID_BUTTON_CANCEL    (GUI_ID_USER + 0x02)
#define ID_LISTVIEW_0     	(GUI_ID_USER + 0x03)
#define ID_TEXT_0     			(GUI_ID_USER + 0x04)
#define ID_TEXT_1     			(GUI_ID_USER + 0x05)
#define ID_TEXT_TESTINFO    (GUI_ID_USER + 0x06)
#define ID_IMAGE_TESTRESULT (GUI_ID_USER + 0x07)
#define ID_LISTVIEW_ERROR   (GUI_ID_USER + 0x08)
#define ID_TEXT_OKLOOP     	(GUI_ID_USER + 0x09)
#define ID_TEXT_NGLOOP     	(GUI_ID_USER + 0x0A)
#define ID_TEXT_ERRORINFO   (GUI_ID_USER + 0x0B)
#define ID_TEXT_OKLOOPNUM   (GUI_ID_USER + 0x0C)
#define ID_TEXT_NGLOOPNUM   (GUI_ID_USER + 0x0D)
#define ID_TEXT_ERRORNODE   (GUI_ID_USER + 0x0E)
#define ID_TEXT_ERRORINFO2  (GUI_ID_USER + 0x0F)
#define ID_TEXT_ERRORTYPE   (GUI_ID_USER + 0x10)
#define ID_TEXT_LOCKINFO    (GUI_ID_USER + 0x11)

#define DEF_LENGTH_ITEM_TABLE_BUFFER	400

///* Private defines -----------------------------------------------------------*/
///* Private macros ------------------------------------------------------------*/
///* Private variables ---------------------------------------------------------*/

static WM_HWIN hWin_wt_tester;
static	WM_HWIN hListview;
static	WM_HWIN hItem_testresult;
//static	WM_HWIN hListview_error;

//static uint32_t TestDelay = 1000;

uint8_t win_stat=1;//0--close window  1--not close window
uint8_t usb_check=0;//0-check usb   1--not check usb
uint8_t res=0;
uint8_t test_res=0;//0-not ok  1--ok

extern uint8_t WT_LogFiles_Write(uint8_t * path, uint8_t * str, uint16_t len);

static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);
void get_systemtime(char *time);
//static char logfilename[80];

static uint16_t items_index[DEF_LENGTH_ITEM_TABLE_BUFFER];
static uint32_t items_show;

extern uint32_t wheel_value;


static uint16_t listview_ID_FirstItem = 0;
static uint8_t  IsNewStatus =0;

static uint8_t File_state = 0;//0:not open 1:open
static uint8_t findpoint_stat = 0;//0:not find 1:find point


void update_table_WRDC(void);
void update_item_WRDC(int16_t *index);
void update_table_WRDC_Error(void);
void update_table_WRDC_OK(void);
void update_table_WRDC_ConnetcError(void);
void update_testinfo(void);


static uint8_t strMonth[][12] = {"01",
                           "02",
                           "03",
                           "04",
                           "05",
                           "06",
                           "07",
                           "08",
                           "09",
                           "10",
                           "11",
                           "12"};

K_ModuleItem_Typedef  wt_test =
{
  1,
  "测试",
  &bm_ICO_test,
  Startup,
  0,//ImageDirectOpen1,
};


/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 480, 222, 0, 0x0, 0 },
	{ LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 0, 0, 480, 200, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "开始", ID_BUTTON_OK, 360, 200, 60, 25, 0, 0x0, 0 },  
  { BUTTON_CreateIndirect, "退出", ID_BUTTON_CANCEL,  420, 200, 60, 25, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "测试数量：", ID_TEXT_0, 5, 202, 75, 25, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "TestNum", ID_TEXT_1, 85, 202, 60, 25, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "TestInfo", ID_TEXT_TESTINFO, 145, 202, 200, 25, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "通过回路：", ID_TEXT_OKLOOP, 15, 12, 75, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "oknum", ID_TEXT_OKLOOPNUM, 90, 12, 70, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "待测回路：", ID_TEXT_NGLOOP, 15, 45, 75, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "ngnum", ID_TEXT_NGLOOPNUM, 90, 45, 70, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "错误回路：", ID_TEXT_ERRORINFO, 15, 80, 75, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "errornode", ID_TEXT_ERRORNODE, 40, 125, 220, 80, 0, 0x0, 0 },
	//{ TEXT_CreateIndirect, "错误类型：", ID_TEXT_ERRORINFO2, 15, 170, 75, 20, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "errortype", ID_TEXT_ERRORTYPE, 160, 80, 70, 40, 0, 0x0, 0 },
	{ IMAGE_CreateIndirect, "Image", ID_IMAGE_TESTRESULT, 290, 2, 240, 192, 0, 0, 0 },
	{ TEXT_CreateIndirect, "lockinfo", ID_TEXT_LOCKINFO, 350, 85, 180, 80, 0, 0, 0 },
	//{ LISTVIEW_CreateIndirect, "Listview_error", ID_LISTVIEW_ERROR, 0, 0, 480, 200, 0, 0x0, 0 },
};


char *itoa(int num, char *str, int radix)   
{
    char  string[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char* ptr = str;
    int i;
    int j;
		
		if(num==0) 
		{
			strcpy(str,"0");
			return str;
		}

    while (num)
    {
        *ptr++  = string[num % radix];
        num    /= radix;

        if (num < radix)
        {
            *ptr++  = string[num];
            *ptr    = '\0';
            break;
        }
    }

    j = ptr - str - 1;

    for (i = 0; i < (ptr - str) / 2; i++)
    {
        int temp = str[i];
        str[i]   = str[j];
        str[j--] = temp;
    }

    return str;
}



///* Private functions ---------------------------------------------------------*/

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
	WM_HWIN hItem;
	HEADER_Handle hHeader;
	char portnum[3];
	char wiretype[20];
	char wireinfo[100];
	uint8_t buf8;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetFont(hItem,&GUI_FontHZ_Song_12);
		BUTTON_SetSkinClassic(hItem);
		BUTTON_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		BUTTON_SetBkColor(hItem,BUTTON_CI_UNPRESSED, GUI_GREEN);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
		BUTTON_SetFont(hItem,&GUI_FontHZ_Song_12);
		BUTTON_SetSkinClassic(hItem);
		BUTTON_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		BUTTON_SetBkColor(hItem,BUTTON_CI_UNPRESSED, GUI_RED);
	
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		hHeader = LISTVIEW_GetHeader(hItem);
		HEADER_SetFont(hHeader,&GUI_FontHZ_Song_12);
		HEADER_SetHeight(hHeader,20);
    LISTVIEW_AddColumn(hItem, 60, "序号", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 80, "测试类型", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 60, "节点", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 60, "节点", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 150, "测试参数", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 70, "测试结果", GUI_TA_HCENTER | GUI_TA_VCENTER);

		LISTVIEW_SetGridVis(hItem,1);
		LISTVIEW_SetDefaultGridColor(GUI_GRAY);
    LISTVIEW_SetRowHeight(hItem, 25);
		LISTVIEW_EnableSort(hItem);
		LISTVIEW_SetFont(hItem, &GUI_FontHZ_Song_16);
		WM_BringToTop(hItem);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
		TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		TEXT_SetText(hItem, "0");  
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TESTINFO);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		TEXT_SetText(hItem, " "); 
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_OKLOOP);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NGLOOP);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ERRORINFO);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_OKLOOPNUM);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		TEXT_SetText(hItem, " ");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NGLOOPNUM);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		TEXT_SetText(hItem, " ");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ERRORNODE);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_48);
		//TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		TEXT_SetTextColor(hItem,GUI_RED);
		TEXT_SetText(hItem, " ");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ERRORINFO2);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_16);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ERRORTYPE);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_24);
		TEXT_SetTextColor(hItem,GUI_RED);
		TEXT_SetText(hItem, " ");
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOCKINFO);
    TEXT_SetFont(hItem,&GUI_FontHZ_Song_32);
		TEXT_SetTextColor(hItem,GUI_RED);
		TEXT_SetText(hItem, "");
		
		hListview = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		WM_SetFocus(hListview);
		
		hItem_testresult = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_TESTRESULT);
		IMAGE_SetBitmap(hItem_testresult, &bmbm_test_ok);
		WM_HideWindow(hItem_testresult);
    break;
	case MY_MESSAGE_OK:
		GUI_EndDialog(pMsg->hWin,0);
		//Number_Windos = 0;
		win_stat=0;//0--close window  1--not close window
		break;
	case MY_MESSAGE_BUTTONOK:
		wt_SetText_Status("开始测试");
		strcpy(wireinfo,(char *)"线束:");
		buf8 = strlen((char *)TestFolder.FilesName[TestFolder.number_CurrentFile]) - 4;
		if(buf8 <= 16)
		{
			strncpy(wiretype,(char *)TestFolder.FilesName[TestFolder.number_CurrentFile],buf8);
			wiretype[buf8]='\0';	
		}
		else
		{
			strncpy(wiretype,(char *)TestFolder.FilesName[TestFolder.number_CurrentFile],14);
			wiretype[14]='.';
			wiretype[15]='.';
			wiretype[16]='\0';
		}
		strcat(wireinfo,wiretype);	
		strcat(wireinfo,"  ");
		strcat(wireinfo,(char *)"点位:");
		strcat(wireinfo,itoa(TestFile.number_point,portnum,10));
		strcat(wireinfo,"，");
		strcat(wireinfo,(char *)"回路:");
		strcat(wireinfo,itoa(TestFile.number_line,portnum,10));
		wt_SetText_Title(wireinfo);
		wt_SetText_Menu(wt_test.name);
//		Listview_test();
		WM_SetFocus(hListview);
		break;
	case WM_PAINT://绘制边框
		GUI_SetColor(GUI_DARKGRAY);
		GUI_SetPenSize(8);
		GUI_DrawRoundedFrame(2, 2, 290, 195, 0, 4);
		GUI_DrawRoundedFrame(2, 2, 480, 195, 0, 4);
		GUI_DrawRoundedFrame(2, 2, 290, 35, 0, 2);
		GUI_DrawRoundedFrame(2, 35, 290, 70, 0, 2);
		GUI_DrawRoundedFrame(2, 70, 290, 195, 0, 2);
		
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

///**
//  * @brief  Image browser window Startup
//  * @param  hWin: pointer to the parent handle.
//  * @param  xpos: X position 
//  * @param  ypos: Y position
//  * @retval None
//  */
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos)
{
	uint8_t buf8=0;
	uint32_t i;
	WM_HWIN hItem;
	WM_HWIN hDialog;
	WM_HWIN hMessagebox;
	WM_HWIN hWarningbox;
	WM_MESSAGE Msg;
	WM_HWIN hParent;

	uint64_t test_nums=0;//测试数量
	//uint8_t test_stat=0;//测试状态0-停止  1-测试
	//char logfilename[50];
//	char filename[40];
	char testinfo[50];
	
//	uint32_t num_rows=0;
	char nodes[5];
	char error_num[5];
	char num[5];

	int32_t wheel_stat=0;
	KeyLed_State.wheel=0;
	
	uint8_t  status_test_last = 0xFF;
	uint8_t  cnt_temp = 0;
	int16_t  index_item = 0;
	int32_t  number_wheel = KeyLed_State.wheel;
	uint32_t cnt_error;
//	uint8_t cnt_ok=0;//
//	uint8_t  status_lock_last = 0;//0-unlock  1-lock
	memset(NG_points,0,8);
	
	test_nums=0;//测试数量
	//For title string
	wt_SetText_Title("");
	wt_SetText_Menu(wt_test.name);
	wt_SetText_Status("准备测试");

	WT_StoreFiles_Init();//读取测试文件
	
	testwindow:
	hWin_wt_tester = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, hWin, xpos, ypos);
	WINDOW_SetBkColor(hWin_wt_tester,GUI_WHITE);
	hItem = WM_GetDialogItem(hWin_wt_tester, ID_LISTVIEW_0);
	LISTVIEW_SetFont(hItem, &GUI_FontHZ_Song_16);
	LISTVIEW_SetGridVis(hItem,1);
	LISTVIEW_SetDefaultGridColor(GUI_GRAY);
	WM_SetFocus(hItem);
	
	//创建告警对话框
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && k_StorageGetStatus(MSD_DISK_UNIT) == 0)	
	{
		TestFile.item_total=0;
		hMessagebox = CreateMessageBox(hWin_wt_tester);
		WM_BringToTop(hMessagebox);
		while(WM_IsWindow(hMessagebox))
		{
			if(BSP_GetKEY_State(KeyOK) == 1 )//wujun added 
			{
				while(BSP_GetKEY_State(KeyOK) == 1)
				{
					osDelay(10);
				}
				Msg.MsgId = MY_MESSAGE_OK;
				Msg.hWinSrc=hWin_wt_tester;
				hItem=WM_GetFocussedWindow();
				hParent=hMessagebox;
				WM_SendMessage(hParent,&Msg);
			}
			GUI_Delay(20);
		}
		buf8 = 1;
	}
	else if(File_state == 0 || TestFile.file_status !=1)//0:not open 1:open
	{
		//if(TestFile.file_status  != 1)//0:not init, 1: init ok, 2: hardware error, 3-no this file
		
		//创建测试文件选择对话框
		hDialog=Create_TestDlgWindow(hWin_wt_tester);
		GUI_Exec();
		lockstat:
		while (WM_IsWindow(hDialog))
		{
			if(BSP_GetLock_State() == 1 && WT_Config.Lockstat == 0xFE) //锁定
			{
				buf8 = 10;
				while(BSP_GetLock_State() == 1)
				{
					osDelay(20);
					buf8--;
					if(buf8 == 0) break;
				}
				Msg.MsgId = MY_MESSAGE_LOCK;
				Msg.hWinSrc=hWin_wt_tester;
				hItem=WM_GetFocussedWindow();
				hParent=WM_GetParent(hItem);
				WM_SendMessage(hParent,&Msg);
				goto lockstat;
			}
			if(BSP_GetLock_State() == 0 ) //解锁
			{
				buf8 = 10;
				while(BSP_GetLock_State() == 1)
				{
					osDelay(20);
					buf8--;
					if(buf8 == 0) break;
				}
				WT_Config.Lockstat = 0;
				WT_Config_Lockstat_Save();
				
				Msg.MsgId = MY_MESSAGE_UNLOCK;
				Msg.hWinSrc=hWin_wt_tester;
				hItem=WM_GetFocussedWindow();
				hParent=WM_GetParent(hItem);
				WM_SendMessage(hParent,&Msg);
			}
			
			if(BSP_GetKEY_State(KeyOK) == 1 )//wujun added 
			{
				while(BSP_GetKEY_State(KeyOK) == 1)
				{
					osDelay(10);
				}
				Msg.MsgId = MY_MESSAGE_OK;
				Msg.hWinSrc=hWin_wt_tester;
				hItem=WM_GetFocussedWindow();
				hParent=WM_GetParent(hItem);
				WM_SendMessage(hParent,&Msg);
				buf8 = 0;
				File_state = 1;//0:not open 1:open
				//日志记录
				wt_SetText_Status("正在准备测试文件");
				if(TestFile.file_status !=1	)
				{
					wt_SetText_Status("文件打开错误！");
				}
//				get_systemtime(filename);
//				if(store_dev == 0)//usb
//				{
//					strcpy(logfilename,path_Logfile);
//				}
//				if(store_dev == 1)//SD
//				{
//					strcpy(logfilename,path_studyfile_sd);
//				}
//				strcat(logfilename,"/");
//				strcat(logfilename,filename);
//				strcat(logfilename,".log");
//				WT_LogFiles_Write((uint8_t *)logfilename,(uint8_t *)"LogInfo",8);//uint8_t * path, uint8_t * str, uint16_t len
				
			}
			if(BSP_GetKEY_State(KeyCancle) == 1 ) 
			{
				while(BSP_GetKEY_State(KeyCancle) == 1)
				{
					osDelay(10);
				}
				Msg.MsgId = MY_MESSAGE_CANCEL;
				Msg.hWinSrc=hWin_wt_tester;
				hItem=WM_GetFocussedWindow();
				hParent=WM_GetParent(hItem);
				WM_SendMessage(hParent,&Msg);	
				buf8 = 2;
			}
			if(BSP_GetKEY_State(KeyWheel) == 1 )//改变控件焦点
			{
				while(BSP_GetKEY_State(KeyWheel) == 1)
				{
					osDelay(10);
				}
				Msg.MsgId = MY_MESSAGE_CLICK;
				hItem=WM_GetFocussedWindow();
				hParent=hDialog;
				WM_SendMessage(hParent,&Msg);
			}
		
			if(BSP_GetKEY_State(KeyDown) == 1)
			{
				buf8 = 10;
				while(BSP_GetKEY_State(KeyDown) == 1)
				{
					osDelay(20);
					buf8--;
					if(buf8 == 0) break;
				}
				Msg.MsgId = MY_MESSAGE_DOWN;
				hItem=WM_GetFocussedWindow();
				hParent=WM_GetParent(hItem);
				WM_SendMessage(hParent,&Msg);
			}
			
			if(BSP_GetKEY_State(KeyUp) == 1)
			{
				buf8 = 10;
				while(BSP_GetKEY_State(KeyUp) == 1)
				{
					osDelay(20);
					buf8--;
					if(buf8 == 0) break;
				}
				Msg.MsgId = MY_MESSAGE_UP;
				WM_SendMessage(hDialog,&Msg);
			}
			if(KeyLed_State.wheel!=wheel_stat)//处理滚轮事件
			{
				if(wheel_stat<KeyLed_State.wheel)//顺时针旋转，向下选择条目
				{
					Msg.MsgId = MY_MESSAGE_DOWN;
					WM_SendMessage(hDialog,&Msg);
				}
				else//逆时针旋转，向上选择条目
				{
					Msg.MsgId = MY_MESSAGE_UP;
					WM_SendMessage(hDialog,&Msg);
				}
				wheel_stat=KeyLed_State.wheel;
			}
		
		  osDelay(40);
			GUI_Delay(20);
		 }
	 }
	else {buf8 = 0;}
	
	// no file, close test dialog
	if(buf8 != 0) 
	{
		// close dialog
		TestFile.command=2;
		KeyLed_State.wheel=0;
		wheel_value=0;
		GUI_EndDialog(hWin_wt_tester,0);
		GUI_Exec();
		win_stat=0;	
		Number_Windos = 0;
		return;
	}
	
	//TestFile.test_num=0;	
	while(1)
	{
//		//检测锁状态
//		lockstat_check:
//		if(BSP_GetLock_State() == 1 && WT_Config.Lockstat == 0xFE)
//		{
//			status_lock_last = 1;
//			continue;
//		}
//		if(BSP_GetLock_State() == 0)
//		{
//			buf8 = 10;
//			while(BSP_GetLock_State() == 0)
//			{
//				osDelay(20);
//				buf8--;
//				if(buf8 == 0) break;
//			}
//			
//			if(status_lock_last == 1)
//			osMessagePut(WireTestEvent, WIRETEST_START_EVENT, 0);//开始测试
//			WT_Config.Lockstat = 0x00;
//			WT_Config_Lockstat_Save();
//			status_lock_last = 0;
//			//TestFile.command = 1;//0-no operate, 1-start, 2-cancel
//			//WM_HideWindow(hListview);
//			hItem = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_LOCKINFO);
//			TEXT_SetText(hItem, "");
//			GUI_Exec();
//		}
		
		test_loop:
		//update 
		if(status_test_last != TestFile.task_status)
		{
			IsNewStatus = 1;
			status_test_last = TestFile.task_status;
			index_item = 0;
			cnt_temp = 0;
			number_wheel = KeyLed_State.wheel;
			listview_ID_FirstItem = 0;
			
			switch(status_test_last)
			{
				case 0:
					WM_ShowWindow(hListview);
					WM_HideWindow(hItem_testresult);
					GUI_Exec();	
					update_table_WRDC();
					break;
				
				case 1:
					//关闭测试成功对话框
					WM_HideWindow(hItem_testresult);
					GUI_Exec();		
					wt_SetText_Status("循环测试中");
					update_table_WRDC_Error();
					break;
				
				default:
					break;
			}
		}
		//更新计数器保存日志
		if(TestFile.test_num != test_nums  ) 
		{						
			test_nums=TestFile.test_num;
			itoa(TestFile.test_num,num,10);
			hItem = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_1);
			TEXT_SetText(hItem, num);  
		}
	
		
		switch(TestFile.task_status)	//0-waiting, 1-testing, 2-test error, 3-test ok, 4-waiting remove wire, 5-testing & Z error
		{
			case 0:	//waiting
				wt_SetText_Status("等待测试");	
				if(BSP_GetKEY_State(KeyDown) == 1) //向下选择条目
				{
					buf8 = 5;
					while(BSP_GetKEY_State(KeyDown) == 1)
					{
						osDelay(20);
						buf8--;
						if(buf8 == 0) break;
					}
					index_item++;
				}
				if(BSP_GetKEY_State(KeyUp) == 1) //向上选择条目
				{
					buf8 = 5;
					while(BSP_GetKEY_State(KeyUp) == 1)
					{
						osDelay(20);
						buf8--;
						if(buf8 == 0) break;
					}
					index_item--;
				}
				if(KeyLed_State.wheel != number_wheel)//滚轮
				{
					index_item += KeyLed_State.wheel - number_wheel;
					number_wheel = KeyLed_State.wheel;
				}				
				if(BSP_GetKEY_State(KeyCancle) == 1) //退出测试程序
				{
					//弹出告警对话框
					while(BSP_GetKEY_State(KeyCancle) == 1)
					{
						osDelay(10);
					}
					hWarningbox = CreateWarningBox(hWin_wt_tester);			
					WM_BringToTop(hWarningbox);
					GUI_Exec();
					win_stat=1;
					while(WM_IsWindow(hWarningbox))
					{
						if(BSP_GetKEY_State(KeyOK) == 1 )//确定关闭
						{
							while(BSP_GetKEY_State(KeyOK) == 1)
							{
								osDelay(10);
							}
							Msg.MsgId = MY_MESSAGE_OK;
							hItem=WM_GetFocussedWindow();
							//hParent=WM_GetParent(hItem);
							hParent=hWarningbox;
							TestFile.command=2;
							KeyLed_State.wheel=0;
							wheel_value=0;
							File_state = 0;
							WM_SendMessage(hParent,&Msg);//将消息发送到控件所属窗口
							if(win_stat != 1  )////0--close window  1--not close window
							{						
								Number_Windos = 0;
								return;
							}
						
						}
						if(BSP_GetKEY_State(KeyCancle) == 1 )//取消关闭
						{
							while(BSP_GetKEY_State(KeyCancle) == 1)
							{
								osDelay(10);
							}
							GUI_EndDialog(hWarningbox,0);
							GUI_Exec();

						}
	
						osDelay(10);
					}		
				}
				if(BSP_GetKEY_State(KeyOK) == 1 ) //执行测试程序
				{
					while(BSP_GetKEY_State(KeyOK) == 1)
					{
						osDelay(20);
					}
					if(TestFile.item_total>0)//开始测试
					{
						wt_SetText_Status("循环测试中");	
						//test_stat=1;
						hItem = WM_GetDialogItem(hWin_wt_tester, ID_BUTTON_OK);
						TEXT_SetBkColor(hItem, GUI_LIGHTGRAY);
						hItem = WM_GetDialogItem(hWin_wt_tester, ID_BUTTON_CANCEL);
						TEXT_SetBkColor(hItem, GUI_RED);
						TEXT_SetText(hItem,(char *)"停止");
						osMessagePut(WireTestEvent, WIRETEST_START_EVENT, 0);//开始测试
						//osMessagePut(Uart24GHzEvent, Uart24GHZ_IMSTEST_Event, 0);//同IMS服务器通信
					}
					WM_HideWindow(hListview);
				}
				if(BSP_GetKEY_State(KeyWheel) == 1 )//跳转到找点窗口
				{
					while(BSP_GetKEY_State(KeyWheel) == 1)
					{
						osDelay(10);
					}
					findpoint:
					findpoint_stat = 0;
					status_test_last = 0xFF;
					GUI_EndDialog(hWin_wt_tester,0);
					GUI_Exec();
					wt_find.startup(WM_HBKWIN, 0, 25);
					goto testwindow;
				}
				
				if(findpoint_stat == 1) goto findpoint;
						
				//update items
				update_item_WRDC(&index_item);
				WM_ShowWindow(hListview);
				osDelay(30);
				break;
			
			case 1: //testing
				if((cnt_temp % 50) == 0)
				{
					update_table_WRDC_Error();
				}
				wt_SetText_Status("循环测试中");	
				//显示对话框
				IMAGE_SetBitmap(hItem_testresult, &bmbm_test_run);
				WM_ShowWindow(hItem_testresult);
				WM_BringToTop(hItem_testresult);
				GUI_Exec();

				if(BSP_GetKEY_State(KeyCancle) == 1) //停止测试程序
				{
					//停止测试
					while(BSP_GetKEY_State(KeyCancle) == 1)
					{
						osDelay(20);
					}
					
					stoptesting:
					
					TestFile.command=2;
					//test_stat=0;
					hItem = WM_GetDialogItem(hWin_wt_tester, ID_BUTTON_OK);
					TEXT_SetBkColor(hItem, GUI_GREEN);
					hItem = WM_GetDialogItem(hWin_wt_tester, ID_BUTTON_CANCEL);
					//TEXT_SetBkColor(hItem, GUI_LIGHTGRAY);
					TEXT_SetText(hItem,(char *)"退出");	
					
					//关闭测试成功对话框
					WM_HideWindow(hItem_testresult);
					GUI_Exec();							
				}
				if(BSP_GetKEY_State(KeyWheel) == 1 )//跳转到找点窗口
				{
					while(BSP_GetKEY_State(KeyWheel) == 1)
					{
						osDelay(10);
					}
					findpoint_stat = 1;//0:not find  1:find
					goto stoptesting;
				}
				
				//update test info
				strcpy(testinfo,(char *)"总条目/错误条目:");
				strcat(testinfo,itoa(TestFile.item_total,error_num,10));
				strcat(testinfo,"/");
				strcat(testinfo,itoa(TestFile.item_error_count,error_num,10));
				hItem = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_TESTINFO);
				TEXT_SetText(hItem, testinfo); 
				//update items
				update_item_WRDC(&index_item);
				WM_HideWindow(hListview);
				//update_testinfo();
				osDelay(10);
				break;
		 
			
			case 2: //test error
				IMAGE_SetBitmap(hItem_testresult, &bmbm_test_fail);
				WM_ShowWindow(hItem_testresult);
				WM_BringToTop(hItem_testresult);
				GUI_Exec();
				osDelay(20);
				break;
			
			case 3:	//test ok
				wt_SetText_Status("测试成功");
				hItem = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_TESTINFO);
				TEXT_SetText(hItem, " "); 
				test_res=1;
				
			//显示测试成功消息框
				IMAGE_SetBitmap(hItem_testresult, &bmbm_test_ok);
				WM_ShowWindow(hItem_testresult);
			  WM_HideWindow(hListview);
				GUI_Exec();

//				//update items
				update_item_WRDC(&index_item);
				osDelay(50);
				break;
			
			case 4:	//waiting remove wire
				if((cnt_temp % 50) == 0)
				{
					update_table_WRDC_OK();
				}
				//统计剩余未拔掉线束条目数
				cnt_error = 0;
				for(i=0;i<TestFile.item_total;i++)
				{
					if(TestFile.test_item[i].type=='Z')
					{
						continue;
					}
					if(TestFile.test_item[i].result != 0)
					{
						continue;
					}
					else 
					{
						cnt_error++;
					}
				}
				strcpy(testinfo,(char *)"等待下一组测试,剩余线束");
				itoa(items_show,nodes,10);
				strcat(testinfo,nodes);
				strcat(testinfo,"组");
				wt_SetText_Status(testinfo);
				hItem = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_TESTINFO);
				TEXT_SetText(hItem, " "); 
				//if(items_num_wrdc == items_show)
				if(items_show > 0)
				{
					IMAGE_SetBitmap(hItem_testresult, &bmbm_test_ok);
					WM_ShowWindow(hItem_testresult);
					WM_BringToTop(hItem_testresult);
					GUI_Exec();		
				}
				else
				{
					WM_HideWindow(hItem_testresult);
					test_res=0;
					GUI_Exec();
					//cnt_ok--;
				}
				
//				if(BSP_GetKEY_State(KeyDown) == 1) //向下选择条目
//				{
//					buf8 = 5;
//					while(BSP_GetKEY_State(KeyDown) == 1)
//					{
//						osDelay(20);
//						buf8--;
//						if(buf8 == 0) break;
//					}
//					index_item++;
//				}
//				if(BSP_GetKEY_State(KeyUp) == 1) //向上选择条目
//				{
//					buf8 = 5;
//					while(BSP_GetKEY_State(KeyUp) == 1)
//					{
//						osDelay(20);
//						buf8--;
//						if(buf8 == 0) break;
//					}
//					index_item--;
//				}
//				if(KeyLed_State.wheel != number_wheel)//滚轮
//				{
//					index_item += KeyLed_State.wheel - number_wheel;
//					number_wheel = KeyLed_State.wheel;
//				}				
				if(BSP_GetKEY_State(KeyCancle) == 1) //停止测试程序
				{
					//停止测试
					while(BSP_GetKEY_State(KeyCancle) == 1)
					{
						osDelay(20);
					}
					TestFile.command=2;
					//test_stat=0;
					LED4_OFF();
					//停止发送红外信号
					//Infra_PWM_OFF();
					UART_Infr_Switch = 1;//关闭红外信号
					
					hItem = WM_GetDialogItem(hWin_wt_tester, ID_BUTTON_OK);
					TEXT_SetBkColor(hItem, GUI_GREEN);
					hItem = WM_GetDialogItem(hWin_wt_tester, ID_BUTTON_CANCEL);
					//TEXT_SetBkColor(hItem, GUI_LIGHTGRAY);
					TEXT_SetText(hItem,(char *)"退出");	
					//关闭测试成功对话框
					WM_HideWindow(hItem_testresult);	
					GUI_Exec();	
					
				}
				//update items
				update_item_WRDC(&index_item);
				WM_HideWindow(hListview);
				osDelay(20);
				break;			
			case 5: //5-testing & Z error
				if((cnt_temp % 50) == 0)
				{
					update_table_WRDC_ConnetcError();
				}
				wt_SetText_Status("测试错误");	
				//显示测试错误消息框
				IMAGE_SetBitmap(hItem_testresult, &bmbm_test_fail);
				WM_ShowWindow(hItem_testresult);
				GUI_Exec();
				
//				//锁定测试界面
//				if(BSP_GetLock_State() == 1)
//				{
//					buf8 = 5;
//					while(BSP_GetLock_State() == 1)
//					{
//						osDelay(20);
//						buf8--;
//						if(buf8 == 0) break;
//					}
//					
//					WT_Config.Lockstat = 0xFE;
//					WT_Config_Lockstat_Save();
//					wt_SetText_Status("状态锁定，请解锁！");	
//					hItem = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_LOCKINFO);
//					TEXT_SetText(hItem, "锁定");
//					WM_HideWindow(hItem_testresult);
//					GUI_Exec();
//					TestFile.command=2;
//					goto lockstat_check;
//				}
				
//				if(BSP_GetKEY_State(KeyDown) == 1) //向下选择条目
//				{
//					buf8 = 5;
//					while(BSP_GetKEY_State(KeyDown) == 1)
//					{
//						osDelay(20);
//						buf8--;
//						if(buf8 == 0) break;
//					}
//					index_item++;
//				}
//				if(BSP_GetKEY_State(KeyUp) == 1) //向上选择条目
//				{
//					buf8 = 5;
//					while(BSP_GetKEY_State(KeyUp) == 1)
//					{
//						osDelay(20);
//						buf8--;
//						if(buf8 == 0) break;
//					}
//					index_item--;
//				}
//				if(KeyLed_State.wheel != number_wheel)//滚轮
//				{
//					index_item += KeyLed_State.wheel - number_wheel;
//					number_wheel = KeyLed_State.wheel;
//				}				
				if(BSP_GetKEY_State(KeyCancle) == 1) //停止测试程序
				{
					//停止测试
					while(BSP_GetKEY_State(KeyCancle) == 1)
					{
						osDelay(20);
					}
					TestFile.command = 2;
					//test_stat=0;
					hItem = WM_GetDialogItem(hWin_wt_tester, ID_BUTTON_OK);
					TEXT_SetBkColor(hItem, GUI_GREEN);
					hItem = WM_GetDialogItem(hWin_wt_tester, ID_BUTTON_CANCEL);
					//TEXT_SetBkColor(hItem, GUI_LIGHTGRAY);
					TEXT_SetText(hItem,(char *)"退出");	
					//关闭测试成功对话框
					WM_HideWindow(hItem_testresult);	
					GUI_Exec();	
					goto test_loop;
				}
				if(BSP_GetKEY_State(KeyWheel) == 1 )//跳转到找点窗口
				{
					while(BSP_GetKEY_State(KeyWheel) == 1)
					{
						osDelay(10);
					}
					findpoint_stat = 1;//0:not find  1:find
					goto stoptesting;
				}
				//update items
				update_item_WRDC(&index_item);
				osDelay(20);
				break;
							
			default:
				break;
		}
		//delay
		osDelay(20); 
		GUI_Exec();
	}
}


//更新WRDC类型结构体
void update_table_WRDC(void)
{
	uint32_t i;
	uint32_t buf32;
	
	for(i=0;i<sizeof(items_index);i++) items_index[i]=0;
	
	//过滤断路条目
	buf32 = 0;
	for(i=0;i<TestFile.item_total;i++)
	{
		if(TestFile.test_item[i].type=='Z')
		{
			continue;
		}
		else 
		{
			if(buf32 < DEF_LENGTH_ITEM_TABLE_BUFFER) items_index[buf32]=i;
			buf32++;
		}
	}
	items_show = buf32;
}

//更新WRDC类型结构体
void update_table_WRDC_Error(void)
{
	uint32_t i;
	uint32_t buf32;
	
	for(i=0;i<sizeof(items_index);i++) items_index[i]=0;
	
	//过滤断路条目
	buf32 = 0;
	for(i=0;i<TestFile.item_total;i++)
	{
//		if(TestFile.test_item[i].type=='Z')
//		{
//			continue;
//		}
		//if(TestFile.test_item[i].result != 2)//0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
		if(TestFile.test_item[i].result == 1 || TestFile.test_item[i].result == 0)
		{
			continue;
		}
		else 
		{
			//items_index[buf32]=i;
			if(buf32 < DEF_LENGTH_ITEM_TABLE_BUFFER) items_index[buf32]=i;
			buf32++;
		}
	}
	items_show = buf32;
}

//更新错误连接条目
void update_table_WRDC_ConnetcError(void)
{
	uint32_t i;
	uint32_t buf32;
	
	for(i=0;i<sizeof(items_index);i++) items_index[i]=0;
	
	//过滤断路条目
	buf32 = 0;
	for(i=0;i<TestFile.item_total;i++)
	{
		if(TestFile.test_item[i].type != 'Z')
		{
			continue;
		}
		if(TestFile.test_item[i].result != 4)//0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
		{
			continue;
		}
		else 
		{
			//items_index[buf32]=i;
			if(buf32 < DEF_LENGTH_ITEM_TABLE_BUFFER) items_index[buf32]=i;
			buf32++;
		}
	}
	items_show = buf32;
}

//更新WRDC类型结构体
void update_table_WRDC_OK(void)
{
	uint32_t i;
	uint32_t buf32;
	uint32_t buf32ALL;
	
	for(i=0;i<sizeof(items_index);i++) items_index[i]=0;
	
	//过滤断路条目
	buf32 = 0;
	buf32ALL = 0;
	for(i=0;i<TestFile.item_total;i++)
	{
		if(TestFile.test_item[i].type=='Z')
		{
			continue;
		}
		if(TestFile.test_item[i].result != 1)
		{
			buf32ALL++;
			continue;
		}
		else 
		{
			//items_index[buf32]=i;
			if(buf32 < DEF_LENGTH_ITEM_TABLE_BUFFER) items_index[buf32]=i;
			buf32++;
			buf32ALL++;
		}
	}
	items_show = buf32;
	//items_num_wrdc = buf32ALL;
}
//更新WRDC显示
void update_item_WRDC(int16_t *index)
{
	WM_HWIN  hItem;
	WM_HWIN  hItem_node;
	WM_HWIN  hItem_error_type;
	WM_HWIN  hItem_okloop;
	WM_HWIN  hItem_ngloop;
	WM_HWIN  hItem_num;
	uint8_t  buf8;
	uint16_t buf16;
	uint16_t i;
	static uint32_t items_show_last=0;

	const char * pText[2][6];
	char id[5];
	char p1[5];
	char p2[5];
	char param[20];
	char points[30];
	
	char ok_loop[6];
	char ng_loop[6];
	char num[8];
	
	hItem = WM_GetDialogItem(hWin_wt_tester, ID_LISTVIEW_0);
	hItem_node = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_ERRORNODE);
	hItem_error_type = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_ERRORTYPE);
	hItem_okloop = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_OKLOOPNUM);
	hItem_ngloop = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_NGLOOPNUM);
	hItem_num = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_1);
	
	if(items_show <= 0) 
	{
		*index=0;
		//delete items
		while(1)
		{
			buf8 = LISTVIEW_GetNumRows(hItem);
			if(buf8 != 0) LISTVIEW_DeleteRow(hItem,0);
			else break;
		}
		return;
	}
	
	// check index
	if(*index < 0) *index=0;
	else if(*index >= items_show) *index = items_show-1;
	//hItem = WM_GetDialogItem(hWin_wt_tester, ID_LISTVIEW_0);
	
	//更新回路及节点信息 wujun added 2015.3.7
	memset(ok_loop,0,6);
	memset(ng_loop,0,6);
	sprintf(ok_loop,"%d",TestFile.cnt_okloop);
	sprintf(ng_loop,"%d",(TestFile.number_line-TestFile.cnt_okloop));
	TEXT_SetText(hItem_okloop, ok_loop);
	TEXT_SetText(hItem_ngloop, ng_loop);
	
	itoa(TestFile.test_num,num,10);
	TEXT_SetText(hItem_num, num);  
	
	// 新状态，更新整个控件
	if(IsNewStatus != 0 || items_show_last != items_show)
	{
		IsNewStatus = 0;
		items_show_last = items_show;
		listview_ID_FirstItem = 0;
		
		//更新节点信息 wujun added 2015.3.5
		for(i=0;i<items_show;i++)
		{
			memset(points,0,30);
			if(TestFile.test_item[items_index[i]].result == 2 && TestFile.test_item[items_index[i]].type == 'W')//断路
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				NG_point1 = TestFile.test_item[items_index[i]].p1;
				NG_point2 = TestFile.test_item[items_index[i]].p2;
				NG_type = '0';
				sprintf(points,"%s-%s",p1,p2);
				sprintf((char *)NG_points,"%s,%s",p1,p2);
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "断路");
				break;
			}
			else if(TestFile.test_item[items_index[i]].result == 3 && TestFile.test_item[items_index[i]].type == 'Z')//错位
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				NG_point1 = TestFile.test_item[items_index[i]].p1;
				NG_point2 = TestFile.test_item[items_index[i]].p2;
				NG_type = '1';
				sprintf(points,"%s-%s",p1,p2);
				sprintf((char *)NG_points,"%s,%s",p1,p2);
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "错位");
				break;
			}
			else if(TestFile.test_item[items_index[i]].result == 4 && TestFile.test_item[items_index[i]].type == 'Z')//短路
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				sprintf(points,"%s-%s",p1,p2);
				sprintf((char *)NG_points,"%s,%s",p1,p2);
				NG_point1 = TestFile.test_item[items_index[i]].p1;
				NG_point2 = TestFile.test_item[items_index[i]].p2;
				NG_type = '2';
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "短路");
				break;
			}
			else if(TestFile.test_item[items_index[i]].result == 2 && TestFile.test_item[items_index[i]].type == 'R')//电阻
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				sprintf(points,"%s-%s",p1,p2);
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "电阻");
				break;
			}
			else if(TestFile.test_item[items_index[i]].result == 2 && TestFile.test_item[items_index[i]].type == 'C')//电容
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				sprintf(points,"%s-%s",p1,p2);
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "电容");
				break;
			}
			else
			{
				TEXT_SetText(hItem_node, "");
				TEXT_SetText(hItem_error_type, "");
				break;
			}
		}
		
		
		//delete items
		while(1)
		{
			buf8 = LISTVIEW_GetNumRows(hItem);
			if(buf8 > 0) LISTVIEW_DeleteRow(hItem,0);
			else break;
		}

		//add item
		if(items_show > 9) 	buf8 = 9;
		else								buf8 = items_show;
		for(i=0;i<buf8;i++)
		{
			buf16 = i;
			itoa(buf16+1,id,10);
			pText[0][0]=id;

			//itoa(TestFile.test_item[items_index[buf16]].p1,p1,10);
			num_converter(TestFile.test_item[items_index[buf16]].p1,p1);
			pText[0][2]=p1;
			num_converter(TestFile.test_item[items_index[buf16]].p2,p2);
			//itoa(TestFile.test_item[items_index[buf16]].p2,p2,10);
			pText[0][3]=p2;

			if(TestFile.test_item[items_index[buf16]].type=='W')
			{
				pText[0][1]="导通";
				sprintf(param,"RGB=%d%d%d",TestFile.test_item[items_index[buf16]].param1,TestFile.test_item[items_index[buf16]].param2,TestFile.test_item[items_index[buf16]].param3);
			}
			else if(TestFile.test_item[items_index[buf16]].type=='D')
			{
				pText[0][1]="二极管";
				sprintf(param,"V=%d",TestFile.test_item[items_index[buf16]].param1);
			}
			else if(TestFile.test_item[items_index[buf16]].type=='C')
			{
				pText[0][1]="电容";
				sprintf(param,"C=%d",TestFile.test_item[items_index[buf16]].param1);
			}
			else if(TestFile.test_item[items_index[buf16]].type=='R')
			{
				pText[0][1]="电阻";
				sprintf(param,"R=%d",TestFile.test_item[items_index[buf16]].param1);
			}
			else if(TestFile.test_item[items_index[buf16]].type=='Z')
			{
				pText[0][1]="高阻";
				sprintf(param,"Z=%d",TestFile.test_item[items_index[buf16]].param2);
			}
			else
			{
				pText[0][1]="开关";
				sprintf(param,"K=%d",TestFile.test_item[items_index[buf16]].param2);
			}
			pText[0][4]=param;
			if(TestFile.test_item[items_index[buf16]].result==0)//not test
			{
				pText[0][5]="-";
			}
			else if(TestFile.test_item[items_index[buf16]].result==1)//ok
			{
				pText[0][5]="OK";
			}
			else if(TestFile.test_item[items_index[buf16]].result==2)
			{
				pText[0][5]="断路";
			}
			else if(TestFile.test_item[items_index[buf16]].result==3)
			{
				pText[0][5]="错位";
			}
			else 
			{
				pText[0][5]="短路";
			}
			
			LISTVIEW_AddRow(hItem,pText[0]);
		}
		LISTVIEW_SetSel(hItem,0);
	}
	else
	{
		if(*index < listview_ID_FirstItem)	//显示上移
		{
			//delete item
			buf8 = listview_ID_FirstItem - *index;
			if(buf8 > 9) buf8=9;
			for(i=0;i<buf8;i++) LISTVIEW_DeleteRow(hItem,8-i);
			
			if(listview_ID_FirstItem > buf8)  listview_ID_FirstItem -= buf8;
			else															listview_ID_FirstItem = 0;

			//add item
			for(i=0;i<buf8;i++)
			{
				//buf16 = *index + buf8 - 1 - i;
				if(buf16 >= DEF_LENGTH_ITEM_TABLE_BUFFER) 	buf16 = DEF_LENGTH_ITEM_TABLE_BUFFER - 1;
				else 										buf16 = *index + buf8 - 1 - i;
				itoa(buf16+1,id,10);
				pText[0][0]=id;

				//itoa(TestFile.test_item[items_index[buf16]].p1,p1,10);
				num_converter(TestFile.test_item[items_index[buf16]].p1,p1);
				pText[0][2]=p1;
				//itoa(TestFile.test_item[items_index[buf16]].p2,p2,10);
				num_converter(TestFile.test_item[items_index[buf16]].p2,p2);
				pText[0][3]=p2;

				if(TestFile.test_item[items_index[buf16]].type=='W')
				{
					pText[0][1]="导通";
					sprintf(param,"RGB=%d%d%d",TestFile.test_item[items_index[buf16]].param1,TestFile.test_item[items_index[buf16]].param2,TestFile.test_item[items_index[buf16]].param3);
				}
				else if(TestFile.test_item[items_index[buf16]].type=='D')
				{
					pText[0][1]="二极管";
					sprintf(param,"V=%d",TestFile.test_item[items_index[buf16]].param1);
				}
				else if(TestFile.test_item[items_index[buf16]].type=='C')
				{
					pText[0][1]="电容";
					sprintf(param,"C=%d",TestFile.test_item[items_index[buf16]].param1);
				}
				else if(TestFile.test_item[items_index[buf16]].type=='R')
				{
					pText[0][1]="电阻";
					sprintf(param,"R=%d",TestFile.test_item[items_index[buf16]].param1);
				}
				else if(TestFile.test_item[items_index[buf16]].type=='Z')
				{
					pText[0][1]="高阻";
					sprintf(param,"Z=%d",TestFile.test_item[items_index[buf16]].param2);
				}
				else
				{
					pText[0][1]="开关";
					sprintf(param,"K=%d",TestFile.test_item[items_index[buf16]].param2);
				}
				pText[0][4]=param;
				if(TestFile.test_item[items_index[buf16]].result==0)//not test
				{
					pText[0][5]="-";
				}
				else if(TestFile.test_item[items_index[buf16]].result==1)//ok
				{
					pText[0][5]="OK";
				}
				else if(TestFile.test_item[items_index[buf16]].result==2)
				{
					pText[0][5]="断路";
				}
				else if(TestFile.test_item[items_index[buf16]].result==3)
				{
					pText[0][5]="错位";
				}
				else 
				{
					pText[0][5]="短路";
				}
				
				LISTVIEW_InsertRow(hItem, 0, pText[0]);
			}
			LISTVIEW_SetSel(hItem,0);
		}
		else if(*index > listview_ID_FirstItem +8)	//显示下移
		{
			//delete item
			buf8 = *index - listview_ID_FirstItem - 8;
			if(buf8 > 9) buf8=9;
			for(i=0;i<buf8;i++) LISTVIEW_DeleteRow(hItem,0);
			listview_ID_FirstItem += buf8;

			//add item
			for(i=0;i<buf8;i++)
			{
				//buf16 = *index - buf8 + 1 + i;
				if(buf16 >= DEF_LENGTH_ITEM_TABLE_BUFFER) 	buf16 = DEF_LENGTH_ITEM_TABLE_BUFFER - 1;
				else 										buf16 = *index - buf8 + 1 + i;
				itoa(buf16+1,id,10);
				pText[1][0]=id;

				//itoa(TestFile.test_item[items_index[buf16]].p1,p1,10);
				num_converter(TestFile.test_item[items_index[buf16]].p1,p1);
				pText[1][2]=p1;
				//itoa(TestFile.test_item[items_index[buf16]].p2,p2,10);
				num_converter(TestFile.test_item[items_index[buf16]].p2,p2);
				pText[1][3]=p2;

				if(TestFile.test_item[items_index[buf16]].type=='W')
				{
					pText[1][1]="导通";
					sprintf(param,"RGB=%d%d%d",TestFile.test_item[items_index[buf16]].param1,TestFile.test_item[items_index[buf16]].param2,TestFile.test_item[items_index[buf16]].param3);
				}
				else if(TestFile.test_item[items_index[buf16]].type=='D')
				{
					pText[1][1]="二极管";
					sprintf(param,"V=%d",TestFile.test_item[items_index[buf16]].param1);
				}
				else if(TestFile.test_item[items_index[buf16]].type=='C')
				{
					pText[1][1]="电容";
					sprintf(param,"C=%d",TestFile.test_item[items_index[buf16]].param1);
				}
				else if(TestFile.test_item[items_index[buf16]].type=='R')
				{
					pText[1][1]="电阻";
					sprintf(param,"R=%d",TestFile.test_item[items_index[buf16]].param1);
				}
				else if(TestFile.test_item[items_index[buf16]].type=='Z')
				{
					pText[1][1]="高阻";
					sprintf(param,"Z=%d",TestFile.test_item[items_index[buf16]].param2);
				}
				else
				{
					pText[1][1]="开关";
					sprintf(param,"K=%d",TestFile.test_item[items_index[buf16]].param2);
				}
				pText[1][4]=param;
				if(TestFile.test_item[items_index[buf16]].result==0)//0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
				{
					pText[1][5]="-";
				}
				else if(TestFile.test_item[items_index[buf16]].result==1)//ok
				{
					pText[1][5]="OK";
				}
				else if(TestFile.test_item[items_index[buf16]].result==2)
				{
					pText[1][5]="断路";
				}
				else if(TestFile.test_item[items_index[buf16]].result==3)
				{
					pText[1][5]="错位";
				}
				else 
				{
					pText[1][5]="短路";
				}
				
				LISTVIEW_AddRow(hItem,pText[1]);
			}
			LISTVIEW_SetSel(hItem,9);
		}
		else	//修改指针
		{
			LISTVIEW_SetSel(hItem,(*index - listview_ID_FirstItem));
		}
	}
}


void get_systemtime(char *time)
{
	uint8_t TempStr[50];
	//uint8_t datestr[50];
	uint8_t timestr[20];
	
	RTC_TimeTypeDef   RTC_Time;
  RTC_DateTypeDef   RTC_DateStructure;
  //uint8_t /*sec,*/ min, hour, day, month;
	uint8_t sec,min, hour, day, month;
  uint16_t year;
	//time
	k_GetTime(&RTC_Time);
	sec    =  RTC_Time.Seconds;
	min    =  RTC_Time.Minutes;
	hour   =  RTC_Time.Hours;
	sprintf((char *)timestr, "%02d-%02d-%02d", hour , min, sec);
	//sprintf((char *)timestr, "%02d:%02d", hour , min);
	//date
	k_GetDate(&RTC_DateStructure);
	year =  RTC_DateStructure.Year + 2014;
	month =  RTC_DateStructure.Month;
	day =  RTC_DateStructure.Date;
	if((day > 0) && (day <= 31) && (month > 0)&& (month <= 12) && (year >= 1900))
	{
		sprintf((char *)TempStr, "%04d-%s-%02d_", year, strMonth[month-1], day);
	}
	else
	{
		sprintf((char *)TempStr, "2014-01-01_");
	}
	strcat((char*)TempStr,(char*)timestr);
	//strcat((char*)TempStr,".log");
	strcpy(time,(char*)TempStr);
}

void get_system_time(char * date ,char * time)
{
	uint8_t TempStr[50];
	//uint8_t datestr[50];
	uint8_t timestr[20];
	
	RTC_TimeTypeDef   RTC_Time;
  RTC_DateTypeDef   RTC_DateStructure;
  //uint8_t /*sec,*/ min, hour, day, month;
	uint8_t sec,min, hour, day, month;
  uint16_t year;
	//time
	k_GetTime(&RTC_Time);
	sec    =  RTC_Time.Seconds;
	min    =  RTC_Time.Minutes;
	hour   =  RTC_Time.Hours;
	sprintf((char *)timestr, "%02d-%02d-%02d", hour , min, sec);
	//sprintf((char *)timestr, "%02d:%02d", hour , min);
	//date
	k_GetDate(&RTC_DateStructure);
	year =  RTC_DateStructure.Year + 2014;
	month =  RTC_DateStructure.Month;
	day =  RTC_DateStructure.Date;
	if((day > 0) && (day <= 31) && (month > 0)&& (month <= 12) && (year >= 1900))
	{
		sprintf((char *)TempStr, "%04d-%s-%02d", year, strMonth[month-1], day);
	}
	else
	{
		sprintf((char *)TempStr, "2014-01-01_");
	}
	//strcat((char*)TempStr,(char*)timestr);
	//strcat((char*)TempStr,".log");
	strcpy(time,(char*)timestr);
	strcpy(date,(char*)TempStr);
}


void update_testinfo(void)
{
	WM_HWIN  hItem_okloop;
	WM_HWIN  hItem_ngloop;
	WM_HWIN  hItem_node;
	WM_HWIN  hItem_error_type;
	char ok_loop[6];
	char ng_loop[6];
	char points[30];
	char p1[5];
	char p2[5];
	uint16_t i;
	
	//更新回路及节点信息 wujun added 2015.3.7
	memset(ok_loop,0,6);
	memset(ng_loop,0,6);
	
	hItem_okloop = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_OKLOOPNUM);
	hItem_ngloop = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_NGLOOPNUM);
	hItem_node = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_ERRORNODE);
	hItem_error_type = WM_GetDialogItem(hWin_wt_tester, ID_TEXT_ERRORTYPE);
	
	sprintf(ok_loop,"%d",TestFile.cnt_okloop);
	sprintf(ng_loop,"%d",(TestFile.number_line-TestFile.cnt_okloop));
	TEXT_SetText(hItem_okloop, ok_loop);
	TEXT_SetText(hItem_ngloop, ng_loop);
	
	//更新节点信息 wujun added 2015.3.5
		for(i=0;i<items_show;i++)
		{
			memset(points,0,30);
			if(TestFile.test_item[items_index[i]].result == 2 && TestFile.test_item[items_index[i]].type == 'W')//断路
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				sprintf(points,"%s-%s",p1,p2);
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "断路");
				break;
			}
			else if(TestFile.test_item[items_index[i]].result == 3 && TestFile.test_item[items_index[i]].type == 'Z')//错位
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				sprintf(points,"%s-%s",p1,p2);
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "错位");
				break;
			}
			else if(TestFile.test_item[items_index[i]].result == 4 && TestFile.test_item[items_index[i]].type == 'Z')//短路
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				sprintf(points,"%s-%s",p1,p2);
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "短路");
				break;
			}
			else if(TestFile.test_item[items_index[i]].result == 2 && TestFile.test_item[items_index[i]].type == 'R')//电阻
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				sprintf(points,"%s-%s",p1,p2);
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "电阻");
				break;
			}
			else if(TestFile.test_item[items_index[i]].result == 2 && TestFile.test_item[items_index[i]].type == 'C')//电容
			{
				num_converter(TestFile.test_item[items_index[i]].p1,p1);
				num_converter(TestFile.test_item[items_index[i]].p2,p2);
				sprintf(points,"%s-%s",p1,p2);
				//sprintf(points,"%d-%d",TestFile.test_item[items_index[i]].p1,TestFile.test_item[items_index[i]].p2);
				TEXT_SetText(hItem_node, points);
				TEXT_SetText(hItem_error_type, "电容");
				break;
			}
			else
			{
				TEXT_SetText(hItem_node, "");
				TEXT_SetText(hItem_error_type, "");
				break;
			}
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
*       CreateSystem Info
*/


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
