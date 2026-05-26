#include"header.h"

/*-----------------------------------------------------------
 * signal_handler()
 * Handles different signals received by the shell.
 *----------------------------------------------------------*/
void signal_handler(int signum)
{
        /* Handles Ctrl + C (SIGINT) */
        if(signum == SIGINT)
        {
                /* If no child process is running */
                if(child_pid == 0)
				{
					/* Print new line and prompt again */
					write(STDOUT_FILENO, "\n", 1);
                    write(STDOUT_FILENO, prompt, strlen(prompt));
				}

                //printf("\nminishell$ ");
                //printf("\n%s\n",prompt);
        }

        /* Handles Ctrl + Z (SIGTSTP) */
        if(signum == SIGTSTP)
        {
                /* Send SIGTSTP to currently running child process */
                if(child_pid != 0)
                    kill(child_pid, SIGTSTP);
        }

        /* Handles child termination signal */
        if(signum == SIGCHLD)
		{
			pid_t pid;
			int st;

			/* Reap all terminated child processes */
			while((pid = waitpid(-1, &st, WNOHANG)) > 0)
			{
				/* Remove completed background process from jobs list */
				delete_node(pid);

			    //printf("\n[bg done] PID %d exited with status %d\n",
                //pid,WEXITSTATUS(st));
			}
		}
}

/*-----------------------------------------------------------
 * scan_input()
 * Reads user input continuously and executes commands.
 *----------------------------------------------------------*/
void scan_input(char *prompt, char *input_string)
{
    /* Load all external commands into array */
    extract_external_commands(external_commands);

    /* Register signal handlers */
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler);

    /* Infinite shell loop */
    while(1)
    {
        /* Display shell prompt */
        printf("%s", prompt);
        fflush(stdout);

        /* Read user input safely */
		if(fgets(input_string, MAX_INPUT, stdin) == NULL)
            continue;

        /* Remove trailing newline character */
        input_string[strcspn(input_string, "\n")] = '\0';

		/* Calculate input length */
		int len = strlen(input_string);

		/* Remove newline if present at end */
		if(len > 0 && input_string[len - 1] == '\n')
		    input_string[--len] = '\0';

		/* Ignore empty input */
		if(len == 0)
		    continue;

        /*---------------------------------------------------
         * Check if user is changing prompt using PS1=
         * Example: PS1=minishell$
         *--------------------------------------------------*/
        if(strncmp(input_string, "PS1=", 4) == 0)
		{
			/* Reject prompt if spaces are present */
			if((strchr(input_string + 4, ' ') != NULL))
			{
				printf("PS1: command not found\n");
				continue;
			}
			else
			{
				/* Copy new prompt safely */
				strncpy(prompt, input_string + 4, MAX_PROMPT - 1);

				/* Ensure null termination */
				prompt[MAX_PROMPT - 1] = '\0';
			}

			/* Go to next iteration with updated prompt */
			continue;
		}
        else
        {
            /* Extract command from input */
            char *cmd = get_command(input_string);

            /* Determine command type */
            int type = check_command_type(cmd);

            /*---------------------------------------------------
             * Built-in command execution
             *--------------------------------------------------*/
            if(type == BUILTIN)
            {
                execute_internal_commands(input_string);
            }

            /*---------------------------------------------------
             * External command execution
             *--------------------------------------------------*/
            else if(type == EXTERNAL)
            {
                int background = 0;

				/* Check for background execution using '&' */
				if(len > 0 && input_string[len - 1] == '&')
				{
					background = 1;

					/* Remove '&' from command */
					input_string[len - 1] = '\0';
				}

				/* Create child process */
				pid_t pid = fork();

				/* Fork failed */
				if(pid < 0)
				{
					perror("");
					continue;
				}

				/* Child process */
				if(pid == 0)
				{
					/* Restore default signal behavior */
					signal(SIGINT, SIG_DFL);
					signal(SIGTSTP, SIG_DFL);
					signal(SIGCHLD, SIG_DFL);

					/* Execute external command */
					execute_external_commands(input_string);

					exit(0);
				}

				/* Background process */
				if(background)
				{
					/* Store background job in jobs list */
					insert_at_first(pid, input_string);

					printf("[bg] PID %d : %s\n", pid, input_string);
				}

				/* Foreground process */
				else
				{
					/* Store child PID globally */
					child_pid = pid;

					/* Wait for child process */
					waitpid(pid, &status, WUNTRACED);

					/* Reset child PID */
					child_pid = 0;

				    /* If process stopped using Ctrl + Z */
				    if(WIFSTOPPED(status))
                    {
                        /* Add stopped process to jobs list */
                        insert_at_first(pid, input_string);

                        printf("[Stopped] PID: %d Command: %s\n",
                                pid, input_string);
                    }
                 }
            }

            /*---------------------------------------------------
             * Invalid command
             *--------------------------------------------------*/
            else
            {
                printf("%s: Command Not Found\n", cmd);
            }
        }
    }
}

