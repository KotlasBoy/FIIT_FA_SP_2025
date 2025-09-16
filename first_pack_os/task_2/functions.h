#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
//#include <limits.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h> ???
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define FILENAME_MAX 512

typedef enum {
    COOL,
    FAILURE, 
    MEMORY_ERROR, 
    NULLPTR,
    INPUT_ERROR,
    WRONG_PARAMETER,
    OVERFLOW,
    FILE_ERROR
} error_status;


error_status string_to_uint32(uint32_t *result, const char *str);
error_status string_to_mask(uint32_t *result, const char *str);

error_status xor_n (char *path, short n);
error_status mask_hex (char *path, char *mask);
error_status copy_n (char *path, uint32_t n);
error_status deep_copy (char* dest_path, char* src_path);

error_status find_str(char **paths, int amount_of_paths, const char *to_find, char *found_in, char *flag_found);