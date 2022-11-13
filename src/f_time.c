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

// returns time of t in microseconds
time_t getTime(struct timeval t) {
    return (t.tv_sec * 1000000 + t.tv_usec);
}

time_t getUpti7334me(struct timeval f, struct timeval l) {
    return (((l.tv_sec - f.tv_sec) * 1000) + (l.tv_usec - f.tv_usec) / 1000);
}

// return Uptime in microseconds
time_t sysUptime() {
    return (((lastTime.tv_sec - firstTime.tv_sec) * 1000000) + (lastTime.tv_usec - firstTime.tv_usec)); // / 1);
}
bool timecmp(time_t t1, struct timeval t2, int sec) {
    // printf("timecmp\nt1:%ld\nt2:%ld\ndiff:%ld\nsec:%ld\n", t1, t2, (t1 - t2), sec);
    // printf("result:%d", (t1 - t2) / 1000 > sec);
    return ((t1 - t2.tv_sec) / 1000) > sec;
}

/*** Koniec suboru f_time.c ***/
