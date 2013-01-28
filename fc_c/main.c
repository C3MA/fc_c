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
    self_test();
    //ReadFile(datei);
    //return network(argc,argv);
    
    return 0;
}
