/**
  ******************************************************************************
  * File Name          : SDMMC.c
  * Description        : This file provides code for the configuration
  *                      of the SDMMC instances.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sdmmc.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SD_HandleTypeDef hsd2;
DMA_HandleTypeDef hdma_sdmmc2_rx;
DMA_HandleTypeDef hdma_sdmmc2_tx;

/* SDMMC2 init function */

void MX_SDMMC2_SD_Init(void)
{

  hsd2.Instance = SDMMC2;
  hsd2.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd2.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
  hsd2.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd2.Init.BusWide = SDMMC_BUS_WIDE_1B;
  hsd2.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd2.Init.ClockDiv = 0;

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(sdHandle->Instance==SDMMC2)
  {
  /* USER CODE BEGIN SDMMC2_MspInit 0 */

  /* USER CODE END SDMMC2_MspInit 0 */
    /* SDMMC2 clock enable */
    __HAL_RCC_SDMMC2_CLK_ENABLE();
  
    /**SDMMC2 GPIO Configuration    
    PB4     ------> SDMMC2_D3
    PB3     ------> SDMMC2_D2
    PD7     ------> SDMMC2_CMD
    PD6     ------> SDMMC2_CK
    PG10     ------> SDMMC2_D1
    PG9     ------> SDMMC2_D0 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_SDMMC2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_SDMMC2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_SDMMC2;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* SDMMC2 DMA Init */
    /* SDMMC2_RX Init */
    hdma_sdmmc2_rx.Instance = DMA2_Stream0;
    hdma_sdmmc2_rx.Init.Channel = DMA_CHANNEL_11;
    hdma_sdmmc2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdmmc2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdmmc2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdmmc2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdmmc2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdmmc2_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdmmc2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdmmc2_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdmmc2_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdmmc2_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdmmc2_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdmmc2_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(sdHandle,hdmarx,hdma_sdmmc2_rx);

    /* SDMMC2_TX Init */
    hdma_sdmmc2_tx.Instance = DMA2_Stream5;
    hdma_sdmmc2_tx.Init.Channel = DMA_CHANNEL_11;
    hdma_sdmmc2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdmmc2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdmmc2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdmmc2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdmmc2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdmmc2_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdmmc2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdmmc2_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdmmc2_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdmmc2_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdmmc2_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdmmc2_tx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(sdHandle,hdmatx,hdma_sdmmc2_tx);

  /* USER CODE BEGIN SDMMC2_MspInit 1 */

  /* USER CODE END SDMMC2_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDMMC2)
  {
  /* USER CODE BEGIN SDMMC2_MspDeInit 0 */

  /* USER CODE END SDMMC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDMMC2_CLK_DISABLE();
  
    /**SDMMC2 GPIO Configuration    
    PB4     ------> SDMMC2_D3
    PB3     ------> SDMMC2_D2
    PD7     ------> SDMMC2_CMD
    PD6     ------> SDMMC2_CK
    PG10     ------> SDMMC2_D1
    PG9     ------> SDMMC2_D0 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4|GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_7|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_10|GPIO_PIN_9);

    /* SDMMC2 DMA DeInit */
    HAL_DMA_DeInit(sdHandle->hdmarx);
    HAL_DMA_DeInit(sdHandle->hdmatx);

    /* SDMMC2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDMMC2_IRQn);
  /* USER CODE BEGIN SDMMC2_MspDeInit 1 */

  /* USER CODE END SDMMC2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
