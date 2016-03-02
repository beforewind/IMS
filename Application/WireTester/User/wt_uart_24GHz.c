/*
*********************************************************************************************************
*                                     MICRIUM BOARD SUPPORT SUPPORT
*
*                          (c) Copyright 2003-2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*                                       SERIAL (UART) INTERFACE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM3220G-EVAL
*                                         Evaluation Board
*
* Filename      : BSP_RS485GKJ.c
* Version       : V1.00
* Programmer(s) : SL
*********************************************************************************************************
*/

/* Define ------------------------------------------------------------------*/
#define  WT_UART_24GHZ_GLOBALS


/* Includes ------------------------------------------------------------------*/
#include "k_bsp.h"
#include "main.h"
#include "verision.h"
#include "wt_task_wiretest.h"
#pragma diag_suppress 870 



static void WT_UART_24GHz_Rx_IT(uint8_t dat);
static uint8_t WT_UART_24GHz_Cnfig(void);
static void Uart24G_RX_Process(void);

static void Uart24GHz_IMSTEST_Require(void);
static void Uart24GHz_IMSREPAIR_Require(void);	
static void Uart24GHz_IMSTESTOK_Reply(void);	
static void Uart24GHz_IMSREPAIROK_Reply(void);
static void Uart24GHz_IMSTESTNG_Reply(void);	
static void Uart24GHz_IMSREPAIRNG_Reply(void);
static void Uart24GHz_IMSPRINTOK_Report(void);
static void Uart24GHz_IMSPRINTNG_Report(void);
static uint16_t last_rec;
static uint8_t ok_cnt;
static uint8_t connect_stat = 0;//0-not connect  1-connect
//static uint16_t frame_cnt = 0;

//static uint32_t str_size=0;

uint8_t task_mode=0; //0-测试模式  1-维修模式
uint8_t test_permit = 0;//0-不允许测试 1-允许测试

void Create_PPID(char *PPID);
void get_system_date(char * date);

extern char repair_PPID[25];
extern char printing_PPID[25];
extern char print_PPID[25];
extern char product_info[45]; 

extern uint8_t NG_point1;
extern uint8_t NG_point2;
extern uint8_t NG_type; //0-断路  1-错位  2-短路
extern uint8_t NG_points[8];

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

extern void set_systemtime(uint8_t sec,uint8_t min, uint8_t hour, uint8_t day, uint8_t month,uint16_t year);
extern char *itoa(int num, char *str, int radix);
extern void wt_SetText_Menu(const char * pTitle); 
extern void wt_SetText_Status(const char * pTitle); 

#define TIME_OUT  500

/**
  * @brief  wire self check task
  * @param  argument: pointer that is passed to the thread function as start argument.
  * @retval None
  */
void UART24GHZThread(void const * argument)
{
	osEvent event;
	uint16_t i;
	static uint16_t cnt=0;
	
	WT_UART_24GHz_Init();	//Init
	
	UART_24GHz_Buffer_RxClear();
	osDelay(6000);
	WT_UART_24GHz_Cnfig();	//config
	osDelay(1000);
	WT_PrintFile_Init();
	
  for( ;; )
  {
    //clear message
		while(1)
		{
			event = osMessageGet( Uart24GHzEvent, 0);
			if(event.status != osEventMessage) break;
		}
		event = osMessageGet(Uart24GHzEvent, osWaitForever );
	//	event = osMessageGet(Uart24GHzEvent, 10000 );
		
    if( event.status == osEventMessage )
    {
      switch(event.value.v)
      {
				case Uart24GHZ_RX_Event:
					Uart24G_RX_Process();
					break;		
				case Uart24GHZ_TXOK_Event://测试结果反馈
					//Uart24GHz_UploadTestRes();
					if(task_mode	== 0)//0-测试模式 1-维修模式
					{
						Uart24GHz_IMSTESTOK_Reply();	
					}
					else Uart24GHz_IMSREPAIROK_Reply();	
					break;
				case Uart24GHZ_TXNG_Event://测试结果反馈
					//Uart24GHz_UploadTestRes();
					if(task_mode	== 0)//0-测试模式 1-维修模式
					{
						Uart24GHz_IMSTESTNG_Reply();	
					}
					else Uart24GHz_IMSREPAIRNG_Reply();	
					break;
				case Uart24GHZ_CONF_Event:
					BSP_UartNRF_PowerOFF();
					osDelay(1000);
					WT_UART_24GHz_Init();
					osDelay(1000);
					UART_24GHz_Buffer_RxClear();
					WT_UART_24GHz_Cnfig();	//config
				  if(connect_stat == 1) wt_SetText_Status("网络连接OK");
					break;
				case Uart24GHZ_IMSTEST_Event://同IMS Server通信，请求测试
					Uart24GHz_IMSTEST_Require();	
					break;
				case Uart24GHZ_IMSREPAIR_Event://同IMS Server通信，维修测试请求
					Uart24GHz_IMSREPAIR_Require();						
					break;
				case Uart24GHZ_IMSPRINTOK_REPORT_Event:
					Uart24GHz_IMSPRINTOK_Report();						
					break;
				case Uart24GHZ_IMSPRINTNG_REPORT_Event:
					Uart24GHz_IMSPRINTNG_Report();						
					break;
				default:
					break;
      }
    }
		else if( event.status == osEventTimeout )//超时重新连接
		{
			last_rec = UART_24GHz_Cnt_Buffer_Rx;
			for(i=0;i<30;i++)
			{
				osDelay(3);
				if(last_rec == UART_24GHz_Cnt_Buffer_Rx ) break;
				else last_rec = UART_24GHz_Cnt_Buffer_Rx;
			}	
			
			if(UART_24GHz_Cnt_Buffer_Rx == 0)
			{
				cnt++;
				//BSP_UartNRF_PowerOFF();
				osDelay(1000);
				WT_UART_24GHz_Init();
				osDelay(1000);
				UART_24GHz_Buffer_RxClear();
				WT_UART_24GHz_Cnfig();	//config
			}
			UART_24GHz_Cnt_Buffer_Rx = 0;
		}
		else	//error
		{
			osDelay(1000);
		}
  }
}


/**
  * @brief  WT_UART_24GHz_Init
  * @param  None
  * @retval None
  */
void WT_UART_24GHz_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	uint32_t i;
	
	for(i=0;i<UART_24GHz_RX_Bufer_Length;i++) UART_24GHz_Buffer_Rx[i]=0;
	UART_24GHz_Cnt_Buffer_Rx = 0;
	Is_UART_24GHz_Rx_Come = 0;
	
  UartHandle_24GHz.Instance        = UART_24GHz;
  UartHandle_24GHz.Init.BaudRate   = 115200;
  UartHandle_24GHz.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle_24GHz.Init.StopBits   = UART_STOPBITS_1;
  UartHandle_24GHz.Init.Parity     = UART_PARITY_NONE;
  UartHandle_24GHz.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle_24GHz.Init.Mode       = UART_MODE_TX_RX;
  HAL_UART_Init(&UartHandle_24GHz);
	
	/* Enable the PowerSW_WireLess_PIN Clock */
  PowerSW_GPIO_CLK_ENABLE();

  /* Configure the PowerSW_WireLess_PIN  */
  GPIO_InitStruct.Pin = PowerSW_WireLess_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(PowerSW_PORT, &GPIO_InitStruct);
	BSP_UartNRF_PowerON();
}


/**
  * @brief  This function handles UART interrupt request.
  * @param  huart: UART handle
  * @retval None
  */
void WT_UART_24GHz_IRQHandler(UART_HandleTypeDef *huart)
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
		WT_UART_24GHz_Rx_IT((uint8_t)(huart->Instance->DR & (uint8_t)0x00FF));	//cndz, 20140723
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
  * @brief  WT_UART_24GHz_Rx
  * @param  uint8_t dat
  * @retval None
  */
static void WT_UART_24GHz_Rx_IT(uint8_t dat)
{
	/* receiver data */
	UART_24GHz_Buffer_Rx[UART_24GHz_Cnt_Buffer_Rx] = dat;
	if(UART_24GHz_Cnt_Buffer_Rx < (UART_24GHz_RX_Bufer_Length-2)) UART_24GHz_Cnt_Buffer_Rx++;

	if(UART_24GHz_Cnt_Buffer_Rx >=3 )
	{
		//Is_UART_COM2_Rx_Come = 1;
		osMessagePut(Uart24GHzEvent, Uart24GHZ_RX_Event, 0);	//收到数据帧
		//UartCOM2_RX_Process();
	}

}

/**
  * @brief  UART_24GHz_Buffer_RxClear
  * @param  None
  * @retval None
  */
static void UART_24GHz_Buffer_RxClear(void)
{
	uint32_t i;
	
	for(i=0;i<UART_24GHz_RX_Bufer_Length;i++) UART_24GHz_Buffer_Rx[i]=0;
	UART_24GHz_Cnt_Buffer_Rx = 0;
	Is_UART_24GHz_Rx_Come = 0;
}

/**
  * @brief  WT_UART_24GHz_Rx
  * @param  uint8_t dat
  * @retval None
  */
uint8_t WT_UART_24GHz_WrBuf(uint8_t* pData, uint8_t length)
{
	while(length--)
	{
		if(UART_WaitOnFlagUntilTimeout(&UartHandle_24GHz, UART_FLAG_TXE, RESET, 100) != HAL_OK)
		{
			return 1;
		}
		UartHandle_24GHz.Instance->DR = (*pData++ & (uint8_t)0xFF);
	}
	
	//if(UART_WaitOnFlagUntilTimeout(&UartHandle_24GHz, UART_FLAG_TXE, RESET, 100) != HAL_OK)
	if(UART_WaitOnFlagUntilTimeout(&UartHandle_24GHz, UART_FLAG_TXE, RESET, 100) != HAL_OK)
	{
		return 1;
	}
	
	return 0;
}


static void WT_GetIP(char *buff)
{
	char * ptr;	
	uint8_t index = 0;
	strtok(buff,",");	
		index=0;
		while(1)
		{
			ptr=strtok(NULL,".,\"");
			if(ptr == 0) break;
			switch(index)
			{
				case 0:
					index++;
					WT_Config.IP_Addr[0]= atoi(ptr);
					break;
				case 1:
					index++;
					WT_Config.IP_Addr[1]= atoi(ptr);
					break;
				case 2:
					index++;
					WT_Config.IP_Addr[2]= atoi(ptr);
					break;
				case 3:
					index++;
					WT_Config.IP_Addr[3]= atoi(ptr);
					break;
				default:
					break;
			}
		}
}

static void WT_GetMAC(char *buff)
{
	uint8_t i=0;
	char ptr[2];
	for(i=0;i<6;i++)
	{
		strncpy(ptr,&buff[4+i*2],2);
		WT_Config.MAC_Addr[i]=strtol(ptr,NULL,16);
	}
	
}

static uint16_t WT_GetSockID(char *buff)
{
	uint16_t sockid=0;
	char ptr[8];
	strcpy(ptr,&buff[4]);
	sockid = atoi(ptr);
	return sockid;
}

static uint8_t WT_UART_24GHz_Cnfig(void)
{
	uint16_t i;
	uint8_t mode=0;
	uint16_t sock_Id=0;
	char buff[30];
	char cmd[50];
	char res[20];
	char serv_ip[20];
	
	connect_stat = 0;//0-not connect  1-connect
	
	memset(cmd,0,50);
	memset(res,0,20);
	memset(serv_ip,0,20);
	
	sprintf(cmd,"AT+SSID=\"%s\"\n",(char *)WT_Config.NetWork);
	sprintf(serv_ip,"%d.%d.%d.%d",WT_Config.Server_IP[0],WT_Config.Server_IP[1],WT_Config.Server_IP[2],WT_Config.Server_IP[3]);
	WT_UART_24GHz_WrBuf((uint8_t *)cmd, strlen(cmd));
	
	while(1)
	{
		for(i=0;i<3000;i++) //wait for receive first data
		{
			osDelay(3);
			if(UART_24GHz_Cnt_Buffer_Rx != 0) break;
		}
		
		if(UART_24GHz_Cnt_Buffer_Rx == 0) //3S内无回复，重发命令
		{
//			WT_UART_24GHz_WrBuf((uint8_t *)cmd, strlen(cmd));
			return 0;
		}
		else //已收到数据，等待接收完成
		{
			last_rec = UART_24GHz_Cnt_Buffer_Rx;
			for(i=0;i<300;i++)
			{
				osDelay(3);
				if(last_rec == UART_24GHz_Cnt_Buffer_Rx) break;
				else last_rec = UART_24GHz_Cnt_Buffer_Rx;
			}
			
			if(i<10)  //接收完好
			{				
				last_rec =0 ;
				memset(buff,0,30);
				strncpy(buff,(char *)UART_24GHz_Buffer_Rx,30);
				
				//接收完成，成功、失败
				if(strncmp(buff,"+OK",3)==0 && ok_cnt==0) 
				{
					mode=1;
					ok_cnt++;
				}
				else if(strncmp(buff,"+OK",3)==0 && ok_cnt==1) 
				{
					mode=2;
					ok_cnt++;
				}
				else if(strncmp(buff,"+OK",3)==0 && ok_cnt==2) 
				{
					mode=3;
					ok_cnt++;
				}
				else if(strncmp(buff,"+OK",3)==0 && ok_cnt==3) 
				{
					mode=4;
					ok_cnt++;
				}
				else if(strncmp(buff,"+OK",3)==0 && ok_cnt==4) 
				{
					mode=5;
					ok_cnt++;
				}
				else if(strncmp(buff,"+OK",3)==0 && ok_cnt==5) 
				{
					WT_GetIP(buff);
					mode=6;
					WT_Config_Ipaddr_Save();
					ok_cnt++;
				}
				else if(strncmp(buff,"+OK",3)==0 && ok_cnt==6) 
				{
					WT_GetMAC(buff);
					mode=7;
					WT_Config_MAC_Save();
					ok_cnt++;
				}
				else if(strncmp(buff,"+OK",3)==0 && ok_cnt==7) 
				{
					sock_Id = WT_GetSockID(buff);
					mode=8;
					ok_cnt++;
				}
				else if(strncmp(buff,"+OK",3)==0 && ok_cnt==8) 
				{
					mode=9;
					ok_cnt++;
				}
				else if(strncmp(buff,"+OK",3)==0 && ok_cnt==9) 
				{
					ok_cnt=0;
					return 0;
				}
				else mode=10;
				
				//clear rx buffer
				UART_24GHz_Buffer_RxClear();
				
				//send new command
				switch(mode)
				{					
					case 1:	//配置KEY
						memset(cmd,0,50);
						sprintf(cmd,"AT+KEY=1,0,\"%s\"\n",(char *)WT_Config.SecKey);
						WT_UART_24GHz_WrBuf((uint8_t *)cmd, strlen(cmd));
						break;
					case 2:	//KEY OK
						WT_UART_24GHz_WrBuf((uint8_t *)"AT+PMTF\n", 8);
						for(i=0;i<300;i++) //wait for receive first data
						{
							osDelay(3);
							if(UART_24GHz_Cnt_Buffer_Rx != 0) break;
						}
						break;
					case 3:	//write flash OK
						WT_UART_24GHz_WrBuf((uint8_t *)"AT+Z\n", 5);
						for(i=0;i<500;i++) //reboot
						{
							osDelay(3);
							//if(UART_24GHz_Cnt_Buffer_Rx != 0) break;
						}
						break;
					case 4:	//reboot OK
						WT_UART_24GHz_WrBuf((uint8_t *)"AT+WJOIN\n", 9);
						for(i=0;i<300;i++) //wait for receive first data
						{
							osDelay(3);
							if(UART_24GHz_Cnt_Buffer_Rx != 0) break;
						}
						break;
					case 5:	//查询IP
						osDelay(TIME_OUT);
						WT_UART_24GHz_WrBuf((uint8_t *)"AT+LKSTT=?\n", 11);
//						for(i=0;i<300;i++) //wait for receive first data
//						{
//							osDelay(3);
//							if(UART_24GHz_Cnt_Buffer_Rx != 0) break;
//						}
						break;
					case 6:	//查询MAC
						WT_UART_24GHz_WrBuf((uint8_t *)"AT+QMAC=?\n", 10);
						for(i=0;i<20;i++) //wait for receive first data
						{
							osDelay(3);
							if(UART_24GHz_Cnt_Buffer_Rx != 0) break;
						}
						break;
					case 7:	//join OK
						memset(cmd,0,50);
						sprintf(cmd,"AT+SKCT=0,0,%s,%d,%d\n",serv_ip,WT_Config.TCP_Port,WT_Config.TCP_Port+WT_Config.Dev_ID);
						WT_UART_24GHz_WrBuf((uint8_t *)cmd, strlen(cmd));
						//osDelay(1000);
						for(i=0;i<300;i++) //wait for receive first data
						{
							osDelay(3);
							if(UART_24GHz_Cnt_Buffer_Rx != 0) break;
						}
						break;
					case 8:	//设置系统默认发送的socket
						memset(cmd,0,50);			
						sprintf(cmd,"AT+SKSDF=%d\n",sock_Id);
						WT_UART_24GHz_WrBuf((uint8_t *)cmd, strlen(cmd));
						for(i=0;i<100;i++) //wait for receive first data
						{
							osDelay(3);
							if(UART_24GHz_Cnt_Buffer_Rx != 0) break;
						}
						break;
					case 9:	//设置透传模式
						WT_UART_24GHz_WrBuf((uint8_t *)"AT+ENTM\r\n", 9);
						connect_stat = 1;
						//WT_UART_24GHz_WrBuf((uint8_t *)"AT+SKSTT=1\r\n", 12);
						break;
					case 10:	//重新连接
						memset(cmd,0,50);
						sprintf(cmd,"AT+SSID=\"%s\"\n",(char *)WT_Config.NetWork);
						WT_UART_24GHz_WrBuf((uint8_t *)cmd, strlen(cmd));
						ok_cnt = 0;
						break;
					default:	
						ok_cnt = 0;				
						break;
				}

			}
			else//一直接收到数据，硬件问题、一直发送数据
			{
				UART_24GHz_Buffer_RxClear();
			}
		}
		osDelay(20);
	}
}


/**
  * @brief  Uart24G_RX_Process
  * @param  None
  * @retval None
  */
static void Uart24G_RX_Process(void)
{
	uint16_t i;

	last_rec = UART_24GHz_Cnt_Buffer_Rx;
	
	
	for(i=0;i<50;i++)
	{
		osDelay(30);
		if(last_rec == UART_24GHz_Cnt_Buffer_Rx) break;
		else last_rec = UART_24GHz_Cnt_Buffer_Rx;
	}
	if(i < 50) 
	{
		last_rec =0 ;
		if(	UART_24GHz_Buffer_Rx[0] == '0' && UART_24GHz_Buffer_Rx[1] == '3' && UART_24GHz_Buffer_Rx[2] == '&'  )//01代表OK,02代表NG
		{
			task_mode	= 0;//0-测试模式 1-维修模式
			if(UART_24GHz_Buffer_Rx[3] == 'Y')//符合要求，开始测试
			{
				test_permit = 1; //0-不允许测试 1-允许测试
				memset(print_PPID,0,25);
				memset(product_info,0,45);
				//获取PPID   3&Y&P1065835-00-C:SA15L0000001
				for(i=0;i<25;i++)
				{
					print_PPID[i] = UART_24GHz_Buffer_Rx[5+i];
				}
				//获取product_info 
				for(i=0;i<45;i++)
				{
					product_info[i] = UART_24GHz_Buffer_Rx[31+i];
				}
				
			}
			else
			{
				wt_SetText_Status("程序名错误");
				test_permit = 0; //0-不允许测试 1-允许测试
			}
		}
		else if(UART_24GHz_Buffer_Rx[0] == '0' && UART_24GHz_Buffer_Rx[1] == '2' && UART_24GHz_Buffer_Rx[2] == '&')
		{
			task_mode	= 1;//0-测试模式 1-维修模式
			if(UART_24GHz_Buffer_Rx[3] == 'Y')//维修结果反馈
			{
				//osMessagePut(WireTestEvent, WIRETEST_START_EVENT, 0);//开始测试
				test_permit = 1; //0-不允许测试 1-允许测试
			}
			else
			{
				wt_SetText_Status("程序名错误");
				test_permit = 0; //0-不允许测试 1-允许测试
			}
		}
		else if(UART_24GHz_Buffer_Rx[0] == '0' && UART_24GHz_Buffer_Rx[1] == '4' && UART_24GHz_Buffer_Rx[2] == '&')
		{
			memset(printing_PPID,0,25);
			//获取PPID   3&Y&P1065835-00-C:SA15L0000001
			for(i=0;i<25;i++)
			{
				printing_PPID[i] = UART_24GHz_Buffer_Rx[5+i];
			}
			if(UART_24GHz_Buffer_Rx[3] == 'Y')//维修结果反馈
			{
				if(WT_Config.Print == 1 ) osMessagePut(UartCOM1Event, UartCOM1_TX_Event, 0);//本地打印OK标签
			}
			else 
			{
				if(WT_Config.Print == 1 ) osMessagePut(UartCOM1Event, UartCOM1_TXNG_Event, 0);//本地打印NG标签
			}
		}
		else if(	(UART_24GHz_Buffer_Rx[0] == '+') && (UART_24GHz_Buffer_Rx[1] == 'O') && (UART_24GHz_Buffer_Rx[2] == 'K') 
			   && UART_24GHz_Buffer_Rx[7] == '0' && UART_24GHz_Buffer_Rx[8] == '3' && UART_24GHz_Buffer_Rx[9] == '&' )
		{
			if( UART_24GHz_Buffer_Rx[10] == 'Y')
			{
				test_permit = 1; //0-不允许测试 1-允许测试
				memset(print_PPID,0,25);
				memset(product_info,0,45);
				//获取PPID   3&Y&P1065835-00-C:SA15L0000001
				for(i=0;i<25;i++)
				{
					print_PPID[i] = UART_24GHz_Buffer_Rx[12+i];
				}
				//获取product_info 
				for(i=0;i<45;i++)
				{
					product_info[i] = UART_24GHz_Buffer_Rx[38+i];
				}
			}
			else
			{
				wt_SetText_Status("程序名错误");
				test_permit = 0; //0-不允许测试 1-允许测试
			}
		}
		
		UART_24GHz_Buffer_RxClear();

	}
	else UART_24GHz_Buffer_RxClear();
}

/**
  * @brief  Uart24GHz_HeatBeat
  * @param  None
  * @retval None
  */
static void Uart24G_HeartBeat(char *buff)
{
	uint16_t i;
	uint16_t j=0;
	uint8_t  TxBuffer[20];
	
	//return data to PC
	/****** 帧头 ****************************************************/
	TxBuffer[j++] = 'L';	//Logo
	TxBuffer[j++] = 'Z';
	TxBuffer[j++] = 'Y';
	TxBuffer[j++] = '1';
	TxBuffer[j++] = buff[4];	//4_5,帧序号
	TxBuffer[j++] = buff[5];	
	TxBuffer[j++] = 12;	//6,帧长度
	//---------------------------------------
	TxBuffer[j++] = 0x01;	//7,心跳命令
	//for(i=0;i<2;i++) TxBuffer[j++] = 0x00;	//8_9,保留字节	
	//TxBuffer[j++] = Uart24G_GetDevStat();	//测试仪工作状态	
	TxBuffer[j++] = 0;	//测试仪工作状态	
	TxBuffer[j++] = 0x00; //测试仪故障  0-正常 其它-故障

	/****** CRC-16 校验 ****************************************************/
	i = Data_CRC16_MOSBUS(TxBuffer, TxBuffer[6]-2); //计算出CRC-16
	TxBuffer[j++] = (i>>8)&0xFF; //CRC-16H
	TxBuffer[j++] = i&0xFF; //CRC-16L

	/****** Send ***********************************************************/
	WT_UART_24GHz_WrBuf(TxBuffer, TxBuffer[6]);
}



/**
* @brief   同IMS服务器通信，将程序名反馈 IMS，以 03 开头，案例：03&8342444
  * @param  None
  * @retval None
  */
static void Uart24GHz_IMSTEST_Require(void)
{
	uint16_t i=0;
	uint16_t j=0;
	uint8_t  TxBuffer[30];
	uint8_t  filename[30];
	memset(TxBuffer,0,30);
	memset(filename,0,30);
	
	/****** 帧头 ****************************************************/
	TxBuffer[i++] = '0';	//Logo
	TxBuffer[i++] = '3';
	TxBuffer[i++] = '&';
	
	if(strlen((char *)TestFile.FileName)>0)
  {
		strncpy((char *)filename,(char *)TestFile.FileName,strlen((char *)TestFile.FileName)-4);
		for(j=0;j<strlen((char *)filename);j++)
		{
			TxBuffer[i++] = filename[j];
		}
		
		/****** Send ***********************************************************/
		WT_UART_24GHz_WrBuf(TxBuffer, 30);
	}
	

}

/**
* @brief   同IMS服务器通信，测试调用程序，扫描 PPID ，并将程序名以及 PPID 反馈给 IMS ，案例：02&C0021512190002&82342313
  * @param  None
  * @retval None
  */
static void Uart24GHz_IMSREPAIR_Require(void)
{
	uint16_t i=0;
	uint16_t j=0;
	uint8_t  TxBuffer[60];
	uint8_t  filename[30];
	uint8_t  TxCnt = 0;
	memset(TxBuffer,0,60);
	memset(filename,0,30);
	
	/****** 帧头 ****************************************************/
	TxBuffer[i++] = '0';	//Logo
	TxBuffer[i++] = '2';	//Logo
	TxBuffer[i++] = '&';
	
	//PPID
	for(j=0;j<25;j++)
	{
		TxBuffer[i++] = repair_PPID[j];
	}
	
	TxBuffer[i++] = '&';
	//设备型号
	if(strlen((char *)TestFile.FileName)>0)
  {
		strncpy((char *)filename,(char *)TestFile.FileName,strlen((char *)TestFile.FileName)-4);
		for(j=0;j<strlen((char *)filename);j++)
		{
			TxBuffer[i++] = filename[j];
		}
		TxCnt = 29+ strlen((char *)filename);
	}
	
	/****** Send ***********************************************************/
	WT_UART_24GHz_WrBuf(TxBuffer, TxCnt);
}

/**
* @brief   生成PPID
  * @param  None
  * @retval None
  */
//void Create_PPID(char *PPID)
//{
//	uint8_t i = 0;
//	uint8_t j = 0;
//	uint8_t buff[14];
//	uint8_t date[8];
//	uint8_t str_sn[4];
//	char dev_ID[3];
//	memset(buff,0,14);
//	memset(date,0,8);
//	memset(str_sn,0,4);
//	memset(dev_ID,0,3);
//	
//	itoa(WT_Config.Dev_ID,dev_ID,10);
//	get_system_date((char *)date);
//	buff[i++] = 'C';
//	if(dev_ID[2] == 0)//首字符补零
//	{
//		buff[i++] = '0';
//		buff[i++] = dev_ID[0];
//		buff[i++] = dev_ID[1];
//	}
//	else
//	{
//		buff[i++] = dev_ID[0];
//		buff[i++] = dev_ID[1];
//		buff[i++] = dev_ID[2];
//	}
//	//date
//	for(j=2;j<8;j++)
//	buff[i++] = date[j];
//	//流水号
//	itoa(sn,(char *)str_sn,10);
//	if(str_sn[1]==0)//前3位补零
//	{
//		buff[i++] = '0';
//		buff[i++] = '0';
//		buff[i++] = '0';
//		buff[i++] = str_sn[0];
//	}
//	if(str_sn[2]==0)//前两位补零
//	{
//		buff[i++] = '0';
//		buff[i++] = '0';
//		buff[i++] = str_sn[0];
//		buff[i++] = str_sn[1];
//	}
//	else if(str_sn[2]!=0 && str_sn[3]==0)//前1位补零
//	{
//		buff[i++] = '0';
//		buff[i++] = str_sn[0];
//		buff[i++] = str_sn[1];
//		buff[i++] = str_sn[2];
//	}
//	else
//	{
//		for(j=0;j<4;j++)
//		buff[i++] = str_sn[j];
//	}
//	memcpy(PPID,(char *)buff,14);
//}


/**
* @brief   获取系统日期
  * @param  None
  * @retval None
  */
void get_system_date(char * date)
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
		sprintf((char *)TempStr, "%02d%s%02d", year, strMonth[month-1], day);
	}
	else
	{
		sprintf((char *)TempStr, "20160101");
	}
	//strcat((char*)TempStr,(char*)timestr);
	//strcat((char*)TempStr,".log");
	strcpy(date,(char*)TempStr);
}

/**
* @brief   测试结果反馈
  * @param  None
  * @retval None
  */
static void Uart24GHz_IMSTESTOK_Reply(void)
{
	uint16_t i=0;
	uint16_t j=0;
	uint8_t  TxBuffer[60];
	uint8_t  filename[30];
	memset(TxBuffer,0,60);
	memset(filename,0,30);
	
	/****** 帧头 ****************************************************/
	TxBuffer[i++] = '0';	//Logo
	TxBuffer[i++] = '1';	//Logo
	TxBuffer[i++] = '&';
	
	//PPID
	//Create_PPID((char *)PPID);
	for(j=0;j<25;j++)
	{
		TxBuffer[i++] = print_PPID[j];
	}
	
	TxBuffer[i++] = '&';
	//测试结果

	TxBuffer[i++] = 'Y';
	
	TxBuffer[i++] = '&';
	TxBuffer[i++] = '0';
	TxBuffer[i++] = '0';
	/****** Send ***********************************************************/
	WT_UART_24GHz_WrBuf(TxBuffer, 33);
}

/**
* @brief   测试结果反馈
  * @param  None
  * @retval None
  */
static void Uart24GHz_IMSTESTNG_Reply(void)
{
	uint16_t i=0;
	uint16_t j=0;
	uint8_t  TxBuffer[50];
	uint8_t  filename[30];
	memset(TxBuffer,0,50);
	memset(filename,0,30);
	
	/****** 帧头 ****************************************************/
	TxBuffer[i++] = '0';	//Logo
	TxBuffer[i++] = '1';	//Logo
	TxBuffer[i++] = '&';
	
	//PPID
	//print_PPID[18] = '6';
	for(j=0;j<25;j++)
	{
		TxBuffer[i++] = print_PPID[j];
	}
	
	TxBuffer[i++] = '&';
	//测试结果
  TxBuffer[i++] = 'N';
	
	TxBuffer[i++] = '&';
	TxBuffer[i++] = '0';
	TxBuffer[i++] = '0';
	
	//错误类型
	TxBuffer[i++] = '&';
	TxBuffer[i++] = NG_type;
	//错误点位
	TxBuffer[i++] = '&';
	for(j=0;j<8;j++)
	{
		TxBuffer[i++] = NG_points[j];
	}
//	TxBuffer[i++] = NG_point1;
//	TxBuffer[i++] = NG_point2;
	/****** Send ***********************************************************/
	WT_UART_24GHz_WrBuf(TxBuffer, 44);
}

/**
* @brief   维修结果反馈
  * @param  None
  * @retval None
  */
static void Uart24GHz_IMSREPAIROK_Reply(void)
{
	uint16_t i=0;
	uint16_t j=0;
	uint8_t  TxBuffer[50];
	memset(TxBuffer,0,50);
	
	/****** 帧头 ****************************************************/
	TxBuffer[i++] = '0';	//Logo
	TxBuffer[i++] = '2';	//Logo
	TxBuffer[i++] = '&';
	
	//PPID
	for(j=0;j<25;j++)
	{
		TxBuffer[i++] = repair_PPID[j];
	}
	
	TxBuffer[i++] = '&';
	//测试结果

	TxBuffer[i++] = 'Y';

	/****** Send ***********************************************************/
	//WT_UART_24GHz_WrBuf(TxBuffer, strlen((char *)TxBuffer));
	WT_UART_24GHz_WrBuf(TxBuffer, 30);
}

/**
* @brief   维修结果反馈
  * @param  None
  * @retval None
  */
static void Uart24GHz_IMSREPAIRNG_Reply(void)
{
	uint16_t i=0;
	uint16_t j=0;
	uint8_t  TxBuffer[50];
	memset(TxBuffer,0,50);
	
	/****** 帧头 ****************************************************/
	TxBuffer[i++] = '0';	//Logo
	TxBuffer[i++] = '2';	//Logo
	TxBuffer[i++] = '&';
	
	//PPID
	for(j=0;j<25;j++)
	{
		TxBuffer[i++] = repair_PPID[j];
	}
	
	TxBuffer[i++] = '&';
	//测试结果

	TxBuffer[i++] = 'N';

	/****** Send ***********************************************************/
	//WT_UART_24GHz_WrBuf(TxBuffer, strlen((char *)TxBuffer));
	WT_UART_24GHz_WrBuf(TxBuffer, 30);
}

/**
* @brief   打印OK确认回复
  * @param  None
  * @retval None
  */
static void Uart24GHz_IMSPRINTOK_Report(void)
{
	uint8_t i=0;
	uint8_t j=0;
	uint8_t  TxBuffer[40];
	memset(TxBuffer,0,40);
	
	/****** 帧头 ****************************************************/
	TxBuffer[i++] = '0';	//Logo
	TxBuffer[i++] = '5';	//Logo
	TxBuffer[i++] = '&';
	TxBuffer[i++] = 'Y';
	TxBuffer[i++] = '&';
	for(j=0;j<25;j++)
	{
		TxBuffer[i++] = printing_PPID[j];
	}

	/****** Send ***********************************************************/
	WT_UART_24GHz_WrBuf(TxBuffer, 30);
}

/**
* @brief   打印NG确认回复
  * @param  None
  * @retval None
  */
static void Uart24GHz_IMSPRINTNG_Report(void)
{
	uint8_t i=0;
	uint8_t j=0;
	uint8_t  TxBuffer[40];
	memset(TxBuffer,0,40);
	
	/****** 帧头 ****************************************************/
	TxBuffer[i++] = '0';	//Logo
	TxBuffer[i++] = '5';	//Logo
	TxBuffer[i++] = '&';
	TxBuffer[i++] = 'N';
	TxBuffer[i++] = '&';
	for(j=0;j<25;j++)
	{
		TxBuffer[i++] = printing_PPID[j];
	}
	//错误类型
	TxBuffer[i++] = '&';
	TxBuffer[i++] = NG_type;
	//错误点位
	TxBuffer[i++] = '&';
	for(j=0;j<8;j++)
	{
		TxBuffer[i++] = NG_points[j];
	}
	/****** Send ***********************************************************/
	WT_UART_24GHz_WrBuf(TxBuffer, 41);
}
