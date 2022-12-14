#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>

#include "tree.h"

#define MIN_ARGS 3
#define MAX_FILES 8
#define MAX_SIZE 64

char files[MAX_FILES][255];

pthread_mutex_t lock;

struct Node *root = NULL;

char *toUpperCase(char *str);

void *file_processing(void *param);

int main(int argc, char const *argv[])
{
    int file_num;
    struct timeval time1, time2;

    if (argc < MIN_ARGS || argc > MIN_ARGS + MAX_FILES)
    {
        fprintf(stderr, "Argument number is inappropriate!\n");
        exit(-1);
    }
    else
    {
        file_num = atoi(argv[2]);

        if (argc != MIN_ARGS + file_num)
        {
            fprintf(stderr, "File number and existing files are incompatible!\n");
            exit(-1);
        }

        pthread_mutex_init(&lock, NULL);

        pthread_t threads[file_num];

        for (int i = 0; i < file_num; i++)
        {
            strcpy(files[i], argv[MIN_ARGS + i]);
        }

        gettimeofday(&time1, NULL);

        for (int i = 0; i < file_num; i++)
        {
            threads[i] = i;
            pthread_create(&threads[i], NULL, file_processing, (void *) files[i]);
        }

        for (int i = 0; i < file_num; i++) 
        {
            pthread_join(threads[i], NULL);
        }

        FILE *fp = fopen(argv[1], "w");

        writeInorder(root, fp);

        gettimeofday(&time2, NULL);

        printf("Total time = %ld miliseconds\n", (time2.tv_usec - time1.tv_usec) );

        fclose(fp);

        free_node(root);

    }

    return 0;
}

void *file_processing(void *param)
{
    char *file_name = (char *) param;

    FILE *fp = fopen(file_name, "r");

    char buffer[MAX_SIZE];

    pthread_mutex_lock(&lock);
    while ( fscanf(fp, "%s", buffer) != EOF )
    {
        insert(&root, toUpperCase(buffer));
    }
    pthread_mutex_unlock(&lock);

    fclose(fp);

    pthread_exit(0);

}

char *toUpperCase(char *str)
{
    char *result = str;
    for (int i = 0; i < strlen(str); i++)
    {
        result[i] = toupper(result[i]);
    }
    return result;
}
