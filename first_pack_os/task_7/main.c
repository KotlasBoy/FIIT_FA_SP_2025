#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

typedef enum {
    OK,
    FAILURE,
    WRONG_PARAMETERS   
} error_state;

/*
struct dirent {
    ino_t d_ino;
    off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;

    char d_name[256];
}
*/

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("Not enough atributes\n");
        return WRONG_PARAMETERS;
    }

    DIR* current_directory;
    struct dirent *entry;
    struct stat file_stat;
    char path[1024] = "\0";

    for (int i = 1; i < argc; ++i) {
        if(!(current_directory = opendir(argv[i]))) {
            printf("Problem with directory <%s>\n", argv[i]);
            continue;
        }

        printf("Directory to read <%s>\n", argv[i]);

        while((entry = readdir(current_directory)) != NULL) {
            
            snprintf(path, sizeof(path), "%s/%s", argv[i], entry->d_name);

            if(stat(path, &file_stat) == -1) {
                printf("\t Broken_file\n");
                continue;
            }

            printf("\t%s", entry->d_name);
            
            if (S_ISBLK(file_stat.st_mode)) {
                printf(" [block device]\n");
            }
            else if (S_ISCHR(file_stat.st_mode)) {
                printf(" [char device]\n");
            }                        
            else if (S_ISDIR(file_stat.st_mode)) {
                printf(" [directory]\n");
            }
            else if (S_ISFIFO(file_stat.st_mode)) {
                printf( "[FIFO]\n");
            }
            else if (S_ISLNK(file_stat.st_mode)) {
                printf(" [link]\n");
            }
            else if (S_ISREG(file_stat.st_mode)) {
                printf(" [file]\n");
            }
            else if (S_ISSOCK(file_stat.st_mode)) {
                printf(" [socket]\n");
            }
            else {
                printf(" [unknown]\n");
            }

        }

    closedir(current_directory);

    }

    return 0;
}



