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

time_t sysTime() {
    return lastTime.tv_sec;
}

bool checkExport(time_t time, int limit) {
    if (lastTime.tv_sec - time > limit)
        return true;
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

/*** Koniec suboru f_time.c ***/
