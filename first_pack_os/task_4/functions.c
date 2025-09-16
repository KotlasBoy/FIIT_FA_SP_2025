/* #include "functions.h"

error_status proccess_command(char *command, positioning *pos, msg_buf *c_msg) {

    if (!command || !pos || !c_msg) {
        return NULLPTR;
    }

    if (strncmp(command, "put", 3) == 0){

        pos->cabbage_in_boat = pos->goat_in_boat = pos->wolf_in_boat = 0;
        pos->boat_is_empty = 1;
        printf("\tPut command\n");
    }
    
    else if (strncmp(command, "move", 4) == 0) {     
        pos->boat_side = (!pos->boat_side);
        printf("\tMove command\n");

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

        if (pos->boat_side == pos->goat_side == pos->cabbage_side == RIGHT){
            strcpy(c_msg->msg_string, "WIN! You won!!!\n");
            return WIN;
        }

        return COOL;
    }

    else if (strncmp(command, "take", 4) == 0) {
        printf("\tTake command\n");

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

            }
            else if (strncmp(command + 5, "goat", 4) == 0) {
                
                if(pos->goat_side != pos->boat_side) {
                    strcpy(c_msg->msg_string, "ERROR! Boat and Goat are on lhe different banks\n");
                    return FAILURE;
                }

                pos->goat_in_boat = 1;
            }
            else if (strncmp(command + 5, "cabbage", 7) == 0) {

                if (pos->cabbage_side != pos->boat_side) {
                    strcpy(c_msg->msg_string, "ERROR! Boat and Cabbage are on lhe different banks\n");
                    return FAILURE;
                }
                
                pos->cabbage_in_boat = 1;
            }
            else {
                strcpy(c_msg->msg_string, "ERROR! Unknown entity\n");
                return FAILURE;
            }
        }
        return COOL;
    }
    
    else {
        strcpy(c_msg->msg_string, "ERROR! Unknown command\n");
        return FAILURE;
    }

} */