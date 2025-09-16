
#include "functions.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong amount of parameters\n");
        return WRONG_PARAMETER;
    }
    
    msg_buf _msg_get, _msg_put;
    int queue_id;
    FILE *input;

    key_t unique_key = ftok("./server.c", 'Z');
    if (unique_key == -1) {
        printf("\tftok() error\n");
        return FILE_ERROR;
    }

    queue_id = msgget(unique_key, 0);
    if (queue_id == -1) {
        printf("\tmsgget() error\n");
        return FAILURE;
    }

    _msg_put.msg_type = 1;
    _msg_put.client_id = getpid();

    _msg_put.pos.boat_is_empty = 1;
    _msg_put.pos.cabbage_in_boat = _msg_put.pos.goat_in_boat = _msg_put.pos.wolf_in_boat = 0;
    _msg_put.pos.cabbage_side = _msg_put.pos.goat_side = _msg_put.pos.wolf_side = _msg_put.pos.boat_side = LEFT;

    input = fopen(argv[1], "r");
    if (!input) {
        printf("\tfopen error\n");
        return FILE_ERROR;
    }

    while(fgets(_msg_put.msg_string, MAX_COMMAND_LENGTH, input) ){

        //FIXME: RM
        printf("new step, string is: %s", _msg_put.msg_string);
        
        if (msgsnd(queue_id, (void*) &_msg_put, sizeof(msg_buf), 0) == -1) {
            printf("\tmsgsnd() error\n");
            fclose(input);
            return FAILURE;
        }

        sleep(1);

        if (msgrcv(queue_id, (void*) &_msg_get, sizeof(_msg_get), _msg_put.client_id, 0) == -1){
            fclose(input);
            printf("\tmsgrcv() error\n");
            return FAILURE;
        }

        _msg_put.pos =_msg_get.pos;    

        printf("\tServer response:  %s\n", _msg_get.msg_string);

        if (strncmp(_msg_get.msg_string, "ERROR!", 6) == 0){
            fclose(input);
            return FAILURE;
        }
        if (strncmp(_msg_get.msg_string, "LOSE!", 5) == 0){
            fclose(input);
            return FAILURE;
        }
        if (strncmp(_msg_get.msg_string, "WIN!", 4) == 0){
            fclose(input);
            return FAILURE;
        }
    }

    return COOL;
}