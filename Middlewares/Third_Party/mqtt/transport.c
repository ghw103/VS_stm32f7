/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - port to the bare metal environment and serial media specifics
 *******************************************************************************/

/** By the way, this is a nice bare bones example, easier to expand to whatever non-OS
media you might have */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "transport.h"

#include "lwip.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/opt.h"
#include "dns.h"

//#include "mqtt.h"
#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
/**
This simple low-level implementation assumes a single connection for a single thread. Thus, single static
variables are used for that connection.
On other scenarios, you might want to put all these variables into a structure and index via the 'sock'
parameter, as some functions show in the comments
The blocking rx function is not supported.
If you plan on writing one, take into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
struct netconn *Netconn;

int transport_getdata(unsigned char * buf, int count);



int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
    int rc;
	void *data;
	
//	struct netbuf *txbuf;
//	txbuf = netbuf_new();				
//	netbuf_alloc(txbuf, buflen);
//	txbuf->p->payload = buf;
//	//txbuf->p->len = buflen;
//	netbuf_data(txbuf, &data, &buflen);
//	do 
//	{
////		
//		netconn_write(Netconn, data, buflen, NETCONN_COPY);
//          
//	} while (netbuf_next(txbuf) >= 0);
//          
//	netbuf_delete(txbuf);
/************************第二种******************888*/
	netconn_write(Netconn, buf, buflen, NETCONN_COPY);
	
	return buflen;

}


int transport_getdata(unsigned char * buf, int count)
{
	return netconn_recv(Netconn, &buf);
		/* nah, it is here for similarity with other transport examples */
}


/**
return >=0 for a connection descriptor, <0 for an error code
*/

int transport_open(transport_coonmsg_t *thisio)
{
	int idx=TRANSPORT_ERROR;	// for multiple connections, you might, basically turn myio into myio[MAX_CONNECTIONS],
printf("idx:%d\n", idx);
	err_t err;
	ip_addr_t DestIPaddr;
	LWIP_UNUSED_ARG(thisio);
	printf("host:%s\n", thisio->host);
	err = netconn_gethostbyname(thisio->host, &DestIPaddr);
#ifdef USE_LCD  
	uint8_t iptxt[20];
	sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&DestIPaddr));
	printf("Static IP address: %s\n", iptxt);  
#endif

	Netconn = netconn_new(NETCONN_TCP);
	if (Netconn != NULL)
	{  
		/* 将连接绑定到端口号 DEST_PORT. */
		err = netconn_bind(Netconn, NULL, DEST_PORT);
		if (err == ERR_OK)
		{
			//设置连接地址
			err = netconn_connect(Netconn, &DestIPaddr, 1883);
				if (err == ERR_OK)//连接成功
				{
					idx = TRANSPORT_AGAIN; 					// and return the index used
				}
				else
				{
					netconn_close(Netconn);
					netconn_delete(Netconn);
					Netconn = netconn_new(NETCONN_TCP);
					if (Netconn != NULL)
					{ 
						err = netconn_bind(Netconn, NULL, DEST_PORT);
						if (err == ERR_OK)
						{
							idx = netconn_connect(Netconn, &DestIPaddr, 1883);							
						}
					}
				}
		}
	}
	else
	{
		netconn_delete(Netconn);	
	}	
	printf("idx:%d\n", idx);
	return idx;
}

int transport_close(int sock)
{
int rc=TRANSPORT_DONE;
	netconn_close(Netconn);
	netconn_delete(Netconn);
	return rc;
}

//
//
//
//
//err_t mqtt_publish(mqtt_client_t *client, 
//	const char *topic, 
//	const void *payload, 
//	u16_t payload_length, 
//	u8_t qos, 
//	u8_t retain, 
//	mqtt_request_cb_t cb, 
//	void *arg)
//{
//}

//#define  BUFF_LEN     (256)
//uint8_t  TempBuff[BUFF_LEN] = "This message from tcp \r\n";
int mqtt_client_connect()
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	transport_coonmsg_t   connect;
	int rc = 0;
	int mysock = 0;

	uint8_t buf[200];
	int buflen = sizeof(buf);

	int len = 0;
	connect.host = "gaohongwei.mqtt.iot.gz.baidubce.com";
	connect.port = 1883;
	transport_open(&connect);
	
	data.clientID.cstring = "MQTT_Client_10";
	data.keepAliveInterval = 60;        //seconds
	data.cleansession = 1;
	data.username.cstring = "gaohongwei/iot";
	data.password.cstring = "f+Q9Lp++T5nysNVcLVfOWpIIDVz8MaVm5dyJA8jEXdU=";
	len = MQTTSerialize_connect(buf, buflen, &data);
	rc = transport_sendPacketBuffer(mysock, buf, len);		
	//transport_close(mysock);
//	printf("close\n");

}


