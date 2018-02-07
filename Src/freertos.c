/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "stm32f7xx_hal.h"
#include "lwip.h"
#include  "lcd_log.h"
//#include "tcp_echoclient.h"
#include "timeouts.h"

#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/opt.h"

#include "adc.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId DHCPTaskHandle;
osThreadId TCPclient_TaskHandle;
osThreadId TCPsever_TaskHandle;
osThreadId UDPclient_TaskHandle;
osThreadId LEDTaskHandle;
osThreadId TempTaskHandle;

/* USER CODE BEGIN Variables */
/* DHCP process states */
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5
__IO uint8_t DHCP_state = DHCP_WAIT_ADDRESS;
#define MAX_DHCP_TRIES  4
/*****************temperaturesensor*********************/
#define TEMP_REFRESH_PERIOD   1000    /* Internal temperature refresh period */
#define MAX_CONVERTED_VALUE   4095    /* Max converted value */
#define AMBIENT_TEMP            25    /* Ambient Temperature */
#define VSENS_AT_AMBIENT_TEMP  760    /* VSENSE value (mv) at ambient temperature */
#define AVG_SLOPE               25    /* Avg_Solpe multiply by 10 */
#define VREF                  3300
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void DHCP_Task(void const * argument);
void TCP_client_Task(void const * argument);
void TCP_sever_Task(void const * argument);
void UDP_client_Task(void const * argument);
void LED_Task(void const * argument);
void Temp_Task(void const * argument);

extern void MX_LWIP_Init(void);
extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of DHCPTask */
//  osThreadDef(DHCPTask, DHCP_Task, osPriorityHigh, 0, 256);
//  DHCPTaskHandle = osThreadCreate(osThread(DHCPTask), NULL);
//
//  /* definition and creation of TCPclient_Task */
//  osThreadDef(TCPclient_Task, TCP_client_Task, osPriorityNormal, 0, 128);
//  TCPclient_TaskHandle = osThreadCreate(osThread(TCPclient_Task), NULL);

  /* definition and creation of TCPsever_Task */
  osThreadDef(TCPsever_Task, TCP_sever_Task, osPriorityNormal, 0, 128);
  TCPsever_TaskHandle = osThreadCreate(osThread(TCPsever_Task), NULL);

  /* definition and creation of UDPclient_Task */
  osThreadDef(UDPclient_Task, UDP_client_Task, osPriorityNormal, 0, 128);
  UDPclient_TaskHandle = osThreadCreate(osThread(UDPclient_Task), NULL);

  /* definition and creation of LEDTask */
  osThreadDef(LEDTask, LED_Task, osPriorityLow, 0, 128);
  LEDTaskHandle = osThreadCreate(osThread(LEDTask), NULL);

  /* definition and creation of TempTask */
  osThreadDef(TempTask, Temp_Task, osPriorityBelowNormal, 0, 128);
  TempTaskHandle = osThreadCreate(osThread(TempTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	  /* definition and creation of DHCPTask */
//	osThreadDef(DHCPTask, DHCP_Task, osPriorityHigh, 0, 256);
//	DHCPTaskHandle = osThreadCreate(osThread(DHCPTask), &gnetif);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();

  /* init code for FATFS */
  MX_FATFS_Init();

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	osThreadDef(DHCPTask, DHCP_Task, osPriorityHigh, 0, 256);
	DHCPTaskHandle = osThreadCreate(osThread(DHCPTask), &gnetif);
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* DHCP_Task function */
void DHCP_Task(void const * argument)
{
  /* USER CODE BEGIN DHCP_Task */
  /* Infinite loop */
	struct netif *netif = (struct netif *) argument;
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;
	struct dhcp *dhcp;
#ifdef USE_LCD 
	uint8_t iptxt[20];
#endif
  for(;;)
  {  
	  BSP_LED_Toggle(LED_RED);
	  switch (DHCP_state)
	  {
	  case DHCP_START:
		  {
			  ip_addr_set_zero_ip4(&netif->ip_addr);
			  ip_addr_set_zero_ip4(&netif->netmask);
			  ip_addr_set_zero_ip4(&netif->gw);       
			  dhcp_start(netif);
			  DHCP_state = DHCP_WAIT_ADDRESS;

		  }
		  break;
      
	  case DHCP_WAIT_ADDRESS:
		  {  
#ifdef USE_LCD
			  printf("  State: Looking for DHCP server ...\n");
#endif
			  if (dhcp_supplied_address(netif)) 
			  {
				  DHCP_state = DHCP_ADDRESS_ASSIGNED;	
         
#ifdef USE_LCD 
				  sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));   
				  printf("IP address assigned by a DHCP server: %s\n", iptxt);
#else
				  BSP_LED_On(LED1);   
#endif 
			  }
			  else
			  {
				  dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    
				  /* DHCP timeout */
				  if (dhcp->tries > MAX_DHCP_TRIES)
				  {
					  DHCP_state = DHCP_TIMEOUT;
            
					  /* Stop DHCP */
					  dhcp_stop(netif);
					  /* Static address used */
					  IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
					  IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
					  IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
					  netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
#ifdef USE_LCD  
					  sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
					  printf("DHCP Timeout !! \n");
					  printf("Static IP address: %s\n", iptxt);  
#else
					  BSP_LED_On(LED1);  
#endif
				  }
			  }
		  }
		  break;
	  case DHCP_ADDRESS_ASSIGNED:
		   {
			   osThreadDef(TCPclient_Task, TCP_client_Task, osPriorityNormal, 0, 256);
			   TCPclient_TaskHandle = osThreadCreate(osThread(TCPclient_Task), NULL);
			   BSP_LED_Off(LED_RED);
			   vTaskDelete(NULL);  //删除本任务 
		   }
			break;
	  case DHCP_LINK_DOWN:
		  {
			  /* Stop DHCP */
			  dhcp_stop(netif);
			  DHCP_state = DHCP_OFF; 
		  }
		  break;
	  default: break;
	  }
    osDelay(250);
  }
  /* USER CODE END DHCP_Task */
}

/* TCP_client_Task function */
void TCP_client_Task(void const * argument)
{
  /* USER CODE BEGIN TCP_client_Task */
  /* Infinite loop */
	struct netconn *Netconn;
	err_t err;
	ip_addr_t DestIPaddr, ipaddr;
	IP4_ADDR(&DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3);
	LWIP_UNUSED_ARG(argument);
	  /* Create a new connection identifier. */
	Netconn = netconn_new(NETCONN_TCP);
	if (Netconn != NULL)
	{  
		/* 将连接绑定到端口号 DEST_PORT. */
		err = netconn_bind(Netconn, NULL, DEST_PORT);
		if (err == ERR_OK)
		{
			//设置连接地址
			err = netconn_connect(Netconn, &DestIPaddr, DEST_PORT);
			for (;;)
			{
				if (err == ERR_OK)//连接成功
				{
					//					do 
					//					{
					//						netbuf_data(buf, &data, &len);
					//						netconn_write(newconn, data, len, NETCONN_COPY);
					//          
					//					} while (netbuf_next(buf) >= 0);
					//          
					//					netbuf_delete(buf);
										printf("ok\n");
					osDelay(100);
					
				}
				else
				{
					printf("error\n");
					netconn_close(Netconn);
					netconn_delete(Netconn);
					Netconn = netconn_new(NETCONN_TCP);
					if (Netconn != NULL)
					{ 
						err = netconn_bind(Netconn, NULL, DEST_PORT);
						if (err == ERR_OK)
						{
							err = netconn_connect(Netconn, &DestIPaddr, DEST_PORT);
						}
					}
				}
				osDelay(1);
			}
		}
	}
	else
	{
		netconn_delete(Netconn);	
	}	
  /* USER CODE END TCP_client_Task */
}

/* TCP_sever_Task function */
void TCP_sever_Task(void const * argument)
{
  /* USER CODE BEGIN TCP_sever_Task */
  /* Infinite loop */
	  struct netconn *conn, *newconn;
  err_t err, accept_err;
  struct netbuf *buf;
  void *data;
  u16_t len;
      
	LWIP_UNUSED_ARG(argument);

  /* Create a new connection identifier. */
  conn = netconn_new(NETCONN_TCP);
  
	if (conn != NULL)
	{  
		/* Bind connection to well known port number 7. */
		err = netconn_bind(conn, NULL, 7);
    
		if (err == ERR_OK)
		{
			/* Tell connection to go into listening mode. */
			netconn_listen(conn);
			for (;;)
			{
				/* 刷新连接. */
				accept_err = netconn_accept(conn, &newconn);
    
				/* Process the new connection. */
				if (accept_err == ERR_OK) 
				{

					while (netconn_recv(newconn, &buf) == ERR_OK) 
					{
						do 
						{
							netbuf_data(buf, &data, &len);
							netconn_write(newconn, data, len, NETCONN_COPY);
          
						} while (netbuf_next(buf) >= 0);
          
						netbuf_delete(buf);
					}
        
					/* Close connection and discard connection identifier. */
					netconn_close(newconn);
					netconn_delete(newconn);
				}
				osDelay(1);
			}
		}
		else
		{
			netconn_delete(newconn);
		}
	}
  /* USER CODE END TCP_sever_Task */
}

/* UDP_client_Task function */
void UDP_client_Task(void const * argument)
{
  /* USER CODE BEGIN UDP_client_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END UDP_client_Task */
}

/* LED_Task function */
void LED_Task(void const * argument)
{
  /* USER CODE BEGIN LED_Task */
  /* Infinite loop */
  for(;;)
  {
	  osDelay(50);
  }
  /* USER CODE END LED_Task */
}

/* Temp_Task function */
void Temp_Task(void const * argument)
{
  /* USER CODE BEGIN Temp_Task */
  /* Infinite loop */
	__IO int32_t ConvertedValue = 0;
	int32_t JTemp = 0x0;
	char desc[50];
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ConvertedValue, 1);
	for (;;)
	{
		BSP_LED_On(LED_GREEN);
		/* Compute the Junction Temperature value */
		JTemp = ((((ConvertedValue * VREF) / MAX_CONVERTED_VALUE) - VSENS_AT_AMBIENT_TEMP) * 10 / AVG_SLOPE) + AMBIENT_TEMP;

		/* Display the Temperature Value on the LCD */
		sprintf(desc, "Internal Temperature is %ld degree C", JTemp);
		BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2 + 45, (uint8_t *)desc, CENTER_MODE);
		BSP_LCD_ClearStringLine(30);
		BSP_LED_Off(LED_GREEN);
		osDelay(2000);
	
	}
  /* USER CODE END Temp_Task */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
