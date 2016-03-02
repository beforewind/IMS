/**
  ******************************************************************************
  * @file    stm32f429i_WireTester.c
  * @author  MCD Application Team
  * @version V2.1.1
  * @date    10-December-2014
  * @brief   This file provides set of firmware functions to manage Leds and
  *          push-button available on STM32F429I-Discovery Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */  
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f429i_WireTester.h"

/** @addtogroup STM32F429I_DISCOVERY
  * @{
  */
      
/** @defgroup STM32F429I_DISCOVERY_LOW_LEVEL 
  * @brief This file provides set of firmware functions to manage Leds and push-button
  *        available on STM32F429I-Discovery Kit from STMicroelectronics.
  * @{
  */ 

/** @defgroup STM32F429I_DISCOVERY_LOW_LEVEL_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32F429I_DISCOVERY_LOW_LEVEL_Private_Defines
  * @{
  */ 
  
  /**
  * @brief STM32F429I DISCO BSP Driver version number V2.1.0
  */
#define __STM32F429I_DISCO_BSP_VERSION_MAIN   (0x02) /*!< [31:24] main version */
#define __STM32F429I_DISCO_BSP_VERSION_SUB1   (0x01) /*!< [23:16] sub1 version */
#define __STM32F429I_DISCO_BSP_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32F429I_DISCO_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */ 
#define __STM32F429I_DISCO_BSP_VERSION        ((__STM32F429I_DISCO_BSP_VERSION_MAIN << 24)\
                                             |(__STM32F429I_DISCO_BSP_VERSION_SUB1 << 16)\
                                             |(__STM32F429I_DISCO_BSP_VERSION_SUB2 << 8 )\
                                             |(__STM32F429I_DISCO_BSP_VERSION_RC)) 
/**
  * @}
  */ 

/** @defgroup STM32F429I_DISCOVERY_LOW_LEVEL_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32F429I_DISCOVERY_LOW_LEVEL_Private_Variables
  * @{
  */ 

uint32_t I2cxTimeout = I2Cx_TIMEOUT_MAX; /*<! Value of Timeout when I2C communication fails */  
uint32_t SpixTimeout = SPIx_TIMEOUT_MAX; /*<! Value of Timeout when SPI communication fails */  

I2C_HandleTypeDef I2cHandle;
static SPI_HandleTypeDef SpiHandle;
static uint8_t Is_LCD_IO_Initialized = 0;

/**
  * @}
  */ 

/** @defgroup STM32F429I_DISCOVERY LOW_LEVEL_Private_FunctionPrototypes
  * @{
  */ 
static void               	I2Cx_Init(void);
//static void               I2Cx_ITConfig(void);
//static void               I2Cx_WriteData(uint8_t Addr, uint8_t Reg, uint8_t Value);
static HAL_StatusTypeDef    I2Cx_WriteBuffer(uint8_t Addr, uint8_t Reg,  uint8_t *pBuffer, uint16_t Length);
//static uint8_t            I2Cx_ReadData(uint8_t Addr, uint8_t Reg);
static HAL_StatusTypeDef    I2Cx_ReadBuffer(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length);
static void               	I2Cx_Error (void);
static void               	I2Cx_MspInit(I2C_HandleTypeDef *hi2c);  
//static HAL_StatusTypeDef  I2Cx_WriteBufferDMA(uint8_t Addr, uint16_t Reg,  uint8_t *pBuffer, uint16_t Length);
//static HAL_StatusTypeDef  I2Cx_ReadBufferDMA(uint8_t Addr, uint16_t Reg, uint8_t *pBuffer, uint16_t Length);
static HAL_StatusTypeDef  	I2Cx_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);


static void               	SPIx_Init(void);
//static void               SPIx_Write(uint16_t Value);
//static uint32_t           SPIx_Read(uint8_t ReadSize);
static uint8_t            	SPIx_WriteRead(uint8_t Byte);
static void               	SPIx_Error (void);
static void               	SPIx_MspInit(SPI_HandleTypeDef *hspi);

/*Link function for LCD peripheral */
void                      	LCD_IO_Init(void);
//void                      LCD_IO_WriteData(uint16_t RegValue);
//void                      LCD_IO_WriteReg(uint8_t Reg);
//uint32_t                  LCD_IO_ReadData(uint16_t RegValue, uint8_t ReadSize);
//void                      LCD_Delay (uint32_t delay);



/* IOExpander IO functions */
//void                      IOE_Init(void);
//void                      IOE_ITConfig (void);
//void                      IOE_Delay(uint32_t Delay);
//void                      IOE_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
//uint8_t                   IOE_Read(uint8_t Addr, uint8_t Reg);
//uint16_t                  IOE_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length);
//void                      IOE_WriteMultiple (uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length);

///* Link function for GYRO peripheral */
//void                      GYRO_IO_Init(void);
//void                      GYRO_IO_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
//void                      GYRO_IO_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

/* Link function for I2C EEPROM peripheral */
void                      EEPROM_IO_Init(void);
HAL_StatusTypeDef         EEPROM_IO_WriteData(uint16_t DevAddress, uint32_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
HAL_StatusTypeDef         EEPROM_IO_ReadData(uint16_t DevAddress, uint32_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
HAL_StatusTypeDef         EEPROM_IO_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);

/**
  * @}
  */ 


/** @defgroup STM32F429I_DISCOVERY LOW_LEVEL_Private_Functions
  * @{
  */ 

/**
  * @brief  This method returns the STM32F429I DISCO BSP Driver revision
  * @param  None
  * @retval version : 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t BSP_GetVersion(void)
{
  return __STM32F429I_DISCO_BSP_VERSION;
}

/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter should be: BUTTON_KEY
  * @param  Button_Mode: Specifies Button mode.
  *   This parameter can be one of following parameters:   
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO 
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                            generation capability  
  * @retval None
  */
//void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
//{
//  GPIO_InitTypeDef GPIO_InitStruct;

//  /* Enable the BUTTON Clock */
//  BUTTONx_GPIO_CLK_ENABLE(Button);
//  __SYSCFG_CLK_ENABLE();

//  if (ButtonMode == BUTTON_MODE_GPIO)
//  {
//  /* Configure Button pin as input */
//    GPIO_InitStruct.Pin = BUTTON_PIN[Button];
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
//    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);
//  }

//  if (ButtonMode == BUTTON_MODE_EXTI)
//  {
//     /* Configure Button pin as input with External interrupt */
//    GPIO_InitStruct.Pin = BUTTON_PIN[Button];
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; 
//    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);

//    /* Enable and set Button EXTI Interrupt to the lowest priority */
//    HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00);
//    HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
//  }
//}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *   This parameter should be: BUTTON_KEY  
  * @retval The Button GPIO pin value.
  */
//uint32_t BSP_PB_GetState(Button_TypeDef Button)
//{
//  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
//}


/******************************************************************************
                            BUS OPERATIONS
*******************************************************************************/

/******************************* ADC Routines**********************************/

/**
  * @brief ADC MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;
  static DMA_HandleTypeDef  hdma_adc;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock */
  ADCx_CHANNEL_GPIO_CLK_ENABLE();
  /* ADC3 Periph clock enable */
  ADCx_CLK_ENABLE();
  /* Enable DMA2 clock */
  DMAx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/ 
  /* ADC3 Channel8 GPIO pin configuration */
  GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
  
  /*##-3- Configure the DMA streams ##########################################*/
  /* Set the parameters to be configured */
  hdma_adc.Instance = ADCx_DMA_STREAM;
  
  hdma_adc.Init.Channel  = ADCx_DMA_CHANNEL;
  hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc.Init.Mode = DMA_NORMAL;//DMA_CIRCULAR;
  hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;         
  hdma_adc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  hdma_adc.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_adc.Init.PeriphBurst = DMA_PBURST_SINGLE; 

  HAL_DMA_Init(&hdma_adc);
    
  /* Associate the initialized DMA handle to the the ADC handle */
  __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(ADCx_DMA_IRQn, 3, 0);   
  HAL_NVIC_EnableIRQ(ADCx_DMA_IRQn);
}
  
/**
  * @brief ADC MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  static DMA_HandleTypeDef  hdma_adc;
  
  /*##-1- Reset peripherals ##################################################*/
  ADCx_FORCE_RESET();
  ADCx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the ADC3 Channel8 GPIO pin */
  HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN);
  
  /*##-3- Disable the DMA Streams ############################################*/
  /* De-Initialize the DMA Stream associate to transmission process */
  HAL_DMA_DeInit(&hdma_adc); 
    
  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(ADCx_DMA_IRQn);
}



/******************************* I2C Routines**********************************/

/**
  * @brief Discovery I2Cx MSP Initialization
  * @param hi2c: I2C handle
  * @retval None
  */
static void I2Cx_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  GPIO_InitStruct;  
  static DMA_HandleTypeDef hdma_tx;
  static DMA_HandleTypeDef hdma_rx;
  
//  I2C_HandleTypeDef* pI2cHandle;
//  pI2cHandle = &I2cHandle;


  if (hi2c->Instance == FM24C64_I2C)
  {
    /*##-1- Configure the GPIOs ################################################*/  

    /* Enable GPIO clock */
    FM24C64_I2C_GPIO_CLK_ENABLE();
      
    /* Configure I2C Tx & Rx as alternate function  */
    GPIO_InitStruct.Pin       = FM24C64_I2C_SCL_PIN | FM24C64_I2C_SDA_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = FM24C64_I2C_SCL_SDA_AF;
    HAL_GPIO_Init(FM24C64_I2C_PORT, &GPIO_InitStruct);
      
    /*##-2- Configure the Discovery I2Cx peripheral #######################################*/ 
    /* Enable I2C3 clock */
    FM24C64_I2C_CLOCK_ENABLE();
    
    /* Force the I2C Periheral Clock Reset */  
    FM24C64_I2C_FORCE_RESET();
      
    /* Release the I2C Periheral Clock Reset */  
    FM24C64_I2C_RELEASE_RESET(); 
    
    /* Enable and set Discovery I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority(FM24C64_I2C_EV_IRQn, 14, 0);
    HAL_NVIC_EnableIRQ(FM24C64_I2C_EV_IRQn);
    
    /* Enable and set Discovery I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority(FM24C64_I2C_ER_IRQn, 14, 0);
    HAL_NVIC_EnableIRQ(FM24C64_I2C_ER_IRQn);  

//    /* I2C DMA TX and RX channels configuration */
//    /* Enable the DMA clock */
//    EEPROM_I2C_DMA_CLK_ENABLE();
//    
//    /* Configure the DMA stream for the EE I2C peripheral TX direction */
//    /* Configure the DMA Stream */
//    hdma_tx.Instance                  = EEPROM_I2C_DMA_STREAM_TX;
//    /* Set the parameters to be configured */
//    hdma_tx.Init.Channel              = EEPROM_I2C_DMA_CHANNEL;  
//    hdma_tx.Init.Direction            = DMA_MEMORY_TO_PERIPH;
//    hdma_tx.Init.PeriphInc            = DMA_PINC_DISABLE;
//    hdma_tx.Init.MemInc               = DMA_MINC_ENABLE;
//    hdma_tx.Init.PeriphDataAlignment  = DMA_PDATAALIGN_BYTE;
//    hdma_tx.Init.MemDataAlignment     = DMA_MDATAALIGN_BYTE;
//    hdma_tx.Init.Mode                 = DMA_NORMAL;
//    hdma_tx.Init.Priority             = DMA_PRIORITY_VERY_HIGH;
//    hdma_tx.Init.FIFOMode             = DMA_FIFOMODE_ENABLE;         
//    hdma_tx.Init.FIFOThreshold        = DMA_FIFO_THRESHOLD_FULL;
//    hdma_tx.Init.MemBurst             = DMA_MBURST_SINGLE;
//    hdma_tx.Init.PeriphBurst          = DMA_PBURST_SINGLE; 

//    /* Associate the initilalized hdma_tx handle to the the pI2cHandle handle */
//    __HAL_LINKDMA(pI2cHandle, hdmatx, hdma_tx);
//    
//    /* Configure the DMA Stream */
//    HAL_DMA_Init(&hdma_tx);
//    
//    /* Configure and enable I2C DMA TX Channel interrupt */
//    HAL_NVIC_SetPriority((IRQn_Type)(EEPROM_I2C_DMA_TX_IRQn), EEPROM_I2C_DMA_PREPRIO, 0);
//    HAL_NVIC_EnableIRQ((IRQn_Type)(EEPROM_I2C_DMA_TX_IRQn));
//    
//    /* Configure the DMA stream for the EE I2C peripheral TX direction */
//    /* Configure the DMA Stream */
//    hdma_rx.Instance                  = EEPROM_I2C_DMA_STREAM_RX;
//    /* Set the parameters to be configured */
//    hdma_rx.Init.Channel              = EEPROM_I2C_DMA_CHANNEL;  
//    hdma_rx.Init.Direction            = DMA_PERIPH_TO_MEMORY;
//    hdma_rx.Init.PeriphInc            = DMA_PINC_DISABLE;
//    hdma_rx.Init.MemInc               = DMA_MINC_ENABLE;
//    hdma_rx.Init.PeriphDataAlignment  = DMA_PDATAALIGN_BYTE;
//    hdma_rx.Init.MemDataAlignment     = DMA_MDATAALIGN_BYTE;
//    hdma_rx.Init.Mode                 = DMA_NORMAL;
//    hdma_rx.Init.Priority             = DMA_PRIORITY_VERY_HIGH;
//    hdma_rx.Init.FIFOMode             = DMA_FIFOMODE_ENABLE;         
//    hdma_rx.Init.FIFOThreshold        = DMA_FIFO_THRESHOLD_FULL;
//    hdma_rx.Init.MemBurst             = DMA_MBURST_SINGLE;
//    hdma_rx.Init.PeriphBurst          = DMA_PBURST_SINGLE; 

//    /* Associate the initilalized hdma_rx handle to the the pI2cHandle handle*/
//    __HAL_LINKDMA(pI2cHandle, hdmarx, hdma_rx);
//    
//    /* Configure the DMA Stream */
//    HAL_DMA_Init(&hdma_rx);
//    
//    /* Configure and enable I2C DMA RX Channel interrupt */
//    HAL_NVIC_SetPriority((IRQn_Type)(EEPROM_I2C_DMA_RX_IRQn), EEPROM_I2C_DMA_PREPRIO, 0);
//    HAL_NVIC_EnableIRQ((IRQn_Type)(EEPROM_I2C_DMA_RX_IRQn));
  }
}

/**
  * @brief Discovery I2Cx Bus initialization
  * @param None
  * @retval None
  */
static void I2Cx_Init(void)
{
  if(HAL_I2C_GetState(&I2cHandle) == HAL_I2C_STATE_RESET)
  {
    I2cHandle.Instance              = FM24C64_I2C;
    I2cHandle.Init.ClockSpeed       = FM24C64_I2C_ADDRESS;
    I2cHandle.Init.DutyCycle        = I2C_DUTYCYCLE_2;
    I2cHandle.Init.OwnAddress1      = 0;
    I2cHandle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLED;
    I2cHandle.Init.OwnAddress2      = 0;
    I2cHandle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLED;
    I2cHandle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLED;  
      
    /* Init the I2C */
    I2Cx_MspInit(&I2cHandle);
    HAL_I2C_Init(&I2cHandle);
  }
}

/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  Value: The target register value to be written 
  * @retval None
  */
//static void I2Cx_WriteData(uint8_t Addr, uint8_t Reg, uint8_t Value)
//  {
//  HAL_StatusTypeDef status = HAL_OK;
//  
//  status = HAL_I2C_Mem_Write(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_16BIT, &Value, 1, I2cxTimeout); 
//  
//  /* Check the communication status */
//  if(status != HAL_OK)
//  {
//    /* Re-Initialize the BUS */
//    I2Cx_Error();
//  }        
//}

/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  pBuffer: The target register value to be written 
  * @param  Length: buffer size to be written
  * @retval None
  */
static HAL_StatusTypeDef I2Cx_WriteBuffer(uint8_t Addr, uint8_t Reg,  uint8_t *pBuffer, uint16_t Length)
  {
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_16BIT, pBuffer, Length, I2cxTimeout); 

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initialize the BUS */
    I2Cx_Error();
  }
	
	return status;
}

/**
  * @brief  Reads a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @retval Data read at register address
  */
//static uint8_t I2Cx_ReadData(uint8_t Addr, uint8_t Reg)
//{
//  HAL_StatusTypeDef status = HAL_OK;
//  uint8_t value = 0;
//  
//  status = HAL_I2C_Mem_Read(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_16BIT, &value, 1, I2cxTimeout);
// 
//  /* Check the communication status */
//  if(status != HAL_OK)
//  {
//    /* Re-Initiaize the BUS */
//    I2Cx_Error();
//  
//  }
//  return value;
//}

/**
  * @brief  Reads multiple data on the BUS.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address 
  * @param  pBuffer: pointer to read data buffer
  * @param  Length: length of the data
  * @retval 0 if no problems to read multiple data
  */
static HAL_StatusTypeDef I2Cx_ReadBuffer(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_16BIT, pBuffer, Length, I2cxTimeout);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initialize the BUS */
    I2Cx_Error();
  }
  
  return status;

}

/**
  * @brief  Write a value in a register of the device through BUS in using DMA mode
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  pBuffer: The target register value to be written 
  * @param  Length: buffer size to be written
  * @retval HAL status
  */
//static HAL_StatusTypeDef I2Cx_WriteBufferDMA(uint8_t Addr, uint16_t Reg,  uint8_t *pBuffer, uint16_t Length)
//  {
//  HAL_StatusTypeDef status = HAL_OK;
//  
//  status = HAL_I2C_Mem_Write_DMA(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_16BIT, pBuffer, Length);

//  /* Check the communication status */
//  if(status != HAL_OK)
//  {
//    /* Re-Initiaize the BUS */
//    I2Cx_Error();
//  }

//  return status;
//}

/**
  * @brief  Reads multiple data on the BUS in using DMA mode.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address 
  * @param  pBuffer: pointer to read data buffer
  * @param  Length: length of the data
  * @retval HAL status
  */
//static HAL_StatusTypeDef I2Cx_ReadBufferDMA(uint8_t Addr, uint16_t Reg, uint8_t *pBuffer, uint16_t Length)
//{
//  HAL_StatusTypeDef status = HAL_OK;

//  status = HAL_I2C_Mem_Read_DMA(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_16BIT, pBuffer, Length);
//  
//  /* Check the communication status */
//  if(status != HAL_OK)
//  {
//    /* Re-Initiaize the BUS */
//    I2Cx_Error();
//  }
//  
//  return status;
//}

/**
* @brief  Checks if target device is ready for communication. 
* @note   This function is used with Memory devices
* @param  DevAddress: Target device address
* @param  Trials: Number of trials
* @retval HAL status
*/
static HAL_StatusTypeDef I2Cx_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (HAL_I2C_IsDeviceReady(&I2cHandle, DevAddress, Trials, I2cxTimeout));
}

/**
  * @brief  I2Cx error treatment function
  * @param  None
  * @retval None
  */
static void I2Cx_Error(void)
{
  /* De-initialize the SPI communication BUS */
  HAL_I2C_DeInit(&I2cHandle);
  
  /* Re-Initialize the SPI communication BUS */
  I2Cx_Init();
}

/******************************* SPI Routines *********************************/

/**
  * @brief  SPIx Bus initialization
  * @param  None
  * @retval None
  */
static void SPIx_Init(void)
{
  if(HAL_SPI_GetState(&SpiHandle) == HAL_SPI_STATE_RESET)
  {
    /* SPI Config */
    SpiHandle.Instance = FM25V01_SPI;
    /* SPI baudrate is set to 21 MHz (PCLK2/SPI_BaudRatePrescaler = 84/4 = 21 MHz) 
       to verify these constraints:
       - FM25V10 SPI interface max baudrate is 40MHz for write/read
       - PCLK2 frequency is set to 84 MHz 
    */  
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    SpiHandle.Init.Direction      = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase       = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity    = SPI_POLARITY_LOW;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    SpiHandle.Init.CRCPolynomial  = 7;
    SpiHandle.Init.DataSize       = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit       = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS            = SPI_NSS_SOFT;
    SpiHandle.Init.TIMode         = SPI_TIMODE_DISABLED;
    SpiHandle.Init.Mode           = SPI_MODE_MASTER;
  
    SPIx_MspInit(&SpiHandle);
    HAL_SPI_Init(&SpiHandle);
		
		
		/* Chip Select */
		FM25V_NSS_H();
		CLK_Delay(10);
  }
}

///**
//  * @brief SPI Read 4 bytes from device
//  * @param  ReadSize Number of bytes to read (max 4 bytes)
//  * @retval Value read on the SPI
//  */
//static uint32_t SPIx_Read(uint8_t ReadSize)
//{
//  HAL_StatusTypeDef status = HAL_OK;
//  uint32_t readvalue;
//  
//  status = HAL_SPI_Receive(&SpiHandle, (uint8_t*) &readvalue, ReadSize, SpixTimeout);
//  
//  /* Check the communication status */
//  if(status != HAL_OK)
//  {
//    /* Re-Initiaize the BUS */
//    SPIx_Error();
//  }

//  return readvalue;
//}

///**
//  * @brief SPI Write a byte to device
//  * @param Value: value to be written
//  * @retval None
//  */
//static void SPIx_Write(uint16_t Value)
//{
//  HAL_StatusTypeDef status = HAL_OK;

//  status = HAL_SPI_Transmit(&SpiHandle, (uint8_t*) &Value, 1, SpixTimeout);

//  /* Check the communication status */
//  if(status != HAL_OK)
//  {
//    /* Re-Initiaize the BUS */
//    SPIx_Error();
//  }
//}

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte: Byte send.
  * @retval The received byte value
  */
static uint8_t SPIx_WriteRead(uint8_t Byte)
{
  uint8_t receivedbyte = 0;
  
  /* Send a Byte through the SPI peripheral */
  /* Read byte from the SPI bus */
  if(HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*) &Byte, (uint8_t*) &receivedbyte, 1, SpixTimeout) != HAL_OK)
  {
    SPIx_Error();
  }
  
  return receivedbyte;
}

/**
  * @brief  SPIx error treatment function.
  * @param  None
  * @retval None
  */
static void SPIx_Error(void)
{
  /* De-initialize the SPI communication BUS */
  HAL_SPI_DeInit(&SpiHandle);
  
  /* Re- Initialize the SPI communication BUS */
  SPIx_Init();
}

/**
  * @brief  SPI MSP Init.
  * @param  hspi: SPI handle
  * @retval None
  */
static void SPIx_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable SPIx clock  */
  FM25V01_SPI_CLK_ENABLE();
	
  /* Enable DISCOVERY_SPI GPIO clock */
  FM25V01_SPI_GPIO_CLK_ENABLE();
	FM25V01_SPI_NSS_GPIO_CLK_ENABLE();
	
  /* configure SPI SCK, MOSI and MISO */    
  GPIO_InitStructure.Pin    = (FM25V01_SPI_MISO_PIN | FM25V01_SPI_SCK_PIN | FM25V01_SPI_MOSI_PIN);
  GPIO_InitStructure.Mode   = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull   = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed  = GPIO_SPEED_MEDIUM;
  GPIO_InitStructure.Alternate = FM25V01_SPI_AF;
  HAL_GPIO_Init(FM25V01_SPI_PORT, &GPIO_InitStructure);      
	
	GPIO_InitStructure.Pin    = FM25V01_SPI_NSS_PIN;
  GPIO_InitStructure.Mode   = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull   = GPIO_PULLUP;
  GPIO_InitStructure.Speed  = GPIO_SPEED_MEDIUM;
  HAL_GPIO_Init(FM25V01_SPI_NSS_PORT, &GPIO_InitStructure);   
}


/********************************* LINK LCD ***********************************/

/**
  * @brief  Configures the LCD_SPI interface.
  * @param  None
  * @retval None
  */
void LCD_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  if(Is_LCD_IO_Initialized == 0)
  {
  	Is_LCD_IO_Initialized = 1; 
	  /* Configure the LCD Control pins ------------------------------------------*/
	  LCD_DISP_GPIO_CLK_ENABLE();
    
	  /* Configure NCS in Output Push-Pull mode */
	  GPIO_InitStructure.Pin     = LCD_DISP_PIN;
	  GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStructure.Pull    = GPIO_NOPULL;
	  GPIO_InitStructure.Speed   = GPIO_SPEED_FAST;
	  HAL_GPIO_Init(LCD_DISP_GPIO_PORT, &GPIO_InitStructure);
    
	  /* Set or Reset the control line */
		LCD_DISP_ON();
  }
}

///**
//  * @brief  Write register value.
//  * @param  None
//  * @retval None
//  */
//void LCD_IO_WriteData(uint16_t RegValue) 
//{
//  /* Set WRX to send data */
////  LCD_WRX_HIGH();
////  
////  /* Reset LCD control line(/CS) and Send data */  
////  LCD_CS_LOW();
////  SPIx_Write(RegValue);
////  
////  /* Deselect : Chip Select high */
////  LCD_CS_HIGH();
//}

///**
//  * @brief  register address.
//  * @param  None
//  * @retval None
//  */
//void LCD_IO_WriteReg(uint8_t Reg) 
//{
//  /* Reset WRX to send command */
////  LCD_WRX_LOW();
////  
////  /* Reset LCD control line(/CS) and Send command */
////  LCD_CS_LOW();
////  SPIx_Write(Reg);
////  
////  /* Deselect : Chip Select high */
////  LCD_CS_HIGH();
//}

///**
//  * @brief  Read register value.
//  * @param  RegValue Address of the register to read
//  * @param  ReadSize Number of bytes to read
//  * @retval Content of the register value
//  */
//uint32_t LCD_IO_ReadData(uint16_t RegValue, uint8_t ReadSize) 
//{
//  uint32_t readvalue = 0;

////  /* Select : Chip Select low */
////  LCD_CS_LOW();

////  /* Reset WRX to send command */
////  LCD_WRX_LOW();
////  
////  SPIx_Write(RegValue);
////  
////  readvalue = SPIx_Read(ReadSize);

////  /* Set WRX to send data */
////  LCD_WRX_HIGH();

////  /* Deselect : Chip Select high */
////  LCD_CS_HIGH();
//  
//  return readvalue;
//}

///**
//  * @brief  
//  * @param  None
//  * @retval None
//  */
//void LCD_Delay (uint32_t Delay)
//{
//  HAL_Delay (Delay);
//}

/**
  * @brief LCD BackLight Init
  * @param  None
  * @retval None
  */
void LCD_BackLight_Init()	//PB8
{
	TIM_HandleTypeDef    TimHandle;	/* Timer handler declaration */
	TIM_OC_InitTypeDef sConfig;	/* Timer Output Compare Configuration Structure declaration */
	GPIO_InitTypeDef   GPIO_InitStruct;
  
  /*##-0- Enable peripherals and GPIO Clocks #################################*/
  /* TIM10 Peripheral clock enable */
  __TIM10_CLK_ENABLE();
    
  /* Enable GPIO Port Clocks */
  __GPIOB_CLK_ENABLE();
  
  /*Configure I/Os #####################################################*/
  /* Configure PB.08 (TIM10_Channel) */
  
  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM10;
  
  /* Channel 1 output */
  GPIO_InitStruct.Pin = LCD_PWM_PIN;
  HAL_GPIO_Init(LCD_PWM_GPIO_PORT, &GPIO_InitStruct);
	
	/*##-1- Configure the TIM peripheral #######################################*/ 
  /* Initialize TIMx peripheral as follow:
       + Prescaler = (HAL_RCC_GetPCLK2Freq()*2)/50000 (50KHz)
       + Period = 100  (to have an output frequency equal to 500 Hz)
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Instance = TIM10;
  
  TimHandle.Init.Prescaler     = (HAL_RCC_GetPCLK2Freq()*2)/50000;
  TimHandle.Init.Period        = 100-1;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
  if(HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }
	
	/*##-2- Configure the PWM channels #########################################*/ 
  /* Common configuration for all channels */
  sConfig.OCMode     = TIM_OCMODE_PWM1;
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode = TIM_OCFAST_DISABLE;

  /* Set the pulse value for channel 1 */
  sConfig.Pulse = 30;  
  if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    //Error_Handler();
  }
	
	/*##-3- Start PWM signals generation #######################################*/ 
  /* Start channel 1 */
  if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Starting Error */
    //Error_Handler();
  }
}

/**
  * @brief  Set LCD Back Light PWM
  * @param  TIMx to select the TIM peripheral
  * @param  Value: 0-100
  * @retval None
  */
static const uint8_t TAB_BackLight_PWM[11] = {1,2,3,5,9,16,28,48,84,147,255};

void LCD_BackLight_SetPWM(uint8_t Value)
{
  /* Check the parameters */
  if(Value > 10) Value = 6;

	/* Set the CCR1 */
	TIM10->CCR1 = TAB_BackLight_PWM[Value];
}

/**
  * @brief  Set Infrared PWM
  * @param  TIMx to select the TIM peripheral
  * @param  Value: 0-100
  * @retval None
  */
void Infra_pwm_Init(void) //PC6
{
 TIM_HandleTypeDef    TimHandle; /* Timer handler declaration */
 TIM_OC_InitTypeDef sConfig; /* Timer Output Compare Configuration Structure declaration */
 GPIO_InitTypeDef   GPIO_InitStruct;
  
  /*##-0- Enable peripherals and GPIO Clocks #################################*/
  /* TIM3 Peripheral clock enable */
  __TIM3_CLK_ENABLE();
    
  /* Enable GPIO Port Clocks */
  __GPIOC_CLK_ENABLE();
  
  /*Configure I/Os #####################################################*/
  /* Configure PC.06 (TIM3_Channel) */
  
  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  
  /* Channel 1 output */
  GPIO_InitStruct.Pin = USART6_TX_PIN;
  HAL_GPIO_Init(USART6_PORT, &GPIO_InitStruct);
 
 /*##-1- Configure the TIM peripheral #######################################*/ 
  /* Initialize TIMx peripheral as follow:
       + Prescaler = (HAL_RCC_GetPCLK1Freq()*2)/80000 (80KHz)
       + Period = 100  (to have an output frequency equal to 500 Hz)
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Instance = TIM3;
  
  TimHandle.Init.Prescaler     = (HAL_RCC_GetPCLK1Freq()*2)/800000 - 1;
  TimHandle.Init.Period        = 100-1;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
  if(HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }
 
 /*##-2- Configure the PWM channels #########################################*/ 
  /* Common configuration for all channels */
  sConfig.OCMode     = TIM_OCMODE_PWM1;
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode = TIM_OCFAST_DISABLE;
 
  /* Set the pulse value for channel 1 */
  sConfig.Pulse = 0;  
  if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    //Error_Handler();
  }
 /*##-3- Start PWM signals generation #######################################*/ 
  /* Start channel 1 */
  if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Starting Error */
    //Error_Handler();
  }
}
 
void Infra_SetPWM(uint8_t Value) //0-100,??10
{
 /* Set the CCR1 */
 TIM3->CCR1 = Value;
}
 
void Infra_PWM_ON(void)
{
 Infra_SetPWM(10);
}
 
void Infra_PWM_OFF(void)
{
 Infra_SetPWM(0);
}


/******************************************************************************
                            LINK OPERATIONS
*******************************************************************************/



/********************************* LINK SPI 25V01 ***********************************/
/**
  * @brief  Configures the FM25V SPI interface.
  * @param  None
  * @retval None
  */
void FM25V_IO_Init(void)
{
  SPIx_Init();	//SPI Init
}

/**
  * @brief  Writes one byte to the FM25V.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the GYRO.
  * @param  WriteAddr : GYRO's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
void FM25V_IO_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	/* Chip Select */
	FM25V_NSS_L();
	/* Send Command - Write Enable */
	SPIx_WriteRead(FM25V_WREN);	//Set Write Enable
	/* Chip Select */
	FM25V_NSS_H();
	
	//CLK_Delay(10);

	/* Chip Select */
	FM25V_NSS_L();
	
  /* Send Command & Address of the indexed register */
  SPIx_WriteRead(FM25V_WRITE);
	SPIx_WriteRead((WriteAddr>>16)&0x01);	//A16
	SPIx_WriteRead((WriteAddr>>8) &0xFF);	//A15_8
	SPIx_WriteRead((WriteAddr>>0) &0xFF);	//A7_0
  
  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    SPIx_WriteRead(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }
	
	/* Chip Select */
	FM25V_NSS_H();
	
	//CLK_Delay(10);
	/* Chip Select */
	FM25V_NSS_L();
	/* Send Command - Write Disable */
	SPIx_WriteRead(FM25V_WRDI);	//Write Disable
	/* Chip Select */
	FM25V_NSS_H();
}

/**
  * @brief  Reads a block of data from the FM25V.
  * @param  pBuffer : pointer to the buffer that receives the data read from the GYRO.
  * @param  ReadAddr : GYRO's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the GYRO.
  * @retval None
  */
void FM25V_IO_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{  
	/* Chip Select */
	FM25V_NSS_L();
	
  /* Send Command & Address of the indexed register */
  SPIx_WriteRead(FM25V_READ);
	//SPIx_WriteRead(FM25V_FSTRD);
	SPIx_WriteRead((ReadAddr>>16)&0x01);	//A16
	SPIx_WriteRead((ReadAddr>>8) &0xFF);	//A15_8
	SPIx_WriteRead((ReadAddr>>0) &0xFF);	//A7_0
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to GYRO (Slave device) */
    *pBuffer = SPIx_WriteRead(0x00);
    NumByteToRead--;
    pBuffer++;
  }
	
	/* Chip Select */
	FM25V_NSS_H();
}  


void FM25V_IO_ReadID(uint8_t* pBuffer, uint16_t NumByteToRead)
{  
	/* Chip Select */
	FM25V_NSS_L();
	
  /* Send Command & Address of the indexed register */
  SPIx_WriteRead(FM25V_RDID);
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to GYRO (Slave device) */
    *pBuffer = SPIx_WriteRead(0x00);
    NumByteToRead--;
    pBuffer++;
  }
	
	/* Chip Select */
	FM25V_NSS_H();
} 


void FM25V_IO_ReadSN(uint8_t* pBuffer, uint16_t NumByteToRead)
{  
	/* Chip Select */
	FM25V_NSS_L();
	CLK_Delay(100);
	
  /* Send Command & Address of the indexed register */
  SPIx_WriteRead(FM25V_SNR);
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to GYRO (Slave device) */
    *pBuffer = SPIx_WriteRead(0x00);
    NumByteToRead--;
    pBuffer++;
  }
	
	/* Chip Select */
	CLK_Delay(100);
	FM25V_NSS_H();
} 

/********************************* LINK I2C EEPROM *****************************/
/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void EEPROM_IO_Init(void)
{
  I2Cx_Init();
}

/**
  * @brief  Writes data to I2C EEPROM driver in using DMA channel.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  pBuffer: Pointer to data buffer
  * @param  BufferSize: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef EEPROM_IO_WriteData(uint16_t DevAddress, uint32_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
	return I2Cx_WriteBuffer(DevAddress, MemAddress,  pBuffer, BufferSize);
	//return (I2Cx_WriteBufferDMA(DevAddress, MemAddress,  pBuffer, BufferSize));
}

/**
  * @brief  Reads data from I2C EEPROM driver in using DMA channel.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  pBuffer: Pointer to data buffer
  * @param  BufferSize: Amount of data to be read
  * @retval HAL status
  */
HAL_StatusTypeDef EEPROM_IO_ReadData(uint16_t DevAddress, uint32_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
	return I2Cx_ReadBuffer(DevAddress, MemAddress,  pBuffer, BufferSize);
	//return (I2Cx_ReadBufferDMA(DevAddress, MemAddress, pBuffer, BufferSize));
}

/**
* @brief  Checks if target device is ready for communication. 
* @note   This function is used with Memory devices
* @param  DevAddress: Target device address
* @param  Trials: Number of trials
* @retval HAL status
*/
HAL_StatusTypeDef EEPROM_IO_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (I2Cx_IsDeviceReady(DevAddress, Trials));
}


/**
  * @brief Clock Delay 
  * @param  delay in Clock
  * @retval None
  */
void CLK_Delay (uint32_t Delay)
{
  while(Delay--);
}

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
      
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
