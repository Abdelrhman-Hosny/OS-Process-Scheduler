#include <string.h>

struct process *readProccesses(char *fileName, int *countArr)
{
    FILE *fp;
    char line[255];
    int lines = 0;
    char *processesArray;

    //opening the file
    char *file;
    file = (char *)malloc(strlen(fileName) + 2);
    strcpy(file, "./");
    strcat(file, fileName);
    fp = fopen(file, "r");
    if (fp == NULL)
    {
        printf("Error in opening the file");
        exit(0);
    }
    //counting number of lines to allocate the memory
    while (fgets(line, sizeof(line), fp))
    {
        if (line[0] == '#')
            continue;
        lines++;
    }
    //closing and reopening the file
    fclose(fp);
    fp = fopen(file, "r");

    //creating an array of structs
    struct process *proccesses = (struct process *)malloc(lines * sizeof(struct process));
    //counter to put the process we read in the array
    int counter = 0;
    int array[4];
    while (fgets(line, sizeof(line), fp))
    {

        if (line[0] == '#')
            continue;

        //adding correct values of process

        char *token = strtok(line, "\t");

        // Keep printing tokens while one of the
        // delimiters present in str[].
        int arr[4];
        int i = 0;
        while (token != NULL)
        {
            // printf("%s\n", token);
            arr[i++] = atoi(token);
            token = strtok(NULL, "\t");
        }
        struct process *currentProcess = (struct process *)malloc(sizeof(struct process));

        //assigning process parameters
        initializeProcessPointer(currentProcess, arr[2], arr[3], arr[0], arr[1]);

        proccesses[counter] = *currentProcess;
        counter++;
        // free(parameters);
        // free(delim);
    }
    *countArr = counter;
    return proccesses;
}
