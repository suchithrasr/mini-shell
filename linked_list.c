#include "header.h"

/*-----------------------------------------------------------
 * Linked list implementation for background job management
 *----------------------------------------------------------*/


/*-----------------------------------------------------------
 * insert_at_first()
 * Inserts a new background job at the beginning of the list
 *----------------------------------------------------------*/
void insert_at_first(pid_t pid, const char* cmd) 
{
    /* Allocate memory for new node */
    Slist* node = malloc(sizeof(Slist));

    /* Check for allocation failure */
    if(node == NULL)
    {
        perror("malloc");
        return;
    }

    /* Store process ID */
    node->pid = pid;

    /* Copy command safely */
    strncpy(node->command, cmd, MAX_INPUT - 1);

    /* Ensure null termination */
    node->command[MAX_INPUT - 1] = '\0';

    /* Insert node at beginning */
    node->next = head;

    /* Update head pointer */
    head = node;
}


/*-----------------------------------------------------------
 * delete_first()
 * Deletes the first node from the linked list
 *----------------------------------------------------------*/
void delete_first(void) 
{
    /* If list is empty */
    if(head == NULL)
        return;

    /* Temporary pointer to current head */
    Slist* temp = head;

    /* Move head to next node */
    head = head->next;

    /* Free memory of deleted node */
    free(temp);
}


/*-----------------------------------------------------------
 * delete_node()
 * Deletes a node with matching PID
 *----------------------------------------------------------*/
void delete_node(pid_t pid) 
{
    Slist* curr = head;
    Slist* prev = NULL;

    /* Traverse linked list */
    while(curr != NULL)
    {
            /* Check if PID matches */
            if(curr->pid == pid)
            {
                /* If node is not first node */
                if(prev)
                {
                    prev->next = curr->next;
                }
                else
                {
                    /* Deleting first node */
                    head = curr->next;
                }

                /* Free node memory */
                free(curr);

                return;
            }

            /* Move pointers forward */
            prev = curr;
            curr = curr->next;
    }
}


/*-----------------------------------------------------------
 * print_list()
 * Displays all background jobs
 *----------------------------------------------------------*/
void print_list(Slist* node)
{
    int i = 1;

    /* Check if list is empty */
    if(node == NULL)
    {
        printf("No background jobs\n");
        return;
    }

    /* Traverse and print each node */
    while(node != NULL)
    {
        printf("[%d] PID: %d Command: %s\n",
                i, node->pid, node->command);

        node = node->next;
        i++;
    }
}