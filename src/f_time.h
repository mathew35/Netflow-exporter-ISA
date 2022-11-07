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

time_t getTime(struct timeval t);

time_t getUptime(struct timeval f, struct timeval l);

time_t sysUptime();

bool timecmp(time_t t1, struct timeval t2, int sec);
#endif
/*** Koniec suboru f_time.h ***/
