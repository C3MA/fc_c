//
//  test.c
//  fc_c
//
//  Created by Christian Platz on 27.01.13.
//  Copyright (c) 2013 C3MA. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "test.h"
#include "fc.h"
#include "proto.h"

void ReadFile(char *name)
{
	FILE *file;
	uint8_t *buffer;
	unsigned long fileLen;
    int id;
    int value;
    int type;
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
    offset = parse(buffer, offset, &id, &type);
    if (type != 0 || id != 1)
    {
        printf("Wrong protobuf file\n");
        return; // wrong protobuf file
    }
    offset = parse_number(buffer, offset, &value);
    switch (value) {
        case 1: /* PING */
        {
            printf("Start PING at offset %d\n", offset);
            offset = recv_ping(buffer, offset, &value);
            printf("Found a value %d\n", value);
            break;
        }
        default:
            break;
    }
    
	free(buffer);
}

void test_print()
{
    //test
    char genn[] = "Hallo";
    char genv[] = "99.1";
    int frame_offset , offset_first;
    uint8_t frame[1024];
    int offset;
    uint8_t buffer[2048];
    
    offset = 0;
    
    offset = send_eos(buffer, offset);
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    
    offset = send_start(buffer, offset);
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    
    offset = send_abort(buffer, offset);
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    
    offset = send_timeout(buffer, offset);
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    
    offset = send_ack(buffer, offset);
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    
    offset = send_nack(buffer, offset);
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    buffer[offset] = 0xFF;
    offset++;
    
    printf("New offset is %d\n", offset);
    
    for (int i=0; i< (offset + 10); i++) {
        printf("%.2X ", buffer[i]);
    }
    printf("\n");

}