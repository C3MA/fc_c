/*
 *  fcclient.c
 *  XCodeFC_C
 *
 *  Created by ollo on 24.04.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include "fcclient.h"
#include "fc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>


#define BUFFERSIZE 2048
#define RECV_BUFFERSIZE 1024
#define CLIENT_NAME "fc_c"
#define CLIENT_VERSION "0.9"
#define SEQUENCID 2342


extern fcclient_t* fcclient_new()
{
	fcclient_t* tmp = malloc(sizeof(fcclient_t));
	memset(tmp, 0, sizeof(fcclient_t));
	return tmp;
}

extern int fcclient_open(fcclient_t* fc, char* host)
{
	struct sockaddr_in serv_addr;
	
	uint8_t buffer[BUFFERSIZE];
    uint8_t output[BUFFERSIZE];
    int offset = 0;
	
	/* Open a connection to the wall */
	if (fc == NULL)
		return -1;
	
    if((fc->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return -2; /*FIXME return enum */
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NETWORK_PORT);
    
    if(inet_pton(AF_INET, host, &serv_addr.sin_addr)<=0)
    {
        /*FIXME ("\n inet_pton error occured\n"); */
        return -3;
    }
	
	if( connect(fc->sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        /*FIXME ("\n Error : Connect Failed \n"); */
        return -4;
    }
	
	
	/* With the opened connection, ask the WALL for its resolution */
	    
    offset = send_inforequest(buffer, offset);
    /*FIXME debug code: printf("send, offset: %d \n",offset);*/
	
	add_header(buffer, output, offset);
	write(fc->sockfd, output, offset+HEADER_LENGTH);
		
	return 1; /* everything was done successful */
}


extern int fcclient_processNetwork(fcclient_t* fc)
{
	int n;
	int offset = 0;
	int type = -1;
	int length = 0;
	uint8_t recvBuff[RECV_BUFFERSIZE];
	
	int counter;
	
    int meta_offset;
    int meta_length;
	char *generator_name;
    char *generator_version;
	
	int errorcode;
	char *descr;
	
	n = read(fc->sockfd, recvBuff, sizeof(recvBuff)-1);
	if (n < HEADER_LENGTH) {
		/* Error : Network read error */
		return -1;
	}
	offset = get_header((uint8_t*)recvBuff, 0, &type, &length);
	if (offset == -1) {
		/* Error : Could not analyze header*/
		return -2;
	}
	printf("typ: %d laenge: %d\n",type,length); /*FIXME remove debug code later */
	
	
	switch (type) {
        case SNIPTYPE_PONG:
            recv_pong((uint8_t*)recvBuff, offset, &counter);
            printf("Pong Counter: %d\n",counter);
            break;
		case SNIPTYPE_INFOANSWER:
			offset = recv_infoanswer(recvBuff, offset, &meta_offset, &meta_length);
            if (offset == -1) {
                printf("recv_infoanswer Faild!\n");
            } else {
                printf("Parse InfoAnswer\n");
            }
            offset = parse_metadata(recvBuff,meta_offset,&(fc->fps), &(fc->width), &(fc->height), &generator_name, &generator_version);
            if (offset == -1) {
                printf("parse Metadata Faild!\n");
                return -3;
            } else {
                printf("Walls definition, fps: %d, width: %d, height: %d, gen._name: %s, gen._version: %s\n",fc->fps,fc->width,fc->height,generator_name,generator_version);
            }
            free(generator_name);
            free(generator_version);
            break;
			
		case SNIPTYPE_ACK:
            offset = recv_ack(recvBuff, offset);
            if (offset == -1) {
                printf("recv_error Faild!\n");
            } else {
                printf("Recive ACK\n"); 
            }
            break;
		case SNIPTYPE_START:
			/* the wall wants us to send something */
			fc->connected = 1;
			break;
			
		case SNIPTYPE_ERROR:
            offset = recv_error(recvBuff, offset, &errorcode, &descr);
            if (offset == -1) {
                printf("recv_error Faild!\n");
            } else {
                printf("Error errorcode: %d, description: %s \n",errorcode,descr);
                switch (errorcode) {
                    case ERRORCODETYPE_OK:
                        printf("Errorcode: OK\n");
                        break;
                    case ERRORCODETYPE_DECODING_ERROR:
                        printf("Errorcode: Decoding Error\n");
                        break;
                    default:
                        printf("Errorcode: Unbekannt\n");
                        break;
                }
				free(descr);
            }
            break;
        default:
            printf("Unknown type: %d\n",type);
            return -5;
    }
	return 1;
}


extern int fcclient_start(fcclient_t* fc)
{
	/* check that the resolution of the wall is set */
	if (fc->width == 0 && fc->height == 0 && fc->fps == 0)
		return -1;
	
	
	uint8_t buffer[BUFFERSIZE];
    uint8_t output[BUFFERSIZE];
	int offset = 0;
	
	
	uint8_t meta[1024];
    int offset_meta;
    char gen_name[] = "Test";
    char gen_version[] = "0.0";
    char color[] = "rot";
    
	fc->fps = 24;
	fc->width = 1;
	fc->height = 1;
	
    offset_meta = create_metadata(meta, 0, fc->fps, fc->width, fc->height, gen_name, gen_version);
    offset = send_request(buffer, offset, color, 1, meta, offset_meta);
	
	add_header(buffer, output, offset);
	write(fc->sockfd, output, offset+HEADER_LENGTH);
	return 1;
}