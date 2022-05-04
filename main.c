#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "alarm.h"
#include "heap.h"

void *alarm_thread(void *arg) {
    binary_heap_t   *alarm_queue = arg;
    alarm_t         *alarm       = malloc(sizeof(alarm_t));
    struct timespec cond_time;
    time_t          now;
    int             status;
    bool            expired;

    /*
     * loop forever, processing commands. the alarm thread will be disintegrated when the process exits. lock the mutex
     * at the start -- it will be unlocked during the condition waits, so the main thread can insert alarms
     */

    status = pthread_mutex_lock(&alarm_queue->mutex);
    if (status != 0) err_abort(status, "lock mutex");
    while (1) {
        /*
         * if the alarm list is empty, wait until an alarm is added
         */
        alarm_queue->max = NULL;
        while (alarm_queue->size == 0) {
            status = pthread_cond_wait(&alarm_queue->cond, &alarm_queue->mutex);
            if (status != 0) err_abort(status, "wait on cond");
        }
        heap_delete(alarm_queue, alarm);
        now     = time(NULL);
        expired = false;
        if (alarm->time + alarm->seconds > now) {
            cond_time.tv_sec  = alarm->time + alarm->seconds;
            cond_time.tv_nsec = 0;
            alarm_queue->max  = alarm;
            while (((alarm_t *) alarm_queue->max)->time + ((alarm_t *) alarm_queue->max)->seconds ==
                   alarm->time + alarm->seconds) {
                status = pthread_cond_timedwait(&alarm_queue->cond, &alarm_queue->mutex, &cond_time);
                if (status == ETIMEDOUT) {
                    expired = true;
                    break;
                }
                if (status != 0) err_abort(status, "cond timedwait");
            }
            if (!expired) heap_insert(alarm_queue, alarm);
        } else expired = true;
        if (expired) {
            printf("(%d) %s\n", alarm->seconds, alarm->message);
        }
    }
}

int main(int argc, char **argv) {

    int       status;
    char      line[128];
    alarm_t   *alarm;
    pthread_t thread;

    binary_heap_t *alarm_queue = malloc(sizeof(binary_heap_t));
    heap(alarm_queue, sizeof(alarm_t), compare_alarm, print_alarm);

    status = pthread_create(&thread, NULL, alarm_thread, alarm_queue);
    if (status != 0) err_abort(status, "create alarm thread");
    while (1) {
        printf("alarm> ");
        if (!fgets(line, sizeof(line), stdin)) exit(0);
        if (strlen(line) <= 1) continue;
        alarm = malloc(sizeof(alarm_t));
        if (!alarm) errno_abort("allocate alarm");

        /*
         * parse input line into seconds (%d) and a message (%64[^\n]), consisting of up to 64 characters separated from
         * seconds by whitespace.
         */
        if (sscanf(line, "%d %64[^\n]", &alarm->seconds, alarm->message) < 2) {
            fprintf(stderr, "bad command\n");
            free(alarm);
        } else {
            status = pthread_mutex_lock(&alarm_queue->mutex);
            if (status != 0) err_abort(status, "lock mutex");
            alarm->time = time(NULL);
            heap_insert(alarm_queue, alarm);
            status = pthread_mutex_unlock(&alarm_queue->mutex);
            if (status != 0) err_abort(status, "unlock mutex");
        }
    }
}
