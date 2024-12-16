#include "int_shm.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

Shm_int new_shm_int(int *block, int shmid)
{
    Shm_int shm;
    shm.value = block;
    shm.shmid = shmid;
    return shm;
}

int get_or_create_shm_int(void *filename, int len, int id)
{
    // Generate a unique key based on a file path and ID
    key_t key = ftok(filename, id);
    if (key == IPC_RESULT_ERROR)
    {
        perror("Erreur lors de la création de la clé pour la memoire partagee");
        exit(1);
    }

    // Create or get the shared memory block
    int shmid = shmget(key, sizeof(int)*len, IPC_CREAT | 0666);
    return shmid;
}

Shm_int attach_shm_int(char *filename, int len, int id)
{
    int shmid = get_or_create_shm_int(filename, len, id);
    if (shmid == IPC_RESULT_ERROR)
    {
        perror("Erreur lors de la création du segment de mémoire partagée");
        exit(2);
    }

    int *block =(int*) shmat(shmid, NULL, 0);
    if (block == (void *)IPC_RESULT_ERROR)
    {
        perror("Erreur lors de l'attachement du segment de memoire partagee");
        exit(3);
    }

    return new_shm_int(block, shmid);
}

bool detach_shm_int(Shm_int shm)
{
    return shmdt(shm.value) != IPC_RESULT_ERROR;
}

bool free_shm_int(Shm_int shm)
{
    bool error = shmctl(shm.shmid, IPC_RMID, NULL) != IPC_RESULT_ERROR;
    return error;
}