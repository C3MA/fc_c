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

    char *color;
    int seqId;
    int meta_offset;
    int meta_length;
    int frames_per_second, width, heigth;
    char *generator_name;
    char *generator_version;
    
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
    /*
     int frame_offset;
     int frame_length;
     int red;
     int green;
     int blue;
     int x;
     int y;
     
    offset = parse(buffer, offset, &id, &type);
    if (id != SNIP_TYPE || type != PROTOTYPE_VARIANT)
        printf("Not a snip");
    offset = parse_number(buffer, offset, &value);
    
    
    if (value != SNIPTYPE_FRAME) {
        printf("Not a Frame");
    }
    
    offset = recv_frame(buffer, offset, &frame_offset, &frame_length);
    if (offset == -1) {
        printf("recv_frame Faild!");
    } else {
        printf("Parse Frame, frame_length: %d\n",frame_length);
    }
    
    frame_offset = frame_parse_pixel(buffer,frame_offset, &red, &green, &blue, &x, &y);
    if (offset == -1) {
        printf("parse Pixel faild");
    } else {
        printf("Parse Pixel, red: %d , green: %d , blue: %d , x: %d , y: %d\n",red,green,blue,x,y);
    }
    
    frame_offset = frame_parse_pixel(buffer,frame_offset, &red, &green, &blue, &x, &y);
    if (offset == -1) {
        printf("parse Pixel faild");
    } else {
        printf("Parse Pixel, red: %d , green: %d , blue: %d , x: %d , y: %d\n",red,green,blue,x,y);
    }
    
    frame_offset = frame_parse_pixel(buffer,frame_offset, &red, &green, &blue, &x, &y);
    if (offset == -1) {
        printf("parse Pixel faild");
    } else {
        printf("Parse Pixel, red: %d , green: %d , blue: %d , x: %d , y: %d\n",red,green,blue,x,y);
    }
    */


     
     
    offset = parse(buffer, offset, &id, &type);
    if (id != SNIP_TYPE || type != PROTOTYPE_VARIANT)
        printf("Not a snip");
    offset = parse_number(buffer, offset, &value);
    
    
    if (value != SNIPTYPE_REQUEST) {
        printf("Not a Request");
    }
    
    offset = recv_request(buffer, offset, &color, &seqId, &meta_offset, &meta_length);
    if (offset == -1) {
        printf("recv_request Faild!");
    } else {
        printf("Parse Request, Color: %s, seqId: %d\n",color,seqId);
    }
    
    
    offset = parse_metadata(buffer,meta_offset,&frames_per_second, &width, &heigth, &generator_name, &generator_version);
   
    if (offset == -1) {
        printf("parse Metadata Faild!");
    } else {
        printf("Metadata, fps: %d, width: %d, height: %d, gen._name: %s, gen._version: %s\n",frames_per_second,width,heigth,generator_name,generator_version);
    }
     
    free(color);
    free(generator_name);
    free(generator_version);

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