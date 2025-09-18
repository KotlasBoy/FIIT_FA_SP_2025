#include "functions.h"

int main(){

    User *all_users = NULL;
    size_t users_amount = 0;
    size_t users_capacity = 0;
    User *current_user = NULL;
    error_state state;

    state = get_users_database(&all_users, &users_amount, &users_capacity);
    switch(state){
 
    }

    printf("\tusers_amount: %ld\n\tusers_capacity: %ld\n\n\tLoading...\n", users_amount, users_capacity);
    sleep(1);

    while(1){
        if(current_user == NULL) {
            state = initial_page_func(&all_users, &users_amount, &users_capacity, &current_user);
            if(state == EXIT)
                break;
        }
        else{
            printf("\n\t~~~~~~~~~~~~~~~~~~~~~~\n\tcurrent user's login: %s\n\tcurrent user's pin-code: %d\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~\n",
            current_user->login, current_user->pin_code);

            show_commands();

            char current_command[MAX_COMMAND_LENGTH];

            state = read_n_chars(current_command, MAX_COMMAND_LENGTH);
            if (state != COOL)
                while(getchar() != '\n');

            if (current_user->sanctions != 0 && current_user->command_counter >= current_user->sanctions) {
                printf("\t You are under sancttions. Unfortunately logging out..\n");
                sleep(1);
                current_user->command_counter = 0;
                current_user = NULL;
                continue;
            }

            if (strcmp(current_command, "Time") == 0){
                time_command();
                (current_user->command_counter)++;
                continue;
            }
            else if (strcmp(current_command, "Date") == 0){
                date_command();
                (current_user->command_counter)++;
                continue;
            }
            else if (strcmp(current_command, "Logout") == 0) {
                current_user = NULL;
                printf("Logged out\n");
            }
            else if (strcmp(current_command, "Howmuch") == 0) {
                state = howmuch_command();
                if( state == FAILURE)
                    printf("\t Command failed\n");
                sleep(1);
                (current_user->command_counter)++;
            }
            else if (strcmp(current_command, "Sanctions") == 0) {
                state = sanctions_command(&all_users, &users_amount);
                if(state == FAILURE){
                    printf("\tCommand wasn't completed\n");
                    sleep(1);
                }
                else{
                    printf("\tSanctions were succesfully applied\n");
                    sleep(1);
                    (current_user->command_counter)++;
                }
            }
            else {
                printf("\tUnknown user command\n");
                sleep(1);
                continue;
            }
        }
    }

    state = put_users_database(&all_users, &users_amount, &users_capacity);
    free(all_users);
    switch(state){
 
    }
    return 0;
}