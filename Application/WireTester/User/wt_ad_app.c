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

#define WT_AD_APP_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include "wt_ad_app.h"
#include "wt_task_wirefindpoint.h"
#include "k_bsp.h"
#include "verision.h"

// Var Define
//static uint32_t cnt_test = 1000;
static uint8_t  cnt_delay_test	= 3;	//delay for channel switch
static uint16_t cnt_delay_test_cap = 1500;
//static uint32_t cnt_t1 = 168/3 * 1000;
//static uint32_t cnt_t2 = 168/3 * 1000 * 10;

#ifdef HD_VERSION1
#define AD_DELAY 150
#else
#define AD_DELAY 50
#endif
/* 2015-3-12，优化导通测试速度，取消测试慢的组合
//#define		WT_WZK_MOD_12R		0		// I8mA,   P0.1
//#define		WT_WZK_MOD_27R		1		// U4.5V,  P0.1
//#define		WT_WZK_MOD_100R		2		// I1mA,   P0.1
//#define		WT_WZK_MOD_125R		3		// I8mA,   P1.0
//#define		WT_WZK_MOD_365R		4		// U4.5V,  P1.0
//#define		WT_WZK_MOD_1K			5		// I1mA,   P1.0
//#define		WT_WZK_MOD_10K		6		// I100uA, P1.0
//static uint8_t __WireTest_WZK_Mode_Source[] = {	AD_SourceAddr_I8mA, AD_SourceAddr_U45V, AD_SourceAddr_I1mA, AD_SourceAddr_I8mA,
//																								AD_SourceAddr_U45V, AD_SourceAddr_I1mA,AD_SourceAddr_I100uA};
//static uint8_t __WireTest_WZK_Mode_Signal[] = {AD_InputV01, AD_InputV01, AD_InputV01, AD_InputV10, AD_InputV10, AD_InputV10, AD_InputV10};
//0:12R, 1:27R, 2:100R, 3:125R; 4:365R, 5:1K, 6:10k, */

//2015-3-12，优化导通测试速度，取消测试慢的组合
//#define		WT_WZK_MOD_27R		0		// U4.5V,  P0.1, 5.5us
//#define		WT_WZK_MOD_365R		1		// U4.5V,  P1.0, 8.5us
static uint8_t __WireTest_WZK_Mode_Source[] = {	AD_SourceAddr_U45V, AD_SourceAddr_I1mA,AD_SourceAddr_I100uA};
static uint8_t __WireTest_WZK_Mode_Signal[] = {AD_InputV10, AD_InputV10,AD_InputV10};
//0:365R, 1:1kR, 2:10kR,
//测试延时等待时间
static uint16_t __WireTest_Wait_Time[] = {30,60,90,120,150,30000};

/**
  * @brief  线束测试 - 导通
  * @param  item, result value
  * @retval 0:ok, 1:value error, 2:test error,
  */
uint8_t AD_TestItem_Wire(uint32_t item, uint32_t* result)
{
	uint8_t res;
	uint8_t cnt_error = 0;
	uint16_t ad_delay= 40;
	uint32_t tmp;
	uint16_t i = 0;
	
	//关闭激励源
//	AD_Update_SourceAddr(AD_SourceAddr_GNDR, AD_SourceAddr_GNDR);
	
	//切换通道
	while(cnt_error < 10)
	{
		res = PortBoard_Update_ChannelAddr(TestFile.test_item[item].p1-1, TestFile.test_item[item].p2-1); //state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
		if(res == 0) 			break;
		else if(res == 4) cnt_error++;
		else							return 2;
	}
	if(cnt_error >= 10) return 2;
	
	//打开激励源
	AD_Update_SourceAddr(__WireTest_WZK_Mode_Source[WT_Config.Mode_TestW], AD_SourceAddr_GND);
	AD_TestRDC_RD();
	
	// delay
	//CLK_Delay((168/3)*10);	//10us
	if(WT_Config.Mode_TestCap == 0) ad_delay = 56*200;
	else if(WT_Config.Mode_TestCap == 1) ad_delay = 56*400;
	else if(WT_Config.Mode_TestCap == 2) ad_delay = 56*600;
	CLK_Delay(ad_delay);	//10us
	
	//读取输入信号
	*result = AD_InputState & __WireTest_WZK_Mode_Signal[WT_Config.Mode_TestW];
	for(i=0;i<50;i++)
	{
		CLK_Delay(56);//1us
		tmp = AD_InputState & __WireTest_WZK_Mode_Signal[WT_Config.Mode_TestW];
		if(tmp != *result)//AD值不稳定，测试不通过。
		return 1;
	}
	
	if(tmp == *result)
	{
		if(tmp == 0)	//0：小于20欧姆，1：大于20欧姆
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else return 1;
}

/**
  * @brief  线束测试 - 高阻
  * @param  item, result value
  * @retval 0:ok, 1:value error, 2:test error,
  */
uint8_t AD_TestItem_Z(uint32_t item, uint32_t* result)
{
	uint8_t res;
	uint8_t cnt_error = 0;
	uint16_t ad_delay= 40;
	
	//关闭激励源
//	AD_Update_SourceAddr(AD_SourceAddr_GNDR, AD_SourceAddr_GNDR);
	
	//切换通道
	while(cnt_error < 10)
	{
		res = PortBoard_Update_ChannelAddr(TestFile.test_item[item].p1-1, TestFile.test_item[item].p2-1); //state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
		if(res == 0) 			break;
		else if(res == 4) cnt_error++;
		else							return 2;
	}
	if(cnt_error >= 10) return 2;
	
	//打开激励源
	AD_Update_SourceAddr(__WireTest_WZK_Mode_Source[WT_Config.Mode_TestZ], AD_SourceAddr_GND);
	AD_TestRDC_RD();
	
	// delay
	if(WT_Config.Mode_TestCap == 0) ad_delay = 56*40;
	else if(WT_Config.Mode_TestCap == 1) ad_delay = 56*300;
	else if(WT_Config.Mode_TestCap == 2) ad_delay = 56*400;
	CLK_Delay(ad_delay);	//10us
	//osDelay(1);//延时1ms, 等待AD值稳定，20150520
	
	//读取输入信号
	*result = AD_InputState & __WireTest_WZK_Mode_Signal[WT_Config.Mode_TestZ];
	if(*result == 0) return 1;
	if((AD_InputState & __WireTest_WZK_Mode_Signal[WT_Config.Mode_TestZ]) != 0)	//0：短路，1：高阻
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
  * @brief  线束测试 - 开关
  * @param  item, result value
  * @retval 0:ok, 1:value error, 2:test error,
  */
#define TimeoutWaitSwitchTest		(100 * 6)	// 6S

uint8_t AD_TestItem_Switch(uint32_t item, uint32_t* result)
{
	uint8_t res, state;
	uint32_t buf32;
	uint8_t cnt_error = 0;
	
	//关闭激励源
	AD_Update_SourceAddr(AD_SourceAddr_GNDR, AD_SourceAddr_GNDR);
	
	//切换通道
	while(cnt_error < 10)
	{
		res = PortBoard_Update_ChannelAddr(TestFile.test_item[item].p1-1, TestFile.test_item[item].p2-1); //state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
		if(res == 0) 			break;
		else if(res == 4) cnt_error++;
		else							return 2;
	}
	if(cnt_error >= 10) return 2;
	AD_TestRDC_RD();
	
	//打开激励源
	AD_Update_SourceAddr(__WireTest_WZK_Mode_Source[WT_Config.Mode_TestK], AD_SourceAddr_GND);
	
	// delay
	osDelay(cnt_delay_test); 
	
	//读取输入信号
	state = AD_InputState & __WireTest_WZK_Mode_Signal[WT_Config.Mode_TestK];
	buf32 = 0;
	while(buf32 < TimeoutWaitSwitchTest)
	{
		if(state != (AD_InputState & __WireTest_WZK_Mode_Signal[WT_Config.Mode_TestK])) //ok
		{
			*result = AD_InputState & __WireTest_WZK_Mode_Signal[WT_Config.Mode_TestK];
			return 0;
		}
		
		// wait for timeout
		osDelay(10); 
		buf32++;
	}
	
	//return timeout
	*result = AD_InputState & __WireTest_WZK_Mode_Signal[WT_Config.Mode_TestK];
	return 1;
}

/**
  * @brief  线束测试 - 找点
  * @param  item, result value
  * @retval 0:connet test point, 1:not detect, 2:test error,
  */
uint8_t AD_TestItem_FindPoint(uint16_t PortNumber)
{
	uint8_t res;
	uint8_t cnt_error = 0;
	
	//关闭激励源
	AD_Update_SourceAddr(AD_SourceAddr_GNDR, AD_SourceAddr_GNDR);
	
	//切换通道
	while(cnt_error < 10)
	{
		res = PortBoard_Update_ChannelAddr(PortNumber, AD_ChannelAddr_NULL); //state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
		if(res == 0) 			break;
		else if(res == 4) cnt_error++;
		else							return 2;
	}
	if(cnt_error >= 10) return 2;
	
	//打开激励源
	AD_Update_SourceAddr(AD_SourceAddr_U45V, AD_SourceAddr_GNDR);
	AD_TestRDC_RD();
	
	// delay
	CLK_Delay((168/3)*10);	//10us
	
	//读取输入信号
	if((AD_InputState & AD_InputV10) == 0)	//连接探针
	{
		return 0;
	}
	else	//悬空
	{
		return 1;
	}
}

/**
  * @brief  线束测试 - 电阻
  * @param  item, result value
  * @retval 0:ok, 1:value error, 2:test error,
  */
uint8_t AD_TestItem_Res(uint32_t item, uint32_t* result)
{
	uint8_t res;
	uint16_t buf16;
//	uint8_t  buf8;
//	uint8_t  mode;
	uint32_t VoltageValue;
	float    ResValue, votage;
//	double   tempad;
	uint8_t  cnt_error = 0;
	
	//关闭激励源
	AD_Update_SourceAddr(AD_SourceAddr_GNDR, AD_SourceAddr_GNDR);
	
	//切换通道
	while(cnt_error < 10)
	{
		res = PortBoard_Update_ChannelAddr(TestFile.test_item[item].p1-1, TestFile.test_item[item].p2-1); //state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
		if(res == 0) 			break;
		else if(res == 4) cnt_error++;
		else							return 2;
	}
	if(cnt_error >= 10) return 2;
	AD_TestRDC_RD();
	
	//打开激励源
	AD_Update_SourceAddr(AD_SourceAddr_I100uA, AD_SourceAddr_GND);
	osDelay(3);//延时3ms 	
		
	//读取输入信号
	*result = AD_InputState & AD_InputV10;
	if(*result !=0)	//档位为1.0V ,电阻值最大为23K
	{
		return 1;
	}
	else //小于23K
	{
		osDelay(30);//延时30ms 	
		//读取AD值
		res = AD_ReadValue(10*1000, &buf16);
		if(res != 0) return 2;

		VoltageValue = AD_GetVoltage_AutoStep(buf16, (uint8_t)(AD_InputState >> 1));
		votage = VoltageValue / (AD_Voltage_MUL * 1.0f);
		ResValue = 1000000 * votage / (49.33333333f * (0.618f - votage) + 61.5f);		
	}
	
	
	//DC 4.5V, zmx, cndz, 20150519，MBoard = V2.1
	//1K  /1.609V  /1773  320
 //1.5K /2.027V  /2279
	//2K  /2.333V  /2646  464
	//3K  /2.747V  /3146  546
	//4K  /3.014V  /3462  599
	//5K  /3.201V  /3689  636
	//6K  /3.339V  /3855  664
	//7K  /3.444V  /3981  685
	//8K  /3.528V  /4080  701
	
	//result
	*result = ResValue;
	
	if((ResValue < TestFile.test_item[item].param2) || (ResValue > TestFile.test_item[item].param3))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
  * @brief  线束测试 - 二极管
  * @param  item, result value
  * @retval 0:ok, 1:value error, 2:test error,
  */
uint8_t AD_TestItem_Diode(uint32_t item, uint32_t* result)
{
	uint8_t res, IndexSmall, IndexBig;
	uint16_t buf16;
	uint32_t VoltageValue[2];
	uint8_t cnt_error = 0;
	
	//关闭激励源
	AD_Update_SourceAddr(AD_SourceAddr_GNDR, AD_SourceAddr_GNDR);
	
	//切换通道
	while(cnt_error < 10)
	{
		res = PortBoard_Update_ChannelAddr(TestFile.test_item[item].p1-1, TestFile.test_item[item].p2-1); //state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
		if(res == 0) 			break;
		else if(res == 4) cnt_error++;
		else							return 2;
	}
	if(cnt_error >= 10) return 2;
	AD_TestRDC_RD();
	
	//打开激励源
	AD_Update_SourceAddr(AD_SourceAddr_U45V, AD_SourceAddr_GND);
	osDelay(cnt_delay_test); 
	
	//读取AD值 - 粗测电阻
	res = AD_ReadValue(10*1000, &buf16);
	if(res != 0) return 2;
	VoltageValue[0] = AD_GetVoltage_AutoStep(buf16, (uint8_t)(AD_InputState >> 1)) / (AD_Voltage_MUL / 100);
	
	//打开激励源
	AD_Update_SourceAddr(AD_SourceAddr_GND, AD_SourceAddr_U45V);
	osDelay(cnt_delay_test); 
	
	//读取AD值 - 粗测电阻
	res = AD_ReadValue(10*1000, &buf16);
	if(res != 0) return 2;
	VoltageValue[1] = AD_GetVoltage_AutoStep(buf16, (uint8_t)(AD_InputState >> 1)) / (AD_Voltage_MUL / 100);
	
	//get index of value
	if(VoltageValue[0] < VoltageValue[1])
	{
		IndexSmall = 0;	//Index of small value
		IndexBig = 1;		//Index of big value
	}
	else
	{
		IndexSmall = 1;	//Index of small value
		IndexBig = 0;		//Index of big value
	}
	*result = VoltageValue[IndexSmall];
	
	// process the result
	if((VoltageValue[IndexBig] - VoltageValue[IndexSmall]) < 50) //正反向压降小于0.5V
	{
		return 1;
	}	
	
	// A K point
	if((VoltageValue[IndexSmall] < (TestFile.test_item[item].param2*100)) || (VoltageValue[IndexSmall] > (TestFile.test_item[item].param3*100)))
	{
		return 1;		
	}
	else
	{
		if(VoltageValue[IndexSmall] < 100)	return 0;	//Normal//正向压降小于1V 
		else																return 0;	//LED
	}
}

/**
  * @brief  线束测试 - 电容
  * @param  item, result value
  * @retval 0:ok, 1:value error, 2:test error,
  */
#define Times_CapADC			300

uint8_t AD_TestItem_Cap(uint32_t item, uint32_t* result)
{
	uint8_t res;
	uint16_t i;
	uint16_t buf16;
	uint16_t VoltageValue;
	uint16_t VoltageBuffer[Times_CapADC];
	float    ValueCAP;
	uint8_t cnt_error = 0;
	
	//关闭激励源
	AD_Update_SourceAddr(AD_SourceAddr_GNDR, AD_SourceAddr_GNDR);
	
	//切换通道
	while(cnt_error < 10)
	{
		res = PortBoard_Update_ChannelAddr(TestFile.test_item[item].p1-1, TestFile.test_item[item].p2-1); //state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
		if(res == 0) 			break;
		else if(res == 4) cnt_error++;
		else							return 2;
	}
	if(cnt_error >= 10) return 2;
	AD_TestRDC_C();
	
	//打开激励源
	AD_Update_SourceAddr(AD_SourceAddr_W400Hz, AD_SourceAddr_GND);
	osDelay(cnt_delay_test_cap);	//4.7uF

	//读取AD值
	for(i=0;i<Times_CapADC;i++)
	{
		res = AD_ReadValue(10*1000, &buf16);
		if(res != 0) return 2;
		VoltageBuffer[i] = AD_GetVoltage_AutoStep(buf16, (uint8_t)(AD_InputState >> 1)) /(AD_Voltage_MUL / 1000);
		osDelay(1);
	}
	VoltageValue = Voltage_GetValue_FromBuffer(VoltageBuffer, Times_CapADC);
	
	//result
	ValueCAP = AD_GetCAP_FromDCU4V5(VoltageValue);	// VoltageValue 放大1000倍
	*result = ValueCAP;
	if((ValueCAP < TestFile.test_item[item].param2) || (ValueCAP > TestFile.test_item[item].param3))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/**
  * @brief  执行一次线束测试
* @param  mode, 0:test, 1:wait for wire remove
	* @retval 0:ok, 1:file errror, 2:test error, 3-user cancel; 
  */
static uint8_t WireTest_OneTime(uint32_t* cnt_error_total, uint32_t* cnt_error_z)
{
	uint8_t  res = 0;
	uint32_t buf32;
	uint32_t cnt_item_error = 0;
	uint32_t cnt_item_Z_error = 0;
	uint64_t i;
	
	//检测取消按钮
	if(TestFile.command == 2) return 3;	//0-no operate, 1-start, 2-cancel
	if(TestFile.item_total == 0) return 1;
	
	for(i=0; i<TestFile.item_total; i++)
	{
		//检测取消按钮
		if(TestFile.command == 2) return 3;	//0-no operate, 1-start, 2-cancel
		
		if(WT_Config.TestRule == 1)//0-循环检测  1-单次检测   wujun added 2015.4.20
		{
			if(TestFile.test_item[i].result == 1)
			{
				if((TestFile.test_item[i].type == 'W') || (TestFile.test_item[i].type == 'R')) continue;
			}
		}
		
		//检测ID
		TestFile.item_Index = i + 1;
		if(TestFile.test_item[i].id != TestFile.item_Index) return 1;
		if(TestFile.test_ErrFlag == 0)	//for max error ID，0-no error, 1-error detect and retest
		{
			TestFile.item_current = TestFile.test_item[i].id;
		}
		else
		{
			if(TestFile.test_item[i].id > TestFile.item_current)	//错误通过测试
			{
				TestFile.item_current = TestFile.test_item[i].id;
				TestFile.test_ErrFlag = 0; //for max error ID，0-no error, 1-error detect and retest
			}
		}

		//测试类型
		switch(TestFile.test_item[i].type)
		{
			case 'W':	//导通
				//切换通道
				res = AD_TestItem_Wire(i, &buf32);	//0:ok, 1:value error, 2:test error,  
				
				// 处理测试结果
				if(res == 0)
				{
					TestFile.test_item[i].result = 1; //0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
				}
				else if(res == 1)
				{
					TestFile.test_item[i].result = 2;
					cnt_item_error++; //return 2;
				}
				else return 2;
				
				break;
			
			case 'K':	//开关
				//切换通道
				res = AD_TestItem_Switch(i, &buf32);	//0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
				
				// 处理测试结果
				if(res == 0)
				{
					TestFile.test_item[i].result = 1;
				}
				else if(res == 1)
				{
					TestFile.test_item[i].result = 2;
					cnt_item_error++; //return 2;
				}
				else return 2;
				break;
				
			case 'Z':	//高阻
				res = AD_TestItem_Z(i, &buf32);	//0:ok, 1:value error, 2:test error,3:signal not stable
				// 处理测试结果
				if(res == 0)
				{
					TestFile.test_item[i].result = 1;//0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
				}
				else if(res == 1)
				{
					TestFile.test_item[i].result = 4;
					cnt_item_error++; //return 2;
					cnt_item_Z_error++;
				}
				else return 2;
				break;
			
			case 'R':	//电阻
				res = AD_TestItem_Res(i, &buf32);	//0:ok, 1:value error, 2:test error,
				
				// 处理测试结果
				if(res == 0)
				{
					TestFile.test_item[i].result = 1;
				}
				else if(res == 1)
				{
					TestFile.test_item[i].result = 2;
					cnt_item_error++; //return 2;
				}
				else return 2;
				break;
			
			case 'D':	//二极管
				res = AD_TestItem_Diode(i, &buf32);	//0:ok, 1:value error, 2:test error,
				
				// 处理测试结果
				if(res == 0)
				{
					TestFile.test_item[i].result = 1;
				}
				else if(res == 1)
				{
					TestFile.test_item[i].result = 2;
					cnt_item_error++; //return 2;
				}
				else return 2;
				break;
			
			case 'C':	//电容
				res = AD_TestItem_Cap(i, &buf32);	//0:ok, 1:value error, 2:test error,
				
				// 处理测试结果
				if(res == 0)
				{
					TestFile.test_item[i].result = 1;
				}
				else if(res == 1)
				{
					TestFile.test_item[i].result = 2;
					cnt_item_error++; //return 2;
				}
				else return 2;
				break;
			
			default:
				return 1;
				//break;
		}
	}
	
	Update_Loopinfo();
	
	*cnt_error_total = cnt_item_error;
	*cnt_error_z = cnt_item_Z_error;
	
	if(cnt_item_Z_error > 0) // z error
	{
		TestFile.task_status = 5;	//0-waiting, 1-testing, 2-test error, 3-test ok, 4-waiting remove wire, 5-testing & Z error
		//BSP_UartAudioNG_BeepOn(255); 	//0-off, 1- 1 beep, 2-2beep,3-3beep,255-on
		//osMessagePut(UartAudioEvent, UartAudioNG_TX_Event, 0);	//播放声音
		LED4_ON_R();
	}
	else // no z error
	{
		TestFile.task_status = 1;	//0-waiting, 1-testing, 2-test error, 3-test ok, 4-waiting remove wire, 5-testing & Z error
		//BSP_UartAudioOK_BeepOn(0); 	//0-off, 1- 1 beep, 2-2beep,3-3beep,255-on
		LED4_OFF();
		//Infra_PWM_OFF();//关闭红外信号
		UART_Infr_Switch = 1;
	}
	
	
	TestFile.item_error_count = cnt_item_error;
	//return ok
	if(cnt_item_error == 0) 
	{
		LED4_OFF();
		//Infra_PWM_OFF();//关闭红外信号
		UART_Infr_Switch = 1;
		//osMessagePut(UartAudioEvent, UartAudioSTOP_TX_Event, 0);	//停止播放声音
		return 0;
	}
	else										return 2;
}


/**
  * @brief  线束测试程序
  * @param  None
	* @retval 0:ok, 1:file errror, 2:test error, 3-user cancel; 
  */
uint8_t WireTest_Process(void)
{
	uint32_t i;
	uint8_t res;
	uint32_t cnt_error_total = 0;
	uint32_t cnt_error_z = 0;
	static uint8_t state_audio = 0;	//0-stop, 1-repeat play, 2- single play
	//uint32_t buf32[2]={0}; //for test only
	uint16_t wait_cnt = 0;
	
	// Init restult
	for(i=0; i<TestFile.item_total; i++)
		TestFile.test_item[i].result = 0;
	TestFile.test_ErrFlag = 0;	//for max error ID，0-no error, 1-error detect and retest

	while(1)
	{
		//检测线束导通
		//buf32[0] = HAL_GetTick();
		res = WireTest_OneTime(&cnt_error_total, &cnt_error_z);	//0:ok, 1:file errror, 2:test error, 3-user cancel; 
		//buf32[1] = HAL_GetTick() - buf32[0];
		if(state_audio == 0)	//0-stop, 1-repeat play, 2- single play
		{
			if(cnt_error_z > 0)
			{
				osMessagePut(UartAudioEvent, UartAudioNG_TX_Event, 0);	//播放声音
				//osDelay(50); 
				state_audio = 1;	//0-stop, 1-repeat play, 2- single play
			}
		}
		else if(state_audio == 1)	//0-stop, 1-repeat play, 2- single play
		{
			if(cnt_error_z == 0)
			{
				osMessagePut(UartAudioEvent, UartAudioSTOP_TX_Event, 0);	//停止声音
				//osDelay(50); 
				state_audio = 0;	//0-stop, 1-repeat play, 2- single play
			}
		}
				
		if(res == 2)	//
		{
			TestFile.test_ErrFlag = 1;	//for max error ID，0-no error, 1-error detect and retest
			if(TestFile.item_Index < TestFile.item_current)
			{
				for(i=TestFile.item_Index;i<TestFile.item_current;i++)
				{
					TestFile.test_item[i].result = 0;	//0:not test, 1:ok, 2:error
				}
			}
		}
		else
		{
			//BSP_UartAudioOK_BeepOn(0); 	//0-off, 1- 1 beep, 2-2beep,3-3beep,255-on
			LED4_OFF();
			//Infra_PWM_OFF();//关闭红外信号
			UART_Infr_Switch = 1;
			return res;
		}
		
		if(TestFile.cnt_okloop > 0 && TestFile.test_ErrFlag == 1) wait_cnt++;//有线束连接时才开始计数
		if(wait_cnt > __WireTest_Wait_Time[WT_Config.Test_Wait] )//等待n次，测试未通过，发送测试结果2
		{
			return 2;
		}
			
		// short dealy
		osDelay(100); 
	}
}

/**
  * @brief  执行一次线束测试，等待线束全部拿走
	* @param  None
	* @retval 0:all remove, 1:file errror, 2:not all remove, 3-user cancel; 
  */
static uint8_t WireTest_WaitRemove_OneTime(void)
{
	uint8_t  res = 0;
	uint32_t buf32;
	uint64_t i;
	uint32_t temp = 0;
	
	//检测取消按钮
	if(TestFile.command == 2) return 3;	//0-no operate, 1-start, 2-cancel
	if(TestFile.item_total == 0) return 1;
	
	for(i=0; i<TestFile.item_total; i++)
	{
		//检测取消按钮
		if(TestFile.command == 2) return 3;	//0-no operate, 1-start, 2-cancel
		
		//检测ID
		TestFile.item_Index = i + 1;
		if(TestFile.test_item[i].id != TestFile.item_Index) return 1;
		if(TestFile.test_ErrFlag == 0)	//for max error ID，0-no error, 1-error detect and retest
		{
			TestFile.item_current = TestFile.test_item[i].id;
		}
		else
		{
			if(TestFile.test_item[i].id > TestFile.item_current)	//错误通过测试
			{
				TestFile.item_current = TestFile.test_item[i].id;
				TestFile.test_ErrFlag = 0; //for max error ID，0-no error, 1-error detect and retest
			}
		}

		//测试类型
		switch(TestFile.test_item[i].type)
		{
			case 'W':	//导通
				//切换通道
			  if(TestFile.test_item[i].result == 0) break;	//0:not test, 1:ok, 2:error
				temp++;
			
				res = AD_TestItem_Wire(i, &buf32);	//0:ok, 1:value error, 2:test error,
				// 处理测试结果
				if(res == 0)	//W - ok
				{
				}
				else if(res == 1) //Z
				{
					TestFile.test_item[i].result = 0;	//0:not test, 1:ok, 2:error
				}
				else
				{
					return 2;
				}
				break;
				
//			case 'Z':	//高阻
//				res = AD_TestItem_Z(i, &buf32);	//0:ok, 1:value error, 2:test error,
//				// 处理测试结果
//				if(res != 1)
//				{
//					TestFile.test_item[i].result = 2;
//					return 2;					
//				}
//				TestFile.test_item[i].result = 0;
//				break;
				
//			case 'K':	//开关
//				//切换通道
//				res = AD_TestItem_Switch(i, &buf32);	
//				
//				// 处理测试结果
//				if(res != 0)
//				{
//					TestFile.test_item[i].result = 2;
//					return 2;					
//				}
//				TestFile.test_item[i].result = 1;
//				break;
				
			case 'R':	//电阻
				if(TestFile.test_item[i].result == 0) break;	//0:not test, 1:ok, 2:error
				temp++;
			
				res = AD_TestItem_Res(i, &buf32);		//0:ok, 1:value error, 2:test error,
				// 处理测试结果
				if(res == 0)	//R - ok
				{
				}
				else if(res == 1) //Z
				{
					TestFile.test_item[i].result = 0;	//0:not test, 1:ok, 2:error
				}
				else
				{
					return 2;
				}
				break;
//			
//			case 'D':	//二极管
//				res = AD_TestItem_Diode(i, &buf32);	
//				// 处理测试结果
//				if(res != 0)
//				{
//					TestFile.test_item[i].result = 2;
//					return 2;					
//				}
//				TestFile.test_item[i].result = 1;
//				break;
//			
			case 'C':	//电容
				res = AD_TestItem_Cap(i, &buf32);	
				// 处理测试结果
				if(res != 0)
				{
					TestFile.test_item[i].result = 2;
					return 2;					
				}
				TestFile.test_item[i].result = 1;
				break;
			
			default:
				//return 1;
				break;
		}
	}
	
	//return ok
	if(temp == 0)	return 0;
	else 					return 2;
}

/**
  * @brief  自动测试模式，等待线束拿走
  * @param  None
	* @retval 0:all remove, 1:file errror, 3-user cancel; 
  */
uint8_t WireTest_WaitForNext(void)				
{
	uint8_t res;
	while(1)
	{
		//检测线束导通
		res = WireTest_WaitRemove_OneTime();	//0:all remove, 1:file errror, 2:not all remove, 3-user cancel; 
		if(res != 2) 
		{
			return res;
		}
		osDelay(100);
	}
}


////自学习项目
//typedef struct{
//  uint64_t id;
//  uint8_t  type;
//	uint8_t  p1;
//	uint8_t  p2;
//	uint32_t  param1; 
//	uint32_t  param2;
//	uint32_t  param3;
//}
//WT_BSP_STUDY_ITEM;

////自学习文件
//typedef struct{
//	WT_BSP_STUDY_ITEM test_item[1000];
//	uint64_t item_total;
//	uint8_t  status;	//0:not init, 1:ok, 2:error
//	uint8_t  command;	//0-no operate, 1-start, 2-cancel
//}
//WT_StudyFileTypedef;  




/**
  * @brief  线束自学习
  * @param  j1, j2, result(0：小于20欧姆，1：大于20欧姆)
  * @retval 0:W, 1:Z, 2-study error,3:R
  */
#define CNT_DELAY_STUDY_ITEM		10000	//
uint8_t AD_StudyItem(uint16_t i, uint16_t j, uint32_t* result)
{
	uint8_t res;
	uint8_t cnt_error = 0;
	uint16_t buf16;
//	uint8_t  mode;
	uint32_t VoltageValue;
	float    ResValue, votage;
	//uint8_t  buf8;
	uint16_t cnt=0;
	uint32_t tmp=0;
	uint16_t cnt_w=0;
	uint16_t cnt_z=0;
	
	*result = 0;
	
	//关闭激励源
//	AD_Update_SourceAddr(AD_SourceAddr_GNDR, AD_SourceAddr_GNDR);
	AD_TestRDC_RD();
	
	//切换通道
	while(cnt_error < 10)
	{
		res = PortBoard_Update_ChannelAddr(i, j); //state：0-ok, 1-板卡不存在, 3-无效地址, 4-通信异常
		if(res == 0) 			break;
		else if(res == 4) cnt_error++;
		else							return 2;
	}
	if(cnt_error >= 10) return 2;
		
	//打开激励源
	if(WT_Config.StudyMode == 0)//自学习模式：导通 //**解决此处bug，原来参数为WT_Config.Mode_TestZ
	{
		AD_Update_SourceAddr(__WireTest_WZK_Mode_Source[WT_Config.Mode_TestW], AD_SourceAddr_GND);
		// delay
		CLK_Delay(168/3 * AD_DELAY);	//version2.0 50us, 20150312  version1.0--->150us
		
		//读取输入信号
		//*result = AD_InputState & (__WireTest_WZK_Mode_Signal[WT_Config.Mode_TestW]);
		
		cnt_error=0;
		while(cnt_error < 10)
		{
			for(cnt=0;cnt<50;cnt++)
			{
				CLK_Delay(56);//1us
				tmp = AD_InputState & __WireTest_WZK_Mode_Signal[WT_Config.Mode_TestW];
				if(tmp == 0) cnt_w++;
				else cnt_z++;
				
			}
			
			if((cnt_w > cnt_z)&&(cnt_w >= 48)) 			return 0; //0：小于37欧姆，1：大于37欧姆
			else if((cnt_z > cnt_w)&&(cnt_z >= 48)) return 1; //0：小于37欧姆，1：大于37欧姆
			else //AD值不稳定，尝试10次
			{
				cnt_error++;
			}
		}
		
		return 4; //0：小于37欧姆，1：大于37欧姆
//		
//			
//			if(cnt_w > cnt_z) *result = 0;
//			else *result = 1;
//			
//		if(*result == 0)	//0：小于37欧姆，1：大于37欧姆
//		{
//			return 0;
//		}
//		else //大于37
//		{
//			//打开激励源
//			AD_Update_SourceAddr(AD_SourceAddr_I100uA, AD_SourceAddr_GND);
//				
//			// delay
//			CLK_Delay(168/3*1000);	//100uA，至少0.6 MS, 20150520
//			
//			//读取输入信号
//			*result = AD_InputState & (__WireTest_WZK_Mode_Signal[WT_Config.Mode_TestW]);
//			if(*result !=0)	//档位为1.0V ,电阻值最大为23K
//			{
//				return 1;
//			}
//			else //小于23K
//			{
//			  return 0;			
//			}
//		}
	}
	//自学习模式：导通+电阻
	AD_Update_SourceAddr(AD_SourceAddr_U45V, AD_SourceAddr_GND);
		
	// delay
	CLK_Delay(168/3 * AD_DELAY);	//version2.0 10us, 20150312  version1.0--->150us
	
	//读取输入信号
	*result = AD_InputState & AD_InputV01;
	if(*result == 0)	//0：小于37欧姆，1：大于37欧姆
	{
		return 0;
	}
	else //大于37
	{
		//打开激励源
		AD_Update_SourceAddr(AD_SourceAddr_I100uA, AD_SourceAddr_GND);
			
		// delay
		CLK_Delay(168/3*1000);	//100uA，至少0.6 MS, 20150520
		
		//读取输入信号
		*result = AD_InputState & AD_InputV10;
		if(*result !=0)	//档位为1.0V ,电阻值最大为23K
		{
			return 1;
		}
		else //小于23K
		{
				osDelay(30);//延时30ms, 等待AD值稳定，20150520
			
				//读取AD值
				res = AD_ReadValue(10*1000, &buf16);
				if(res != 0) return 2;

				VoltageValue = AD_GetVoltage_AutoStep(buf16, (uint8_t)(AD_InputState >> 1));
				votage = VoltageValue / (AD_Voltage_MUL * 1.0f);
				ResValue = 1000000 * votage / (49.33333333f * (0.618f - votage) + 61.5f);
			
				//result
				*result = ResValue;
			  if(*result == 0) return 1;
				return 3; //小于50K,测量电阻
		
		}
	}
}

uint16_t get_index_total_bill(uint8_t m, uint8_t n)
{
	uint16_t NumOfPort = SelfCheckItem.port_board_number * 64;
 
	//return ((256 + (256 - m)) * (m+1) / 2 - 256 + (n - m) - 1);
	return ((NumOfPort + (NumOfPort - m)) * (m+1) / 2 - NumOfPort + (n - m) - 1);

}

/**
  * @brief  优化学习文件
  * @param  None
  * @retval result, 0:ok,
  */
uint8_t StudyFile_Optimize(void)
{
	uint16_t i, j, m, n;
	uint16_t PortState[256];
	uint8_t  cnt_line;
	uint8_t  PortBoardInfo[4][3];	//byte0:Point count, byte1:min Point, byte2:max Point,
	uint8_t  min_point, max_point;
	uint16_t index, index_Total;
	uint16_t port_pL0, port_pH0;
	uint16_t port_pL1, port_pH1;
	uint32_t total;
	
	//init
	total = StudyFile.item_total;
	
	//标记端口状态
	for(i=0;i<256;i++) PortState[i] = 0;
	for(i=0;i<StudyFile.item_total;i++)
	{
		if(StudyFile.study_item[i].type == 'W' || StudyFile.study_item[i].type == 'R')
		{
			PortState[StudyFile.study_item[i].p1 - 1] += 1;	// cnt
			PortState[StudyFile.study_item[i].p2 - 1] += 1;	// cnt
		}
	}
	
	//计算点位数量
	//标记板卡状态
	for(i=0;i<4;i++) {for(j=0;j<3;j--){PortBoardInfo[i][j] = 0;}}//byte0:Point count, byte1:min Point, byte2:max Point,
	for(i=0;i<4;i++)	//board 1-4
	{
		min_point = 63;
		max_point = 0;
		for(j=0;j<64;j++)	//port 1-64
		{
			if(PortState[i*64 + j] != 0)
			{
				PortBoardInfo[i][0]++;
				if(j < min_point) min_point = j;
				if(j > max_point) max_point = j;
			}
		}
		PortBoardInfo[i][1] = min_point;
		PortBoardInfo[i][2] = max_point;
	}
	StudyFile.number_point = PortBoardInfo[0][0]+PortBoardInfo[1][0]+PortBoardInfo[2][0]+PortBoardInfo[3][0];
	
	//计算回路数量
	cnt_line  = 0;
	for(i=0;i<StudyFile.item_total;i++)
	{
		if(StudyFile.study_item[i].type == 'W' || StudyFile.study_item[i].type == 'R')
		{
			if((PortState[StudyFile.study_item[i].p1 - 1] == 1) && (PortState[StudyFile.study_item[i].p2 - 1] == 1) )
			{
				cnt_line++;
			}
			else if((PortState[StudyFile.study_item[i].p1 - 1] > 1) && (PortState[StudyFile.study_item[i].p2 - 1] == 1))
			{
				PortState[StudyFile.study_item[i].p1 - 1]--;
			}
			else if((PortState[StudyFile.study_item[i].p1 - 1] == 1) && (PortState[StudyFile.study_item[i].p2 - 1] > 1))
			{
				PortState[StudyFile.study_item[i].p2 - 1]--;
			}
			else if((PortState[StudyFile.study_item[i].p1 - 1] > 1) && (PortState[StudyFile.study_item[i].p2 - 1] > 1))
			{
				PortState[StudyFile.study_item[i].p1 - 1]--;
				PortState[StudyFile.study_item[i].p2 - 1]--;
			}
			else continue;
		}
	}
	StudyFile.number_line = cnt_line;
	
	//--------------------------------------------------------------------------------------------------------------------
	//修改最大最小点位（优化模式、完整模式）
	//--------------------------------------------------------------------------------------------------------------------
	if(WT_Config.StudyOptimizingMode == 0) //0:full test, 1:only test Z bettwn max & min W point,
	{
		//完整模式
		for(i=0;i<4;i++)
		{
			if(PortBoardInfo[i][0] > 0) //find point
			{
				PortBoardInfo[i][1] = 0;	//min
				PortBoardInfo[i][2] = 63;	//max
			}
		}
	}
		
	//=========================================================
	/* step 3: 整理学习文件 */
	//=========================================================
	index = 0;
	for(i=0;i<4;i++) //first board
	{
		if(PortBoardInfo[i][0] != 0)	//first board - point
		{
			port_pL0 = i*64 + PortBoardInfo[i][1];
			port_pH0 = i*64 + PortBoardInfo[i][2];
			for(m=port_pL0;m<=port_pH0;m++) //First point
			{
				for(j=i;j<4;j++) // second board
				{
					if(PortBoardInfo[j][0] != 0)	//second board - point
					{
						port_pL1 = j*64 + PortBoardInfo[j][1];
						port_pH1 = j*64 + PortBoardInfo[j][2];
						for(n=port_pL1;n<=port_pH1;n++) //second point
						{
							if(n > m) //不是同一点
							{
								index_Total = get_index_total_bill(m,n);
								//copy to new pointer
								StudyFile.study_item[index] = StudyFile.study_item[index_Total];
								StudyFile.study_item[index].id = index + 1;
								index++;	
							} //end if(m != n) //不是同一点
						} //end for(n=port_pL1;n<=port_pH1;n++) //second point
					} //end if(PortBoardInfo[j][0] != 0)	//second board - point
				} //for(j=i;j<4;j++) // second board
			} //end for(m=port_pL0;m<=port_pH0;m++) //First point
		} //end if(PortBoardInfo[i][0] != 0)	//first board - point
	} //end for(i=0;i<4;i++) //first board
	StudyFile.item_total = index;
	
	//=========================================================
	/* step 4: 清除缓冲区 */
	//=========================================================
	for(;index<total;index++)
	{
		StudyFile.study_item[index].id = 0;
		StudyFile.study_item[index].p1 = 0;
		StudyFile.study_item[index].p2 = 0;
		StudyFile.study_item[index].param1 = 0;
		StudyFile.study_item[index].param2 = 0;
		StudyFile.study_item[index].param3 = 0;
		StudyFile.study_item[index].type = 0;
	}

	StudyFile.item_index = 0;
	//return
	return 0;
}


/**
  * @brief  线束自学习程序
  * @param  None
* @retval 0:ok, 1:study error, 2:user cancel; 3：R
  */
uint8_t WireStudy_Process(uint16_t 	NumStudyPort)	//0:ok, 1:file errror, 2:test error, 3-user cancel; 
{
	uint8_t  res;
	uint16_t i, j;
	uint32_t buf32 = 0;
	
	//init
	StudyFile.item_total = 0;
	StudyFile.item_index = 0;
	StudyFile.max_point = 0;
	StudyFile.min_point = 0;
	
	for(i=0;i<NumStudyPort-1;i++)
	{
		for(j=i+1;j<NumStudyPort;j++)
		{
			// user cancle
			if(StudyFile.command == 2) return 2; //0-no operate, 1-start, 2-cancel
			
			//test
			res = AD_StudyItem(i, j, &buf32);	//0:W, 1:Z, 2-study error,3：R
			if(res == 0)	//W
			{
				StudyFile.study_item[StudyFile.item_index].type = 'W';
			}
			else if(res == 1)	//Z
			{
				StudyFile.study_item[StudyFile.item_index].type = 'Z';
			}
			else if(res == 3)	//R
			{
				StudyFile.study_item[StudyFile.item_index].type = 'R';
			}
			else if(res == 4)	//signal not stable
			{
				return 1; //study error
			}
			else	//error
			{
				return 2;
			}
			
			//update result
			StudyFile.study_item[StudyFile.item_index].p1 = i+1;
			StudyFile.study_item[StudyFile.item_index].p2 = j+1;
			StudyFile.study_item[StudyFile.item_index].id = StudyFile.item_index+1;
			StudyFile.study_item[StudyFile.item_index].param1 = buf32;
			if(res == 3)
			{
				StudyFile.study_item[StudyFile.item_index].param2 = buf32*0.5;
				StudyFile.study_item[StudyFile.item_index].param3 = buf32*1.5;
			}
			else
			{
				StudyFile.study_item[StudyFile.item_index].param2 = 0;
				StudyFile.study_item[StudyFile.item_index].param3 = 0;
			}
			StudyFile.item_index++;
		}		
	}

	StudyFile.item_total = StudyFile.item_index;
	return 0;
}

/**
  * @brief  执行一次找点扫描
	* @param  None
  * @retval 0:ok, 1:error, 2-user cancel
  */
static uint8_t FindPoint_OneTime(uint16_t PortNumber)
{
	uint8_t  res = 0;
	uint16_t i;
	uint8_t  buf8[256] = {0};
	uint16_t cnt = 0;
	static uint16_t cnt_last = 0;
	static uint8_t audio_stat = 0; //0-未播放  1-已播放
	
	
	//检测取消按钮
	if(FindPoint.command == 2) return 2;	//0-no operate, 1-start, 2-cancel
	if((PortNumber <= 0) || (PortNumber > 256*4)) return 1;
	for(i=0; i<PortNumber; i++)
	{
		//检测取消按钮
		if(FindPoint.command == 2) return 2;	//0-no operate, 1-start, 2-cancel
		
		res = AD_TestItem_FindPoint(i);	//0:connet test point, 1:not detect, 2:test error,
		// 处理测试结果
		if(res == 0)	//0:connet test point
		{
			buf8[cnt] = i;
			cnt++;
		}
		else if(res == 1) //1:not detect
		{
		}
		else
		{
			return 1;
		}
	}

	if(cnt == cnt_last)
	{
		// beep on/off
//		if(cnt > 0) BSP_UartAudioFind_BeepOn(255); 	//0-off, 1- 1 beep, 2-2beep,3-3beep,255-on
//		else				BSP_UartAudioFind_BeepOn(0); 		//0-off, 1- 1 beep, 2-2beep,3-3beep,255-on
		//
		//update data
		FindPoint.task_status = 2; //0-waiting, 1-finding, 2-finding & update data	
		FindPoint.Items_count = cnt;
		for(i=0;i<cnt;i++) 	FindPoint.Items_Point[i]= buf8[i];
		for(;i<256;i++)			FindPoint.Items_Point[i]= 0;
		FindPoint.task_status = 1; //0-waiting, 1-finding, 2-finding & update data	
	}
	cnt_last = cnt;
	if(cnt > 0) 
	{
		if(audio_stat == 0)
		{
			osMessagePut(UartAudioEvent, UartAudioOK_REPEAT_TX_Event, 0);
			audio_stat = 1;
		}
	}
	else 
	{
		if(audio_stat == 1)
		{
			osMessagePut(UartAudioEvent, UartAudioSTOP_TX_Event, 0);
			audio_stat = 0;
		}
	}
	return 0;
}


/**
  * @brief  WireFindPoint_Process
  * @param  None
  * @retval 0-ok, 1-error, 2-user cancel
  */
uint8_t WireFindPoint_Process(uint16_t PortNumber)
{
	uint8_t res;
	uint32_t i;

	FindPoint.task_status = 1;	//0-waiting, 1-finding
	FindPoint.Items_count = 0;
	for(i=0;i<sizeof(FindPoint.Items_Point);i++) FindPoint.Items_Point[i] = 0;

	BSP_AD_ChangeMode(1);	//0-Normal Mode, 1-FindPoint Mode;
	while(1)
	{
		res = FindPoint_OneTime(PortNumber);	//0:ok, 1:error, 2-user cancel
		if(res != 0)	//
		{
			//BSP_UartAudioFind_BeepOn(0); //0-off, 1- 1 beep, 2-2beep,3-3beep,255-on
			BSP_AD_ChangeMode(0);	//0-Normal Mode, 1-FindPoint Mode;
			return res;
		}
		osDelay(100);
	}

}

/**
  * @brief  更新回路信息
  * @param  None
  * @retval 0:ok,
  */
uint8_t Update_Loopinfo(void)
{
	uint16_t i;
	uint16_t cnt_line;
	uint32_t total;
	uint32_t index;
	uint16_t PortState[256];

	total = TestFile.item_total;
	index = total;
	//标记端口状态
	for(i=0;i<256;i++)
	{
		PortState[i] = 0;
	}
	while(index)
	{
		index--;
		if(TestFile.test_item[index].type == 'W' || TestFile.test_item[index].type == 'R')
		{
			PortState[TestFile.test_item[index].p1 - 1] += 1;	// cnt
			PortState[TestFile.test_item[index].p2 - 1] += 1;	// cnt
		}
	}
	
	//计算回路数量
	cnt_line  = 0;
	
	for(i=0;i<total;i++)
	{
		if(TestFile.test_item[i].type == 'W' || TestFile.test_item[i].type == 'R')
		{
			if((PortState[TestFile.test_item[i].p1 - 1] == 1) && (PortState[TestFile.test_item[i].p2 - 1] == 1) )
			{
				if(TestFile.test_item[i].result == 1) //0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
				{
					cnt_line++;
				}
			}
			else if((PortState[TestFile.test_item[i].p1 - 1] > 1) && (PortState[TestFile.test_item[i].p2 - 1] == 1))
			{
				if(TestFile.test_item[i].result == 1) //0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
				{
					PortState[TestFile.test_item[i].p1 - 1] = PortState[TestFile.test_item[i].p1 - 1] -1;
				}
			}
			else if((PortState[TestFile.test_item[i].p1 - 1] == 1) && (PortState[TestFile.test_item[i].p2 - 1] > 1))
			{
				if(TestFile.test_item[i].result == 1) //0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
				{
					PortState[TestFile.test_item[i].p2 - 1] = PortState[TestFile.test_item[i].p2 - 1] -1;
				}
			}
			else if((PortState[TestFile.test_item[i].p1 - 1] > 1) && (PortState[TestFile.test_item[i].p2 - 1] > 1))
			{
				if(TestFile.test_item[i].result == 1) //0:not test, 1:ok, 2:open circuit error 3:dislocation error 4:Z error
				{
					PortState[TestFile.test_item[i].p1 - 1] = PortState[TestFile.test_item[i].p1 - 1] -1;
					PortState[TestFile.test_item[i].p2 - 1] = PortState[TestFile.test_item[i].p2 - 1] -1;
				}
			}
			else continue;
		}
	}
	TestFile.cnt_okloop = cnt_line;
	return 0;
}
