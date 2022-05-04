#include "alarm.h"

alarm_t *make_alarm(int seconds, char *msg) {
    alarm_t *alarm = malloc(sizeof(alarm_t));
    alarm->time    = time(NULL);
    alarm->seconds = seconds;
    strncpy(alarm->message, msg, sizeof(alarm->message));
    return alarm;
}

// if the first alarm expires before the second one, this function returns true; otherwise, this function returns false
bool compare_alarm(const void *_first, const void *_second) {
    const alarm_t *first = _first, *second = _second;
    return (first->time + first->seconds) < (second->time + second->seconds);
}

void print_alarm(const void *_alarm) {
    const alarm_t *alarm = _alarm;
    printf("{\n");
    printf("\tseconds: %d\n", alarm->seconds);
    printf("\ttime: %lu\n", alarm->time);
    printf("\texpiry: %lu\n", alarm->time + alarm->seconds);
    printf("\tmsg: %s\n", alarm->message);
    printf("}\n");
}