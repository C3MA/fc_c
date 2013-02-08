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

int main(int argc, char *argv[])
{
    
    //char datei[] = "/tmp/a/frame_big";
    //self_test_request();
    //self_test_ping(9999999);
    //self_test_pong(5);
    self_test_frame();
    self_test_start();
    self_test_ack();
    self_test_nack();
    self_test_timeout();
    self_test_abort();
    self_test_eos();
    self_test_error(ERRORCODETYPE_OK);
    //ReadFile(datei);
    //return network(argc,argv);
    
    return 0;
}
