#include"headers.h"
#include<dirent.h>
#include<string.h>

struct process
{
    int processId;
    int arrivalTime;
    int runTime;
    int remainingTime;
    int priority;
};
struct processes* readProccesses(char* fileName, int *countArr)
{
    FILE *fp;
    char  line[255];
    int lines= 0 ;
    char *processesArray;
   
    //opening the file
    char* file;
    file = malloc(strlen(fileName)+2);
    strcpy(file,"./");
    strcat(file,fileName);
    fp = fopen(file,"r");
    if(fp ==NULL)
    {
        printf("Error in opening the file");
        exit(0);
    }
    //counting number of lines to allocate the memory
    while(fgets(line, sizeof(line), fp))
    {
        if(line[0]=='#')
           continue;
        lines++;
    }
    //closing and reopening the file
    fclose(fp);
    fp = fopen(file,"r");

    //creating an array of structs
    struct process *proccesses = (struct process*)malloc(lines *sizeof(struct process));
    //counter to put the process we read in the array
    int counter = 0;
    int array[4];
    while(fgets(line, sizeof(line), fp))
    {
        
        if(line[0]=='#')
           continue;

        //adding correct values of process
        
        char* token = strtok(line, "\t");

        // Keep printing tokens while one of the
        // delimiters present in str[].
        int arr[4];
        int i = 0 ;
        while (token != NULL) {
            // printf("%s\n", token);
            arr[i++] = atoi(token);
            token = strtok(NULL, "\t");
        }
        struct process *currentProcess = (struct process*)malloc(sizeof(struct process)) ;
        
        //assigning process parameters
        
        currentProcess->processId = arr[0];
        currentProcess->arrivalTime = arr[1];
        currentProcess->runTime = arr[2];
        currentProcess->priority = arr[3];


        proccesses[counter] = *currentProcess;
        counter++;
        // free(parameters);
        // free(delim);
        
       
    }
    *countArr = counter;
    return proccesses;
    
}