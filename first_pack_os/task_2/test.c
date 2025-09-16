#include <stdio.h>
#include <string.h>

typedef enum {
    COOL,
    FAILURE
} status;

status f(int f) {
    return f > 0 ? COOL : FAILURE;
}

int main() {


    if (f(-5) == FAILURE)
        printf("dgdfgfdsgfdgf");


    char *gg = "hello, world";
    int g = 5;

    char *new_s;

    sprintf(new_s, "%s.%d", gg, g);
    
    printf("%s    %d\n\n", new_s, strlen(new_s));

    printf("%.4s\n", new_s);
}

