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
<<<<<<< HEAD
<<<<<<< HEAD
void topic_received(MessageData* data)
=======
void messageArrived(MessageData* data)
>>>>>>> parent of 104ec5b... 移植其他mqtt客户端
{
	printf("Message arrived on topic %.*s: %.*s\n",
		data->topicName->lenstring.len,
		data->topicName->lenstring.data,
		data->message->payloadlen,
		data->message->payload);
}
=======

>>>>>>> parent of 18aad25... 基本实现mqtt系统函数发布消息
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
  osThreadDef(DHCP, DHCP_Task, osPriorityHigh, 0, 256);
  DHCPHandle = osThreadCreate(osThread(DHCP), NULL);

  /* definition and creation of LED */
  osThreadDef(LED, LED_Task, osPriorityLow, 0, 128);
  LEDHandle = osThreadCreate(osThread(LED), NULL);

  /* definition and creation of Temp */
  osThreadDef(Temp, Temp_Task, osPriorityBelowNormal, 0, 256);
  TempHandle = osThreadCreate(osThread(Temp), NULL);

  /* definition and creation of MQTT_Client */
  osThreadDef(MQTT_Client, MQTT_Client_Task, osPriorityHigh, 0, 1024);
  MQTT_ClientHandle = osThreadCreate(osThread(MQTT_Client), NULL);

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
<<<<<<< HEAD
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
				osThreadDef(MQTT_Client, MQTT_Client_Task, osPriorityAboveNormal, 0, 1024);
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
=======
  for(;;)
  {
    osDelay(1);
  }
>>>>>>> parent of 18aad25... 基本实现mqtt系统函数发布消息
  /* USER CODE END DHCP_Task */
}

/* LED_Task function */
void LED_Task(void const * argument)
{
  /* USER CODE BEGIN LED_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LED_Task */
}

/* Temp_Task function */
void Temp_Task(void const * argument)
{
  /* USER CODE BEGIN Temp_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Temp_Task */
}

/* MQTT_Client_Task function */
void MQTT_Client_Task(void const * argument)
{
  /* USER CODE BEGIN MQTT_Client_Task */
  /* Infinite loop */
<<<<<<< HEAD
<<<<<<< HEAD
	// testing mosquitto server
#define MQTT_HOST "176.122.166.83"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt_clein1"
#define MQTT_PASS ""
#define mqtt_client_id "MQTT on STM32"
	struct Network network;
	MQTTClient client = DefaultClient;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	unsigned char mqtt_buf[100];
	unsigned char mqtt_readbuf[100];

	NewNetwork(&network);
	ConnectNetwork(&network, MQTT_HOST, MQTT_PORT);
	NewMQTTClient(&client, &network, 10000, mqtt_buf, 100, mqtt_readbuf, 100);
	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = mqtt_client_id;  // you client's unique identifier
	data.username.cstring = MQTT_USER;
	data.password.cstring = MQTT_PASS;
	data.keepAliveInterval = 10;  // interval for PING message to be sent (seconds)
	data.cleansession = 0;
	MQTTConnect(&client, &data);
	MQTTSubscribe(&client, "mytopic", QOS1, topic_received);
=======
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
	char* port= "1883";
	if ((rc = NetworkConnect(&network, address, port)) != 0)
		printf("Return code from network connect is %d\n", rc);

#if defined(MQTT_TASK)
	if ((rc = MQTTStartTask(&client)) != pdPASS)
		printf("Return code from start tasks is %d\n", rc);
#endif
>>>>>>> parent of 104ec5b... 移植其他mqtt客户端

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
	for(;;)
	{
		osDelay(1);
//		MQTTMessage message;
//		char payload[30];
//		++count;
//		message.qos = 2;
//		message.retained = 0;
//		sprintf(payload, "message number %d", count);
//		message.payloadlen = strlen(payload);
//		message.payload = payload;
//		if ((rc = MQTTPublish(&client, "temperaturesensor", &message)) != 0)
//			printf("Return code from MQTT publish is %d\n", rc);
//#if !defined(MQTT_TASK)
//		if((rc = MQTTYield(&client, 1000)) != 0)
//			printf("Return code from yield is %d\n", rc);
//#endif
	}
=======
  for(;;)
  {
    osDelay(1);
  }
>>>>>>> parent of 18aad25... 基本实现mqtt系统函数发布消息
  /* USER CODE END MQTT_Client_Task */
}

/* mqtt_run_Task function */
void mqtt_run_Task(void const * argument)
{
  /* USER CODE BEGIN mqtt_run_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END mqtt_run_Task */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
