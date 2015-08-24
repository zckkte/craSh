#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#define BUFFER_SIZE 256
#define LEN(x) sizeof(x)/sizeof(x[0])

int chdir_proc(char**);
int help_proc(char**);

typedef int (* function_t)(char ** args);

struct procedure_t {
    char * signature;
    function_t call;
} procedures[] = { { "cd", &chdir_proc }, { "help", &help_proc } };

int help_proc(char ** args) {
    printf("*** Be honest with yourself, you don't need help ***\n");
    return 0;
}

int chdir_proc(char ** args) {
    if(*args == NULL) return -1;
    else if(chdir(*args++) != 0) {
        fprintf(stderr, "*** craSh: cd ***");
        return -1;
    }
    return 1;
}

void read_line(char * buffer) {
    fgets(buffer, BUFFER_SIZE, stdin);
    if(buffer == NULL) {
        fprintf(stderr, "*** craSh: failed to read line from stdin ***\n");
        exit(1);
    }
}

void parse(char * line, char ** argv) {
    const char * delimiters = " \t\n\a\r";
    char * token = strtok(line, delimiters);
    while(token != NULL) {
        *argv++ = token;
        token = strtok(NULL, delimiters);
    }
    *argv = NULL;
}

void execute(char ** argv) {
    pid_t pid;
    int status;

    int i;
    for(i = 0; i < LEN(procedures); i++) {
        if(strcmp(*argv, procedures[i].signature) == 0) {
            procedures[i].call(argv + 1);
            return;
        }
    }

    if ((pid = fork()) < 0) { /* fork a child process */
        fprintf(stderr, "*** craSh: failed to fork child process ***\n");
        exit(1);
    } else if (pid == 0) { /* child process */
        if (execvp(argv[0], argv) < 0) { /* execute the command  */
            fprintf(stderr, "*** craSh: failed to execute command ***\n");
            exit(1);
        }
    } else { /* parent process */
        while (wait(&status) != pid); /* wait for completion */
    }
}

int main(void) {
    char line[BUFFER_SIZE];
    char * argv[BUFFER_SIZE];
    printf("<<< welcome to craSh - a very simple command interpreter >>>\n");
    while(true) {
        printf("craSh> ");
        read_line(line);
        parse(line, argv);
        if (strcmp(argv[0], "exit") == 0) exit(0);
        execute(argv);
    }
}
