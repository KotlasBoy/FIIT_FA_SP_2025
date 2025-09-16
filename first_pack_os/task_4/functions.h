

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_COMMAND_LENGTH 128

typedef enum {
    COOL,
    FAILURE,
    FILE_ERROR,
    MEMORY_ERROR,
    NULLPTR,
    WRONG_PARAMETER,
    WIN,
    LOSE
} error_status;

typedef enum {
    LEFT, 
    RIGHT
} side;

typedef struct {
    side boat_side;
    side wolf_side;
    side goat_side;
    side cabbage_side;

    short wolf_in_boat;
    short goat_in_boat;
    short cabbage_in_boat;
    short boat_is_empty;
} positioning;

typedef struct {
    long msg_type;
    char msg_string[MAX_COMMAND_LENGTH];
    pid_t client_id;
    positioning pos;
}   msg_buf;

error_status proccess_command(char *command, positioning *pos, msg_buf *c_msg);

