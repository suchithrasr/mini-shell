#ifndef MAIN_H
#define MAIN_H

/*-----------------------------------------------------------
 * Header files
 *----------------------------------------------------------*/
#include <stdio.h>          // printf(), fgets()
#include <string.h>         // strcpy(), strtok(), strlen()
#include <stdlib.h>         // malloc(), free(), exit()
#include <stdio_ext.h>      // __fpurge()
#include <sys/types.h>      // pid_t
#include <sys/stat.h>       // file status information
#include <fcntl.h>          // file control options
#include <unistd.h>         // fork(), execvp(), getcwd()
#include <signal.h>         // signal handling
#include <sys/wait.h>       // wait(), waitpid()

/*-----------------------------------------------------------
 * Command type definitions
 *----------------------------------------------------------*/
#define BUILTIN         1   // Internal shell command
#define EXTERNAL        2   // External Linux command
#define NO_COMMAND      3   // Invalid command


/*-----------------------------------------------------------
 * Buffer size definitions
 *----------------------------------------------------------*/
#define MAX_INPUT 256       // Maximum input command length
#define MAX_PROMPT 64       // Maximum shell prompt size
#define CMD_SIZE 64         // Maximum command size
#define MAX_ARGS 64         // Maximum number of arguments
#define MAX_EXT_CMD 300     // Maximum external commands supported


/*-----------------------------------------------------------
 * Structure for background job linked list
 *----------------------------------------------------------*/
typedef struct Slist
{
    int pid;                        // Process ID of background job

    char command[MAX_INPUT];        // Command string

    struct Slist *next;             // Pointer to next node

} Slist;


/*-----------------------------------------------------------
 * Global variables
 *----------------------------------------------------------*/

/* Shell prompt string */
extern char prompt[MAX_PROMPT];

/* Stores user input */
extern char input_string[MAX_INPUT];

/* Stores currently running child process PID */
extern pid_t child_pid;

/* Stores process execution status */
extern int status;

/* Array of external command strings */
extern char *external_commands[MAX_EXT_CMD];

/* Head pointer for background job linked list */
extern Slist *head;


/*-----------------------------------------------------------
 * ANSI color codes for terminal output
 *----------------------------------------------------------*/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


/*-----------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------*/

/*---------------- scan_input.c ----------------*/

/* Reads and processes user input */
void scan_input(char *prompt, char *input_string);


/*---------------- commands.c ----------------*/

/* Extracts command from user input */
char *get_command(char *input_string);

/* Determines whether command is builtin/external */
int check_command_type(char *command);

/* Loads external commands into array */
void extract_external_commands(char **external_commands);

/* Executes shell built-in commands */
void execute_internal_commands(char *input_string);

/* Executes external Linux commands */
void execute_external_commands(char *input_string);

/* Handles signals like SIGINT, SIGTSTP, SIGCHLD */
void signal_handler(int sig_num);


/*---------------- lists.c ----------------*/

/* Inserts background job into linked list */
void insert_at_first(pid_t pid, const char* cmd);

/* Deletes node with matching PID */
void delete_node(pid_t pid);

/* Deletes first node from list */
void delete_first(void);

/* Prints all background jobs */
void print_list(Slist* node);


/*-----------------------------------------------------------
 * End of header file
 *----------------------------------------------------------*/
#endif