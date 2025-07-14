#include <stdio.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define STATE_TO_STRING(STATE) ((STATE) == EMPTY ? "EMPTY" : (STATE) == MAN ? "MAN" : "WOMAN")
#define STR_MAX_LENGTH 10
#define VISITORS_MAX_NUMBER 15

typedef enum {
    EMPTY,
    MAN,
    WOMAN
} bath_status;

typedef enum {
    COOL,
    FAILURE,
    NULLPTR
} error_status;

typedef struct {
    size_t capacity;
    size_t size;
    bath_status status;
    int sem_id;
} Bath;

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};

error_status sem_init(Bath *current_bath);
error_status string_to_size_t (char str[], size_t *dest);

void* start_routine(void* arg);

error_status woman_wants_to_enter(Bath *current_bath);
error_status man_wants_to_enter(Bath *current_bath);
error_status woman_leaves(Bath *current_bath);
error_status man_leaves(Bath *current_bath);
