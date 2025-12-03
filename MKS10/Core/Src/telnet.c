/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */


#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"

#define TELNET_THREAD_PRIO  ( tskIDLE_PRIORITY + 4 )

#define CMD_BUFFER_LEN 256

static void telnet_process_command(char *cmd, struct netconn *conn)
{
	char databuff[CMD_BUFFER_LEN];
	sprintf(databuff, "RX: %s\r\n ");
	netconn_write(conn, databuff, strlen(databuff), NETCONN_COPY);

	char *token;
	token = strtok(cmd, " ");
	if (strcasecmp(token, "HELLO") == 0) {
		sprintf(databuff, "Communication OK!\r\n");
		netconn_write(conn, databuff, strlen(databuff), NETCONN_COPY);
	}
	else if (strcasecmp(token, "LED1") == 0) {
		token = strtok(NULL, " ");
		if (strcasecmp(token, "ON") == 0) {
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
		} else if (strcasecmp(token, "OFF") == 0) {
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
		}
		sprintf(databuff, "OK\r\n");
		netconn_write(conn, databuff, strlen(databuff), NETCONN_COPY);
	}
	else if (strcasecmp(token, "LED2") == 0) {
		token = strtok(NULL, " ");
		if (strcasecmp(token, "ON") == 0) {
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
		} else if (strcasecmp(token, "OFF") == 0) {
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
		}
		sprintf(databuff, "OK\r\n");
		netconn_write(conn, databuff, strlen(databuff), NETCONN_COPY);
	}
	else if (strcasecmp(token, "STATUS") == 0) {
		if (HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin)) {
			sprintf(databuff,"LED 1 is ON\r\n");
			netconn_write(conn, databuff, strlen(databuff), NETCONN_COPY);
		}else {
			sprintf(databuff,"LED 1 is OFF\r\n");
			netconn_write(conn, databuff, strlen(databuff), NETCONN_COPY);
		}
		if (HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin)) {
			sprintf(databuff,"LED 2 is ON\r\n");
			netconn_write(conn, databuff, strlen(databuff), NETCONN_COPY);
		}else {
			sprintf(databuff,"LED 2 is OFF\r\n");
			netconn_write(conn, databuff, strlen(databuff), NETCONN_COPY);
		}
	}
}


static void telnet_byte_available(uint8_t c, struct netconn *conn)
{
	static uint16_t cnt;
	static char data[CMD_BUFFER_LEN];
	if (cnt < CMD_BUFFER_LEN && c >= 32 && c <= 127) data[cnt++] = c;
	if (c == '\n' || c == '\r') {
		data[cnt] = '\0';
		telnet_process_command(data, conn);
		cnt = 0;
	}
}
/*-----------------------------------------------------------------------------------*/
static void TELNET_thread(void *arg)
{
  struct netconn *conn, *newconn;
  err_t err, accept_err;
  struct netbuf *buf;
  uint8_t *data;
  u16_t len;
      
  LWIP_UNUSED_ARG(arg);

  /* Create a new connection identifier. */
  conn = netconn_new(NETCONN_TCP);
  
  if (conn!=NULL)
  {  
    /* Bind connection to well known port number 7. */
    err = netconn_bind(conn, NULL, 23);
    
    if (err == ERR_OK)
    {
      /* Tell connection to go into listening mode. */
      netconn_listen(conn);
    
      while (1) 
      {
        /* Grab new connection. */
         accept_err = netconn_accept(conn, &newconn);
    
        /* Process the new connection. */
        if (accept_err == ERR_OK) 
        {

          while (netconn_recv(newconn, &buf) == ERR_OK) 
          {
            do 
            {
            	netbuf_data(buf, (void**)&data, &len);
            	while (len--) telnet_byte_available(*data++, newconn);

              //netconn_write(newconn, data, len, NETCONN_COPY);
          
            } 
            while (netbuf_next(buf) >= 0);
          
            netbuf_delete(buf);
          }
        
          /* Close connection and discard connection identifier. */
          netconn_close(newconn);
          netconn_delete(newconn);
        }
      }
    }
    else
    {
      netconn_delete(newconn);
    }
  }
}
/*-----------------------------------------------------------------------------------*/



void TELNET_init(void)
{
  sys_thread_new("TELNET_thread", TELNET_thread, NULL, DEFAULT_THREAD_STACKSIZE, TELNET_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */
