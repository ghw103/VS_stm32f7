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
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/

#include <sys/types.h>

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif


#define INVALID_SOCKET SOCKET_ERROR
//#include <sys/socket.h>
//#include <sys/param.h>
//#include <sys/time.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
//#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
//#include "lwip/inet.h"
//#include "lwip/sockets.h"
//#include "mbedtls/net_sockets.h"

#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "lwip.h"
#include "netif/ethernet.h"
#include "MQTTClient.h"
#include "transport.h"
//#if defined(WIN32)
//#include <Iphlpapi.h>
//#else
//#include <sys/ioctl.h>
//#include <net/if.h>
//#endif

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
//static int mysock = INVALID_SOCKET;


int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
	//rc = send(sock, buf, buflen,0);
	
	rc = write(sock, buf, buflen);
	return rc;
}


int transport_getdata(Network* n ,unsigned char* buf, int count)
{
	int rc = recv(n->my_socket, buf, count, 0);
	//printf("received %d bytes count %d\n", rc, (int)count);
	return rc;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc = recv(sock, buf, count, 0);	
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
		return 0;
	}
	return rc;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(Network* n,char* addr, int port)
{

	int type = SOCK_STREAM;
	struct sockaddr_in address;
#if defined(AF_INET6)
	struct sockaddr_in6 address6;
#endif
	int rc = -1;

	sa_family_t family = AF_INET;

	struct addrinfo *result = NULL;
	struct addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL };
	static struct timeval tv;

	n->my_socket = -1;
	if (addr[0] == '[')
		++addr;

	if ((rc = getaddrinfo(addr, port, &hints, &result)) == 0)
	{
		struct addrinfo* res = result;

		/* prefer ip4 addresses */
		while (res)
		{
			if (res->ai_family == AF_INET)
			{
				result = res;
				break;
			}
			res = res->ai_next;
		}

#if defined(AF_INET6)
		if (result->ai_family == AF_INET6)
		{
			address6.sin6_port =((struct sockaddr_in6*)(result->ai_addr))->sin6_port; //htons(port);
			address6.sin6_family = family = AF_INET6;
			address6.sin6_addr = ((struct sockaddr_in6*)(result->ai_addr))->sin6_addr;
		}
		else
#endif
		if (result->ai_family == AF_INET)
		{
			address.sin_port = ((struct sockaddr_in*)(result->ai_addr))->sin_port; // ((struct sockaddr_in*)(result->ai_addr))->sin_port; // htons(port);
			address.sin_family = family = AF_INET;
			address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
		}
		else
			rc = -1;

		freeaddrinfo(result);
	}

	if (rc == 0)
	{
		n->my_socket =	socket(family, type, 0);
		if (n->my_socket != -1)
		{
#if defined(NOSIGPIPE)
			int opt = 1;

			if (setsockopt(*sock, SOL_SOCKET, SO_NOSIGPIPE, (void*)&opt, sizeof(opt)) != 0)
				Log(TRACE_MIN, -1, "Could not set SO_NOSIGPIPE for socket %d", *sock);
#endif

			if (family == AF_INET)
				rc = connect(n->my_socket, (struct sockaddr*)&address, sizeof(address));
#if defined(AF_INET6)
			else
				rc = connect(n->my_socket, (struct sockaddr*)&address6, sizeof(address6));
#endif
		}
	}


//	tv.tv_sec = 1; /* 1 second Timeout */
//	tv.tv_usec = 0;  
//	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
#ifdef USE_LCD 

	
	uint8_t iptxt[20];
	sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&address.sin_addr));
	printf("Static IP address: %s\n", iptxt);
#endif
	return rc;
}

int transport_close(int sock)
{
int rc;

	rc = shutdown(sock, SHUT_WR);
	rc = recv(sock, NULL, (size_t)0, 0);
	rc = close(sock);

	return rc;
}



