/*
******************************************************************************
* @progect LZY Wire Cube Tester
* @file    wt_uart_COM1.c
* @author  LZY Zhang Manxin
* @version V1.0.0
* @date    2014-12-09
* @brief   ...
******************************************************************************
*/

/* Define ------------------------------------------------------------------*/
#define  WT_UART_COM1_GLOBALS


/* Includes ------------------------------------------------------------------*/
#include "k_bsp.h"
#include "main.h"


/* Private functions ---------------------------------------------------------*/
static void WT_UART_COM1_Rx_IT(uint8_t dat);
static void UART_COM1_Buffer_RxClear(void);
static void UartCOM1_RX_Process(void);

extern void get_system_time(char * date ,char * time);
extern char *itoa(int num, char *str, int radix);

static void UartCOM1_PrintOk(void);
static void UartCOM1_PrintNG(void);


/* Static variable ---------------------------------------------------------*/

extern uint8_t task_mode; //0-测试模式  1-维修模式
extern char printing_PPID[25];//打印PPID
extern char product_info[45]; //产品信息
	
/**
  * @brief  wire self check task
  * @param  argument: pointer that is passed to the thread function as start argument.
  * @retval None
  */
void UARTCOM1Thread(void const * argument)
{
	osEvent event;
	
	WT_UART_COM1_Init();	//Init
	UART_COM1_Buffer_RxClear();

  for( ;; )
  {
		//clear message
		while(1)
		{
			event = osMessageGet( UartCOM1Event, 0);
			if(event.status != osEventMessage) break;
		}
		//wait new message
    event = osMessageGet(UartCOM1Event, osWaitForever );
    
    if( event.status == osEventMessage )
    {
      switch(event.value.v)
      {
				case UartCOM1_RX_Event:
					UartCOM1_RX_Process();
					break;
				
				case UartCOM1_TX_Event:		
					if(task_mode == 1) break;//维修模式不用打印标签
					//print_mode = 0;//0-print ok  1-print NG
					//UartCOM1_Send_StartPrint_Cmd();		
				  UartCOM1_PrintOk();
					break;
				case UartCOM1_TXNG_Event:
					if(task_mode == 1) break;//维修模式不用打印标签
					//print_mode = 1;//0-print ok  1-print NG
					UartCOM1_PrintNG();//测试NG也要打印标签
					break;
				default:
					break;
      }
    }
		else	//error
		{
			osDelay(500);
		}
  }
}


/**
  * @brief  WT_UART_COM1_Init
  * @param  None
  * @retval None
  */
void WT_UART_COM1_Init(void)
{
	uint32_t i;
	
	for(i=0;i<UART_COM1_RX_Bufer_Length;i++) UART_COM1_Buffer_Rx[i]=0;
	UART_COM1_Cnt_Buffer_Rx = 0;

  UartHandle_COM1.Instance        = UART_COM1;
  UartHandle_COM1.Init.BaudRate   = 9600;
  UartHandle_COM1.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle_COM1.Init.StopBits   = UART_STOPBITS_1;
  UartHandle_COM1.Init.Parity     = UART_PARITY_NONE;
  UartHandle_COM1.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle_COM1.Init.Mode       = UART_MODE_TX_RX;
  HAL_UART_Init(&UartHandle_COM1);	
}


/**
  * @brief  This function handles UART interrupt request.
  * @param  huart: UART handle
  * @retval None
  */
void WT_UART_COM1_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t tmp1 = 0, tmp2 = 0;

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
  /* UART parity error interrupt occurred ------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE);
    
    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART frame error interrupt occurred -------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);
    
    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART noise error interrupt occurred -------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);
    
    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
    
    huart->ErrorCode |= HAL_UART_ERROR_ORE;
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    //UART_Receive_IT(huart);
		WT_UART_COM1_Rx_IT((uint8_t)(huart->Instance->DR & (uint8_t)0x00FF));	//cndz, 20140723
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
  }
  
  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TC);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC);
  /* UART in mode Transmitter ------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  {
    //UART_Transmit_IT(huart);
  }
  
  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
    /* Set the UART state ready to be able to start again the process */
    huart->State = HAL_UART_STATE_READY;
    
    HAL_UART_ErrorCallback(huart);
  }  
}

/**
  * @brief  UART_COM1_Buffer_RxClear
  * @param  None
  * @retval None
  */
static void UART_COM1_Buffer_RxClear(void)
{
	uint32_t i;
	
	for(i=0;i<UART_COM1_RX_Bufer_Length;i++) UART_COM1_Buffer_Rx[i]=0;
	UART_COM1_Cnt_Buffer_Rx = 0;
//	Is_UART_COM1_Rx_Come = 0;
}

/**
  * @brief  WT_UART_COM1_Rx
  * @param  uint8_t dat
  * @retval None
  */
uint8_t WT_UART_COM1_WrBuf(uint8_t* pData, uint16_t length)
{
	while(length--)
	{
		if(UART_WaitOnFlagUntilTimeout(&UartHandle_COM1, UART_FLAG_TXE, RESET, 100) != HAL_OK)
		{
			return 1;
		}
		UartHandle_COM1.Instance->DR = (*pData++ & (uint8_t)0xFF);
	}
	
	if(UART_WaitOnFlagUntilTimeout(&UartHandle_COM1, UART_FLAG_TXE, RESET, 100) != HAL_OK)
	{
		return 1;
	}
	
	return 0;
}

/**************************************************/
/*** Aplication ***********************************/
/**************************************************/

/**
  * @brief  WT_UART_COM1_Rx_IT
  * @param  uint8_t dat
  * @retval None
  */
static void WT_UART_COM1_Rx_IT(uint8_t dat)
{
//	uint8_t buf8=0;
	
	/* receiver data */
	UART_COM1_Buffer_Rx[UART_COM1_Cnt_Buffer_Rx] = dat;
	if(UART_COM1_Cnt_Buffer_Rx < (UART_COM1_RX_Bufer_Length-2)) UART_COM1_Cnt_Buffer_Rx++;
	
	//检测桢头标志
	if(UART_COM1_Cnt_Buffer_Rx == 1)
	{
		if(UART_COM1_Buffer_Rx[0] != 0x7e ) 
		{
			UART_COM1_Buffer_RxClear();
			return;
		}
	}
	else if(UART_COM1_Cnt_Buffer_Rx <= 4) //通信地址、帧格式
	{
		//Byte[1] -	帧长度
		//Byte[2]	-	功能
		//Byte[3]	-	参数
		//Byte[4]	-	累加和
		if(UART_COM1_Cnt_Buffer_Rx == 2)
		{
			if((UART_COM1_Buffer_Rx[1] < 4) |													//长度太短
				 (UART_COM1_Buffer_Rx[1] > UART_COM1_RX_Bufer_Length))	//长度太长
			{
				UART_COM1_Buffer_RxClear();
				return;
			}
		}
	}
	else //wait frame end
	{
		if(UART_COM1_Cnt_Buffer_Rx >= UART_COM1_Buffer_Rx[1])
		{
			//Is_UART_COM1_Rx_Come = 1;
			osMessagePut(UartCOM1Event, UartCOM1_RX_Event, 0);	//收到数据帧
			//UartCOM1_RX_Process();
		}
	}
}



/**
  * @brief  Update_Printtime
  * @param  None
  * @retval None
  */
//static void Update_Print_Time()
//{
//	uint16_t i;
//	char date_replace[20];
//	char time_replace[20];
//	
//	memset(date_replace,0,20);
//	memset(time_replace,0,20);
//	get_system_time(date_replace,time_replace);
//	for(i=0;i<8;i++)
//	{
//		PrintFile.PrintFilestr[76+i] = time_replace[i];
//	}

//}

/**
  * @brief  Update_PrintDate
		更新年，月。月份用字幕代替。直接在字符串中替换，避免每次读取文件再替换关键字
  * @param  None
  * @retval None
  */
//static void Update_Print_Date()
//{
//	uint16_t i;
//	char date_replace[4];
//	char date[20];
//	char time[20];
//	char month[2];
//	char month_index[12]={'A','B','C','D','E','F','G','H','I','J','K','L'};
//	
//	uint8_t mon = 0;
//	
//	memset(date,0,20);
//	memset(time,0,20);
//	memset(date_replace,0,4);
//	get_system_time(date,time);
//	month[0] = date[5];
//	month[1] = date[6];
//	mon = atoi(month);
//	date_replace[0] = date[2];
//	date_replace[1] = date[3];
//	date_replace[2] = month_index[mon-1];
//	
//	for(i=0;i<3;i++)
//	{
//		PrintFile.PrintFilestr[187+i] = date_replace[i];
//		PrintFile.PrintFilestr[231+i] = date_replace[i];
//	}
//	
//}

/**
  * @brief  Update_Print_Serialnum
		更新序列号。直接在字符串中替换，避免每次读取文件再替换关键字
  * @param  None
  * @retval None
  */
//static void Update_Print_Serialnum()
//{
//	uint8_t i;
//	uint8_t j,k;
//	char sn_replace[7];
//	char sn_sort[7];
//	
//	memset(sn_replace,0,7);
//	memset(sn_sort,0,7);
//	
//	itoa(WT_Config.Serial_Num,sn_replace,10);
//	for(i=0;i<7;i++)
//	{
//		if(sn_replace[i]==0)
//		{
//			for(j=0;j<7-i;j++)
//			{
//				sn_sort[j]= '0';
//			}
//			for(k=0;k<i;k++) 
//			{
//				sn_sort[j+k]= sn_replace[k];
//			}
//			break;
//		}
//	}

////变量替换
//	for(i=0;i<7;i++)
//	{
//		PrintFile.PrintFilestr[190+i] = sn_sort[i];
//		PrintFile.PrintFilestr[234+i] = sn_sort[i];
//	}

//}

/**
  * @brief  Update_Print_Serialnum
		更新打印文件序列号。直接在字符串中替换，避免每次读取文件再替换关键字
  * @param  None
  * @retval None
  */
static void Update_Print_Data()
{
	uint8_t i;
	char sn_sort[5];

	memset(sn_sort,0,5);
	
	for(i=0;i<5;i++)
	{
		sn_sort[i] = printing_PPID[19+i];
	}
	WT_Config.Serial_Num = atoi(sn_sort);
	
	//Product_info替换
	for(i=0;i<15;i++)
	{
		PrintFile.PrintFilestr[69+i] = product_info[i];
		PrintFile.PrintFilestr[107+i] = product_info[15+i];
		PrintFile.PrintFilestr[145+i] = product_info[30+i];
	}
	
	//TPN内容替换
	for(i=0;i<12;i++)
	{
		PrintFile.PrintFilestr[190+i] = printing_PPID[1+i];
	}
	//TSN内容替换
	for(i=0;i<10;i++)
	{
		PrintFile.PrintFilestr[232+i] = printing_PPID[14+i];
	}

	//二维码PPID变量替换
	for(i=0;i<25;i++)
	{
		PrintFile.PrintFilestr[263+i] = printing_PPID[i];
	}

}

/**
  * @brief  Update_Print_Serialnum
		更新打印文件序列号。直接在字符串中替换，避免每次读取文件再替换关键字
		流水号变为6位，第一位固定为8或6.==>8代表正常生产良品；6代表不良品需返修
  * @param  None
  * @retval None
  */
static void Update_Print_DataNG()
{
	uint8_t i;
	char sn_sort[5];

	memset(sn_sort,0,5);
	
	printing_PPID[18] = '6';
	
	for(i=0;i<5;i++)
	{
		sn_sort[i] = printing_PPID[19+i];
	}
	WT_Config.Serial_Num = atoi(sn_sort);
	
	//Product_info替换
	for(i=0;i<15;i++)
	{
		PrintFile.PrintFilestr[69+i] = product_info[i];
		PrintFile.PrintFilestr[107+i] = product_info[15+i];
		PrintFile.PrintFilestr[145+i] = product_info[30+i];
	}
	
	//TPN内容替换
	for(i=0;i<12;i++)
	{
		PrintFile.PrintFilestr[190+i] = printing_PPID[1+i];
	}
	//TSN内容替换
	for(i=0;i<10;i++)
	{
		PrintFile.PrintFilestr[232+i] = printing_PPID[14+i];
	}

	//二维码PPID变量替换
	for(i=0;i<25;i++)
	{
		PrintFile.PrintFilestr[263+i] = printing_PPID[i];
	}

}


/**
  * @brief  UartCOM1_RX_Process
  * @param  None
  * @retval None
  */
static void UartCOM1_RX_Process(void)
{
	//Process
	switch(UART_COM1_Buffer_Rx[2])
	{
		case 0x01:	//开始发送打印命令返回值
			PrintFile.print_status = 3;//0-stop 1-wait, 2-print ok, 3-print run, 4-print error
			break;
		
		case 0x02:	//发送打印内容返回值
			PrintFile.print_status = 3;//0-stop 1-wait, 2-print ok, 3-print run, 4-print error
			break;
		
		case 0x03:	//结束发送打印内容命令返回值
			//UartCOM1_QueryInfo_Status();
			PrintFile.print_status = 2;//0-stop 1-wait, 2-print ok, 3-print run, 4-print error
			break;
		
		default:
			break;
	}
	
	// clear rx buffer
	UART_COM1_Buffer_RxClear();
}

/**
  * @brief  UartCOM1_PrintOk
  * @param  None
  * @retval None
  */
static void UartCOM1_PrintOk(void)
{
//	Update_Print_Date();
//	Update_Print_Serialnum();
	Update_Print_Data();
  //PrintFile.sum_str = strlen((char *)PrintFile.PrintFilestr);
	PrintFile.sum_str = 295;
	/****** Send ***********************************************************/
	WT_UART_COM1_WrBuf(PrintFile.PrintFilestr, PrintFile.sum_str);
//	WT_Config_Serial_Num_Save();//保存打印流水号
	osMessagePut(Uart24GHzEvent, Uart24GHZ_IMSPRINTOK_REPORT_Event, 0);	//wifi发送打印确认信息	
}

/**
  * @brief  UartCOM1_PrintNG
  * @param  None
  * @retval None
  */
static void UartCOM1_PrintNG(void)
{
//	Update_Print_Date();
//	Update_Print_Serialnum();
	Update_Print_DataNG();
  PrintFile.sum_str = 295;
	/****** Send ***********************************************************/
	WT_UART_COM1_WrBuf(PrintFile.PrintFilestr, PrintFile.sum_str);
//	WT_Config_Serial_Num_Save();//保存打印流水号
	osMessagePut(Uart24GHzEvent, Uart24GHZ_IMSPRINTNG_REPORT_Event, 0);	//wifi发送打印确认信息	
}

