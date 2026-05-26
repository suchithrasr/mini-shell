#include"header.h"

/*-----------------------------------------------------------
 * Array of supported built-in shell commands
 *----------------------------------------------------------*/
char *builtins[] =
{
    "echo", "printf", "read", "cd", "pwd", "pushd",
    "popd", "dirs", "let", "eval", "set", "unset",
    "export", "declare", "typeset", "readonly",
    "getopts", "source", "exit", "exec", "shopt",
    "caller", "true", "type", "hash", "bind",
    "help", "jobs", "fg", "bg", NULL
};

/* Buffer to store extracted command */
char cmd[CMD_SIZE];


/*-----------------------------------------------------------
 * get_command()
 * Extracts command from user input
 * Example:
 * Input  -> "ls -l"
 * Output -> "ls"
 *----------------------------------------------------------*/
char *get_command(char *input_string)
{
        int i = 0;

        /* Check for NULL input */
        if(input_string == NULL)
                return NULL;

        /* Skip leading spaces */
        while(input_string[i] == ' ')
                i++;

        /* Copy characters until space */
        while(input_string[i] != ' ')
		{
			/* Prevent buffer overflow */
			if(i >= CMD_SIZE - 1)
			    break;

			cmd[i] = input_string[i];
			i++;
		}

        /* Add null terminator */
        cmd[i] = '\0';

        return cmd;
}


/*-----------------------------------------------------------
 * check_command_type()
 * Checks whether command is:
 * 1. Built-in command
 * 2. External command
 * 3. Invalid command
 *----------------------------------------------------------*/
int check_command_type(char *command)
{
        int i = 0;

        if(command == NULL)
                return NO_COMMAND;

        /* Check built-in commands */
        while(builtins[i] != NULL)
        {
                if(strcmp(command, builtins[i]) == 0)
                        return BUILTIN;

                i++;
        }

        /* Check external commands */
        i = 0;

        while(external_commands[i] != NULL)
        {
                if(strcmp(command, external_commands[i]) == 0)
                        return EXTERNAL;

                i++;
        }

        /* Command not found */
        return NO_COMMAND;
}


/*-----------------------------------------------------------
 * extract_external_commands()
 * Reads external commands from file and stores them
 *----------------------------------------------------------*/
void extract_external_commands(char **external_commands)
{
        char buffer[100];
        int i = 0;

        /* Open external commands file */
        FILE *fp = fopen("ext_cmds.txt", "r");

        if(fp == NULL)
        {
                perror("fopen");
                return;
        }

        /* Read commands one by one */
        while(fscanf(fp, "%99s", buffer) != EOF)
        {
                /* Allocate memory for command */
                external_commands[i] = malloc(strlen(buffer) + 1);

                if(external_commands[i] == NULL)
                {
                        perror("malloc");
                        break;
                }

                /* Copy command */
                strcpy(external_commands[i], buffer);

                i++;
        }

        /* Mark end of array */
        external_commands[i] = NULL;

        fclose(fp);
}


/*-----------------------------------------------------------
 * execute_internal_commands()
 * Executes built-in shell commands
 *----------------------------------------------------------*/
void execute_internal_commands(char *input_string)
{
        char *cmd = get_command(input_string);

        if(cmd == NULL)
                return;

        /*---------------------------------------------------
         * 1. exit command
         *--------------------------------------------------*/
        if(strcmp(cmd, "exit") == 0)
        {
                exit(0);
        }

        /*---------------------------------------------------
         * 2. pwd command
         * Displays current working directory
         *--------------------------------------------------*/
        else if(strcmp(cmd, "pwd") == 0)
        {
                char cwd[1024];

                if(getcwd(cwd, sizeof(cwd)) != NULL)
                   printf("%s\n", cwd);
                else
                   perror("pwd");

                return;
        }

        /*---------------------------------------------------
         * 3. cd command
         * Changes current directory
         *--------------------------------------------------*/
        else if(strncmp(input_string, "cd", 2) == 0)
        {
                char *path = input_string + 2;

                /* Skip spaces */
                while(*path == ' ')
                        path++;

                /* Missing directory */
                if(*path == '\0')
                        printf("cd:missing argument\n");

                /* Change directory */
                else if(chdir(path) != 0)
                {
                        perror("cd");
                }
                else
                {
                        char buffer[100];

                        /* Print updated directory */
                        if(getcwd(buffer, sizeof(buffer)) != NULL)
                                printf("%s\n", buffer);
                        else
                                perror("getcwd");
                }

                return;
        }

       /*---------------------------------------------------
        * 4. echo command family
        *--------------------------------------------------*/
       else if(strcmp(cmd, "echo") == 0)
       {
            /* echo $$ -> current shell PID */
            if(strcmp(input_string, "echo $$") == 0)
            {
                 printf("%d\n", getpid());

                 return;
            }

            /* echo $? -> previous command status */
            else if(strcmp(input_string, "echo $?") == 0)
            {
                printf("%d\n", WEXITSTATUS(status));

                return;
            }

            /* echo $SHELL -> shell path */
            else if(strcmp(input_string, "echo $SHELL") == 0)
            {
                char *shell_path = getenv("SHELL");

                if(shell_path)
                    printf("%s\n", shell_path);

                return;
            }

            /* Normal echo */
            else
            {
                printf("%s\n", input_string + 5);

                return;
            }
        }

        /*---------------------------------------------------
         * 5. jobs command
         * Displays background jobs
         *--------------------------------------------------*/
        else if(strcmp(cmd, "jobs") == 0)
        {
                print_list(head);

                return;
        }

        /*---------------------------------------------------
         * 6. fg command
         * Brings background job to foreground
         *--------------------------------------------------*/
        else if(strcmp(cmd, "fg") == 0)
        {
               /* No jobs available */
               if(head == NULL)
		       {
			        printf("fg: No such jobs\n");

                    return;
               }

		       /* Get most recent background process */
		       pid_t fg_pid = head->pid;

               char cmd_copy[MAX_INPUT];

               /* Copy command before deleting node */
               strcpy(cmd_copy, head->command);

               /* Remove from background jobs list */
               delete_first();

               /* Set current child PID */
               child_pid = fg_pid;

               /* Resume stopped process */
               kill(fg_pid, SIGCONT);

               /* Wait for foreground process */
               waitpid(fg_pid, &status, WUNTRACED);

               /* Reset child PID */
               child_pid = 0;

			   /* If stopped again using Ctrl + Z */
			   if(WIFSTOPPED(status))
			   {
				    insert_at_first(fg_pid, cmd_copy);

                    printf("[Stopped] PID: %d Command: %s\n",
                            fg_pid, cmd_copy);
               }

			   return;
        }

        /*---------------------------------------------------
         * 7. bg command
         * Resumes stopped process in background
         *--------------------------------------------------*/
        else if(strcmp(cmd, "bg") == 0)
        {
                if(head == NULL)
                {
                        printf("bg:no jobs\n");

                        return;
                }

                pid_t bg_pid = head->pid;

			    /* Continue background process */
			    kill(bg_pid, SIGCONT);

			    printf("[bg] PID %d resumed in background\n",
                        bg_pid);

			    return;
        }

	    /* Invalid internal command */
	    printf("%s: command not found\n", input_string);
}


/*-----------------------------------------------------------
 * execute_external_commands()
 * Executes external Linux commands
 * Supports both:
 * 1. Single commands
 * 2. Pipe commands
 *----------------------------------------------------------*/
void execute_external_commands(char* input_string)
{
	char *cmds[10];
    int cmd_count = 0;

    /*---------------------------------------------------
     * Single command execution (No pipe)
     *--------------------------------------------------*/
	if(strchr(input_string, '|') == NULL)
    {
        char *argv[MAX_ARGS];
        int i = 0;

        /* Tokenize command arguments */
        char *token = strtok(input_string, " ");

        while(token != NULL && i < MAX_ARGS - 1)
        {
            argv[i++] = token;

            token = strtok(NULL, " ");
        }

        argv[i] = NULL;

        /* Execute command */
        execvp(argv[0], argv);

        perror("execvp");

        exit(1);
    }

    /*---------------------------------------------------
     * Pipe command handling
     *--------------------------------------------------*/

    /* Split commands using pipe symbol */
    char *pipe_token = strtok(input_string, "|");

    while(pipe_token != NULL && cmd_count < 10)
    {
        cmds[cmd_count++] = pipe_token;

        pipe_token = strtok(NULL, "|");
    }

    int prev_fd = -1;

    /* Execute each command in pipeline */
    for(int i = 0; i < cmd_count; i++)
    {
        int fd[2];

        /* Create pipe except for last command */
        if(i < cmd_count - 1)
        {
            if(pipe(fd) == -1)
            {
                perror("pipe");

                exit(1);
            }
        }

        /* Create child process */
        pid_t pid = fork();

        /*---------------------------------------------------
         * Child process
         *--------------------------------------------------*/
        if(pid == 0)
        {
            /* Redirect input from previous pipe */
            if(prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);

                close(prev_fd);
            }

            /* Redirect output to next pipe */
            if(i < cmd_count - 1)
            {
                close(fd[0]);

                dup2(fd[1], STDOUT_FILENO);

                close(fd[1]);
            }

            /* Convert command into argv format */
            char *argv[MAX_ARGS];
            int j = 0;

            char *token = strtok(cmds[i], " \n");

            while(token != NULL && j < MAX_ARGS - 1)
            {
                argv[j++] = token;

                token = strtok(NULL, " \n");
            }

            argv[j] = NULL;

            /* Execute command */
            execvp(argv[0], argv);

            perror("execvp");

            exit(1);
        }

        /*---------------------------------------------------
         * Parent process
         *--------------------------------------------------*/
        else if(pid > 0)
        {
            /* Close previously used file descriptor */
            if(prev_fd != -1)
                close(prev_fd);

            /* Store current pipe read end */
            if(i < cmd_count - 1)
            {
                close(fd[1]);

                prev_fd = fd[0];
            }
        }

        /* Fork failure */
        else
        {
            perror("fork");

            exit(1);
        }
    }
}