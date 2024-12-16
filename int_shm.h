#include <stdbool.h>

#ifndef IPC_RESULT_ERROR
#define IPC_RESULT_ERROR (-1)
#endif

typedef struct
{
    int *value;
    int shmid;
} Shm_int;

Shm_int attach_shm_int(char *filename, int len, int id);
bool detach_shm_int(Shm_int shm);
bool free_shm_int(Shm_int shm);