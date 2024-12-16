#include "char_shm.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

Shm_char new_shm_char(char *block, int shmid)
{
    Shm_char shm;
    shm.value = block;
    shm.shmid = shmid;
    return shm;
}

int get_or_create_shm_char(void *filename, int len, int id)
{
    // Generate a unique key based on a file path and ID
    key_t key = ftok(filename, id);
    if (key == IPC_RESULT_ERROR)
    {
        perror("Erreur lors de la création de la clé pour la memoire partagee");
        exit(1);
    }

    // Create or get the shared memory block
    int shmid = shmget(key, sizeof(char) * len, IPC_CREAT | 0644);
    return shmid;
}

Shm_char attach_shm_char(char *filename, int len, int id)
{
    int shmid = get_or_create_shm_char(filename, len, id);
    if (shmid == IPC_RESULT_ERROR)
    {
        perror("Erreur lors de la création du segment de mémoire partagée");
        exit(2);
    }

    char *block =(char*) shmat(shmid, NULL, 0);
    if (block == (char *)IPC_RESULT_ERROR)
    {
        perror("Erreur lors de l'attachement du segment de memoire partagee");
        exit(3);
    }

    return new_shm_char(block, shmid);
}

bool detach_shm_char(Shm_char shm)
{
    return shmdt(shm.value) != IPC_RESULT_ERROR;
}

bool free_shm_char(Shm_char shm)
{
    bool error = shmctl(shm.shmid, IPC_RMID, NULL) != IPC_RESULT_ERROR;
    return error;
}

char *get_str(Shm_char shm, int ind,int str_size){
    return shm.value+ind*str_size;
}