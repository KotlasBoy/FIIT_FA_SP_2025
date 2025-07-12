#include "functions.h"


error_state initial_page_func(User** all_users, size_t *users_amount, size_t *users_capacity, User **current_user) {
    system("clear");
    printf("\n\n\t\tWelcome to primitive command Line Shell!\n\n \
    \t\tPress <enter> to begin.\n\n");

    printf("\t\t  __\n \
    \t\t>(. )\n \
    \t\t |  (     /)\n \
    \t\t |   \\___/ )\n \
    \t\t (   ----- )  >@)_//   >@)_//  >@)_//  >@)_//\n \
    \t\t  \\_______/    (__)     (__)    (__)    (__)\n \
    \t\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n\n");

    char ch;
    while ((ch = getchar()) != '\n') {}

    error_state state;
    state = authorization(all_users, users_amount, users_capacity, current_user);

    return state;
}

error_state authorization (User** all_users, size_t *users_amount, size_t *users_capacity, User **current_user) {
    system("clear");
    printf("\n\n\t\tAuthorization menu\n\n \
    \t- to sign in write <1>\n \
    \t- to sign up write <2>\n \
    \t- to exit <3> \n\n");

    error_state state;

    int ch = getchar();

    switch (ch) {
        case '1':
            while((ch = getchar()) != '\n');
            state = sign_in_func(all_users, users_amount, current_user);
            break;
        case '2':
            while((ch = getchar()) != '\n');
            state = sign_up_func(all_users, users_amount, users_capacity, current_user);
            break;
        case '3':
            while((ch = getchar()) != '\n');
            state = EXIT;
            break;
        default:
            state = WRONG_ATTRIBUTES;
            system("clear");
            printf("\n\n\t\tWrong parameters passed. Going to initial page...\n");
            while(ch != '\n')
                ch = getchar();
            sleep(2);
            break;
    }
    return state;
}

error_state sign_up_func(User** all_users, size_t *users_amount, size_t *users_capacity, User **current_user) {
    system("clear");
    printf("\n\n\tSign up\n\n");

    char current_login[LOGIN_LENGTH + 1];
    char str_pin[LOGIN_LENGTH + 1];
    int current_pin_code;
    error_state state;

    printf("\tEnter login:\n\t");
    state = read_n_chars(current_login, LOGIN_LENGTH + 1);
    switch(state){
        case COOL:
            if ((state = is_valid_login(current_login)) != COOL) {
                printf("\tInvalid login\n");
                sleep(2);
                return FAILURE;
            }
            break;
        case (NULLPTR):
            printf("\tINTERNAL_ERROR\n");
            sleep(2);
            return FAILURE;
        case (WRONG_ATTRIBUTES):
            printf("\tInvalid login\n");
            sleep(2);
            return FAILURE;
        default:
            printf("\tUnknown error\n");
            sleep(2);
            return FAILURE;
    }

    for(int i = 0; i < *users_amount; ++i) {
        if (strcmp(current_login, (*all_users)[i].login) == 0) {
            printf("\tNot unique login\n");
            sleep(2);
            return FAILURE;
        }
    }

    printf("\n\tEnter pin-code:\n\t");
    
    if (scanf("%6s", str_pin) != 1 || string_to_int(str_pin, &current_pin_code) || current_pin_code < 0 || current_pin_code > 100000)
    {
        while (getchar() != '\n');
        printf("\tInvalid pin-code format\n");
        sleep(2);
        return FAILURE;
    }

    if (*users_amount == *users_capacity) {
        User* tmp = (User*) realloc(*all_users, sizeof(User) * (*users_capacity + 5));
        if(!tmp){
            printf("\tINNER ERROR\n");
            sleep(2);
            return MEMORY_ERROR;
        }
        *users_capacity += 5;
        *all_users = tmp;
    }

    strcpy((*all_users)[*users_amount].login, current_login);
    (*all_users)[*users_amount].pin_code = current_pin_code;
    (*all_users)[*users_amount].sanctions = 0;
    (*all_users)[*users_amount].command_counter = 0;

    *current_user = &(*all_users)[*users_amount];
    ++(*users_amount);

    return COOL;
}

error_state sign_in_func(User** all_users, size_t *users_amount, User **current_user) {

    system("clear");

    printf("\n\n\tSign in\n\n");

    char current_login[LOGIN_LENGTH + 1];
    char str_pin[7];
    int current_pin_code;
    error_state state;

    printf("\tEnter login:\n\t");
    state = read_n_chars(current_login, LOGIN_LENGTH + 1);
    switch(state){
        case COOL:
            if ((state = is_valid_login(current_login)) != COOL) {
                printf("\tInvalid login\n");
                sleep(2);
                return FAILURE;
            }
            break;
        case (NULLPTR):
            printf("\tINTERNAL_ERROR\n");
            sleep(2);
            return FAILURE;
        case (WRONG_ATTRIBUTES):
            printf("\tInvalid login\n");
            sleep(2);
            return FAILURE;
        default:
            printf("\tUnknown error\n");
            sleep(2);
            return FAILURE;
    }

    printf("\n\tEnter pin-code:\n\t");

    if (scanf("%6s", str_pin) != 1 || string_to_int(str_pin, &current_pin_code) || current_pin_code < 0 || current_pin_code > 100000)
    {
        while (getchar() != '\n')
            ;
        printf("\tInvalid pin-code\n");
        sleep(2);
        return FAILURE;
    }

    User *found = NULL;
    for(int i = 0; i < *users_amount; ++i) {
        if (strcmp(current_login, (*all_users)[i].login) == 0){
            found = &(*all_users)[i];
            break;
        }
    }

    if (!found){
        printf("\tUser was not found\n");
        sleep(2);
        return FAILURE;
    }

    if (current_pin_code != found->pin_code) {
        printf("\tWrong password\n");
        sleep(2);
        return FAILURE;
    }

    *current_user = found;
    return COOL;
}

error_state is_valid_login (char current_login[]) {
    if (!(*current_login) || strlen(current_login) < 1)
        return WRONG_ATTRIBUTES;
    
    for (int i = 0; i < strlen(current_login); ++i) {
        if (isalnum((current_login)[i]) == 0)
           return WRONG_ATTRIBUTES;
    }

    return  COOL;
}

error_state read_n_chars(char* destination, size_t limit) {
    if (!destination)
        return NULLPTR;

    char ch;
    size_t counter;

    while((isspace(ch = getchar())))
    ;

    for (counter = 0; counter < limit; ++ counter) {

        if (isspace(ch)){
            destination[counter] = '\0';
            return COOL;
        }

        destination[counter] = ch;
        ch = getchar();

    }

    if(!isspace(ch)){
        while((ch = getchar()) != '\n')
        ;
        return WRONG_ATTRIBUTES;
    }
        
    return COOL;
}

error_state string_to_int(const char *str, int *result)
{
    char *endinp;
    long res;
    if (!str || !result)
        return MEMORY_ERROR;

    res = strtol(str, &endinp, 10);
    if (res >= INT_MAX || res <= INT_MIN)
        return WRONG_ATTRIBUTES;
    if (*endinp != '\0')
        return WRONG_ATTRIBUTES;
    *result = (int)res;
    return COOL;
}

void show_commands()
{
    printf("\n\tAvailable commands:\n");
    printf("\tTime\n");
    printf("\tDate\n");
    printf("\tHowmuch <time> flag\n");
    printf("\tLogout\n");
    printf("\tSanctions username <number>\n");
    printf("\n");
}

void time_command(){
    time_t tt = time(NULL);
    struct tm *now = localtime(&tt);
    printf("\tTime: %d:%d:%d\n", now->tm_hour, now->tm_min, now->tm_sec);
    sleep(2);
}

void date_command(){
    time_t tt = time(NULL);
    struct tm *now = localtime(&tt);
    printf("\tDate: %d.%d.%d\n", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
    sleep(2);
}

error_state sanctions_command(User **all_users, size_t *users_amount) {
    char flag_user[MAX_STRING_LENGTH], number[MAX_COMMAND_LENGTH], approve[MAX_STRING_LENGTH];

    error_state state;
    state = read_n_chars(flag_user, MAX_STRING_LENGTH);
    if(state != COOL) {
        char tmp_ch;
        while ((tmp_ch = getchar()) != '\n');
        printf("\\tInvalid parameters\n");
        sleep(2);
        return FAILURE;
    }

    state = read_n_chars(number, MAX_STRING_LENGTH);
    if(state != COOL) {
        char tmp_ch;
        while ((tmp_ch = getchar()) != '\n');
        printf("\tInvalid parameters\n");
        sleep(2);
        return FAILURE;
    }

    int new_limit;
    state = string_to_int(number, &new_limit);
    if (state != COOL) {
        printf("\t Something wrong with number\n");
        sleep(2);
        return FAILURE;
    }

    User *found = NULL;
    for(int i = 0; i < *users_amount; ++i) {
        if (strcmp(flag_user, (*all_users)[i].login) == 0){
            found = &(*all_users)[i];
            break;
        }
    }

    if (!found){
        printf("\tUser was not found\n");
        sleep(2);
        return FAILURE;
    }

//approve will be lower
    printf("\t Enter <approve code>:\n\t");

    state = read_n_chars(approve, MAX_STRING_LENGTH);
    if(state != COOL) {
        char tmp_ch;
        while ((tmp_ch = getchar()) != '\n');
        printf("\\tInvalid parameters\n");
        sleep(2);
        return FAILURE;
    }
    else if(strcmp(approve, "12345") != 0){
        printf("\tWrong <approve code>. Delayed\n");
        sleep(2);
        return FAILURE;
    }

    found->sanctions = new_limit;

    return COOL;

}

error_state howmuch_command() {
    char time_str[12], flag[3];
    if (read_n_chars(time_str, 10) || read_n_chars(flag, 2) || flag[0] != '-' ||
        (flag[1] != 's' && flag[1] != 'm' && flag[1] != 'h' && flag[1] != 'y')) {
            while (getchar() != '\n')
                ;
            printf("\tInput error. Usage: Howmuch <time> flag (-s -m -h -y).\n");
            sleep(2);
            return FAILURE;
    }

    struct tm start_tm;
    time_t start_time, current_time;
    double diff;

    if (!time_str || !flag)
        return MEMORY_ERROR;

    if (parse_date(time_str, &start_tm))
        return FAILURE;

    int year = start_tm.tm_year + 1900;
    int month = start_tm.tm_mon + 1;
    int day = start_tm.tm_mday;

    if (year < 1900 || month < 1 || month > 12 || day < 1 || day > days_in_month(month, year))
        return FAILURE;

    start_time = mktime(&start_tm);
    if (start_time == -1)
        return FAILURE;

    current_time = time(NULL);

    if (current_time == -1)
        return FAILURE;

    diff = difftime(current_time, start_time);

    if (strcmp(flag, "-s") == 0)
    {
        printf("%.0lf seconds\n", floor(diff));
        sleep(2);
    }
    else if (strcmp(flag, "-m") == 0)
    {
        printf("%.0lf minutes\n", floor(diff / 60.0));
        sleep(2);
    }
    else if (strcmp(flag, "-h") == 0)
    {
        printf("%.0lf hours\n", floor(diff / 3600.0));
        sleep(2);
    }
    else if (strcmp(flag, "-y") == 0)
    {
        printf("%.0lf years\n", floor(diff / (365.25 * 24 * 3600)));
        sleep(2);
    }
    else
    {
        return FAILURE;
    }
    return COOL;

}

error_state parse_date(const char *date_str, struct tm *tm)
{
    int day, month, year;
    if (!date_str || !tm)
        return MEMORY_ERROR;

    if (sscanf(date_str, "%d:%d:%d", &day, &month, &year) != 3)
        return FAILURE;

    tm->tm_mday = day;
    tm->tm_mon = month - 1;
    tm->tm_year = year - 1900;
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
    tm->tm_isdst = -1;

    return COOL;
}


bool is_leap_year(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(int month, int year)
{
    int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap_year(year))
    {
        return 29;
    }
    return days[month];
}


error_state get_users_database(User** all_users, size_t *users_amount, size_t *users_capacity){
    if (!all_users || !users_amount || !users_capacity)
        return NULLPTR;
    
    FILE* database = NULL;
    
    database = fopen(USER_FILE, "rb");

    if (!database && (*users_amount == 0))
        return COOL;
    else if (!database && (*users_amount == 1))
        return OPEN_FILE_ERROR;
    
    if (fread(users_amount, sizeof(size_t), 1, database) != 1){
        fclose(database);
        return READING_FILE_ERROR;
    }

    *all_users = (User*) malloc(sizeof(User) * *users_amount);
    if (!*all_users){
        fclose(database);
        return MEMORY_ERROR;
    }

    if (fread(*all_users, sizeof(User), *users_amount, database) != *users_amount){
        fclose(database);
        free(*all_users);
        return READING_FILE_ERROR;
    }

    *users_capacity = *users_amount;
    fclose(database);
    return COOL;
    
}

error_state put_users_database(User** all_users, size_t *users_amount, size_t *users_capacity) {

    if (!all_users || !users_amount || !users_capacity)
        return NULLPTR;
    
    FILE* database = fopen(USER_FILE, "wb");
    if (!database && (users_amount == 0))
        return OPEN_FILE_ERROR;

    if (fwrite(users_amount,  sizeof(size_t), 1, database) != 1){
        fclose(database);
        return READING_FILE_ERROR;
    }

    if(fwrite(*all_users, sizeof(User), *users_amount, database) != *users_amount) {
        fclose(database);
        return READING_FILE_ERROR;
    }

    fclose(database);
    return COOL;
}