#include <functions.h>

/* 
struct sembuf {
        short sem_num;
        short sem_op;
        short sem_flg;
} */

error_status short_sem_op(int sem_id, int sem_num, int sem_op) {
        struct sembuf tmp;
        tmp.sem_num = sem_num;
        tmp.sem_op = sem_op;
        tmp.sem_flg = 0;
        if (semop(sem_id, &tmp, 1) == -1)
                return IPC_ERROR;
        return COOL;
}



/* 

        is_reg_file     (struct stat)
        
        sem_op

        del_sem

        my_ls

        
        
         */