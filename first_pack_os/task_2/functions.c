#include "functions.h"


error_status string_to_uint32(uint32_t *result, const char *str)
{
    char *endinp;
    unsigned long res;

    if (!str || !result)
        return MEMORY_ERROR;

    res = strtoul(str, &endinp, 10);
    if (res > UINT32_MAX)
        return INPUT_ERROR;
    if (*endinp != '\0')
        return INPUT_ERROR;

    *result = (uint32_t)res;
    return COOL;
}

error_status string_to_mask(uint32_t *result, const char *str)
{
    char *endinp;
    unsigned long res;

    if (!str || !result)
        return MEMORY_ERROR;

    res = strtoul(str, &endinp, 16);
    if (res > UINT32_MAX)
        return INPUT_ERROR;
    if (*endinp != '\0')
        return INPUT_ERROR;

    *result = (uint32_t)res;
    return COOL;
}

error_status mask_hex(char *path, char *mask) {
    if (!path || !mask)
        return NULLPTR;
    
    FILE *input;
    char *abs_path;
    uint32_t num_mask, result = 0;
    unsigned int buffer = 0;
    if(string_to_mask(&num_mask, mask) != COOL)
        return FAILURE;
    
    if (get_absolute_path(path, abs_path) != COOL){
        printf("Problem to find <%s> path\n", path);
        return FAILURE;
    }
    input = fopen(abs_path, "rb");
    if (!input)
        return FILE_ERROR;


    while (fread(buffer, sizeof(unsigned int), 1, input) > 0){
        if ((buffer & num_mask) > 0)
            ++result;
    }
    if (result)
        printf("\tThe result of operattion is %lu\n", result);
    else
        printf("\tOperation was failed\n");

    return COOL;
}

error_status xorN (char *path, short n){
    if (!path)
        return NULLPTR;
    
    char *abs_path;
    if (get_absolute_path(path, abs_path) != COOL){
        printf("Problem to find <%s> path\n", path);
        return FAILURE;
    }
    
    unsigned short buffer_length = 1;
    FILE *input = fopen(abs_path, "rb");
    if (!input)
        return FILE_ERROR;

    char *buffer, *result;
    for(int i = 0; i < n; ++i) 
        buffer_length *= 2;

    buffer = (char *) malloc(sizeof(unsigned char) * buffer_length);
    if (!buffer) {
        fclose(input);
        return MEMORY_ERROR;
    }

    result = (char*) malloc(sizeof(unsigned char) * buffer_length);
    if (!result) {
        fclose(input);
        free(buffer);
        return MEMORY_ERROR;
    }

    for (int i = 0; i < buffer_length; ++i) {
        buffer[i] = result[i] = 0;
    }

    while (fread(buffer, sizeof(unsigned char), buffer_length, input) > 0) {
        for(unsigned short i = 0; i < buffer_length; ++i) {
            result[i] ^= buffer[i];
        }
    }

    printf("\t XOR result is: <%s>\n", result);
    fclose(input);
    free(buffer);
    free(result);

    return COOL;
}

error_status copy_n (char *path, uint32_t n){
    if (!path)
        return NULLPTR;
    
    char *abs_path;
    if (get_absolute_path(path, abs_path) != COOL){
        printf("Problem to find <%s> path\n", path);
        return FAILURE;
    }

    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();

        if (pid > 0){           //parent
            continue;
        } else if (pid == 0) {  //child
            char *new_file_path = NULL, *extension = '.';
            char *dot_position = strrchr(abs_path, '.');

            if (!dot_position) {
                sprintf(new_file_path, "%s_%d", abs_path, i);
            }
            else {
                unsigned short extension_length = strlen(abs_path) - strlen(dot_position);
                char *file_number;

                strncpy(new_file_path, abs_path, strlen(abs_path) - extension_length);      // ".../name"
                strcat(extension, dot_position);                                            // ".txt"
                sprintf(file_number, "_%d", i);                                             // "_5"
                strcat(new_file_path, file_number);                                         // ".../name_5"
                strcat(new_file_path, extension);                                           // ".../name_5.txt"
            }
        
            if (deep_copy(new_file_path, abs_path) != COOL)
                printf("\t%d copy failed\n", i);
            else 
                printf("\t copy %d created\n", i);
            exit(COOL);
        }
        else {
            printf("\t fork() failed on %d step\n", i);
            return FAILURE;
        }
    }
    return COOL;
}

error_status deep_copy (char* dest_path, char* src_path) {
    if (!dest_path || !src_path)
        return NULLPTR;

    FILE *dest, *src;

    dest = fopen(dest_path, "wb");
    if (!dest)
        return FILE_ERROR;

    src = fopen(src_path, "rb");
    if (!src) {
        fclose (dest_path);
        return FILE_ERROR;
    }

    unsigned char ch;
    while (fread(&ch, sizeof(unsigned char), sizeof(ch), src)) {
        if (fwrite(&ch, sizeof(unsigned char), sizeof(ch), dest) != sizeof(ch)){
            printf("\tDeep coopy error\n");
            fclose(src);
            return FILE_ERROR;
        }
    }

    fclose(dest);
    fclose(src);

    return COOL;
}

error_status find_str(char **paths, int amount_of_paths, const char *to_find, char *found_in, char *flag_found) {
    if (!paths || !to_find)
        return NULLPTR;

    int shm_id = 0, idx = 0, to_find_size = strlen(to_find);
    char *shared, ch;
    pid_t pid;


    shm_id == shmget(IPC_PRIVATE, (amount_of_paths + 1) * sizeof(char), IPC_CREAT | 0666);

    if (shm_id == -1)
        return MEMORY_ERROR;

    shared = (char *) shmat(shm_id, NULL, 0);
    if (shared == (void *)-1) {
        shmctl(shm_id, IPC_RMID, NULL);
        return MEMORY_ERROR;
    }

    memset(shared, 0, amount_of_paths + 1);

    for (int i = 0; i < amount_of_paths; ++i)
    {
        pid = fork();
        if (pid == 0)
        { // Дочерний процесс
            FILE *file = fopen(paths[i], "r");
            if (!file)
                exit(FILE_ERROR);

            while ((ch = fgetc(file)) != EOF)
            {
            if (to_find[idx] == ch)
                {
                    if (idx == to_find_size - 1)
                    {
                        shared[amount_of_paths] = 1;
                        shared[i] = 1;
                        break;
                    }
                    idx++;
                }
                else
                {
                    fseek(file, -idx, SEEK_CUR);
                    idx = 0;
                }
            }

            fclose(file);

            exit(COOL);
        }
        else if (pid < 0)
        {
            shmdt(shared);
            shmctl(shm_id, IPC_RMID, NULL);
            return FAILURE;
        }
    }

    // Ожидание завершения всех дочерних процессов
    for (int i = 0; i < amount_of_paths; i++)
    {
        wait(NULL);
    }

    // Копируем результаты из разделяемой памяти
    *flag_found = shared[amount_of_paths];

    memcpy(found_in, shared, amount_of_paths * sizeof(char));

    // Освобождаем ресурсы
    shmdt(shared);
    shmctl(shm_id, IPC_RMID, NULL);

    return COOL;
}



error_status get_absolute_path(char* current_path, char* absolute_path){

    if (!current_path || !absolute_path){
        return NULLPTR;
    }

    char current_dir[FILENAME_MAX + 1];                         // TODO:  #define FILENAME_MAX = number
    getcwd(current_dir, FILENAME_MAX);                          // TODO: include <stdio.h>
    if (current_dir == NULL){
        return NULLPTR;
    }

    int curr_dir_len = strlen(current_dir);     //without \n
    int curr_path_len = strlen(current_path);
    int curr_elem = 0, valid_char_count = 0;
    int delete_amount = 0;

    if(current_path[curr_path_len - 1] == '/'){     //file can't end with '/'
        return WRONG_PARAMETER;
    }    
    if (current_path[0] == '/'){                    // already absolute for UNIX system
        strcpy(absolute_path, current_path);
        return COOL;
    }

    char* dirty_abs_path = (char*) malloc(sizeof(char) * (curr_dir_len + curr_path_len + 2));       // + / + \n
    if(!dirty_abs_path)
        return MEMORY_ERROR;

    strcpy(dirty_abs_path, current_dir);           //get dirty absolute path with /../ or /./
    strcat(dirty_abs_path, "/");                                                                                                           
    strcat(dirty_abs_path, current_path);

    curr_elem = curr_dir_len + curr_path_len - 1; //index of the last elem 
    while(curr_elem > 0){
        //  dir1/../dir2/  case
        if(dirty_abs_path[curr_elem] == '.' &&  dirty_abs_path[curr_elem - 1] == '.'){
            if(dirty_abs_path[curr_elem - 2] != '/'){
                free(dirty_abs_path);
                return WRONG_PARAMETER;
            }
            
            ++delete_amount;
            dirty_abs_path[curr_elem] = dirty_abs_path[curr_elem - 1] = dirty_abs_path[curr_elem - 2] = '\0';
            curr_elem -= 3;
        }
        //  /./ case
        else if (dirty_abs_path[curr_elem] == '.' && dirty_abs_path[curr_elem - 1] == '/'){
            dirty_abs_path[curr_elem--] = '\0';
            dirty_abs_path[curr_elem--] = '\0';
        }
        //    dir//dir  case
        else if(dirty_abs_path[curr_elem] == '/' && dirty_abs_path[curr_elem - 1] == '/'){
            dirty_abs_path[curr_elem--] = '\0';
        }
        else{
            if(delete_amount > 0){
                dirty_abs_path[curr_elem--] = '\0'; 
                while(dirty_abs_path[curr_elem] != '/'){
                    dirty_abs_path[curr_elem--] = '\0';
                }
                dirty_abs_path[curr_elem--] = '\0';
                --delete_amount;
            }
            else{
                ++valid_char_count;
                --curr_elem;
            }
        }
    }

    if(valid_char_count + 1 > FILENAME_MAX){      
        free(dirty_abs_path);
        return OVERFLOW;
    }

    curr_elem = 0;
    for(int i = 0; i <= (curr_dir_len + curr_path_len); ++i){
        if (dirty_abs_path[i] != '\0'){
            absolute_path[curr_elem] = dirty_abs_path[i];
            ++curr_elem;
        }
    }

    free(dirty_abs_path);
    return COOL;
}
