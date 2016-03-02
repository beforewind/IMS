/**
  ******************************************************************************
  * @progect LZY Wire Cube Tester
	* @file    wt_bsp_file.c
  * @author  LZY Zhang Manxin
  * @version V1.0.0
  * @date    2014-07-18
  * @brief   This file provides the E2PROM functions
  ******************************************************************************
  */

#define WT_BSP_FILE_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include "wt_bsp_file.h"
#include "k_rtc.h"
#include "stm32f429i_wt_eeprom.h"


//FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);				/* Open or create a file */
//FRESULT f_close (FIL* fp);											/* Close an open file object */
//FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);			/* Read data from a file */
//FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* Write data to a file */
//FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	/* Forward data to the stream */
//FRESULT f_lseek (FIL* fp, DWORD ofs);								/* Move file pointer of a file object */
//FRESULT f_truncate (FIL* fp);										/* Truncate file */
//FRESULT f_sync (FIL* fp);											/* Flush cached data of a writing file */
//FRESULT f_opendir (DIR* dp, const TCHAR* path);						/* Open a directory */
//FRESULT f_closedir (DIR* dp);										/* Close an open directory */
//FRESULT f_readdir (DIR* dp, FILINFO* fno);							/* Read a directory item */
//FRESULT f_mkdir (const TCHAR* path);								/* Create a sub directory */
//FRESULT f_unlink (const TCHAR* path);								/* Delete an existing file or directory */
//FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new);	/* Rename/Move a file or directory */
//FRESULT f_stat (const TCHAR* path, FILINFO* fno);					/* Get file status */
//FRESULT f_chmod (const TCHAR* path, BYTE value, BYTE mask);			/* Change attribute of the file/dir */
//FRESULT f_utime (const TCHAR* path, const FILINFO* fno);			/* Change times-tamp of the file/dir */
//FRESULT f_chdir (const TCHAR* path);								/* Change current directory */
//FRESULT f_chdrive (const TCHAR* path);								/* Change current drive */
//FRESULT f_getcwd (TCHAR* buff, UINT len);							/* Get current directory */
//FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs);	/* Get number of free clusters on the drive */
//FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* sn);	/* Get volume label */
//FRESULT f_setlabel (const TCHAR* label);							/* Set volume label */
//FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt);			/* Mount/Unmount a logical drive */
//FRESULT f_mkfs (const TCHAR* path, BYTE sfd, UINT au);				/* Create a file system on the volume */
//FRESULT f_fdisk (BYTE pdrv, const DWORD szt[], void* work);			/* Divide a physical drive into some partitions */
//int f_putc (TCHAR c, FIL* fp);										/* Put a character to the file */
//int f_puts (const TCHAR* str, FIL* cp);								/* Put a string to the file */
//int f_printf (FIL* fp, const TCHAR* str, ...);						/* Put a formatted string to the file */
//TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);						/* Get a string from the file */

extern void get_systemtime(char *time);
extern void get_system_time(char * date ,char * time);
extern char *itoa(int num, char *str, int radix);

#define IO_MAXSIZE  200

/**
  * @brief  初始化测试文件
  * @param  None
  * @retval None
	* //0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
  */
void WT_TestFolder_Init(void)
{
	FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn,*p;
  uint8_t i=0;
	uint8_t buf8;

	// Init
	TestFolder.number_TotalFile = 0;
	TestFolder.number_CurrentFile = 0;
	TestFolder.status = 0;	
	
	// Check USB
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && store_dev == 0 )	//no usb
	//if(k_StorageGetStatus(MSD_DISK_UNIT) == 0)	//no sd card 
	{
		TestFolder.status = 3;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		return;
	}
	// Check SD
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1 )	//no sd
	//if(k_StorageGetStatus(MSD_DISK_UNIT) == 0)	//no sd card 
	{
		TestFolder.status = 3;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		return;
	}
	
#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif
	
	if(store_dev==0) //usb
  res = f_opendir(&dir, path_testfile);
	if(store_dev==1) //sd
	res = f_opendir(&dir, path_testfile_sd);
	//res = f_opendir(&dir, "1:/LZY_WireTester/Test Files");
  if (res == FR_OK)
  {
    
    while (i < WT_Number_TestFiles_MAX)
    {
      res = f_readdir(&dir, &fno);
      
      if (res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }
			
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
			p = fn;
			while(*p) p++;
			buf8 = p - fn;
			if(buf8 > WT_FILE_NAME_SIZE+4) continue;
			if((fn[buf8-4] != '.') || (fn[buf8-3] != 'w') || (fn[buf8-2] != 't') || (fn[buf8-1] != 'r'))
			{
				continue;
			}
			strncpy((char *)TestFolder.FilesName[i], (char *)fn, buf8+1);
			i++;
		}
		
		if(i==0)	TestFolder.status = 2;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		else			
		{
			TestFolder.status = 1;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
			TestFolder.number_TotalFile = i;
		}
	}
	else
	{
		TestFolder.status = 4;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
	}
}

/**
  * @brief  初始化测试内容
  * @param  None
  * @retval None
  */
void WT_TestItem_Init (char * filename)
{
	FRESULT res;
	FIL file;
	char str[100];
	char line[100];
	uint8_t  j;
	uint32_t i=0,index=0;
	char *ptr;
	char *delim="@:/AZKDVFHLMCRWJCRGBSTIO";
	uint8_t  read_ok = 0;//0-文件读取不完整  1-文件读取完整

	// Init
	//TestFile.test_num=0;
	TestFile.item_current = 0;
	TestFile.item_Index = 0;
	TestFile.item_total = 0;
	TestFile.item_error_count = 0;
	TestFile.test_ErrFlag = 0;	//for max error ID，0-no error, 1-error detect and retest
	TestFile.file_status = 0;	//0:not init, 1: init ok, 2: hardware error, 3-no this file
	TestFile.task_status = 0;	//0-waiting, 1-testing, 2-test error, 3-test ok, 4-waiting remove wire, 5-testing & Z error
	TestFile.command = 0;	//0-no operate, 1-start, 2-cancel
	TestFile.test_num = 0;//测试总数
	TestFile.number_point = 0;	//端口数量
	TestFile.number_line = 0;		//通道数量
	
	if(k_StorageGetStatus(USB_DISK_UNIT) || k_StorageGetStatus(MSD_DISK_UNIT))
	{
		if(store_dev == 0) strcpy (str,(char *)path_testfile);
		if(store_dev == 1) strcpy (str,(char *)path_testfile_sd);
//		//strcpy (str,(char *)"1:/LZY_WireTester/Test Files");
		strcat (str,"/");
//    strcat (str,(char *)TestFolder.FilesName[0]);
		strcat (str,filename);
			
		// init head file
		line[0] = '-';
		line[1] = 0;
		strcpy ((char*)TestFile.FileName,(char *)line);
		strcpy ((char*)TestFile.FileHeader_FormatFlag,(char *)line);
		strcpy ((char*)TestFile.FileHeader_FormatVersion,(char *)line);
		strcpy ((char*)TestFile.FileHeader_ProductName,(char *)line);
		strcpy ((char*)TestFile.FileHeader_ProductModel,(char *)line);
		strcpy ((char*)TestFile.FileHeader_ProductCompany,(char *)line);
		strcpy ((char*)TestFile.FileHeader_CreatDate,(char *)line);
		strcpy ((char*)TestFile.FileHeader_ProductVersion,(char *)line);
		strcpy ((char*)TestFile.FileHeader_DrawingNumber,(char *)line);
		strcpy ((char*)TestFile.FileHeader_AttachInfo,(char *)line);
		strcpy ((char*)TestFile.FileHeader_SerialNumber,(char *)line);
		strcpy ((char*)TestFile.FileHeader_PrintInfo,(char *)line);
		strcpy ((char*)TestFile.FileHeader_FileName,(char *)line);
		//open file
		res = f_open(&file, str, FA_OPEN_EXISTING | FA_READ);
		if (res == FR_OK)
		{		
			strcpy ((char*)TestFile.FileName,(char *)filename);
			while(1)
			{
				if(f_gets(line, sizeof line, &file) == 0) break;
				if((line[0] == '#') && (line[2] == ':'))	// head mark
				{
					//fine second ':'
					//----------------------------------------
					j=0;
					for(index=3;index<20;index++)
					{
						if(line[index]== ':') //fine title mark
						{
							index++;
							j = index;
							break;
						}
					}
					if(j==0) // no title mark
					{
						index = 3;
						j = index;
					}
					//check if length more than 20 chars
					//----------------------------------------
					for(;index<16+j;index++)
					{
						if(line[index]== '\n') 
						{
							line[index] = 0;
							index = 0;
							break;
						}
						else if(line[index]== 0) 
						{
							index = 0;
							break;
						}
					}
					//length more than 20 chars
					//----------------------------------------
					if(index != 0) //length error
					{
						line[index++] = '.';
						line[index++] = '.';
						line[index++] = '.';
						line[index++] = 0;
					}
					// update to struct
					//----------------------------------------
					switch(line[1])
					{
						case '1':
							strcpy ((char*)TestFile.FileHeader_FormatFlag,(char *)&line[j]);
							break;
						case '2':
							strcpy ((char*)TestFile.FileHeader_FormatVersion,(char *)&line[j]);
							break;
						case '3':
							strcpy ((char*)TestFile.FileHeader_ProductName,(char *)&line[j]);
							break;
						case '4':
							strcpy ((char*)TestFile.FileHeader_ProductModel,(char *)&line[j]);
							break;
						case '5':
							strcpy ((char*)TestFile.FileHeader_ProductCompany,(char *)&line[j]);
							break;
						case '6':
							strcpy ((char*)TestFile.FileHeader_CreatDate,(char *)&line[j]);
							break;
						case '7':
							strcpy ((char*)TestFile.FileHeader_ProductVersion,(char *)&line[j]);
							break;
						case '8':
							strcpy ((char*)TestFile.FileHeader_DrawingNumber,(char *)&line[j]);
							break;
						case '9':
							strcpy ((char*)TestFile.FileHeader_AttachInfo,(char *)&line[j]);
							break;
						case 'A':
							strcpy ((char*)TestFile.FileHeader_SerialNumber,(char *)&line[j]);
							break;
						case 'B':
							strcpy ((char*)TestFile.FileHeader_PrintInfo,(char *)&line[j]);
							break;
						case 'C':
							strcpy ((char*)TestFile.FileHeader_FileName,(char *)&line[j]);
							break;
						default:
							break;
					}
				}
				else //not head lines
				{
					break;
				}
			}				
		}

		f_close(&file);
		
		// init test items
		res = f_open(&file, str, FA_OPEN_EXISTING | FA_READ);
		if (res == FR_OK)
		{		
			i=0;
			while(f_gets(line, sizeof line, &file))
			{
				//-----------------------------------------------------------
				// Head line
				//-----------------------------------------------------------
				if(strchr(line, '#'))
				{
					if(strstr(line, "#D:"))
					{
						strtok((line),delim);
						index=0;
						while(1)
						{
							ptr=strtok(NULL,delim);
							if(ptr == 0) break;
							
							switch(index)
							{
								case 0:
									TestFile.number_point=atoi(ptr);
									index++;
									break;
								case 1:
									TestFile.number_line=atoi(ptr);
									index++;
									break;
								default:
									break;
							}
						}
					}
					else continue;
				}
				//-----------------------------------------------------------
				// W test line
				//-----------------------------------------------------------
				else if(strstr(line, ":W/"))
				{	
					strtok((line),delim);
					index=0;
					while(1)
					{
						ptr=strtok(NULL,delim);
						if(ptr == 0) break;
						
						switch(index)
						{
							case 0:
								TestFile.test_item[i].id=i+1;
								TestFile.test_item[i].type='W';
								TestFile.test_item[i].p1=atoi(ptr);
								index++;
								break;
							case 1:
								TestFile.test_item[i].p2=atoi(ptr);
								index++;
								break;
							case 2:
								TestFile.test_item[i].param1=atoi(ptr);
								index++;
								break;
							case 3:
								TestFile.test_item[i].param2=atoi(ptr);
								index++;
								break;
							case 4:
								TestFile.test_item[i].param3=atoi(ptr);
								index++;
								break;
							case 5:
								TestFile.test_item[i].index=atoi(ptr);
								index++;
								break;
							case 6:
								TestFile.test_item[i].plus_tun=atoi(ptr);
								index++;
								break;
							case 7:
								TestFile.test_item[i].plus_type=atoi(ptr);
								index++;
								break;
							case 8:
								TestFile.test_item[i].out_tun=atoi(ptr);
								index++;
								break;
							case 9:
								TestFile.test_item[i].out_type=atoi(ptr);
								index++;
								break;
							default:
								break;
						}
					}
					TestFile.test_item[i].result = 0;
					i++;
					TestFile.item_total = i;
				}
				//-----------------------------------------------------------
				// D test line
				//-----------------------------------------------------------
				else if(strstr(line, ":D/"))
				{	
					strtok((line),delim);
					index=0;
					while(1)
					{
						ptr=strtok(NULL,delim);
						if(ptr == 0) break;
						
						switch(index)
						{
							case 0:
								TestFile.test_item[i].id=i+1;
								TestFile.test_item[i].type='D';
								TestFile.test_item[i].p1=atoi(ptr);
								index++;
								break;
							case 1:
								TestFile.test_item[i].p2=atoi(ptr);
								index++;
								break;
							case 2:
								TestFile.test_item[i].param1=atoi(ptr);
								index++;
								break;
							case 3:
								TestFile.test_item[i].param2=atoi(ptr);
								index++;
								break;
							case 4:
								TestFile.test_item[i].param3=atoi(ptr);
								index++;
								break;
							case 5:
								TestFile.test_item[i].index=atoi(ptr);
								index++;
								break;
							case 6:
								TestFile.test_item[i].plus_tun=atoi(ptr);
								index++;
								break;
							case 7:
								TestFile.test_item[i].plus_type=atoi(ptr);
								index++;
								break;
							case 8:
								TestFile.test_item[i].out_tun=atoi(ptr);
								index++;
								break;
							case 9:
								TestFile.test_item[i].out_type=atoi(ptr);
								index++;
								break;
							default:
								break;
						}
					}
					TestFile.test_item[i].result = 0;
					i++;
					TestFile.item_total = i;
				}		
				//-----------------------------------------------------------
				// C test line
				//-----------------------------------------------------------
				else if(strstr(line, ":C/"))
				{	
					strtok((line),delim);
					index=0;
					while(1)
					{
						ptr=strtok(NULL,delim);
						if(ptr == 0) break;
						
						switch(index)
						{
							case 0:
								TestFile.test_item[i].id=i+1;
								TestFile.test_item[i].type='C';
								TestFile.test_item[i].p1=atoi(ptr);
								index++;
								break;
							case 1:
								TestFile.test_item[i].p2=atoi(ptr);
								index++;
								break;
							case 2:
								TestFile.test_item[i].param1=atoi(ptr);
								index++;
								break;
							case 3:
								TestFile.test_item[i].param2=atoi(ptr);
								index++;
								break;
							case 4:
								TestFile.test_item[i].param3=atoi(ptr);
								index++;
								break;
							case 5:
								TestFile.test_item[i].index=atoi(ptr);
								index++;
								break;
							case 6:
								TestFile.test_item[i].plus_tun=atoi(ptr);
								index++;
								break;
							case 7:
								TestFile.test_item[i].plus_type=atoi(ptr);
								index++;
								break;
							case 8:
								TestFile.test_item[i].out_tun=atoi(ptr);
								index++;
								break;
							case 9:
								TestFile.test_item[i].out_type=atoi(ptr);
								index++;
								break;
							default:
								break;
						}
					}
					TestFile.test_item[i].result = 0;
					i++;
					TestFile.item_total = i;
				}		
				//-----------------------------------------------------------
				// R test line
				//-----------------------------------------------------------
				else if(strstr(line, ":R/"))
				{	
					strtok((line),delim);
					index=0;
					while(1)
					{
						ptr=strtok(NULL,delim);
						if(ptr == 0) break;
						
						switch(index)
						{
							case 0:
								TestFile.test_item[i].id=i+1;
								TestFile.test_item[i].type='R';
								TestFile.test_item[i].p1=atoi(ptr);
								index++;
								break;
							case 1:
								TestFile.test_item[i].p2=atoi(ptr);
								index++;
								break;
							case 2:
								TestFile.test_item[i].param1=atoi(ptr);
								index++;
								break;
							case 3:
								TestFile.test_item[i].param2=atoi(ptr);
								index++;
								break;
							case 4:
								TestFile.test_item[i].param3=atoi(ptr);
								index++;
								break;
							case 5:
								TestFile.test_item[i].index=atoi(ptr);
								index++;
								break;
							case 6:
								TestFile.test_item[i].plus_tun=atoi(ptr);
								index++;
								break;
							case 7:
								TestFile.test_item[i].plus_type=atoi(ptr);
								index++;
								break;
							case 8:
								TestFile.test_item[i].out_tun=atoi(ptr);
								index++;
								break;
							case 9:
								TestFile.test_item[i].out_type=atoi(ptr);
								index++;
								break;
							default:
								break;
						}
					}
					TestFile.test_item[i].result = 0;
					i++;
					TestFile.item_total = i;
				}		
				//-----------------------------------------------------------
				// K test line
				//-----------------------------------------------------------
				else if(strstr(line, ":K/"))
				{	
					strtok((line),delim);
					index=0;
					while(1)
					{
						ptr=strtok(NULL,delim);
						if(ptr == 0) break;
						
						switch(index)
						{
							case 0:
								TestFile.test_item[i].id=i+1;
								TestFile.test_item[i].type='K';
								TestFile.test_item[i].p1=atoi(ptr);
								index++;
								break;
							case 1:
								TestFile.test_item[i].p2=atoi(ptr);
								index++;
								break;
							case 2:
								TestFile.test_item[i].param1=atoi(ptr);
								index++;
								break;
							case 3:
								TestFile.test_item[i].param2=atoi(ptr);
								index++;
								break;
							case 4:
								TestFile.test_item[i].param3=atoi(ptr);
								index++;
								break;
							case 5:
								TestFile.test_item[i].index=atoi(ptr);
								index++;
								break;
							case 6:
								TestFile.test_item[i].plus_tun=atoi(ptr);
								index++;
								break;
							case 7:
								TestFile.test_item[i].plus_type=atoi(ptr);
								index++;
								break;
							case 8:
								TestFile.test_item[i].out_tun=atoi(ptr);
								index++;
								break;
							case 9:
								TestFile.test_item[i].out_type=atoi(ptr);
								index++;
								break;
							default:
								break;
						}
					}
					TestFile.test_item[i].result = 0;
					i++;
					TestFile.item_total = i;
				}								
				//-----------------------------------------------------------
				// Z test line
				//-----------------------------------------------------------
				else if(strstr(line, ":Z/"))
				{	
					strtok((line),delim);
					index=0;
					while(1)
					{
						ptr=strtok(NULL,delim);
						if(ptr == 0) break;
						
						switch(index)
						{
							case 0:
								TestFile.test_item[i].id=i+1;
								TestFile.test_item[i].type='Z';
								TestFile.test_item[i].p1=atoi(ptr);
								index++;
								break;
							case 1:
								TestFile.test_item[i].p2=atoi(ptr);
								index++;
								break;
							case 2:
								TestFile.test_item[i].param1=atoi(ptr);
								index++;
								break;
							case 3:
								TestFile.test_item[i].param2=atoi(ptr);
								index++;
								break;
							case 4:
								TestFile.test_item[i].param3=atoi(ptr);
								index++;
								break;
							case 5:
								TestFile.test_item[i].index=atoi(ptr);
								index++;
								break;
							case 6:
								TestFile.test_item[i].plus_tun=atoi(ptr);
								index++;
								break;
							case 7:
								TestFile.test_item[i].plus_type=atoi(ptr);
								index++;
								break;
							case 8:
								TestFile.test_item[i].out_tun=atoi(ptr);
								index++;
								break;
							case 9:
								TestFile.test_item[i].out_type=atoi(ptr);
								index++;
								break;
							default:
								break;
						}
					}
					TestFile.test_item[i].result = 0;
					i++;
					TestFile.item_total = i;
				}
				//-----------------------------------------------------------
				// End line
				//-----------------------------------------------------------
				else if(strstr(line, ":E"))
				{
					read_ok = 1;
					break;
				}
				continue;	
			}
			//f_close(&file);
			if(1 == read_ok) TestFile.file_status = 1;	//0:not init, 1: init ok, 2: hardware error, 3-no this file
			else TestFile.file_status = 0;
		}
		else	
		{
			TestFile.file_status = 3;	//0:not init, 1: init ok, 2: hardware error, 3-no this file
		}
		f_close(&file);
	}
	else
	{
		TestFile.file_status = 2;	//0:not init, 1: init ok, 2: hardware error, 3-no this file
	}
}

/**
  * @brief  检测规则初始化
  * @param  None
* @retval 0:ok, 1:error
  */
uint8_t WT_StoreFiles_Init(void)
{
	uint8_t res=1;
	
	//read folder
	WT_TestFolder_Init();	
	if(TestFolder.status == 1)	//0-not init, 1-inited, 2-no files, 3-error
	{
//		WT_TestItem_Init();
		if(TestFile.file_status == 1) res=0;//0:not init, 1: init ok, 2: hardware error, 3-no this file
	}
	return res;
}

/**
  * @brief  读取测试文件
  * @param  None
  * @retval None
  */
void WT_StoreFiles_Read(void)
{
	//uint8_t res=1;
	
	FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;
  uint8_t i=0;
	
	// Check USB
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && store_dev == 0)	//no usb
	//if(k_StorageGetStatus(MSD_DISK_UNIT) == 0)	//no sd card 
	{
		TestFolder.status = 3;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		return;
	}
	
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1)	//no usb
	//if(k_StorageGetStatus(MSD_DISK_UNIT) == 0)	//no sd card 
	{
		TestFolder.status = 3;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		return;
	}
	
#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif
	
  //res = f_opendir(&dir, path_testfile);
	if(store_dev == 0) res = f_opendir(&dir, path_testfile);
	if(store_dev == 1) res = f_opendir(&dir, path_testfile_sd);
	//res = f_opendir(&dir, "1:/LZY_WireTester/Test Files");
  if (res == FR_OK)
  {
    
    while (i < WT_Number_TestFiles_MAX)
    {
      res = f_readdir(&dir, &fno);
      
      if (res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }
			
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
			
			strncpy((char *)TestFolder.FilesName[i], (char *)fn, WT_FILE_NAME_SIZE);
			i++;
		}
		
		if(i==0)	TestFolder.status = 2;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		else			
		{
			TestFolder.status = 1;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
			TestFolder.number_TotalFile = i;
		}
	}
	else
	{
		TestFolder.status = 4;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
	}
}

/**
  * @brief  自学习初始化
  * @param  None
* @retval 0:ok, 1:error
  */
uint8_t WT_StudyFiles_Init(void)
{
	uint8_t res=1;
	
	//read folder
	WT_TestFolder_Init();	
	if(TestFolder.status == 1)	//0-not init, 1-inited, 2-no files, 3-error
	{
//		WT_TestItem_Init();
		if(TestFile.file_status == 1) res=0;//0:not init, 1: init ok, 2: hardware error, 3-no this file
	}
	return res;
}

/**
  * @brief  打印初始化
  * @param  None
* @retval 0:ok, 1:error
  */
uint8_t WT_PrintFile_Init(void)
{
	uint8_t res=1;
	
	char str_mode[] = "test";
	store_dev = 1;
	//read folder
	WT_PrintFolder_Init();
	//打开prn文件
	res=WT_PrintFiles_Init((char *)PrintFolder.FilesName[0],str_mode);//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
	if(res == 1)
	{
		PrintFile.print_status = 1;	
	}
	else
	{
		PrintFile.print_status = 4;//0-stop 1-wait, 2-print ok, 3-print run, 4-print error
	}

	return res;
}

/**
  * @brief  初始化自学习文件
  * @param  None
  * @retval None
	* //0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
  */
//static void WT_StudyFolder_Init(void)
//{
//	FRESULT res;
//  FILINFO fno;
//  DIR dir;
//  char *fn;
//  uint8_t i=0;

//	// Init
//	StudyFolder.number_TotalFile = 0;
//	StudyFolder.number_CurrentFile = 0;
//	StudyFolder.status = 0;	
//	
//	// Check USB
//	if(k_StorageGetStatus(USB_DISK_UNIT) == 0)	//no usb
//	//if(k_StorageGetStatus(MSD_DISK_UNIT) == 0)	//no sd card 
//	{
//		StudyFolder.status = 3;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
//		return;
//	}
//	
//#if _USE_LFN
//  static char lfn[_MAX_LFN];
//  fno.lfname = lfn;
//  fno.lfsize = sizeof(lfn);
//#endif
//	
//  res = f_opendir(&dir, path_studyfile);
//	//res = f_opendir(&dir, "1:/LZY_WireTester/Test Files");
//  if (res == FR_OK)
//  {
//    
//    while (i < WT_Number_TestFiles_MAX)
//    {
//      res = f_readdir(&dir, &fno);
//      
//      if (res != FR_OK || fno.fname[0] == 0)
//      {
//        break;
//      }
//      if (fno.fname[0] == '.')
//      {
//        continue;
//      }
//			
//#if _USE_LFN
//      fn = *fno.lfname ? fno.lfname : fno.fname;
//#else
//      fn = fno.fname;
//#endif
//			
//			strncpy((char *)StudyFolder.FilesName[i], (char *)fn, WT_FILE_NAME_SIZE);
//			i++;
//		}
//		
//		if(i==0)	StudyFolder.status = 2;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
//		else			
//		{
//			StudyFolder.status = 1;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
//			StudyFolder.number_TotalFile = i;
//		}
//	}
//	else
//	{
//		StudyFolder.status = 4;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
//	}
//}


/**
  * @brief  读取自学习文件
  * @param  None
  * @retval None
  */
void WT_StudyFiles_Read(void)
{
	//uint8_t res=1;
	
	FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;
  uint8_t i=0;
	
	// Check USB
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && store_dev == 0)	//no usb
	//if(k_StorageGetStatus(MSD_DISK_UNIT) == 0)	//no sd card 
	{
		StudyFolder.status = 3;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		return;
	}
	// Check SD
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1)	//no sd
	//if(k_StorageGetStatus(MSD_DISK_UNIT) == 0)	//no sd card 
	{
		StudyFolder.status = 3;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		return;
	}
#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif
	
 // res = f_opendir(&dir, path_studyfile);
	if(store_dev == 0) res = f_opendir(&dir, path_studyfile);
	if(store_dev == 1) res = f_opendir(&dir, path_studyfile_sd);
	//res = f_opendir(&dir, "1:/LZY_WireTester/Test Files");
  if (res == FR_OK)
  {
    
    while (i < WT_Number_TestFiles_MAX)
    {
      res = f_readdir(&dir, &fno);
      
      if (res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }
			
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
			
			strncpy((char *)StudyFolder.FilesName[i], (char *)fn, WT_FILE_NAME_SIZE);
			i++;
		}
		
		if(i==0)	StudyFolder.status = 2;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		else			
		{
			StudyFolder.status = 1;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
			StudyFolder.number_TotalFile = i;
		}
	}
	else
	{
		StudyFolder.status = 4;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
	}
}



/**
  * @brief  保存测试日志文件
  * @param  None
  * @retval 0-ok, 1-hardware error, 2-file operate error
  */
uint8_t WT_LogFiles_Write(uint8_t * path, uint8_t * str, uint16_t len)
{
	FRESULT  res;
	FIL			 fil;
	uint8_t* p;
	uint8_t  i;
	uint8_t  buf8[100];
	uint32_t buf32;
	
	// Check disc
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && store_dev == 0)	//no usb or SD
	{
		return 1;
	}
	
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1)	//no usb or SD
	{
		return 1;
	}
	
	// open file
	res = f_open(&fil, (const TCHAR*)path, FA_OPEN_EXISTING | FA_WRITE);
	if((res == FR_NO_FILE)||(res == FR_NO_PATH)) //文件不存在
	{
		p = path;
		for(;(*p)!=0;p++)
		{
			if(*p == '/')
			{
				for(i=0;i<(p-path);i++) buf8[i]=*(path+i);
				buf8[i] = 0;
				res = f_mkdir((const TCHAR*) buf8);
				if((res != 0)&&(res != FR_EXIST)) {return 2;}
			}
		}
		
		res = f_open(&fil, (const TCHAR*)path, FA_OPEN_ALWAYS | FA_WRITE); //可写方式打开 没有文件则创建 
		if(res)
		{
			f_close(&fil);
		}
	}
	else if(res != FR_OK)	//ok
	{
		return 1;
	}

	//Write string to file
	if(len > 0)
	{
		res = f_lseek(&fil, f_size(&fil)); 
		if(res != FR_OK) return 2;
		
		res = f_write(&fil, str, len/*sizeof(String)*/, &buf32); 
		if(res != FR_OK) return 2;
		
		res = f_write(&fil, "\r\n", 2, &buf32); 
		if(res != FR_OK) return 2;
	}
	
	//Save to file
	f_close(&fil);
	return 0;
}

/**
  * @brief  从学习文件获取字符串
  * @param  None
  * @retval 0-ok, 1-error
  */
static uint8_t GetString_FromStudyItem(uint8_t * str, uint64_t index, uint32_t * len)
{
	uint8_t i = 0;
	uint8_t j;
	char    buf8[50];
	char  para[5];
	
	str[i++] = '@';

	sprintf(buf8,"%d",(uint32_t)StudyFile.study_item[index].id);
	j=0;
	while(buf8[j])
	{
		str[i++] = buf8[j];
		j++;
	}
	str[i++] = ':';
	
	str[i++] = StudyFile.study_item[index].type;
	str[i++] = '/';
	
	str[i++] = 'J';
	str[i++] = ':';
	sprintf(buf8,"%d",(uint32_t)StudyFile.study_item[index].p1);
	j=0;
	while(buf8[j])
	{
		str[i++] = buf8[j];
		j++;
	}
	str[i++] = '/';
	
	str[i++] = 'J';
	str[i++] = ':';
	sprintf(buf8,"%d",(uint32_t)StudyFile.study_item[index].p2);
	j=0;
	while(buf8[j])
	{
		str[i++] = buf8[j];
		j++;
	}
	str[i++] = '/';
	
	str[i++] = 'C';
	str[i++] = 'R';
	str[i++] = ':';
	if(StudyFile.study_item[index].type == 'R')
	{
		itoa(StudyFile.study_item[index].param1,para,10);
		str[i++] = para[0];
		str[i++] = para[1];
		str[i++] = para[2];
		str[i++] = para[3];
	}
	else str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'C';
	str[i++] = 'G';
	str[i++] = ':';
	if(StudyFile.study_item[index].type == 'R')
	{
		itoa(StudyFile.study_item[index].param2,para,10);
		str[i++] = para[0];
		str[i++] = para[1];
		str[i++] = para[2];
		str[i++] = para[3];
	}
	else str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'C';
	str[i++] = 'B';
	str[i++] = ':';
	if(StudyFile.study_item[index].type == 'R')
	{
		itoa(StudyFile.study_item[index].param3,para,10);
		str[i++] = para[0];
		str[i++] = para[1];
		str[i++] = para[2];
		str[i++] = para[3];
	}
	else str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'S';
	str[i++] = ':';
	str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'T';
	str[i++] = ':';
	str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'T';
	str[i++] = 'I';
	str[i++] = ':';
	str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'O';
	str[i++] = ':';
	str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'T';
	str[i++] = 'O';
	str[i++] = ':';
	str[i++] = '0';

	str[i++] = '\r';
	str[i++] = '\n';
	
	str[i] = 0;
	*len = i;
	return 0;	
}

/**
  * @brief  从测试文件获取字符串
  * @param  None
  * @retval 0-ok, 1-error
  */
static uint8_t GetString_FromTestItem(uint8_t * str, uint64_t index, uint32_t * len)
{
	uint8_t i = 0;
	uint8_t j;
	char    buf8[50];
	char  para[5];
	
	str[i++] = '@';

	sprintf(buf8,"%d",(uint32_t)TestFile.test_item[index].id);
	j=0;
	while(buf8[j])
	{
		str[i++] = buf8[j];
		j++;
	}
	str[i++] = ':';
	
	str[i++] = TestFile.test_item[index].type;
	str[i++] = '/';
	
	str[i++] = 'J';
	str[i++] = ':';
	sprintf(buf8,"%d",(uint32_t)TestFile.test_item[index].p1);
	j=0;
	while(buf8[j])
	{
		str[i++] = buf8[j];
		j++;
	}
	str[i++] = '/';
	
	str[i++] = 'J';
	str[i++] = ':';
	sprintf(buf8,"%d",(uint32_t)TestFile.test_item[index].p2);
	j=0;
	while(buf8[j])
	{
		str[i++] = buf8[j];
		j++;
	}
	str[i++] = '/';
	
	str[i++] = 'C';
	str[i++] = 'R';
	str[i++] = ':';
	if(TestFile.test_item[index].type == 'R')
	{
		itoa(TestFile.test_item[index].param1,para,10);
		str[i++] = para[0];
		str[i++] = para[1];
		str[i++] = para[2];
		str[i++] = para[3];
	}
	else str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'C';
	str[i++] = 'G';
	str[i++] = ':';
	if(TestFile.test_item[index].type == 'R')
	{
		itoa(TestFile.test_item[index].param2,para,10);
		str[i++] = para[0];
		str[i++] = para[1];
		str[i++] = para[2];
		str[i++] = para[3];
	}
	else str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'C';
	str[i++] = 'B';
	str[i++] = ':';
	if(TestFile.test_item[index].type == 'R')
	{
		itoa(TestFile.test_item[index].param3,para,10);
		str[i++] = para[0];
		str[i++] = para[1];
		str[i++] = para[2];
		str[i++] = para[3];
	}
	else str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'S';
	str[i++] = ':';
	str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'T';
	str[i++] = ':';
	str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'T';
	str[i++] = 'I';
	str[i++] = ':';
	str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'O';
	str[i++] = ':';
	str[i++] = '0';
	str[i++] = '/';
	
	str[i++] = 'T';
	str[i++] = 'O';
	str[i++] = ':';
	str[i++] = '0';

	str[i++] = '\r';
	str[i++] = '\n';
	
	str[i] = 0;
	*len = i;
	return 0;	
}

#define StudyFile_Title_1		"#1:LZY WIRED TESTER\r\n"
#define StudyFile_Title_2		"#2:VER 1.0\r\n"
#define StudyFile_Title_3		"#3:NAME: WIRED TESTER\r\n"
#define StudyFile_Title_4		"#4:MODEL:TESTER1\r\n"
#define StudyFile_Title_5		"#5:VENDOR:LZY Co,LTD\r\n"
#define StudyFile_Title_6		"#6:DATE:2014-06-09\r\n"
#define StudyFile_Title_7		"#7:CONF:v1.0\r\n"
#define StudyFile_Title_8		"#8:DRAWING NUM00001\r\n"
#define StudyFile_Title_9		"#9:ATTACH:Test\r\n"
#define StudyFile_Title_A		"#A:SERIAL NUM:0001\r\n"
#define StudyFile_Title_B		"#B:P:5 L:1 L:2 L:3 L:7 L:8\r\n"
#define StudyFile_Title_C		"#C:test.wtr"


/**
  * @brief  保存学习的策略文件
  * @param  None
  * @retval 0-ok, 1-hardware error, 2-file operate error, 
  */
uint8_t WT_StudyFiles_Write(uint8_t * path)
{
	FRESULT  res;
	DIR 		 dir;
	FILINFO  fno;
	FIL			 fil;
	char *	 fn;
	uint8_t  *p, *fn_filename;
	uint8_t  buf8[80], temp8[10];
	uint8_t  temp;
	uint32_t buf32;
	uint32_t i, j,len;
	uint64_t index;
  char  	 systime[30];
	char  	 str_time[40];
	
	// Check disc
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && store_dev == 0)	//no usb or SD
	{
		return 1;
	}
	
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1)	//no usb or SD
	{
		return 1;
	}
	
	// 判断文件夹是否村存在，不存在则创建	
  if(store_dev == 0) res = f_opendir(&dir, path_studyfile);
	if(store_dev == 1) res = f_opendir(&dir, path_studyfile_sd);
  if(res == FR_NO_PATH)	//目录不存在，创建目录
	{
		p = path;
		for(;(*p)!=0;p++)
		{
			if(*p == '/')
			{
				for(i=0;i<(p-path);i++) buf8[i]=*(path+i);
				buf8[i] = 0;
				res = f_mkdir((const TCHAR*) buf8);
				if((res != 0)&&(res != FR_EXIST)) {return 2;}
			}
		}
		//open the new dir
		res = f_opendir(&dir, path_studyfile);
		if(res != FR_OK) return 1;
	}
	else if (res != FR_OK)
  {
		return 1;
	}
	
	// 判断文件是否村存在 
	#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
	#endif
	
	p = path;
	i = 0;
	for(;(*p)!=0;p++)
	{
		if(*p == '.')
		{
			p--;
			for(;(*p)!='/';p--)
			{
				i++;
				if(i>50) return 2;
			}
			p++;
			break;
		}
	}
	fn_filename = p;
	i = 0;	//0:ok, 1:error, 
	
	for (;;) 
	{
		res = f_readdir(&dir, &fno);                   /* Read a directory item */
		if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
		if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
		#if _USE_LFN
		fn = *fno.lfname ? fno.lfname : fno.fname;
		#else
		fn = fno.fname;
		#endif
		if (fno.fattrib & AM_DIR) continue;  /* It is a directory */
		else /* It is a file. */
		{
			// compare filename & new filename
			p = fn_filename;
			while(*fn != 0)
			{
				if(*fn++ != *p++)
				{
					i = 1;
					break;
				}
				if(i == 1) break;
			}
			
			if((i == 0) && (*p == 0))	//fine the same filename
			{
				i = 1;
				break;
			}
			else	//no filename
			{
				i=0;
			}
		}
	}
	f_closedir(&dir); //关闭文件夹

	//存在文件，处理
	if(i==1)	//文件已存在，返回、重命名、删除等操作
	{
		//return 3;
		res = f_unlink((const TCHAR*)path);
		if(res != 0) return 2;
	}
	
	//保存学习文件
	res = f_open(&fil, (const TCHAR*)path, FA_OPEN_ALWAYS | FA_WRITE); //可写方式打开 没有文件则创建 
	if(res)
	{
		f_close(&fil);
		return 2;
	}

	res = f_lseek(&fil, f_size(&fil)); 
	if(res != FR_OK) return 2;
	
	//update title
	get_systemtime(systime);
	strcpy(str_time,"#6:DATE:");
	strcat(str_time,systime);
	strcat(str_time,"\r\n\0");
	res = f_write(&fil, StudyFile_Title_1, sizeof(StudyFile_Title_1)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_2, sizeof(StudyFile_Title_2)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_3, sizeof(StudyFile_Title_3)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_4, sizeof(StudyFile_Title_4)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_5, sizeof(StudyFile_Title_5)-1, &buf32); 
	//res = f_write(&fil, StudyFile_Title_6, sizeof(StudyFile_Title_6)-1, &buf32); 
	res = f_write(&fil, str_time, strlen(str_time)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_7, sizeof(StudyFile_Title_7)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_8, sizeof(StudyFile_Title_8)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_9, sizeof(StudyFile_Title_9)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_A, sizeof(StudyFile_Title_A)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_B, sizeof(StudyFile_Title_B)-1, &buf32); 
	p = fn_filename;
	i=0;
	while(*p) {i++; p++;}
	strcpy ((char *)buf8, "#C:");
	strcat ((char *)buf8,(char *)fn_filename);
	strcat ((char *)buf8,"\r\n");
	
	for(j=0;j<sizeof(temp8);j++) temp8[j]=0;
	sprintf((char *)temp8,"%d/%d\0", StudyFile.number_point, StudyFile.number_line);
	j=strlen((char *)temp8);
	strcat ((char *)buf8, "#D:");
	strcat ((char *)buf8,(char *) temp8);
	strcat ((char *)buf8,"\r\n");
	
	res = f_write(&fil, buf8, 3+i+2+5+j, &buf32); 
	if(res != 0) return 2;
	
	//save data to file
	for(index=0;index<StudyFile.item_total;index++)
	{
		temp = GetString_FromStudyItem(buf8, index, &len);
		if(temp != 0) return 2;
		for(i=0;i<2000;i++){}
			
		res = f_write(&fil, buf8, len, &buf32); 
		for(i=0;i<2000;i++){}
		if(res != FR_OK) return 2;
	}
	
	// save end mark to file
	for(i=0;i<sizeof(temp8);i++) temp8[i]=0;
	sprintf((char *) temp8,"%d", StudyFile.item_total+1);
	strcpy ((char *)buf8, "@");
	i=0;
	while(1)
	{
		buf8[i+1] = temp8[i];
		if(temp8[i] == 0) break;
		i++;
	}
	strcat ((char *)buf8,":E\r\n");
	res = f_write(&fil, buf8, i+4, &buf32); 
	
	//Save to file
	f_close(&fil);
	return 0;
}


/**
  * @brief  初始化打印文件
  * @param  None
  * @retval None
	* //0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
  */
void WT_PrintFolder_Init(void)
{
	FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn,*p;
  uint8_t i=0;
	uint8_t buf8;

	// Init
	PrintFolder.number_TotalFile = 0;
	PrintFolder.number_CurrentFile = 0;
	
	PrintFolder.status = 0;	
	
	PrintFile.print_status = 0;
	PrintFile.sum_str = 0;
	PrintFile.number_Cmdline = 0;
	memset(PrintFile.PrintFilestr,0,0x10000);
	
	// Check USB
	if(k_StorageGetStatus(USB_DISK_UNIT) == 1 && store_dev == 0 )	//no usb
	{
		PrintFolder.status = 3;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		return;
	}
	// Check SD
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1 )	//no sd
	{
		PrintFolder.status = 3;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		return;
	}
	
#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif
	
	if(store_dev==0) //usb
  res = f_opendir(&dir, path_printfile);
	if(store_dev==1) //sd
	res = f_opendir(&dir, path_printfile_sd);
	//res = f_opendir(&dir, "1:/LZY_WireTester/Test Files");
  if (res == FR_OK)
  {
    
    while (i < WT_Number_PrintFiles_MAX)
    {
      res = f_readdir(&dir, &fno);
      
      if (res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }
			
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
			p = fn;
			while(*p) p++;
			buf8 = p - fn;
			if(buf8 > WT_FILE_NAME_SIZE+4) continue;
			if((fn[buf8-4] != '.') || (fn[buf8-3] != 'p') || (fn[buf8-2] != 'r') || (fn[buf8-1] != 'n'))
			{
				continue;
			}
			strncpy((char *)PrintFolder.FilesName[i], (char *)fn, buf8+1);
			i++;
		}
		
		if(i==0)	PrintFolder.status = 2;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
		else			
		{
			PrintFolder.status = 1;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
			PrintFolder.number_TotalFile = i;
		}
	}
	else
	{
		PrintFolder.status = 4;	//0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
	}
}

/**
  * @brief  字符串中子字符串的替换
  * @param  None
  * @retval None
  */
void StrReplace(char* strSrc, char* strFind, char* strReplace,int src_len)
{
  int len=0;
  int temp_len = 0;
  unsigned int find_len = 0;
  int lastLen = 0;
  find_len = strlen(strFind);

  while (len < src_len)
  {
    if (*strSrc == *strFind)
    {
      if (strncmp(strSrc, strFind, find_len) == 0)
      {
        char* q = strSrc+find_len;
        char* p = q;//p,q均指向剩余字符串的首地址
        char* repl = strReplace;
       
				lastLen = src_len - len;
        char* temp = (char *)malloc(lastLen+1);//临时开辟一段内存保存剩下的字符串，防止内存覆盖
        for (int k = 0; k < lastLen; k++)
        {
          *(temp+k) = *(p+k);
        }
        *(temp+lastLen) = '\0';
        while (*repl != '\0')
        {
          *strSrc++ = *repl++;
        }
        p = strSrc;
        char* pTemp = temp;//回收动态开辟内存
				temp_len = 0;
        while (temp_len < lastLen+1)
        {
          *p++ = *pTemp++;
					temp_len ++;
        }
        free(temp);
        *p = '\0';
      }
      else
        strSrc++;
    }
    else
      strSrc++;
		len++;
  }
}


/**
  * @brief  初始化打印文件
  * @param  None
  * @retval None
	* //0-not init, 1-inited, 2-no files, 3-hardware error, 4-no folder
  */
uint8_t WT_PrintFiles_Init(char * filename, char * wire_mode)
{
	FRESULT res;
	FIL file;
	char str[100];
	uint32_t br =0;
	uint8_t  read_delay = 0;
//	char *str_div1 = "$Y.m.d$";
//	char *str_div2 = "@h:m:s@";
//	char *str_div3 = "@model@";
//	char date_replace[20];
//	char time_replace[20];
//	
//	memset(date_replace,0,20);
//	memset(time_replace,0,20);
	
	if(k_StorageGetStatus(USB_DISK_UNIT) || k_StorageGetStatus(MSD_DISK_UNIT))
	{
		if(store_dev == 0) strcpy (str,(char *)path_printfile);
		if(store_dev == 1) strcpy (str,(char *)path_printfile_sd);

		strcat (str,"/");
		strcat (str,filename);
		
		// open file
		res = f_open(&file, str, FA_OPEN_EXISTING | FA_READ);
		if (res == FR_OK)
		{		
			PrintFile.sum_str = f_size(&file);
			while(1)
		  {
				res=f_read(&file,PrintFile.PrintFilestr,PrintFile.sum_str,&br);
				if(res == FR_OK)
				{
//					get_system_time(date_replace,time_replace);
//					StrReplace((char *)PrintFile.PrintFilestr,str_div1,date_replace,PrintFile.sum_str);
//					StrReplace((char *)PrintFile.PrintFilestr,str_div2,time_replace,PrintFile.sum_str);
//					StrReplace((char *)PrintFile.PrintFilestr,str_div3,wire_mode,PrintFile.sum_str);
					PrintFile.sum_str = strlen((char *)PrintFile.PrintFilestr);
					break;
				}
				else read_delay++;
				if(read_delay > 5) return 2;
		  } 

			PrintFile.number_Cmdline = PrintFile.sum_str/100 +1;
			f_close(&file);
			return 1;	//0:not init, 1: init ok, 2: hardware error, 3-no this file
		}
		else	
		{
			f_close(&file);
			return 3;	//0:not init, 1: init ok, 2: hardware error, 3-no this file
		}
		
	}
	else
	{
		f_close(&file);
		return 2;	//0:not init, 1: init ok, 2: hardware error, 3-no this file
	}
}

/**
  * @brief  创建测试文件
  * @param  None
  * @retval 0-ok, 1-hardware error, 2-file operate error, 
  */
uint8_t WT_TestFiles_Create(uint8_t * path)
{
	FRESULT  res;
	FIL			 fil;
//	uint8_t* p;
//	uint8_t  i;
//	uint8_t  buf8[100];
	//uint32_t buf32;
	
	// Check disc
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && store_dev == 0)	//no usb 
	{
		return 1;
	}
	
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1)	//no SD
	{
		return 1;
	}
	
	// open file
	//res = f_open(&fil, (const TCHAR*)path, FA_OPEN_EXISTING | FA_WRITE);
	res = f_open(&fil, (const TCHAR*)path, FA_CREATE_NEW | FA_WRITE);
	if(res == FR_EXIST )	//文件存在
	{
		res = f_unlink((const TCHAR*)path);
		if(res != 0) return 2;
		
		//保存学习文件
		res = f_open(&fil, (const TCHAR*)path, FA_CREATE_NEW | FA_WRITE); //可写方式打开 没有文件则创建 
		if(res)
		{
			f_close(&fil);
			return 2;
		}
	}
	else if(res != FR_OK)	//ok
	{
		return 1;
	}
	
	//Save to file
	f_close(&fil);
	return 0;
	
}


/**
  * @brief  添加数据到文件末尾
  * @param  None
  * @retval 0-ok, 1-hardware error, 2-file operate error, 
  */
uint8_t WT_TestFiles_Write(uint8_t * path, uint8_t * data)
{
	FRESULT  res;
	FIL			 fil;
	uint8_t* p;
	uint8_t  i;
	uint8_t  buf8[100];
	uint32_t buf32;
	
	// Check disc
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && store_dev == 0)	//no usb 
	{
		return 1;
	}
	
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1)	//no SD
	{
		return 1;
	}
	
	// open file
	res = f_open(&fil, (const TCHAR*)path, FA_OPEN_EXISTING | FA_WRITE);
	if((res == FR_NO_FILE)||(res == FR_NO_PATH)) //文件不存在
	{
		p = path;
		for(;(*p)!=0;p++)
		{
			if(*p == '/')
			{
				for(i=0;i<(p-path);i++) buf8[i]=*(path+i);
				buf8[i] = 0;
				res = f_mkdir((const TCHAR*) buf8);
				if((res != 0)&&(res != FR_EXIST)) {return 2;}
			}
		}
		
		res = f_open(&fil, (const TCHAR*)path, FA_OPEN_ALWAYS | FA_WRITE); //可写方式打开 没有文件则创建 
		if(res)
		{
			f_close(&fil);
		}
	}
	else if(res != FR_OK)	//ok
	{
		return 1;
	}

	//Write string to file
	res = f_lseek(&fil, f_size(&fil)); 
	if(res != FR_OK) return 2;
	//i = strlen((char *)data);
	res = f_write(&fil, data, strlen((char *)data), &buf32); 
	if(res != 0) return 2;
	
	//Save to file
	f_close(&fil);
	return 0;

}


/**
  * @brief  读取测试文件，每次读取200字节，并存到Flash
  * @param  None
  * @retval 0-ok, 1-hardware error, 2-file operate error, 
  */
uint8_t WT_TestFile_Write2Flash (char * filename)
{
	FRESULT res;
	FIL file;
	char str[100];
	char line[IO_MAXSIZE];
	uint32_t i=0;
	uint16_t len=0;
	uint32_t buf32;
	uint8_t length[2];
	char file_name[50];
	
	memset(file_name,0,50);
	strncpy(file_name,filename,(strlen(filename)-4));
	
	if(k_StorageGetStatus(USB_DISK_UNIT) || k_StorageGetStatus(MSD_DISK_UNIT))
	{
		if(store_dev == 0) strcpy (str,(char *)path_testfile);
		if(store_dev == 1) strcpy (str,(char *)path_testfile_sd);
		strcat (str,"/");
		strcat (str,filename);

		//open file
		res = f_open(&file, str, FA_OPEN_EXISTING | FA_READ);
		if (res == FR_OK)
		{		
			len = (f_size(&file)/IO_MAXSIZE)+1;
			length[0] = len >> 8;
			length[1] = len & 0xFF;
			FM25V_IO_Write(length,0,2);//头两个字节保存字节长度
			FM25V_IO_Write((uint8_t *)file_name,2,50);//50个字节保存文件名
			for(i=0;i<len;i++)
			{
				res = f_lseek(&file, i*IO_MAXSIZE); 
				if (res == FR_OK)
				{
					memset(line,0,IO_MAXSIZE);
					f_read(&file,line,IO_MAXSIZE,&buf32);
					FM25V_IO_Write((uint8_t *)line,(52+i*IO_MAXSIZE),IO_MAXSIZE);
				}
			}
			return 0;
		}
		else return 2;
	}
	else return 1;
}


/**
  * @brief  读取Flash数据,每次读200字节，写入文件到SD卡
  * @param  None
  * @retval 0-ok, 1-hardware error, 2-file operate error, 
  */
uint8_t WT_TestFile_Write2SD(uint8_t * path)
{
	FRESULT res;
	DIR 		 dir;
	FILINFO  fno;
	FIL			 fil;
	char *	 fn;
	uint8_t  *p, *fn_filename;
	uint8_t  buf8[60];
	uint32_t i=0;
	uint16_t len=0;
	uint32_t buf32;
	uint8_t line[IO_MAXSIZE];
	uint8_t tmp[2];
	
	// Check disc
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && store_dev == 0)	//no usb or SD
	{
		return 1;
	}
	
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1)	//no usb or SD
	{
		return 1;
	}
	
	// 判断文件夹是否村存在，不存在则创建	
  if(store_dev == 0) res = f_opendir(&dir, path_studyfile);
	if(store_dev == 1) res = f_opendir(&dir, path_studyfile_sd);
  if(res == FR_NO_PATH)	//目录不存在，创建目录
	{
		p = path;
		for(;(*p)!=0;p++)
		{
			if(*p == '/')
			{
				for(i=0;i<(p-path);i++) buf8[i]=*(path+i);
				buf8[i] = 0;
				res = f_mkdir((const TCHAR*) buf8);
				if((res != 0)&&(res != FR_EXIST)) {return 2;}
			}
		}
		//open the new dir
		res = f_opendir(&dir, path_studyfile);
		if(res != FR_OK) return 1;
	}
	else if (res != FR_OK)
  {
		return 1;
	}
	
	// 判断文件是否村存在 
	#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
	#endif
	
	p = path;
	i = 0;
	for(;(*p)!=0;p++)
	{
		if(*p == '.')
		{
			p--;
			for(;(*p)!='/';p--)
			{
				i++;
				if(i>50) return 2;
			}
			p++;
			break;
		}
	}
	fn_filename = p;
	i = 0;	//0:ok, 1:error, 
	
	for (;;) 
	{
		res = f_readdir(&dir, &fno);                   /* Read a directory item */
		if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
		if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
		#if _USE_LFN
		fn = *fno.lfname ? fno.lfname : fno.fname;
		#else
		fn = fno.fname;
		#endif
		if (fno.fattrib & AM_DIR) continue;  /* It is a directory */
		else /* It is a file. */
		{
			// compare filename & new filename
			p = fn_filename;
			while(*fn != 0)
			{
				if(*fn++ != *p++)
				{
					i = 1;
					break;
				}
				if(i == 1) break;
			}
			
			if((i == 0) && (*p == 0))	//fine the same filename
			{
				i = 1;
				break;
			}
			else	//no filename
			{
				i=0;
			}
		}
	}
	f_closedir(&dir); //关闭文件夹

	//存在文件，处理
	if(i==1)	//文件已存在，返回、重命名、删除等操作
	{
		//return 3;
		res = f_unlink((const TCHAR*)path);
		if(res != 0) return 2;
	}
	
	//保存学习文件
	res = f_open(&fil, (const TCHAR*)path, FA_OPEN_ALWAYS | FA_WRITE); //可写方式打开 没有文件则创建 
	if(res)
	{
		f_close(&fil);
		return 2;
	}

	res = f_lseek(&fil, f_size(&fil)); 
	if(res != FR_OK) return 2;
	
	FM25V_IO_Read(tmp,0,2);
	len = (tmp[0] << 8 ) + tmp[1];
	for(i=0;i<len;i++)
	{
		FM25V_IO_Read(line,52+i*IO_MAXSIZE,IO_MAXSIZE);
		res = f_write(&fil, line, IO_MAXSIZE, &buf32); 
		if(res != 0) return 2;
	}

	//Save to file
	f_close(&fil);
	return 0;
}

/**
  * @brief  保存测试文件
  * @param  None
  * @retval 0-ok, 1-hardware error, 2-file operate error, 
  */
uint8_t WT_TestFiles_Write2card(uint8_t * path)
{
	FRESULT  res;
	DIR 		 dir;
	FILINFO  fno;
	FIL			 fil;
	char *	 fn;
	uint8_t  *p, *fn_filename;
	uint8_t  buf8[80], temp8[10];
	uint8_t  temp;
	uint32_t buf32;
	uint32_t i, j,len;
	uint64_t index;
  char  	 systime[30];
	char  	 str_time[40];
	
	// Check disc
	if(k_StorageGetStatus(USB_DISK_UNIT) == 0 && store_dev == 0)	//no usb or SD
	{
		return 1;
	}
	
	if(k_StorageGetStatus(MSD_DISK_UNIT) == 0 && store_dev == 1)	//no usb or SD
	{
		return 1;
	}
	
	// 判断文件夹是否村存在，不存在则创建	
  if(store_dev == 0) res = f_opendir(&dir, path_studyfile);
	if(store_dev == 1) res = f_opendir(&dir, path_studyfile_sd);
  if(res == FR_NO_PATH)	//目录不存在，创建目录
	{
		p = path;
		for(;(*p)!=0;p++)
		{
			if(*p == '/')
			{
				for(i=0;i<(p-path);i++) buf8[i]=*(path+i);
				buf8[i] = 0;
				res = f_mkdir((const TCHAR*) buf8);
				if((res != 0)&&(res != FR_EXIST)) {return 2;}
			}
		}
		//open the new dir
		res = f_opendir(&dir, path_studyfile);
		if(res != FR_OK) return 1;
	}
	else if (res != FR_OK)
  {
		return 1;
	}
	
	// 判断文件是否村存在 
	#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
	#endif
	
	p = path;
	i = 0;
	for(;(*p)!=0;p++)
	{
		if(*p == '.')
		{
			p--;
			for(;(*p)!='/';p--)
			{
				i++;
				if(i>50) return 2;
			}
			p++;
			break;
		}
	}
	fn_filename = p;
	i = 0;	//0:ok, 1:error, 
	
	for (;;) 
	{
		res = f_readdir(&dir, &fno);                   /* Read a directory item */
		if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
		if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
		#if _USE_LFN
		fn = *fno.lfname ? fno.lfname : fno.fname;
		#else
		fn = fno.fname;
		#endif
		if (fno.fattrib & AM_DIR) continue;  /* It is a directory */
		else /* It is a file. */
		{
			// compare filename & new filename
			p = fn_filename;
			while(*fn != 0)
			{
				if(*fn++ != *p++)
				{
					i = 1;
					break;
				}
				if(i == 1) break;
			}
			
			if((i == 0) && (*p == 0))	//fine the same filename
			{
				i = 1;
				break;
			}
			else	//no filename
			{
				i=0;
			}
		}
	}
	f_closedir(&dir); //关闭文件夹

	//存在文件，处理
	if(i==1)	//文件已存在，返回、重命名、删除等操作
	{
		//return 3;
		res = f_unlink((const TCHAR*)path);
		if(res != 0) return 2;
	}
	
	//保存测试文件
	res = f_open(&fil, (const TCHAR*)path, FA_OPEN_ALWAYS | FA_WRITE); //可写方式打开 没有文件则创建 
	if(res)
	{
		f_close(&fil);
		return 2;
	}

	res = f_lseek(&fil, f_size(&fil)); 
	if(res != FR_OK) return 2;
	
	//update title
	get_systemtime(systime);
	strcpy(str_time,"#6:DATE:");
	strcat(str_time,systime);
	strcat(str_time,"\r\n\0");
//	res = f_write(&fil, TestFile.FileHeader_FormatFlag, sizeof(TestFile.FileHeader_FormatFlag), &buf32); 
//	res = f_write(&fil, TestFile.FileHeader_FormatVersion, sizeof(TestFile.FileHeader_FormatVersion), &buf32); 
//	res = f_write(&fil, TestFile.FileHeader_ProductName, sizeof(TestFile.FileHeader_ProductName), &buf32); 
//	res = f_write(&fil, TestFile.FileHeader_ProductModel, sizeof(TestFile.FileHeader_ProductModel), &buf32); 
//	res = f_write(&fil, TestFile.FileHeader_ProductCompany, sizeof(TestFile.FileHeader_ProductCompany), &buf32); 
//	//res = f_write(&fil, StudyFile_Title_6, sizeof(StudyFile_Title_6)-1, &buf32); 
//	res = f_write(&fil, str_time, strlen(str_time)-1, &buf32); 
//	res = f_write(&fil, TestFile.FileHeader_ProductVersion, sizeof(TestFile.FileHeader_ProductVersion), &buf32); 
//	res = f_write(&fil, TestFile.FileHeader_DrawingNumber, sizeof(TestFile.FileHeader_DrawingNumber), &buf32); 
//	res = f_write(&fil, TestFile.FileHeader_AttachInfo, sizeof(TestFile.FileHeader_AttachInfo), &buf32); 
//	res = f_write(&fil, TestFile.FileHeader_SerialNumber, sizeof(TestFile.FileHeader_SerialNumber), &buf32); 
//	res = f_write(&fil, TestFile.FileHeader_PrintInfo, sizeof(TestFile.FileHeader_PrintInfo), &buf32); 
	
	res = f_write(&fil, StudyFile_Title_1, sizeof(StudyFile_Title_1)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_2, sizeof(StudyFile_Title_2)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_3, sizeof(StudyFile_Title_3)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_4, sizeof(StudyFile_Title_4)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_5, sizeof(StudyFile_Title_5)-1, &buf32); 
	//res = f_write(&fil, StudyFile_Title_6, sizeof(StudyFile_Title_6)-1, &buf32); 
	res = f_write(&fil, str_time, strlen(str_time)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_7, sizeof(StudyFile_Title_7)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_8, sizeof(StudyFile_Title_8)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_9, sizeof(StudyFile_Title_9)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_A, sizeof(StudyFile_Title_A)-1, &buf32); 
	res = f_write(&fil, StudyFile_Title_B, sizeof(StudyFile_Title_B)-1, &buf32); 
	p = fn_filename;
	i=0;
	while(*p) {i++; p++;}
	strcpy ((char *)buf8, "#C:");
	strcat ((char *)buf8,(char *)fn_filename);
	strcat ((char *)buf8,"\r\n");
	
	for(j=0;j<sizeof(temp8);j++) temp8[j]=0;
	sprintf((char *)temp8,"%d/%d\0", TestFile.number_point, TestFile.number_line);
	j=strlen((char *)temp8);
	strcat ((char *)buf8, "#D:");
	strcat ((char *)buf8,(char *) temp8);
	strcat ((char *)buf8,"\r\n");
	
	res = f_write(&fil, buf8, 3+i+2+5+j, &buf32); 
	if(res != 0) return 2;
	
	//save data to file
	for(index=0;index<TestFile.item_total;index++)
	{
		temp = GetString_FromTestItem(buf8, index, &len);
		if(temp != 0) return 2;
		for(i=0;i<2000;i++){}
			
		res = f_write(&fil, buf8, len, &buf32); 
		for(i=0;i<2000;i++){}
		if(res != FR_OK) return 2;
	}
	
	// save end mark to file
	for(i=0;i<sizeof(temp8);i++) temp8[i]=0;
	sprintf((char *) temp8,"%d", TestFile.item_total+1);
	strcpy ((char *)buf8, "@");
	i=0;
	while(1)
	{
		buf8[i+1] = temp8[i];
		if(temp8[i] == 0) break;
		i++;
	}
	strcat ((char *)buf8,":E\r\n");
	res = f_write(&fil, buf8, i+4, &buf32); 
	
	//Save to file
	f_close(&fil);
	return 0;
}
