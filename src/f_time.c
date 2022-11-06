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

bool checkExport(time_t time, int limit) {
    if (lastTime.tv_sec - time > limit)
        return true;
    return false;
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
time_t sysUptime() {
    return ((lastTime.tv_sec - firstTime.tv_sec) * 1000) + (lastTime.tv_usec - firstTime.tv_usec) / 1000;
}
bool timecmp(struct timeval t1, struct timeval t2, int sec) {
    return ((t1.tv_sec - t2.tv_sec) * 1000 + (t1.tv_usec - t2.tv_usec) / 1000) > sec * 1000;
}

/*** Koniec suboru f_time.c ***/
