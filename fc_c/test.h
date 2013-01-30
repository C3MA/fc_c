//
//  test.h
//  fc_c
//
//  Created by Christian Platz on 27.01.13.
//  Copyright (c) 2013 C3MA. All rights reserved.
//

#ifndef fc_c_test_h
#define fc_c_test_h

#include <stdint.h>

int test_recv(uint8_t *buffer, int offset);
void test_print();
void ReadFile(char *name);
void self_test_request();
void self_test_ping(int counter);
void self_test_pong(int counter);
void self_test_frame();
void self_test_start();
void self_test_ack();
void self_test_nack();
void self_test_timeout();
void self_test_abort();
void self_test_eos();
void self_test_error();
void self_test_sequence();

#endif
