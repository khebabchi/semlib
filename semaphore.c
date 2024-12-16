
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stddef.h>
#include <sys/sem.h>
#include <stdarg.h>
#include "semaphore.h"
#include <stdlib.h>
#include <stdio.h>


// Wrapper for initializing a semaphore
// the last argument is the semaphore initial_values
SemSet new_sem(key_t key, int sem_count, int sem_id)
{
    SemSet sem;
    sem.key_id = key;
    sem.sem_count = sem_count;
    sem.sem_id = sem_id;
    print_sem(sem);
    return sem;
}
void print_sem(SemSet sem)
{
    printf("sem_id:%d, sem_key:%d, sem_count:%d", sem.sem_id, sem.key_id, sem.sem_count);
}

SemSet sem_create(const char *path, int id, int sem_count, ... /* initial values */)
{
    // getting the initial values
    va_list initial_values;
    va_start(initial_values, sem_count);

    //---- Creating the key ------

    key_t key = ftok(path, id);
    if (key == IPC_RESULT_ERROR)
    {
        perror("Erreur lors de la création de la clé pour l'ensemble des semaphores");
        exit(1);
    }

    //---- Creating the semaphores ------

    int sem_id = semget(key, sem_count, 0666 | IPC_CREAT);
    if (sem_id == IPC_RESULT_ERROR)
    {
        perror("Erreur lors de la création des sémaphores");
        exit(2);
    }
    printf("Created Semaphore set with ID: %d\n", sem_id);

    // initializing the semaphores
    union semun arg;
    // Loop over each semaphore and set its initial value
    for (int i = 0; i < sem_count; i++)
    {
        int initial_value = va_arg(initial_values, int); // Get the next value from the variadic arguments

        // Set the semaphore value using semctl
        arg.val = initial_value; // Assign the value to the union's val field
        if (semctl(sem_id, i, SETVAL, arg) == IPC_RESULT_ERROR)
        {
            // Print the error with the semaphore index
            perror("Erreur lors de l'initialisation du sémaphore");
            exit(3);
        }
        else
        {
            printf("Semaphore %d initialized with value %d\n", i, initial_value);
        }
    }
    SemSet sem= new_sem(key, sem_count, sem_id);
    print_sem(sem);
    return sem;
}

SemSet sem_create_all(const char *path, int id, int sem_count, int initial_value)
{

    key_t key = ftok(path, id);
    if (key == IPC_RESULT_ERROR)
    {
        perror("Erreur lors de la création de la clé pour l'ensemble des semaphores");
        exit(1);
    }

    //---- Creating the semaphores ------

    int sem_id = semget(key, sem_count, 0666 | IPC_CREAT);
    if (sem_id == IPC_RESULT_ERROR)
    {
        perror("Erreur lors de la création des sémaphores");
        exit(2);
    }
    printf("Created Semaphore set with ID: %d\n", sem_id);

    // initializing the semaphores
    union semun arg;
    arg.val = initial_value; // Assign the value to the union's val field
    if (semctl(sem_id, 0, SETALL, arg) == IPC_RESULT_ERROR)
    {
        // Print the error with the semaphore index
        perror("Erreur lors de l'initialisation du sémaphore");
        exit(3);
    }
    else
    {
        printf("All Semaphores initialized with value %d\n",  initial_value);
    }
    return new_sem(key, sem_count, sem_id);
}
// to use when there is communication beween independent proccesses
SemSet attach_sem(const char *path, int id, int sem_count)
{
    // Generate the key for the semaphore set
    key_t key = ftok(path, id);
    if (key == IPC_RESULT_ERROR)
    {
        perror("attach_sem :: Error generating key for the semaphore set");
        exit(EXIT_FAILURE);
    }

    // Get the semaphore set ID
    int sem_id = semget(key, sem_count, 0666);
    if (sem_id == IPC_RESULT_ERROR)
    {
        perror("attach_sem :: Error attaching to semaphore set");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the SemSet struct
    

    printf("Attached to semaphore set with ID: %d\n", sem_id);
    return new_sem(key,sem_count,sem_id);
}

void P(SemSet sem, int sem_num)
{
    struct sembuf p_op = {sem_num, -1, 0};
    if (semop(sem.sem_id, &p_op, 1) == IPC_RESULT_ERROR)
    {
        perror("P operation failed ");
        exit(1);
    }
}

void V(SemSet sem, int sem_num)
{
    struct sembuf v_op = {sem_num, 1, 0};
    if (semop(sem.sem_id, &v_op, 1) == IPC_RESULT_ERROR)
    {
        perror("V operation failed");
        exit(1);
    }
}

// Wrapper for deleting a semaphore
void sem_destroy(SemSet sem)
{
    if (semctl(sem.sem_id, 0, IPC_RMID) == IPC_RESULT_ERROR)
    {
        perror("semctl - destroy");
        exit(1);
    }
}
