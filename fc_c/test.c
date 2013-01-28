//
//  test.c
//  fc_c
//
//  Created by Christian Platz on 27.01.13.
//  Copyright (c) 2013 C3MA. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "fc.h"
#include "proto.h"

int test_recv(uint8_t *buffer, int offset)
{
    int id, type, value;
    
    char *color;
    int seqId;
    int meta_offset;
    int meta_length;
    int frames_per_second, width, heigth;
    char *generator_name;
    char *generator_version;
    
    int frame_offset;
    int frame_offset_start;
    int frame_length;
    int red;
    int green;
    int blue;
    int x;
    int y;
    
    offset = parse(buffer, offset, &id, &type);
    if (id != SNIP_TYPE || type != PROTOTYPE_VARIANT)
        printf("Not a snip\n");
    offset = parse_number(buffer, offset, &value);
    
    switch (value) {
        case SNIPTYPE_PING:
            offset = recv_ping(buffer, offset, &value);
            printf("Recive Ping, with count: %d\n", value);
            break;
            
        case SNIPTYPE_PONG:
            offset = recv_pong(buffer, offset, &value);
            printf("Recive Pong, with count: %d\n", value);
            break;
            
        case SNIPTYPE_ERROR:
            printf("Not implementet yet :-/\n");
            return -1;
            break;
            
        case SNIPTYPE_REQUEST:
            offset = recv_request(buffer, offset, &color, &seqId, &meta_offset, &meta_length);
            if (offset == -1) {
                printf("recv_request Faild!\n");
            } else {
                printf("Parse Request, Color: %s, seqId: %d\n",color,seqId);
            }
            offset = parse_metadata(buffer,meta_offset,&frames_per_second, &width, &heigth, &generator_name, &generator_version);
            if (offset == -1) {
                printf("parse Metadata Faild!\n");
            } else {
                printf("Metadata, fps: %d, width: %d, height: %d, gen._name: %s, gen._version: %s\n",frames_per_second,width,heigth,generator_name,generator_version);
            }
            free(color);
            free(generator_name);
            free(generator_version);
            break;
            
        case SNIPTYPE_START:
            printf("Recive Start\n");
            offset = recv_start(buffer, offset);
            break;
            
        case SNIPTYPE_FRAME:
            offset = recv_frame(buffer, offset, &frame_offset, &frame_length);
            if (offset == -1) {
                printf("recv_frame Faild!\n");
            } else {
                printf("Parse Frame, frame_length: %d\n",frame_length);
            }
            frame_offset_start = frame_offset;
            do {
                frame_offset = frame_parse_pixel(buffer,frame_offset, &red, &green, &blue, &x, &y);
                if (offset == -1) {
                    printf("parse Pixel faild\n");
                } else {
                    printf("Parse Pixel, red: %d , green: %d , blue: %d , x: %d , y: %d\n",red,green,blue,x,y);
                }
            } while (frame_offset < (frame_offset_start+frame_length));
            break;
            
        case SNIPTYPE_ACK:
            printf("Recive ACK\n");
            offset = recv_ack(buffer, offset);
            break;
            
        case SNIPTYPE_NACK:
            printf("Recive NACK\n");
            offset = recv_nack(buffer, offset);
            break;
            
        case SNIPTYPE_TIMEOUT:
            printf("Recive Timeout\n");
            offset = recv_timeout(buffer, offset);
            break;
            
        case SNIPTYPE_ABORT:
            printf("Recive Abort\n");
            offset = recv_abort(buffer, offset);
            break;
            
        case SNIPTYPE_EOS:
            printf("Recive EOS\n");
            offset = recv_eos(buffer, offset);
            break;
        default:
            printf("SNIP_TYPE unbekannt\n");
            return -1;
    }
    return offset;
}

void ReadFile(char *name)
{
	FILE *file;
	uint8_t *buffer;
	unsigned long fileLen;
    int offset = 0; // start from the beginning of the stream

    
	//Open file
	file = fopen(name, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s", name);
		return;
	}
	
	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);
    
	//Allocate memory
	buffer=(uint8_t *)malloc(fileLen+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
        fclose(file);
		return;
	}
    
    
	//Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	fclose(file);

    // Add test code here!
       
    offset = test_recv(buffer, offset);
    printf("Offset = %d \n",offset);
	free(buffer);
}

void test_print()
{
    //test
    /*
    char genn[] = "Hallo";
    char genv[] = "99.1";
    int frame_offset , offset_first;
    uint8_t frame[1024];
     */
    int offset;
    uint8_t buffer[2048];
    
    offset = 0;
    
    offset = send_eos(buffer, offset);
        
    printf("New offset is %d\n", offset);
    
    for (int i=0; i< (offset + 10); i++) {
        printf("%.2X ", buffer[i]);
    }
    printf("\n");

}

void self_test()
{
    uint8_t buffer[1024];
    int offset = 0;
    
    offset = send_eos(buffer, offset);
    printf("send_eos, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    }
}