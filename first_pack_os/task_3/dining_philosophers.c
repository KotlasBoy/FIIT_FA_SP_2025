#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N 5

typedef enum {
    THINKING,
    HUNGRY,
    EATING
} philosopher_state;

typedef struct {
    unsigned int id;
    philosopher_state *state;
    pthread_mutex_t *S;
} passing_argument;

pthread_mutex_t the_mutex;      //<<<<<<<<<<<<<<<<<<<<<<<<<<<< mutex
void* philosopher (void* row_struct);

void take_fork (unsigned int phil_id, philosopher_state **state, pthread_mutex_t *S);

void put_fork (unsigned int phil_id, philosopher_state **state, pthread_mutex_t *S);

void test (unsigned int phil_id, philosopher_state **state, pthread_mutex_t *S);

int main() {
    philosopher_state state[N];
    pthread_mutex_t S[N];
    passing_argument philosophers[N];
    pthread_t threads[N];

    srand(time(NULL));

    for(int i = 0; i < N; ++i){             
        philosophers[i].id = i;
        
        pthread_mutex_init (&S[i], NULL);
        state[i] = THINKING;

        philosophers[i].S = S;
        philosophers[i].state = state;
    }

    pthread_mutex_init (&the_mutex, NULL);

    for(int i = 0; i < N; ++i) {
        printf("Philosopher %u is THINKING\n", i);
        pthread_create (&(threads[i]), NULL, philosopher, (void*) &(philosophers[i]));    //TODO: fill in right attributes
    }

    for(int i = 0; i < N; ++i) {
        pthread_join (threads[i], NULL);
        pthread_mutex_destroy(&(S[i]));
    }

    pthread_mutex_destroy(&the_mutex);

    return 0;
}



void* philosopher (void* row_struct) {
    unsigned int phil_id = ((passing_argument*) row_struct)->id;
    philosopher_state *state = ((passing_argument*) row_struct)->state;
    pthread_mutex_t *S = ((passing_argument*) row_struct)->S;

    while(1) {
        sleep(rand() % 3);

        take_fork(phil_id, &state, S);

        sleep(rand() % 3);

        put_fork(phil_id, &state, S);
    }
}

void take_fork (unsigned int phil_id, philosopher_state **state, pthread_mutex_t *S) {

    pthread_mutex_lock(&the_mutex);
    
    (*state)[phil_id] = HUNGRY;
    printf("Philosopher %u is HUNGRY\n", phil_id);

    test(phil_id, state, &(S[phil_id]));

    pthread_mutex_unlock(&the_mutex);

    pthread_mutex_lock(&(S[phil_id])) ;

    sleep(1);
}

void put_fork (unsigned int phil_id, philosopher_state **state, pthread_mutex_t *S) {

    pthread_mutex_lock(&the_mutex);

    printf("Philosopher %u is putting down forks %u and %u\n", phil_id, (phil_id + N  - 1) % N, phil_id);
    (*state)[phil_id] = THINKING;
    printf("Philosopher %u is THINKING\n", phil_id);

    test((phil_id + 1) % N, state, &(S[(phil_id + 1) % N]));
    test((phil_id + N - 1) % N, state, &(S[(phil_id + N - 1) % N]));

    pthread_mutex_unlock(&the_mutex);
}

void test (unsigned int phil_id, philosopher_state **state, pthread_mutex_t *S) {
    if ((*state)[phil_id] == HUNGRY && (*state)[(phil_id + 1) % N] != EATING && (*state)[(phil_id + N - 1) % N] != EATING ) {
        (*state)[phil_id] = EATING;

        printf("Philosopher %u is picking up forks %u and %u\n", phil_id, (phil_id + N  - 1) % N, phil_id);
        printf("Philosopher %u is EATING\n", phil_id);
        pthread_mutex_unlock(S);
    }
}
