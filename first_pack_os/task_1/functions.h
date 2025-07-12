#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define USER_FILE "users_info.dat" 
#define LOGIN_LENGTH 6
#define MAX_STRING_LENGTH 1024
#define MAX_COMMAND_LENGTH 50


typedef enum {
    COOL,
    WRONG_ATTRIBUTES,
    NULLPTR,
    FAILURE,
    OPEN_FILE_ERROR,
    READING_FILE_ERROR,
    MEMORY_ERROR,
    EXIT
                            //TODO: new states
} error_state;

typedef struct User {
    char login[7];
    unsigned int pin_code;
    int sanctions;
    unsigned int command_counter;
} User;


//work with file
error_state get_users_database(User** all_users, size_t *users_amount, size_t *users_capacity);
error_state put_users_database(User** all_users, size_t *users_amount, size_t *users_capacity);


error_state initial_page_func(User** all_users, size_t *users_amount, size_t *users_capacity, User **current_user);

error_state authorization (User** all_users, size_t *users_amount, size_t *users_capacity, User **current_user);
error_state sign_in_func(User** all_users, size_t *users_amount, User **current_user);
error_state sign_up_func(User** all_users, size_t *users_amount, size_t *user_capacity, User **current_user);

// helpers
error_state is_valid_login(char current_login[]);
error_state read_n_chars(char* destination, size_t limit);
error_state string_to_int(const char *str, int *result);
error_state parse_date(const char *date_str, struct tm *tm);
bool is_leap_year(int year);
int days_in_month(int month, int year);

//commands
void time_command();
void date_command();
error_state howmuch_command();
error_state sanctions_command(User **all_users, size_t *users_amount);
