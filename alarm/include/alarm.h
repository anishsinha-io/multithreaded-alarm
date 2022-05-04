#ifndef __ALARM__
#define __ALARM__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct alarm_tag {
    int    seconds;
    time_t time;
    char   message[64];
} alarm_t;

bool compare_alarm(const void *_first, const void *_second);
void print_alarm(const void *_alarm);
alarm_t *make_alarm(int seconds, char *msg);

#endif