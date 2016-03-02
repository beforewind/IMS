/**
  ******************************************************************************
  * @progect LZY Wire Cube Tester
	* @file    wt_bsp_ad.c
  * @author  LZY Zhang Manxin
  * @version V1.0.0
  * @date    2014-07-18
  * @brief   This file provides the E2PROM functions
  ******************************************************************************
  */

#define WT_BSP_AD_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include "wt_bsp_ad.h"
#include "wt_bsp_usart.h"
#include "cmsis_os.h"
#include "wt_bsp_key_led.h"




//static void WT_UART_AD_Rx_IT(uint8_t dat);

uint32_t debug_view;


/**
  * @brief  Configures AD IO GPIO.
  * @param  None
  * @retval None
  */
static void BSP_AD_IO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* Enable the GPIO Clock */
  AD_SOURSE_GPIO_CLK_ENABLE();
	AD_CTL_GPIO_CLK_ENABLE();
	AD_SIGNAL_GPIO_CLK_ENABLE();
//	AD_CONNECT_GPIO_CLK_ENABLE();
//	AD_UART_EA_GPIO_CLK_ENABLE();
	PS_MISO_GPIO_CLK_ENABLE();
	PS_MOSI_GPIO_CLK_ENABLE();
	
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	
  /* Configure the AD_SOURSE pin */
	GPIO_InitStruct.Pin = AD_SOURSE_A0_PIN | AD_SOURSE_A1_PIN | AD_SOURSE_A2_PIN |
												AD_SOURSE_C0_PIN | AD_SOURSE_C1_PIN | AD_SOURSE_C2_PIN |
												AD_SOURSE_RDC_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(AD_SOURSE_PORT, &GPIO_InitStruct);

	/* Configure the AD_CTL_FindPoint_PIN pin ，切换到找点模式，Channel D 接地*/
	GPIO_InitStruct.Pin = AD_CTL_FindPoint_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(AD_CTL_PORT, &GPIO_InitStruct);
	
//	GPIO_InitStruct.Pin = AD_CTL_RFU_PIN;	//次引脚用来控制SPI Flash
//  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	HAL_GPIO_Init(AD_CTL_PORT, &GPIO_InitStruct);
	
	/* Configure the AD_SIGNAL pin */
	GPIO_InitStruct.Pin = AD_SIGNAL_V01_PIN | AD_SIGNAL_V10_PIN | AD_SIGNAL_SBD_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(AD_SIGNAL_PORT, &GPIO_InitStruct);
	
	
	/* Configure the MOSI：板卡接收*/
	GPIO_InitStruct.Pin = PS_MOSI_EN_sRX_PIN | PS_MOSI_CLK_PIN | PS_MOSI_DAT0_PIN | PS_MOSI_DAT1_PIN | PS_MOSI_EN_sTX_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(PS_MOSI_PORT, &GPIO_InitStruct);
	
	/* Configure the MISO：板卡发送*/
	GPIO_InitStruct.Pin = PS_MISO_DAT1 | PS_MISO_DAT2 | PS_MISO_DAT3 | PS_MISO_DAT4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(PS_MISO_PORT, &GPIO_InitStruct);

	BSP_AD_ChangeMode(0);	//0-Normal Mode, 1-FindPoint Mode;
}

/**
  * @brief  change FindPoint mode or test mode.
  * @param  0-Normal Mode, 1-FindPoint Mode;
  * @retval None
  */
void BSP_AD_ChangeMode(uint8_t mode)	//0-Normal Mode, 1-FindPoint Mode;
{
	if(mode == 1) //find point
	{
		HAL_GPIO_WritePin(AD_CTL_PORT, AD_CTL_FindPoint_PIN, GPIO_PIN_RESET);
	}
	else //normal
	{
		HAL_GPIO_WritePin(AD_CTL_PORT, AD_CTL_FindPoint_PIN, GPIO_PIN_SET);
	}
	
}

/**
  * @brief  Configures AD Converter.
  * @param  None
  * @retval None
  */
static void BSP_AD_Converter_Init(void)
{
	ADC_ChannelConfTypeDef sConfig;
	
	/*##-1- Configure the ADC peripheral #######################################*/
  AdcHandle.Instance          = ADCx;
  
  AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;	//30Mhz(TAP), 36MHz(MAX) --- 19.5MHz
  AdcHandle.Init.Resolution = ADC_RESOLUTION12b;
  AdcHandle.Init.ScanConvMode = DISABLE;
  AdcHandle.Init.ContinuousConvMode = ENABLE;
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;
  AdcHandle.Init.NbrOfDiscConversion = 0;
  AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
  AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion = 1;
  AdcHandle.Init.DMAContinuousRequests = ENABLE;
  AdcHandle.Init.EOCSelection = DISABLE;
      
  if(HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    /* Initiliazation Error */
    //Error_Handler(); 
  }
	
	/*##-2- Configure ADC regular channel ######################################*/  
  sConfig.Channel = ADCx_CHANNEL;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfig.Offset = 0;
  
  if(HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    //Error_Handler(); 
  }

}



/**
  * @brief  Configures AD IO GPIO.
  * @param  None
  * @retval None
  */
//static void BSP_AD_UART_Init(void)
//{
//	uint32_t i;
//	
//	for(i=0;i<UART_AD_RX_Bufer_Length;i++) UART_AD_Buffer_Rx[i]=0;
//	UART_AD_Cnt_Buffer_Rx = 0;
//	Is_UART_AD_Rx_Come = 0;
//	
//	/* USART configuration */
//  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
//  /* UART1 configured as follow:
//      - Word Length = 8 Bits
//      - Stop Bit = One Stop bit
//      - Parity = None
//      - BaudRate = 9600 baud
//      - Hardware flow control disabled (RTS and CTS signals) */
//  UartHandle_AD.Instance        = UART_AD;
//  UartHandle_AD.Init.BaudRate   = 375000*3;	//max baudrate = 42MHz/16 = 2.625MHz, now = 1.125MHz
//  UartHandle_AD.Init.WordLength = UART_WORDLENGTH_8B;
//  UartHandle_AD.Init.StopBits   = UART_STOPBITS_1;
//  UartHandle_AD.Init.Parity     = UART_PARITY_NONE;
//  UartHandle_AD.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
//  UartHandle_AD.Init.Mode       = UART_MODE_TX_RX;
//  HAL_UART_Init(&UartHandle_AD);
//}

/**
  * @brief  This function handles UART interrupt request.
  * @param  huart: UART handle
  * @retval None
  */
//void WT_UART_AD_IRQHandler(UART_HandleTypeDef *huart)
//{
//  uint32_t tmp1 = 0, tmp2 = 0;

//  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
//  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
//  /* UART parity error interrupt occurred ------------------------------------*/
//  if((tmp1 != RESET) && (tmp2 != RESET))
//  { 
//    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE);
//    
//    huart->ErrorCode |= HAL_UART_ERROR_PE;
//  }
//  
//  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
//  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
//  /* UART frame error interrupt occurred -------------------------------------*/
//  if((tmp1 != RESET) && (tmp2 != RESET))
//  { 
//    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);
//    
//    huart->ErrorCode |= HAL_UART_ERROR_FE;
//  }
//  
//  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
//  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
//  /* UART noise error interrupt occurred -------------------------------------*/
//  if((tmp1 != RESET) && (tmp2 != RESET))
//  { 
//    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);
//    
//    huart->ErrorCode |= HAL_UART_ERROR_NE;
//  }
//  
//  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
//  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
//  /* UART Over-Run interrupt occurred ----------------------------------------*/
//  if((tmp1 != RESET) && (tmp2 != RESET))
//  { 
//    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
//    
//    huart->ErrorCode |= HAL_UART_ERROR_ORE;
//  }
//  
//  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
//  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
//  /* UART in mode Receiver ---------------------------------------------------*/
//  if((tmp1 != RESET) && (tmp2 != RESET))
//  { 
//    //UART_Receive_IT(huart);
//		WT_UART_AD_Rx_IT((uint8_t)(huart->Instance->DR & (uint8_t)0x00FF));	//cndz, 20140723
//    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
//  }
//  
//  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TC);
//  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TC);
//  /* UART in mode Transmitter ------------------------------------------------*/
//  if((tmp1 != RESET) && (tmp2 != RESET))
//  {
//    //UART_Transmit_IT(huart);
//  }
//  
//  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
//  {
//    /* Set the UART state ready to be able to start again the process */
//    huart->State = HAL_UART_STATE_READY;
//    
//    HAL_UART_ErrorCallback(huart);
//  }  
//}


/**
  * @brief  WT_UART_AD_Rx
  * @param  uint8_t dat
  * @retval None
  */
//static void WT_UART_AD_Rx_IT(uint8_t dat)
//{
//	/* receiver data */
//	UART_AD_Buffer_Rx[UART_AD_Cnt_Buffer_Rx] = dat;
//	
//	if(UART_AD_Cnt_Buffer_Rx == 0)
//	{
//		if(UART_AD_Buffer_Rx[0] == 0x7E) UART_AD_Cnt_Buffer_Rx++;
//	}
//	else if(UART_AD_Cnt_Buffer_Rx < (UART_AD_RX_Bufer_Length - 1))
//	{
//		UART_AD_Cnt_Buffer_Rx++;
//	}
//	
//	Is_UART_AD_Rx_Come = 1;
//	debug_view++;
//}


/**
  * @brief  WT_UART_AD_WrBuf
  * @param  uint8_t dat
  * @retval None
  */
//static uint8_t WT_UART_AD_WrBuf(uint8_t* pData, uint8_t length)
//{
//	while(length--)
//	{
//		if(UART_WaitOnFlagUntilTimeout(&UartHandle_AD, UART_FLAG_TXE, RESET, 100) != HAL_OK)
//		{
//			return 1;
//		}
//		UartHandle_AD.Instance->DR = (*pData++ & (uint8_t)0xFF);
//	}
//	
//	if(UART_WaitOnFlagUntilTimeout(&UartHandle_AD, UART_FLAG_TXE, RESET, 100) != HAL_OK)
//	{
//		return 1;
//	}
//	
//	return 0;
//}

/**
  * @brief  WT_UART_AD_WrBuf
  * @param  None
  * @retval None
  */
//static void UART_AD_Buffer_RxClear(void) //接收缓冲区复位
//{
//	uint32_t i;
//	
//	for(i=0;i<UART_AD_RX_Bufer_Length;i++) UART_AD_Buffer_Rx[i]=0;
//	UART_AD_Cnt_Buffer_Rx = 0;
//	Is_UART_AD_Rx_Come = 0;
//	debug_view = 0;
//}

/**
  * @brief  send command to FPGA
  * @param  uint8_t clks,uint16_t DAT0, uint16_t DAT1
  * @retval void
  */
static void WT_MOSI_Sendbytes(uint8_t clks,uint16_t DAT0, uint16_t DAT1)
{
	uint8_t cnt_delay = 1;
	uint8_t i;
	
	// Init state
	PS_MOSI_CLK_L;
	PS_MOSI_EN_sRX_L;
//	CLK_Delay(cnt_delay);

	//发送命令，设置通道地址
	for(i=clks;i>0;i--)
	{
		if((DAT0 & (1<<(i-1))) != 0)		PS_MOSI_DAT0_H;
		else														PS_MOSI_DAT0_L;
		if((DAT1 & (1<<(i-1))) != 0)		PS_MOSI_DAT1_H;
		else														PS_MOSI_DAT1_L;
		PS_MOSI_CLK_H;
		CLK_Delay(cnt_delay);
		
		PS_MOSI_CLK_L;
		CLK_Delay(cnt_delay);
	}
	
	//命令发送结束
	PS_MOSI_EN_sRX_H;
	PS_MOSI_CLK_L;
//	PS_MOSI_DAT0_L;
//	PS_MOSI_DAT1_L;
}

/**
  * @brief  read FPGA info
  * @param  uint8_t clks,uint16_t DAT0, uint16_t DAT1
  * @retval void
  */
static void WT_MISO_Readbytes(uint8_t clks,uint16_t *DAT1, uint16_t *DAT2, uint16_t *DAT3, uint16_t *DAT4)
{
	uint8_t cnt_delay = 1;
	uint8_t i;
	uint16_t miso_dat1=0, miso_dat2=0, miso_dat3=0, miso_dat4=0;

	//init 
	PS_MOSI_EN_sTX_L;
	PS_MOSI_CLK_L;
//	CLK_Delay(cnt_delay);
	for(i=clks ;i>0;i--)
	{
		PS_MOSI_CLK_H;
		CLK_Delay(cnt_delay);
		PS_MOSI_CLK_L;
		CLK_Delay(cnt_delay);
		miso_dat1 |= HAL_GPIO_ReadPin(PS_MISO_PORT,PS_MISO_DAT1) << (i-1);
		miso_dat2 |= HAL_GPIO_ReadPin(PS_MISO_PORT,PS_MISO_DAT2) << (i-1);
		miso_dat3 |= HAL_GPIO_ReadPin(PS_MISO_PORT,PS_MISO_DAT3) << (i-1);
		miso_dat4 |= HAL_GPIO_ReadPin(PS_MISO_PORT,PS_MISO_DAT4) << (i-1);
	}
	
	//stop
	PS_MOSI_EN_sTX_H;
	PS_MOSI_CLK_L;
	
	*DAT1 = miso_dat1;
	*DAT2 = miso_dat2;
	*DAT3 = miso_dat3;
	*DAT4 = miso_dat4;
}


/**
  * @brief  PortBoard_Update_ChannelAddr
  * @param  addrAB / addrCD: 16 bit
						bit16-9: 	0
						bit8:			0-turn on address, 1-turn off address
						bit7-0:		address(0-255)
  * @retval state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
  */
uint8_t PortBoard_Update_ChannelAddr(uint16_t addrAB, uint16_t addrCD)
{
	uint8_t CHAB = 0, CHCD = 0;
//	uint32_t cnt_delay = 10;	//
	uint16_t mosi_dat0, mosi_dat1;
	uint16_t miso_dat1 = 0,miso_dat2 = 0,miso_dat3 = 0,miso_dat4 = 0;	

	// check address
	if(((addrAB & 0xFE00) != 0) || ((addrCD & 0xFE00) != 0))
	{
		return 3;
	}
	mosi_dat0 = addrAB;
	mosi_dat1 = addrCD;
	
	//设置通道地址
	WT_MOSI_Sendbytes(10,mosi_dat0,mosi_dat1);
//	CLK_Delay(cnt_delay);
	
	//查询板卡通道
	WT_MISO_Readbytes(2,&miso_dat1,&miso_dat2,&miso_dat3,&miso_dat4);
	
	//检测AB通道是否打开
	if((miso_dat1 & (1<<1)) == 0) CHAB++;
	if((miso_dat2 & (1<<1)) == 0) CHAB++;
	if((miso_dat3 & (1<<1)) == 0) CHAB++;
	if((miso_dat4 & (1<<1)) == 0) CHAB++;
	
	//检测CD通道是否打开
	if((miso_dat1 & (1<<0)) == 0) CHCD++;
	if((miso_dat2 & (1<<0)) == 0) CHCD++;
	if((miso_dat3 & (1<<0)) == 0) CHCD++;
	if((miso_dat4 & (1<<0)) == 0) CHCD++;
	
	if( CHAB <= 1 && CHCD <= 1 )
	{
		return 0;	//ok
	}
	else
	{
		return 4;	//通信异常
	}
}

/**
  * @brief  PortBoard_Read_ChannelSetting
  * @param  uint32_t *info
  * @retval state：0-ok, 1-error
  */
uint8_t PortBoard_Read_ChannelSetting(uint32_t *info)
{
//	uint32_t cnt_delay = 10;	//
	uint8_t mosi_dat0 = 2, mosi_dat1 = 2;
	uint16_t miso_dat1 = 0,miso_dat2 = 0,miso_dat3 = 0,miso_dat4 = 0;	

	//查询板卡通道地址（1010）
	WT_MOSI_Sendbytes(2,mosi_dat0,mosi_dat1);
//	CLK_Delay(cnt_delay);
	
	//查询板卡通道
	WT_MISO_Readbytes(14,&miso_dat1,&miso_dat2,&miso_dat3,&miso_dat4);
	return 0;	//ok
}

/**
  * @brief  PortBoard_Read_BoardInfo
  * @param  uint32_t *info
  * @retval state：0-ok, 1-error
  */
uint8_t PortBoard_Read_BoardInfo(uint32_t *info)
{
//	uint32_t cnt_delay = 10;	//
	uint8_t mosi_dat0 = 2, mosi_dat1 = 3;
	uint16_t miso_dat1 = 0,miso_dat2 = 0,miso_dat3 = 0,miso_dat4 = 0;	

	//查询板卡信息（1011）
	WT_MOSI_Sendbytes(2,mosi_dat0,mosi_dat1);
//	CLK_Delay(cnt_delay);
	
	//查询板卡通道
	WT_MISO_Readbytes(8,&miso_dat1,&miso_dat2,&miso_dat3,&miso_dat4);
	*info = ((miso_dat4 & 0xFF) << 24) | 
	        ((miso_dat3 & 0xFF) << 16) |
	        ((miso_dat2 & 0xFF) << 8)  | 
	         (miso_dat1 & 0xFF);
	return 0;	//ok
}


	
/**
  * @brief  AD_Source_UpdateAddr
  * @param  addrSourceA, addrSourceC
  * @retval None
  */
void AD_Update_SourceAddr(uint8_t addrSourceA, uint8_t addrSourceC)
{
	uint16_t buf16;
	
	
	addrSourceA &= 0x07;
	addrSourceC &= 0x07;
	buf16 = (addrSourceA<<4) | ((addrSourceC&0x03)<<7) | ((addrSourceC&0x04)<<9);

	AD_SOURSE_PORT->ODR &= 0xF60F;
	AD_SOURSE_PORT->ODR |= buf16;
}

/**
  * @brief  AD_UART_UpdateEn
  * @param  BoardID
  * @retval None
  */
//static void AD_UART_UpdateEn(uint8_t BoardID)
//{
//	uint16_t buf16;
//	
//	buf16 = (1 << BoardID) << 2;
////	AD_UART_EA_PORT->ODR &= 0xFFC3;
////	AD_UART_EA_PORT->ODR |= buf16;
//	PS_MOSI_PORT->ODR &= 0xFFC3;
//	PS_MOSI_PORT->ODR |= buf16;
//}

/**
  * @brief  AD_UART_UpdateAddr
  * @param  aboard_id, ddrAB, addrCD (0-64)
  * @retval state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
  */
//static uint8_t AD_UART_UpdateAddr(uint8_t board_id, uint8_t addrAB, uint8_t addrCD)
//{
//	uint8_t buf8;
//	
//	//检查地址
//	if((addrAB > 64) | (addrCD > 64)) return 3; //无效地址
//	
//	//选择控制板
//	switch(board_id)
//	{
//		case PortBoard_01:
//			if((AD_State.PortBoard_Connected & (1<<0)) == 0) return 1; //板卡不存在 
//			AD_UART_UpdateEn(board_id);
//			buf8 = AD_UART_CMD_UpdateChannel(addrAB, addrCD);
//			if(buf8 != 0) return 4;	//通信异常
//			break;
//		
//		case PortBoard_02:
//			if((AD_State.PortBoard_Connected & (1<<1)) == 0) return 1; //板卡不存在 
//			AD_UART_UpdateEn(board_id);
//			buf8 = AD_UART_CMD_UpdateChannel(addrAB, addrCD);
//			if(buf8 != 0) return 4;	//通信异常
//			break;
//		
//		case PortBoard_03:
//			if((AD_State.PortBoard_Connected & (1<<2)) == 0) return 1; //板卡不存在 
//			AD_UART_UpdateEn(board_id);
//			buf8 = AD_UART_CMD_UpdateChannel(addrAB, addrCD);
//			if(buf8 != 0) return 4;	//通信异常
//			break;
//		
//		case PortBoard_04:
//			if((AD_State.PortBoard_Connected & (1<<3)) == 0) return 1; //板卡不存在 
//			AD_UART_UpdateEn(board_id);
//			buf8 = AD_UART_CMD_UpdateChannel(addrAB, addrCD);
//			if(buf8 != 0) return 4;	//通信异常
//			break;
//		
//		default:
//			return 1; //板卡不存在 
//			//break;
//	}
//	return 0;
//}



/**
  * @brief  Configures AD GPIO.
  * @param  None
  * @retval None
  */
void BSP_AD_Init(void)
{
	uint8_t buf8;
	uint32_t buf32;
	
  BSP_AD_IO_Init();
	BSP_AD_Converter_Init();
	//BSP_AD_UART_Init();
		
	// Init
	buf8 = (HAL_GPIO_Read(PS_MOSI_PORT) >> 3);
	AD_State.PortBoard_Connected = (~buf8) & 0x0F;
	AD_State.ChannelAB_Addr_State = AD_ChannelAddr_NULL;
	AD_State.ChannelCD_Addr_State = AD_ChannelAddr_NULL;
	AD_State.SourceA_Addr = AD_SourceAddr_GNDR;
	AD_State.SourceC_Addr = AD_SourceAddr_GNDR;
	AD_State.Channel_Addr_Error = 0;
	
	//Update Init Value
	PortBoard_Update_ChannelAddr(AD_State.ChannelAB_Addr_State, AD_State.ChannelCD_Addr_State);
	AD_Update_SourceAddr(AD_SourceAddr_GNDR, AD_SourceAddr_GNDR);
	PortBoard_Read_BoardInfo(&buf32);
	PortBoard_Read_ChannelSetting(&buf32);
}


/**
  * @brief  Get AD Value For AD Buffer
  * @param  AD Buffer
  * @retval AD Value
  * 对缓冲区数据升序排列,
  * 然后去掉最低和最高LOST_VAL个数,取平均值
  */
#define LOST_VAL 5	  //丢弃参数，共丢弃5*2=10个

uint16_t AD_GetValue_FromBuffer(uint16_t* pData, uint32_t Length)
{
	uint16_t i, j;
	uint16_t sum=0;
	uint16_t temp;
		    
	for(i=0;i<Length-1; i++)//排序
	{
		for(j=i+1;j<Length;j++)
		{
			if(pData[i]>pData[j])//升序排列
			{
				temp=pData[i];
				pData[i]=pData[j];
				pData[j]=temp;
			}
		}
	}
	
	sum=0;
	for(i=LOST_VAL;i<Length-LOST_VAL;i++) sum+=pData[i];
	temp=sum/(Length-2*LOST_VAL);
	return temp;  
}

/**
  * @brief  Get AD Value For AD Buffer
  * @param  AD Buffer
  * @retval AD Value
  * 对缓冲区数据升序排列,
  * 然后去掉最低和最高LOST_VAL个数,取平均值
  */
#define LOST_VAL_Voltage 100	  //丢弃参数，共丢弃200个

uint16_t Voltage_GetValue_FromBuffer(uint16_t* pData, uint32_t Length)
{
	uint16_t i, j;
	uint32_t sum=0;
	uint16_t temp;
		    
	for(i=0;i<Length-1; i++)//排序
	{
		for(j=i+1;j<Length;j++)
		{
			if(pData[i]>pData[j])//升序排列
			{
				temp=pData[i];
				pData[i]=pData[j];
				pData[j]=temp;
			}
		}
	}
	
	sum=0;
	for(i=LOST_VAL;i<Length-LOST_VAL;i++) sum+=pData[i];
	temp=sum/(Length-2*LOST_VAL);
	return temp;  
}

/**
  * @brief  Conversion complete callback in non blocking mode 
  * @param  AdcHandle : AdcHandle handle
  * @note   This example shows a simple way to report end of conversion, and 
  *         you can add your own implementation.    
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle_0)
{
	AD_IsConvertFinished = 1;	//1-finished, 0-waiting
	HAL_ADC_Stop_DMA(AdcHandle_0);
}

/**
  * @brief  Configures AD GPIO.
  * @param  None
	* @retval 0:ok, 1:error
  */
uint8_t AD_WaitFinishConvert(uint32_t cnt)
{
	while(cnt--)
	{
		if(AD_IsConvertFinished == 1) return 0; //1-finished, 0-waiting
	}
	HAL_ADC_Stop_DMA(&AdcHandle);
	return 1;
}



/**
  * @brief  Configures AD GPIO.
  * @param  None
	* @retval 0:ok, 1:error
  */
uint8_t AD_ReadValue(uint32_t cnt, uint16_t * buf16)
{
	uint8_t res;
	uint32_t i;
	
	AD_IsConvertFinished = 0;	//1-finished, 0-waiting
	for(i=0;i<ADC_Bufer_Length;i++) BufADC_Value[i]=0;	//reset buffer
	HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)BufADC_Value, ADC_Bufer_Length);	//satart converter

	res = AD_WaitFinishConvert(10*1000);	//timeout 1ms
	if(res != 0) return 1;
	*buf16 = AD_GetValue_FromBuffer(BufADC_Value, ADC_Bufer_Length);
	return 0;
}

/**
  * @brief  AD_GetVoltage_AutoStep.
  * @param  uint16_t value, uint8_t mode)
* @retval voltate(放大倍数：4095*300 = 1,228,500)
  */
// V_0.1 = (0.1V * AD) / 4095 / 30;
// V_1.0 = (1.0V * AD) / 4095 / 3;
// V_3.3 = (3.3V * AD) / 4095 * 6;
// buf64 ------> 放大倍数（4095*300 = 1,228,500）
// return -----> 放大倍数 AD_Voltage_MUL

uint64_t AD_GetVoltage_AutoStep(uint16_t value, uint8_t mode)
{
	float BufferV;
	
	mode = mode & 0x03;
	switch(mode)
	{
		case 0:	//小于0.1V
			// 2014-8-14, zmx
			// 0.002V / A187
			// 0.016V / A681
			// 0.089V / A3208
			BufferV = ((0.088-0.054)/(2134-888)) * (value-888) + 0.054f;
			break;
		
		case 1:	//小于1.0V
			// 2014-8-13, zmx
			// 0.100V / B405
			// 0.120V / B442
			// 0.863V / B3188
			BufferV = value * 3.3 / 4095 / 2.767;
			break;
		
		case 3:	//大于1.0V
			// 2014-8-13, zmx
			// 1.272V / C61
			// 
			#ifdef HD_VERSION1
			BufferV = value * 3.3 / 4095 *6.2;
			#else
			BufferV = value * 3.3 / 4095 / 0.872;
			#endif
			
			break;
		
		default:	//error
			BufferV = 0;
			break;
	}

	BufferV = BufferV * AD_Voltage_MUL;
	return BufferV;	
}

/**
  * @brief  AD_GetRes_FromDCU4V5.
  * @param  uint16_t value, uint8_t mode)
	* @retval res（欧姆）
  */
// 1M   - 1208C
// 100K - 1190C

uint32_t AD_GetRes_FromDCU4V5(uint16_t value)	//大于120K
{
	uint64_t buf64;

	buf64 = (value - 1190) * 900 * 1000 * 1000 /(1208 - 1190) + 100*1000*1000;

	return buf64/100;
}

/**
  * @brief  AD_GetCAP_FromDCU4V5.
  * @param  VoltageValue(放大1000倍, mv)
	* @retval CAP(pF)
  */
// NC:		6071
// 102:		6071
// 104:		6059
// 1uF:		5755
// 100uF:	1065
// 470uF: 1041
//
uint32_t AD_GetCAP_FromDCU4V5(uint16_t VoltageValue)	// VoltageValue 放大1000倍
{
	uint32_t cap;
	
	if(VoltageValue > 6065)	//102
	{
		cap = 10 * 100;		
	}
	else if(VoltageValue > 6060)	//103
	{
		cap = 10 * 1000;
	}
	else if(VoltageValue > 5900)	//104
	{
		cap = 10 * 10000;
	}
	else if(VoltageValue > 5000)	//105 - 5755
	{
		cap = 10 * 100000;
	}
	else if(VoltageValue > 4000)	//106(10u) - 4500
	{
		cap = 10 * 1000000;
	}
	else if(VoltageValue > 2500)	//476(47u) - 3000
	{
		cap = 47 * 1000000;
	}
	else if(VoltageValue > 1050)	//107(100u) - 1065
	{
		cap = 10 * 10000000;
	}
	else if(VoltageValue > 1000)	//477(470u) - 1041
	{
		cap = 47 * 10000000;
	}
	else
	{
		cap = 10 * 100000000;
	}
	
	// cap
	return cap;	
}


/**
  * @brief  AD_UART_ReadBoardInfo
  * @param  aboard_id, buf
  * @retval state：0-ok, 1-error
  */

//uint8_t AD_UART_ReadBoardInfo(uint8_t board_id, uint8_t * buf)
//{

//	uint16_t i;
//	uint32_t cnt_delay = 10;	
//	uint16_t mosi_dat0 = 0,mosi_dat1 = 0;
//	uint16_t miso_dat0 = 0,miso_dat1 = 0,miso_dat2 = 0,miso_dat3 = 0;
//	
//	if(board_id > 3) return 1;
//	//AD_UART_UpdateEn(board_id);
//	
//	//init
//	//UART_AD_Buffer_RxClear();

//	//发送查询板卡信息命令（1011）
//	mosi_dat0 |= 0x02;
//	mosi_dat1 |= 0x03;
//	WT_MOSI_Sendbytes(2,mosi_dat0,mosi_dat1);
//	CLK_Delay(cnt_delay);
//	
//	// Wait for return
//	i = UART_AD_RX_Timeout;
//	while(i--)
//	{
//		//获取板卡信息
//		WT_MISO_Readbytes(8,&miso_dat0,&miso_dat1,&miso_dat2,&miso_dat3);
//		
//		if(board_id == 0)
//		{
//			if( ((miso_dat0 >> 6) & 0x03) == 0 && ((miso_dat0 >> 4) & 0x03) == 0x01)
//			{
//				buf[0] = (miso_dat0 >> 6) & 0x03;
//				buf[1] = (miso_dat0 >> 4) & 0x03;
//				buf[3] = miso_dat0 & 0x0F;
//				return 0;	//ok
//			}
//			else return 1;
//		}
//		if(board_id == 1)
//		{
//			if( ((miso_dat1 >> 6) & 0x03) == 1 && ((miso_dat1 >> 4) & 0x03) == 0x01)
//			{
//				buf[0] = (miso_dat1 >> 6) & 0x03;
//				buf[1] = (miso_dat1 >> 4) & 0x03;
//				buf[3] = miso_dat1 & 0x0F;
//				return 0;	//ok
//			}
//			else return 1;
//		}
//		if(board_id == 2)
//		{
//			if( ((miso_dat2 >> 6) & 0x03) == 2 && ((miso_dat2 >> 4) & 0x03) == 0x01)
//			{
//				buf[0] = (miso_dat2 >> 6) & 0x03;
//				buf[1] = (miso_dat2 >> 4) & 0x03;
//				buf[3] = miso_dat2 & 0x0F;
//				return 0;	//ok
//			}
//			else return 1;
//		}
//		if(board_id == 3)
//		{
//			if( ((miso_dat3 >> 6) & 0x03) == 0 && ((miso_dat3 >> 4) & 0x03) == 0x01)
//			{
//				buf[0] = (miso_dat3 >> 6) & 0x03;
//				buf[1] = (miso_dat3 >> 4) & 0x03;
//				buf[3] = miso_dat3 & 0x0F;
//				return 0;	//ok
//			}
//			else return 1;
//		}
//		// os dealy
//		osDelay(1);
//	}
	
//	//timeout 
//	return 1;
//}

