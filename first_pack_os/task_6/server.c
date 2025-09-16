#include <functions.h>



int main() {
    key_t shm_key, sem_key;
    shm_key = ftok("server", 'Z');
    sem_key = ftok("functions.c", 'Z');
    sh_size *shared_sizes;
    int shm_id = 0, sem_id = 0;
    union semun args;
    unsigned short sem_array[4] = {1, 0, 0, 0};
    
    
    shm_id = shmget(shm_key, sizeof(sh_size), 0666 | IPC_CREAT);
    if (shm_id == -1) {
        printf("shmget error\n");
        return IPC_ERROR;
    }

    shared_sizes = (sh_size *) shmat(shm_id, 0, 0);
    if ( shared_sizes == -1) {
        printf("shamat error\n");
        return IPC_ERROR;
    }

    sem_id = semget(sem_key, SEM_NUM, 0666 | IPC_CREAT);
    if (sem_id == -1) {
        if (shmdt(shared_sizes) == -1)
            printf("shmdt error\n");
        if (shmctl(shm_id, IPC_RMID, 0) == -1)
            printf("scmctl error\n");
        printf("semget\n");
        return IPC_ERROR;
    }

    args.array = sem_array;

    /* 
    shmget

    shnmat

    semget


    semctl SETALL

    /// end of preparation

    sem_op(sem_id, 1, -1)   parse

    dirs = (char **) malloc

    shmget

    shmat

    if sem_op
        write and read in shm
         
        my_ls

        free(dirs)

        // send ans

        if (sem_op 2)

        if(sem_op 3)

        if (shmdt)

        if (shmctl)

        if (sem_op 1)


        // server stopped

        if (shmdt == -1)

        if (shmctl == -1)

     */
}




