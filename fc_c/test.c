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
    
    int errorcode;
    char *descr;
    
    offset = parse(buffer, offset, &id, &type);
    if (id != SNIP_TYPE || type != PROTOTYPE_VARIANT){
        printf("Not a snip\n");
        return -1;
    }
    offset = parse_number(buffer, offset, &value);
        
    switch (value) {
        case SNIPTYPE_PING:
            offset = recv_ping(buffer, offset, &value);
            if (offset == -1) {
                printf("recv_ping Faild!\n");
            } else {
                printf("Recive Ping, with count: %d\n", value);
            }
            break;
            
        case SNIPTYPE_PONG:
            offset = recv_pong(buffer, offset, &value);
            if (offset == -1) {
                printf("recv_pong Faild!\n");
            } else {
                printf("Recive Pong, with count: %d\n", value);
            }
            break;
            
        case SNIPTYPE_ERROR:
            offset = recv_error(buffer, offset, &errorcode, &descr);
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
                free(descr);
                }
            }
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
                return -1;
            } else {
                printf("Metadata, fps: %d, width: %d, height: %d, gen._name: %s, gen._version: %s\n",frames_per_second,width,heigth,generator_name,generator_version);
            }
            free(color);
            free(generator_name);
            free(generator_version);
            break;
            
        case SNIPTYPE_START:
            offset = recv_start(buffer, offset);
            if (offset == -1) {
                printf("recv_start Faild!\n");
            } else {
                printf("Recive Start\n");
            }
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
            offset = recv_ack(buffer, offset);
            if (offset == -1) {
                printf("recv_error Faild!\n");
            } else {
                printf("Recive ACK\n"); 
            }
            break;
            
        case SNIPTYPE_NACK:
            offset = recv_nack(buffer, offset);
            if (offset == -1) {
                printf("recv_nack Faild!\n");
            } else {
                printf("Recive NACK\n");
            }
            break;
            
        case SNIPTYPE_TIMEOUT:
            offset = recv_timeout(buffer, offset);
            if (offset == -1) {
                printf("recv_timeout Faild!\n");
            } else {
                printf("Recive Timeout\n");
            }
            break;
            
        case SNIPTYPE_ABORT:
            offset = recv_abort(buffer, offset);
            if (offset == -1) {
                printf("recv_abort Faild!\n");
            } else {
                printf("Recive Abort\n");
            }
            break;
            
        case SNIPTYPE_EOS:
            offset = recv_eos(buffer, offset);
            if (offset == -1) {
                printf("recv_eos Faild!\n");
            } else {
                printf("Recive EOS\n");
            }
            break;
        default:
            printf("SNIP_TYPE unbekannt\n");
            break;
    }
    if (offset == -1){
        //send error
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

void self_test_request()
{
    uint8_t buffer[1024];
    uint8_t meta[1024];
    int offset = 0;
    int offset_meta;
    char gname[] = "Bach";
    char gversion[] = "999.9";
    char color[] = "schwarz";
    
    offset_meta = create_metadata(meta, 0, 23, 42, 44, gname, gversion);
    offset = send_request(buffer, offset, color, 19, meta, offset_meta);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!");
    }
}
    
void self_test_ping(int counter)
{
    uint8_t buffer[1024];
    int offset = 0;
    
    offset = send_ping(buffer, offset, counter);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_pong(int counter)
{
    uint8_t buffer[1024];
    int offset = 0;
    
    offset = send_pong(buffer, offset, counter);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_frame()
{
    uint8_t buffer[1024];
    uint8_t frame[1024];
    int offset = 0;
    int offset_frame = 0;
    
    offset_frame = frame_add_pixel(frame, offset_frame, 255, 255, 255, 1024, 1024);
    offset_frame = frame_add_pixel(frame, offset_frame, 255, 255, 255, 1024, 1024);
    offset_frame = frame_add_pixel(frame, offset_frame, 255, 255, 255, 1024, 1024);
    offset_frame = frame_add_pixel(frame, offset_frame, 255, 255, 255, 1024, 1024);
    offset_frame = frame_add_pixel(frame, offset_frame, 255, 255, 255, 1024, 1024);
    offset_frame = frame_add_pixel(frame, offset_frame, 255, 255, 255, 1024, 1024);
        
    offset = send_frame(buffer, offset, frame, (long)offset_frame);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_start()
{
    uint8_t buffer[1024];
    int offset = 0;
    
    offset = send_start(buffer, offset);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_ack()
{
    uint8_t buffer[1024];
    int offset = 0;
    
    offset = send_ack(buffer, offset);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_nack()
{
    uint8_t buffer[1024];
    int offset = 0;
    
    offset = send_nack(buffer, offset);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_timeout()
{
    uint8_t buffer[1024];
    int offset = 0;
    
    offset = send_timeout(buffer, offset);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_abort()
{
    uint8_t buffer[1024];
    int offset = 0;
    
    offset = send_abort(buffer, offset);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_eos()
{
    uint8_t buffer[1024];
    int offset = 0;
    
    offset = send_eos(buffer, offset);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_error(int errorcode)
{
    uint8_t buffer[1024];
    int offset = 0;
    char descr[] = "Gut";
    
    offset = send_error(buffer, offset, errorcode, descr);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = test_recv(buffer, offset);
        printf("Offset: %d \n",offset);
    } else {
        printf("Fehler beim Senden!\n");
    }
}

void self_test_sequence()
{
    uint8_t buffer[1024];
    uint8_t frame[1024];
    int offset = 0;
    
    uint8_t meta[1024];
    char gname[] = "Bach";
    char gversion[] = "999.9";
    
    int meta_offset;
    int meta_length;
    int frames_per_second, width, heigth;
    char *generator_name;
    char *generator_version;
    
    int frame_offset = 0;
    int frame_offset_start;
    int frame_length;
    int red;
    int green;
    int blue;
    int x;
    int y;
    
    meta_offset = create_metadata(meta, 0, 23, 42, 44, gname, gversion);
    
    frame_offset = frame_add_pixel(frame, frame_offset, 255, 255, 255, 1024, 1024);
    frame_offset = frame_add_pixel(frame, frame_offset, 255, 255, 255, 1024, 1024);
    frame_offset = frame_add_pixel(frame, frame_offset, 255, 255, 255, 1024, 1024);
    frame_offset = frame_add_pixel(frame, frame_offset, 255, 255, 255, 1024, 1024);
    frame_offset = frame_add_pixel(frame, frame_offset, 255, 255, 255, 1024, 1024);
    frame_offset = frame_add_pixel(frame, frame_offset, 255, 255, 255, 1024, 1024);
    
    offset = create_sequence(buffer, offset, meta, (long) meta_offset, frame, (long) frame_offset);
    printf("send, offset: %d \n",offset);
    if (offset != -1) {
        offset = 0;
        
        offset = parse_sequence(buffer, offset, &meta_offset, &meta_length, &frame_offset, &frame_length);
        meta_offset = parse_metadata(buffer, meta_offset, &frames_per_second, &width, &heigth, &generator_name, &generator_version);
        if (offset == -1) {
            printf("parse Metadata Faild!\n");
        } else {
            printf("Metadata, fps: %d, width: %d, height: %d, gen._name: %s, gen._version: %s\n",frames_per_second,width,heigth,generator_name,generator_version);
        }
        free(generator_name);
        free(generator_version);
        
        frame_offset_start = frame_offset;
        do {
            frame_offset = frame_parse_pixel(buffer,frame_offset, &red, &green, &blue, &x, &y);
            if (offset == -1) {
                printf("parse Pixel faild\n");
            } else {
                printf("Parse Pixel, red: %d , green: %d , blue: %d , x: %d , y: %d\n",red,green,blue,x,y);
            }
        } while (frame_offset < (frame_offset_start+frame_length));

        printf("Offset: %d \n",offset);
    } else {
        printf("Create Error!\n");
    }
}