#include "functions.h"

int main(int argc, char* argv[]){

    if (argc < 3)
        return INPUT_ERROR;


    if (strcmp(argv[argc - 2], "mask") == 0) {
        if (argc < 4)
            return INPUT_ERROR;
        
        for(int i = 1; i < argc - 2; ++i) {
            if (mask_hex(argv[i], argv[argc - 1]) != COOL){
                printf("mask_hex for file <%s> failed\n", argv[i]);
            }
        }
    }

    else if (strcmp(argv[argc - 2], "find") == 0) {
        if (argc < 4)
            return INPUT_ERROR;

        char *find_in, flag;
        find_in = (char *)malloc(sizeof(char) * argc - 3);
        if (!find_in)
        {
            printf("Memory error\n");
            return MEMORY_ERROR;
        }
        
        if (find_string((const char **)argv + 1, argc - 3, argv[argc - 1], find_in, &flag) != COOL){
            printf("\tFailed\n");
            free(find_in);
            return FAILURE;
        }
        else {
            for (int i = 0; i < argc - 3; ++i)
                printf("%s - %s\n", argv[i + 1], (flag && find_in[i]) ? "FOUND" : "NOT FOUND");
            
            free(find_in);
        }

    }
    
    if (strncmp(argv[argc - 1], "xor", 3)  == 0) {     //
        short n = argv[argc - 1][3] - '0';
        if (n < 2 || n > 6 || argv[argc - 1][3] != '\0')
            return INPUT_ERROR;
        
        for(int i = 1; i < argc - 1; ++i) {
            if (xor_n(argv[i], n) != COOL)
                printf("\txor of file <%s> failed\n", argv[i]);
        }
    }

    else if (strncmp(argv[argc - 1], "copy", 3) == 0) {
        char *n_str;
        strncpy(n_str, argv[argc - 1] + 4, strlen(argv[argc - 1] - 4));

        uint32_t n;
        error_status status = string_to_uint_32(&n, n_str);

        for(int i = 1; i < argc - 1; ++i){
            printf("\tfile <%s>\n", argv[i]);
            if (copy_n(argv[i], n) != COOL)
                printf("\tfailed\n");
            else
                printf("\tdone\n");
        }
    }

    else {
        printf("\tUnknown command\n");
        return FAILURE;
    }
    
    return COOL;
}