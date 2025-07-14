#include "functions.h"

error_status sem_init(Bath* current_bath) {
    if (!current_bath)
        return NULLPTR;

    key_t unique_key;
    unique_key = ftok("./main.c", 'Z');

    current_bath->sem_id = semget(unique_key, 1, 0666 | IPC_CREAT);
    if (current_bath->sem_id == -1)
        return FAILURE;
     union semun current_semun;
    current_semun.val = current_bath->capacity; 
    if (semctl(current_bath->sem_id, 0, SETVAL, current_semun) == -1)
        return FAILURE;;

    return COOL;
}

error_status man_wants_to_enter (Bath* current_bath) {
    if (!current_bath)
        return NULLPTR;
    
    struct sembuf operation;

    operation.sem_num = 0;
    operation.sem_op = -1;     
    operation.sem_flg = 0;

    if (semop(current_bath->sem_id, &operation, 1) == -1)           //take semaphore
        return FAILURE;
    
    while(current_bath->status == WOMAN || current_bath->size == current_bath->capacity) {
        
        operation.sem_op = 1;
        if (semop(current_bath->sem_id, &operation, 1) == -1)       //release semaphore
            return FAILURE;

        operation.sem_op = -1;
        if (semop(current_bath->sem_id, &operation, 1) == -1)       //take semaphore
            return FAILURE;
    }
    printf("\tMan entered\n");

    current_bath->status = MAN;
    current_bath->size += 1;

    return COOL;
}

error_status woman_wants_to_enter (Bath* current_bath) {
    if (!current_bath)
        return NULLPTR;
    
    struct sembuf operation;

    operation.sem_num = 0;
    operation.sem_op = -1;      // take semaphore
    operation.sem_flg = 0;

    if (semop(current_bath->sem_id, &operation, 1) == -1)
        return FAILURE;
    
    while(current_bath->status == MAN || current_bath->size == current_bath->capacity) {
        
        operation.sem_op = 1;
        if (semop(current_bath->sem_id, &operation, 1) == -1)       //release semaphore
            return FAILURE;

        operation.sem_op = -1;
        if (semop(current_bath->sem_id, &operation, 1) == -1)       //take semaphore
            return FAILURE;
    }

    printf("\tWoman entered\n");

    current_bath->status = WOMAN;
    current_bath->size += 1;

    return COOL;
}

error_status man_leaves (Bath* current_bath) {
    if (!current_bath)
        return FAILURE;

    struct sembuf operation;
    operation.sem_num = 0;
    operation.sem_op = 1;     
    operation.sem_flg = 0;

    if (semop(current_bath->sem_id, &operation, 1) == -1)               //FUCKUP HERE
        return FAILURE;             

    printf("\tMan left the bathroom\n");

    if(current_bath->size == 1)
        current_bath->status = EMPTY;

    current_bath->size -= 1;

    return COOL;
}

error_status woman_leaves (Bath* current_bath) {
    if (!current_bath)
        return FAILURE;

    struct sembuf operation;

    operation.sem_num = 0;
    operation.sem_op = 1;     
    operation.sem_flg = 0;

    if (semop(current_bath->sem_id, &operation, 1) == -1)       
        return FAILURE;

    if(current_bath->size == 1)             ///
        current_bath->status = EMPTY;

    current_bath->size -= 1;

    printf("\tWoman left the bathroom\n");

    return COOL;
}

void* start_routine(void* arg){
    if (!arg)
        return (void*)0;
    
    Bath *current_bath = (Bath*) arg;

    int tmp = rand() % 2;
    if (tmp) {
        printf("\tMan wants to enter\n");
        man_wants_to_enter(current_bath);

        sleep(rand() % 2);

        man_leaves(current_bath);
    }
    else {
        printf("\tWoman wants to enter\n");
        woman_wants_to_enter(current_bath);

        sleep(rand() % 2);
        
        woman_leaves(current_bath);
    }
    return (void*)0;
}

error_status string_to_size_t (char str[], size_t *dest){
    if (!str || !dest || strlen(str) == 0)
        return NULLPTR;

    size_t i;
    *dest = 0;
    for(i = 0; i < STR_MAX_LENGTH ; ++i) {

        if (str[i] == '\0')
            break;

        else if (isdigit(str[i])) {
            *dest *= 10;
            *dest += str[i] - '0';
        }
        else 
            return FAILURE;
    }
    if (*dest > VISITORS_MAX_NUMBER)
        return FAILURE;

    return COOL;
}