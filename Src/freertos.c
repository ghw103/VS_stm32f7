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
#include  "lcd_log.h"
#include "timeouts.h"

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/apps/fs.h"
#include "lwip.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/opt.h"
#include "dns.h"

#include "adc.h"

#include "MQTTClient.h"
//#include "mqtt.h"
#include "mqtt_opts.h"
#include "transport.h"
#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId DHCPHandle;
osThreadId LEDHandle;
osThreadId TempHandle;
osThreadId MQTT_ClientHandle;
osThreadId mqtt_runHandle;
osMessageQId tempQueueHandle;

/* USER CODE BEGIN Variables */
osThreadId defaultTaskHandle;
osThreadId DHCPTaskHandle;
osThreadId TCPclient_TaskHandle;
osThreadId TCPsever_TaskHandle;
osThreadId Socketclient_TaskHandle;
osThreadId LEDTaskHandle;
osThreadId TempTaskHandle;
osThreadId MQTT_ClientTaskHandle;
osThreadId MQTT_publishTaskHandle;
osMessageQId tempQueueHandle;

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
void LED_Task(void const * argument);
void Temp_Task(void const * argument);
void MQTT_Client_Task(void const * argument);
void mqtt_run_Task(void const * argument);

extern void MX_LWIP_Init(void);
extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void messageArrived(MessageData* data)
{
	printf("Message arrived on topic %.*s: %.*s\n",
		data->topicName->lenstring.len,
		data->topicName->lenstring.data,
		data->message->payloadlen,
		data->message->payload);
}
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

  /* definition and creation of DHCP */
//  osThreadDef(DHCP, DHCP_Task, osPriorityHigh, 0, 256);
//  DHCPHandle = osThreadCreate(osThread(DHCP), NULL);

  /* definition and creation of LED */
  osThreadDef(LED, LED_Task, osPriorityLow, 0, 128);
  LEDHandle = osThreadCreate(osThread(LED), NULL);

  /* definition and creation of Temp */
  osThreadDef(Temp, Temp_Task, osPriorityBelowNormal, 0, 256);
  TempHandle = osThreadCreate(osThread(Temp), NULL);

  /* definition and creation of MQTT_Client */
//  osThreadDef(MQTT_Client, MQTT_Client_Task, osPriorityHigh, 0, 1024);
//  MQTT_ClientHandle = osThreadCreate(osThread(MQTT_Client), NULL);

  /* definition and creation of mqtt_run */
  osThreadDef(mqtt_run, mqtt_run_Task, osPriorityIdle, 0, 128);
  mqtt_runHandle = osThreadCreate(osThread(mqtt_run), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of tempQueue */
  osMessageQDef(tempQueue, 10, uint32_t);
  tempQueueHandle = osMessageCreate(osMessageQ(tempQueue), NULL);

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
	osThreadDef(DHCP, DHCP_Task, osPriorityHigh, 0, 256);
	DHCPHandle = osThreadCreate(osThread(DHCP), &gnetif);
  for(;;)
  {
	  vTaskDelay(NULL);
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
	for (;;)
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
#ifdef USE_LCD
				printf("Looking for DHCP server ...\n");
#endif
			}
			break;

		case DHCP_WAIT_ADDRESS:
			{
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
					//				  printf("%d\n", dhcp->tries);
												/* DHCP timeout */
												if(dhcp->tries > MAX_DHCP_TRIES)
					{
						DHCP_state = DHCP_TIMEOUT;
						/* Stop DHCP */
						dhcp_stop(netif);
						/* Static address used */
						IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
						IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
						IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
						netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
						DHCP_state = DHCP_ADDRESS_ASSIGNED;
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
				/* definition and creation of MQTT_ClientTask */
				osThreadDef(MQTT_Client, MQTT_Client_Task, osPriorityHigh, 0, 1024);
				MQTT_ClientHandle = osThreadCreate(osThread(MQTT_Client), NULL);
				BSP_LED_Off(LED_RED);
				vTaskDelete(NULL);  
			}
			break;
		case DHCP_LINK_DOWN:
			{
				/* Stop DHCP */
				dhcp_stop(netif);
				DHCP_state = DHCP_OFF;
			}
			break;
		default:
			break;
		}
		osDelay(250);
	}
  /* USER CODE END DHCP_Task */
}

/* LED_Task function */
void LED_Task(void const * argument)
{
  /* USER CODE BEGIN LED_Task */
  /* Infinite loop */

  for(;;)
  {
	
	  BSP_LED_Toggle(LED_RED);
	 // printf("h\r\n");
    osDelay(1000);
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

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ConvertedValue, 1);
	for (;;)
	{
		BSP_LED_On(LED_GREEN);
		/* Compute the Junction Temperature value */
		JTemp = ((((ConvertedValue * VREF) / MAX_CONVERTED_VALUE) - VSENS_AT_AMBIENT_TEMP) * 10 / AVG_SLOPE) + AMBIENT_TEMP;
//		xQueueSend(tempQueueHandle, (void *)&JTemp, (TickType_t) 10);

		BSP_LED_Off(LED_GREEN);
		osDelay(2000);

	}
  /* USER CODE END Temp_Task */
}

/* MQTT_Client_Task function */
void MQTT_Client_Task(void const * argument)
{
  /* USER CODE BEGIN MQTT_Client_Task */
  /* Infinite loop */
	MQTTMessage message;
	    char payload[30];
		message.qos = 2;
		message.retained = 0;
		sprintf(payload, "message number %d",123);
		message.payloadlen = strlen(payload);
		message.payload = payload;
	
	
	MQTTClient client;
	Network network;
	unsigned char sendbuf[80], readbuf[80];
	int rc = 0, 
	count = 0;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;

	//pvParameters = 0;
	NetworkInit(&network);
	MQTTClientInit(&client, &network, 30000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

		char* address = "66.154.108.162";
	//	char* address = "192.168.100.17";
//		char* address = "176.122.166.83";
	
	char* port= "1883";
	if ((rc = NetworkConnect(&network, address, port)) != 0)
		printf("Return code from network connect is %d\n", rc);

#if defined(MQTT_TASK)
	if ((rc = MQTTStartTask(&client)) != pdPASS)
		printf("Return code from start tasks is %d\n", rc);
#endif

	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = "MQTT_Client_1";
	connectData.keepAliveInterval = 10;          //seconds
	connectData.cleansession = 1;
	connectData.username.cstring ="gaohongwei/iot";
//	connectData.password.cstring = "f+Q9Lp++T5nysNVcLVfOWpIIDVz8MaVm5dyJA8jEXdU=";
	if ((rc = MQTTConnect(&client, &connectData)) != 0)
		printf("Return code from MQTT connect is %d\n", rc);
	else
		printf("MQTT Connected\n");

		if ((rc = MQTTSubscribe(&client, "temperaturesensor", 2, messageArrived)) != 0)
		printf("Return code from MQTT subscribe is %d\n", rc);
			if ((rc = MQTTPublish(&client, "temperaturesensor", &message)) != 0)
			printf("Return code from MQTT publish is %d\n", rc);
	for(;;)
	{
//		osDelay(1);
////		MQTTMessage message;
////		char payload[30];
////		++count;
////		message.qos = 2;
////		message.retained = 0;
////		sprintf(payload, "message number %d", count);
////		message.payloadlen = strlen(payload);
////		message.payload = payload;
////		if ((rc = MQTTPublish(&client, "temperaturesensor", &message)) != 0)
////			printf("Return code from MQTT publish is %d\n", rc);
#if !defined(MQTT_TASK)
		if((rc = MQTTYield(&client, 1000)) != 0)
			printf("Return code from yield is %d\n", rc);
#endif
	}
  /* USER CODE END MQTT_Client_Task */
}

/* mqtt_run_Task function */
void mqtt_run_Task(void const * argument)
{
  /* USER CODE BEGIN mqtt_run_Task */
  /* Infinite loop */
  for(;;)
  {
//	  cycle(c, &timer);
//	   int len = MQTTSerialize_pingreq(c->buf, c->buf_size);
//	  if (len > 0 && (rc = sendPacket(c, len, &timer)) == uSUCCESS) // send the ping packet
    osDelay(20000);
  }
  /* USER CODE END mqtt_run_Task */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
