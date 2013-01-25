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

int main(int argc, char *argv[])
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
    
    //test
    char genn[] = "Hallo";
    char genv[] = "99.1";
    int frame_offset , offset_first;
    uint8_t frame[1024];
    
    /*
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
    */
    /* DEBUG FUNCTION */
    
    offset = 0;
    
    offset = send_eos(buffer, offset);
    
    
    printf("New offset is %d\n", offset);
    
    for (int i=0; i< (offset + 10); i++) {
        printf("%.2X ", buffer[i]);
    }
    printf("\n");

    /*
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
    */
    return 0;
}