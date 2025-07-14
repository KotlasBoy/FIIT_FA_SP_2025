#include "functions.c"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong amount of parameters\n");
        return FAILURE;
    }
    
    size_t bath_cap;
    error_status status;
    status = string_to_size_t(argv[1], &bath_cap);
    if (status != COOL) {
        printf("input error\n");
        return 0;
    }


    pthread_t threads[VISITORS_MAX_NUMBER];

    Bath current_bath;
    current_bath.capacity = bath_cap;
    current_bath.status = EMPTY;
    current_bath.size = 0;

    status = sem_init(&current_bath);

    srand(time(0));

    for(int i = 0; i < VISITORS_MAX_NUMBER; ++i) {
        pthread_create(&(threads[i]), NULL, start_routine, (void*) &current_bath);     
        if (status != COOL){
            printf("thread error\n");
            for (int j = 0; j < i; ++j)
                pthread_join(threads[i], NULL);
            return FAILURE;
        }
    }

    for(int i = 0; i < VISITORS_MAX_NUMBER; ++i) {
        pthread_join(threads[i], NULL);
    }

    semctl(current_bath.sem_id, 0, IPC_RMID, (int) 0);

    return COOL;
}