/**
 * ISA
 * @file f_time.h
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow sysTime
 */

// Header Guard
#ifndef F_TIME_H
#define F_TIME_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct timeval sysTime();

void setLatest(struct timeval time);

void setFirst(struct timeval time);

time_t sysUptime();

#endif
/*** Koniec suboru f_time.h ***/
