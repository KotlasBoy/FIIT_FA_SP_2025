#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SEM_NUM 4

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
                           /* Linux specific part: */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};
#endif

typedef struct {
    size_t b_size;
    size_t count_files;
} sh_size;


typedef enum error_status {
    COOL,
    FAILURE,
    IPC_ERROR,
    SHM_ERROR,
    NULLPTR
} error_status;




