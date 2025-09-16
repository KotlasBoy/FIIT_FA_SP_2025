
//RM ME FIXME:
#include "functions.h"

error_status proccess_command(char *command, positioning *pos, msg_buf *c_msg) {

    printf("\t%s\n", command);

     if (!command || !pos || !c_msg) {
        return NULLPTR;
    }

    if (strncmp(command, "put", 3) == 0){

        pos->cabbage_in_boat = pos->goat_in_boat = pos->wolf_in_boat = 0;
        pos->boat_is_empty = 1;
    }
    
    else if (strncmp(command, "move", 4) == 0) {     
        pos->boat_side = (pos->boat_side + 1) % 2;

        if (pos->cabbage_in_boat) {
            pos->cabbage_side = (pos->cabbage_side + 1) % 2;

            if (pos->goat_side == pos->wolf_side) {
                strcpy(c_msg->msg_string, "LOSE! You left goat and wolf together. You lost\n");
                return LOSE;
            }

            if (pos->cabbage_side == LEFT)
                strcpy(c_msg->msg_string, "Boat and Cabbage on lhe left bank now\n");
            else 
                strcpy(c_msg->msg_string, "Boat and Cabbage on lhe right bank now\n");
        }
        else if (pos->goat_in_boat) {
            pos->goat_side = (pos->goat_side + 1) % 2;

            if (pos->goat_side == LEFT)
                strcpy(c_msg->msg_string, "Boat and Goat on lhe left bank now\n");
            else 
                strcpy(c_msg->msg_string, "Boat and Goat on lhe right bank now\n");
        }
        else if (pos->wolf_in_boat) {
            pos->wolf_side = (pos->wolf_side + 1) % 2;

            if (pos->goat_side == pos->cabbage_side){
                strcpy(c_msg->msg_string, "LOSE! You left goat and cabbage together. You lost\n");
                return LOSE;
            }

            if (pos->wolf_side == LEFT)
                strcpy(c_msg->msg_string, "Boat and Wolf on lhe left bank now\n");
            else 
                strcpy(c_msg->msg_string, "Boat and Wolf on lhe right bank now\n");
        }
        else {
            if (pos->goat_side == pos->wolf_side) {
                strcpy(c_msg->msg_string, "LOSE! You left goat and wolf together. You lost\n");
                return LOSE;
            }

            if (pos->goat_side == pos->cabbage_side){
                strcpy(c_msg->msg_string, "LOSE! You left goat and cabbage together. You lost\n");
                return LOSE;
            }

            if (pos->boat_side == LEFT)
                strcpy(c_msg->msg_string, "Only Boat on lhe left bank now\n");
            else 
                strcpy(c_msg->msg_string, "Only Boat on lhe right bank now\n");
        }

        if (pos->boat_side == pos->goat_side && pos->goat_side  == pos->cabbage_side && pos->cabbage_side == RIGHT){
            strcpy(c_msg->msg_string, "WIN! You won!!!\n");
            return WIN;
        }

        return COOL;
    }

    else if (strncmp(command, "take", 4) == 0) {

        if (!(pos->boat_is_empty)){
            strcpy(c_msg->msg_string, "ERROR! Boat isn't empty\n");
            printf("%d",pos->boat_is_empty);
            return FAILURE;
        }
        else {
            if (strncmp(command + 5, "wolf", 4) == 0) {

                if (pos->boat_side != pos->wolf_side) {
                    strcpy(c_msg->msg_string, "ERROR! Boat and Wolf are on lhe different banks\n");
                    return FAILURE;
                }

                pos->wolf_in_boat = 1;
                pos->boat_is_empty = 0;
                strcpy(c_msg->msg_string, "Wolf in boat\n");
            }
            else if (strncmp(command + 5, "goat", 4) == 0) {
                
                if(pos->goat_side != pos->boat_side) {
                    strcpy(c_msg->msg_string, "ERROR! Boat and Goat are on lhe different banks\n");
                    return FAILURE;
                }

                pos->goat_in_boat = 1;
                pos->boat_is_empty = 0;
                strcpy(c_msg->msg_string, "Goat in boat\n");
            }
            else if (strncmp(command + 5, "cabbage", 7) == 0) {

                if (pos->cabbage_side != pos->boat_side) {
                    strcpy(c_msg->msg_string, "ERROR! Boat and Cabbage are on lhe different banks\n");
                    return FAILURE;
                }
                strcpy(c_msg->msg_string, "Cabbage in boat\n");                
                pos->cabbage_in_boat = 1;
                pos->boat_is_empty = 0;
            }
            else {
                strcpy(c_msg->msg_string, "ERROR! Unknown entity\n");
                return FAILURE;
            }
        }
    }
    
    else {
        strcpy(c_msg->msg_string, "ERROR! Unknown command\n");
        return FAILURE;
    }
 
    return COOL;
}





int main(int argc, char *argv[]) {

    int queue_id;
    msg_buf _msg_get, _msg_put;
    

    key_t unique_key = ftok("./server.c", 'Z');
    if (unique_key == -1) {
        printf("\tftok() error\n");
        return FILE_ERROR;
    }

    queue_id = msgget(unique_key, 0666 | IPC_CREAT);
    if (queue_id == -1) {
        printf("\tmsgget() error\n");
        return FAILURE;
    }

    while(1) {
        
        if (msgrcv(queue_id, &_msg_get, sizeof(msg_buf), 1, 0) == -1){
            printf("\tmsgrcv() error\n");   
            return FAILURE;
        }

        error_status status = proccess_command(_msg_get.msg_string, &_msg_get.pos, &_msg_put);
        _msg_put.msg_type = _msg_get.client_id;
        _msg_put.pos = _msg_get.pos;
        
        if (msgsnd(queue_id, &_msg_put, sizeof(msg_buf), 0) == -1){
            printf("\tmsgsnd() error\n");
            return FAILURE;
        }

        if (status == FAILURE || status == LOSE || status == WIN){
            printf("Programm finished\n");
            //break;
        }

    }

    msgctl(queue_id, IPC_RMID, 0);

    return COOL;

}