//
//  main.c
//  fc_c
//

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "fc.h"
#include "test.h"
#include "util.h"

int network(int argc, char *argv[])
{
    int sockfd = 0;
    long n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;
    uint8_t buffer[2048];
    uint8_t output[2048];
    int offset;
    int counter;
    int type=-1;
    int length = 0;
    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }
    
    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NETWORK_PORT);
    
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }
    
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
    
    offset = send_ping(buffer, 0, 42);
    add_header(buffer, output, offset);
    write(sockfd, output, offset+HEADER_LENGTH);
    
    do {
        n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
        if (n < HEADER_LENGTH) {
            printf("\n Error : Network read error\n");
            return 1;
        }
        offset = get_header((uint8_t*)recvBuff, 0, &type, &length);
        if (offset == -1) {
            printf("\n Error : Could not analyze header\n");
            return 1;
        }
        printf("typ: %d laenge: %d\n",type,length);
    } while (type == -1);
    
    switch (type) {
        case 2:
            recv_pong((uint8_t*)recvBuff, offset, &counter);
            printf("Pong Counter: %d\n",counter);
            break;
            
        default:
            printf("Unknown type: %d",type);
            break;
    }
    return 0;
}

void test_ascii_convertion()
{
	char buffer[5];
	memset( &buffer, 0, sizeof(buffer));
	fc_util_itoa_rightalign(buffer, 4, 3);
	printf("%d is\t'%s'\n", 3, buffer);
	fc_util_itoa_rightalign(buffer, 4, 12);
	printf("%d is\t'%s'\n", 12, buffer);
	fc_util_itoa_rightalign(buffer, 4, 456);
	printf("%d is\t'%s'\n", 456, buffer);
	fc_util_itoa_rightalign(buffer, 4, 7890);
	printf("%d is\t'%s'\n", 7890, buffer);
	fc_util_itoa_rightalign(buffer, 4, 66666);
	printf("%d is\t'%s'\n", 66666, buffer);
}

int main(int argc, char *argv[])
{
    //char datei[] = "/tmp/a/frame_big";
    //self_test_request();
    //self_test_ping(9999999);
    //self_test_pong(5);
    //self_test_frame();
    //self_test_start();
    //self_test_ack();
    //self_test_nack();
    //self_test_timeout();
    //self_test_abort();
    //self_test_eos();
    //self_test_error(ERRORCODETYPE_OK);
    //self_test_sequence();
    //self_test_inforequest();
    //self_test_infoanswer();
    //ReadFile(datei);
    
	//return network(argc,argv);
    
	test_ascii_convertion();
	
    return 0;
}