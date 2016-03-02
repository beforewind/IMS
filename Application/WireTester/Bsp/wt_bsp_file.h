/**
  ******************************************************************************
  * @file    wt_bsp_file.h
  * @author  zhang manxin
  * @version V1.0.0
  * @date    2014-7-18
  * @brief   This file contains all the functions prototypes for the E2PROM driver.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WT_BSP_FILE_H
#define __WT_BSP_FILE_H

#ifdef __cplusplus
 extern "C" {
#endif   
   
/* Includes ------------------------------------------------------------------*/
#include <stdint.h> 
#include "stm32f429i_WireTester.h"
#include "k_storage.h"



/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   WT_BSP_FILE_GLOBALS
#define  WT_BSP_FILE_EXT
#else
#define  WT_BSP_FILE_EXT  extern
#endif



/* Exported constants --------------------------------------------------------*/
#define WT_Number_TestFiles_MAX							50
#define WT_Number_PrintFiles_MAX						20
#define WT_FILE_NAME_SIZE                  	50


/* Path define --------------------------------------------------------*/

#define path_testfile												"0:/LZY_WireTester/TestFiles"
#define path_studyfile											"0:/LZY_WireTester/TestFiles"
#define path_Logfile												"0:/LZY_WireTester/LogFiles"
#define path_Default												"0:/LZY_WireTester/TestFiles"
#define path_printfile											"0:/LZY_WireTester/PrintFiles"

#define path_testfile_sd										"1:/LZY_WireTester/TestFiles"
#define path_studyfile_sd										"1:/LZY_WireTester/TestFiles"
#define path_Logfile_sd											"1:/LZY_WireTester/LogFiles"
#define path_printfile_sd										"1:/LZY_WireTester/PrintFiles"
#define path_Default_sd											"1:/LZY_WireTester/TestFiles"

/* Exported constants --------------------------------------------------------*/
//存储设备选择
WT_BSP_FILE_EXT uint8_t store_dev;  //0-usb  1-sd

//测试板卡选择
//WT_BSP_FILE_EXT uint8_t test_board_number;  //0-usb  1-sd

//测试文件目录
typedef struct{
  uint8_t status;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
  uint8_t number_TotalFile;
	uint8_t number_CurrentFile;
	uint8_t FilesName[WT_Number_TestFiles_MAX][WT_FILE_NAME_SIZE+5];
}
WT_TestFolderTypedef;

//打印文件目录
typedef struct{
  uint8_t status;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
  uint8_t number_TotalFile;
	uint8_t number_CurrentFile;
//	uint16_t number_Cmdline;
//	uint8_t print_status;//0-wait, 1-print ok, 2-print run, 3-print error
	uint8_t FilesName[WT_Number_PrintFiles_MAX][WT_FILE_NAME_SIZE+5];
}
WT_PrintFolderTypedef;

//打印文件
typedef struct node{
	uint8_t CommandLine[50];
	struct node *next; 
}
WT_PrintNodeTypedef;

//打印标签文件
typedef struct{
	uint8_t  PrintFilestr[0x10000];
	uint16_t sum_str;	 		//总字节数
	uint16_t number_Cmdline;
	uint8_t print_status;//0-stop 1-wait, 2-print ok, 3-print run, 4-print error
}
WT_PrintFileTypedef;  

//typedef struct node WT_PrintNodeTypedef; 

//测试项目
typedef struct{
  uint32_t id;
  uint8_t  type;
	uint16_t  p1;
	uint16_t  p2;
	uint32_t  param1; 
	uint32_t  param2;
	uint32_t  param3;
	uint8_t  index;
	uint8_t  plus_tun;
	uint8_t  plus_type;
	uint8_t  out_tun;
	uint8_t  out_type;
	uint8_t  result;	//0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
}
WT_BSP_TEST_ITEM;

//#1:LZY WIRED TESTER
//#2:VER 1.0
//#3:NAME: WIRED TESTER
//#4:MODEL:TESTER1
//#5:VENDOR:LZY Co,LTD
//#6:DATE:2014-06-09
//#7:CONF:v1.0
//#8:DRAWING NUM：00001
//#9:ATTACH:Test
//#A:SERIAL NUM:0001
//#B:P:5 L:1 L:2 L:3 L:7 L:8
//#C:WIRETEST001.wt

//测试文件
typedef struct{
	uint8_t  FileName[WT_FILE_NAME_SIZE+5];							//文件名，50+5
	uint8_t  FileHeader_FormatFlag[20];									//#1:LZY WIRED TESTER
	uint8_t  FileHeader_FormatVersion[20];							//#2:VER 1.0
	uint8_t  FileHeader_ProductName[20];								//#3:NAME: WIRED TESTER
	uint8_t  FileHeader_ProductModel[20];								//#4:MODEL:TESTER1
	uint8_t  FileHeader_ProductCompany[20];							//#5:VENDOR:LZY Co,LTD
	uint8_t  FileHeader_CreatDate[20];									//#6:DATE:2014-06-09
	uint8_t  FileHeader_ProductVersion[20];							//#7:CONF:v1.0
	uint8_t  FileHeader_DrawingNumber[20];							//#8:DRAWING NUM：00001
	uint8_t  FileHeader_AttachInfo[20];									//#9:ATTACH:Test
	uint8_t  FileHeader_SerialNumber[20];								//#A:SERIAL NUM:0001
	uint8_t  FileHeader_PrintInfo[20];									//#B:P:5 L:1 L:2 L:3 L:7 L:8
	uint8_t  FileHeader_FileName[WT_FILE_NAME_SIZE+5];	//#C:WIRETEST001.wtr
	WT_BSP_TEST_ITEM test_item[32640];
	uint32_t item_current;
	uint32_t item_Index;
	uint32_t item_total;
	uint32_t item_error_count;
	uint8_t  test_ErrFlag; //for max error ID，0-no error, 1-error detect and retest
	uint8_t  file_status;	//0:not init, 1: init ok, 2: hardware error, 3-no this file
	uint8_t  task_status;	//0-waiting, 1-testing, 2-test error, 3-test ok, 4-waiting remove wire, 5-testing & Z error
	uint8_t  command;	//0-no operate, 1-start, 2-cancel
	uint64_t test_num;//测试总数
	uint16_t number_point;	//端口数量
	uint16_t number_line;		//通道数量
	uint16_t cnt_okloop;//通过回路数
}
WT_TestFileTypedef;  

//自学习文件目录
typedef struct{
  uint8_t status;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
  uint8_t number_TotalFile;
	uint8_t number_CurrentFile;
	uint8_t FilesName[WT_Number_TestFiles_MAX][WT_FILE_NAME_SIZE+5];
}
WT_StudyFolderTypedef;

//自学习项目
typedef struct{
  uint32_t id;
  uint8_t  type;
	uint16_t  p1;
	uint16_t  p2;
	uint32_t  param1; 
	uint32_t  param2;
	uint32_t  param3;
}
WT_BSP_STUDY_ITEM;

//自学习文件
typedef struct{
	WT_BSP_STUDY_ITEM study_item[32640];
	uint32_t item_total;
	uint32_t item_index;
	uint8_t  file_status;	//
	uint8_t  task_status;	//0:not study, 1:studying, 2:study ok, 3:study error,
	uint8_t  command;	//0-no operate, 1-start, 2-cancel
	uint16_t number_point;	//端口数量
	uint16_t number_line;		//通道数量
	uint16_t max_point;		//最大点序号
	uint16_t min_point;		//最小点序号
}
WT_StudyFileTypedef;  



// SDRAM Define
// 0xD000_0000 - 0xD4FF_FFFF:		FileTest
// 0xD050_0000 - 0xD064_FFFF:		TFT_Layer0
// 0xD065_0000 - 0xD07F_FFFF:		TFT_Layer1

/* variables ---------------------------------------------------------*/

/** @defgroup WireTester_Variables
  * @{
  */

WT_BSP_FILE_EXT WT_TestFolderTypedef 		TestFolder		__attribute__(( at(0xD0000000) ));
WT_BSP_FILE_EXT WT_TestFileTypedef 			TestFile 			__attribute__(( at(0xD0100000) ));
WT_BSP_FILE_EXT WT_StudyFileTypedef			StudyFile			__attribute__(( at(0xD0200000) ));
WT_BSP_FILE_EXT WT_StudyFolderTypedef 	StudyFolder		__attribute__(( at(0xD0300000) ));
WT_BSP_FILE_EXT WT_PrintFolderTypedef 	PrintFolder		__attribute__(( at(0xD0400000) ));
WT_BSP_FILE_EXT WT_PrintFileTypedef     PrintFile			__attribute__(( at(0xD0410000) ));
//WT_BSP_FILE_EXT uint8_t    WIFI_RECBUFF[1024]         __attribute__(( at(0xD0430000) ));
//WT_BSP_FILE_EXT uint8_t    WIFI_TMPBUFF[1024]         __attribute__(( at(0xD0431000) ));
//WT_BSP_FILE_EXT uint8_t    WIFI_RECDATA[1024*800]     __attribute__(( at(0xD0431000) ));
//WT_BSP_FILE_EXT WT_PrintNodeTypedef     *Printnode;


/* Exported functions --------------------------------------------------------*/
  
/** @defgroup Exported_Functions
  * @{
  */
void WT_TestFolder_Init(void);
void WT_TestItem_Init(char * filename);
uint8_t WT_StoreFiles_Init(void);
uint8_t WT_StudyFiles_Init(void);
uint8_t WT_StudyFiles_Write(uint8_t * path);
void WT_StoreFiles_Read(void);
void WT_PrintFolder_Init(void);
uint8_t WT_PrintFiles_Init(char * filename,char * wire_mode);
uint8_t WT_TestFiles_Create(uint8_t * path);
uint8_t WT_TestFiles_Write(uint8_t * path, uint8_t * data);
uint8_t WT_TestFile_Write2Flash (char * filename);
uint8_t WT_TestFile_Write2SD(uint8_t * path);
uint8_t WT_TestFiles_Write2card(uint8_t * path);
uint8_t WT_PrintFile_Init(void);
//extern function


#ifdef __cplusplus
}
#endif
#endif /* __WT_BSP_FILE_H */

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
