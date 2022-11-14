/**
 * ISA
 * @file f_time.c
 * @authors Matus Vrablik (xvrab05)
 * @brief NetFlow sysTime
 */

#include "f_time.h"
struct timeval lastTime;
struct timeval firstTime;
bool firstSet = false;

struct timeval sysTime() {
    return lastTime;
}

void setLatest(struct timeval time) {
    lastTime = time;
}

void setFirst(struct timeval time) {
    if (!firstSet) {
        firstTime = time;
        firstSet = true;
    }
}
// return Uptime in microseconds
time_t sysUptime() {
    return (((lastTime.tv_sec - firstTime.tv_sec) * 1000000) + (lastTime.tv_usec - firstTime.tv_usec));
}

/*** Koniec suboru f_time.c ***/
