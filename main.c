#include"header.h"

/*-----------------------------------------------------------
 * Global variable to store child process exit status
 *----------------------------------------------------------*/
int status = 0;

/*-----------------------------------------------------------
 * Array to store all external commands
 *----------------------------------------------------------*/
char *external_commands[MAX_EXT_CMD];

/*-----------------------------------------------------------
 * Stores PID of currently running child process
 *----------------------------------------------------------*/
pid_t child_pid = 0;

/*-----------------------------------------------------------
 * Head pointer for linked list used in background jobs
 *----------------------------------------------------------*/
Slist *head = NULL;

/*-----------------------------------------------------------
 * Buffer to store shell prompt
 *----------------------------------------------------------*/
char prompt[MAX_PROMPT];

/*-----------------------------------------------------------
 * Buffer to store user input command
 *----------------------------------------------------------*/
char input_string[MAX_INPUT];


/*-----------------------------------------------------------
 * main()
 * Entry point of minishell program
 *----------------------------------------------------------*/
int main()
{
        /* Clear terminal screen */
        system("clear");

        /*
         * Initialize default shell prompt
         * Example: minishell$:
         */
        strcpy(prompt, "minishell$: ");

        /* Start shell input handling */
        scan_input(prompt, input_string);

        return 0;
}