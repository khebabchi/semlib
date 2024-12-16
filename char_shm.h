#include <stdbool.h>

#define IPC_RESULT_ERROR (-1)

typedef struct
{
    char *value;
    int shmid;
} Shm_char;

Shm_char attach_shm_char(char *filename, int len, int id);
bool detach_shm_char(Shm_char shm);
bool free_shm_char(Shm_char shm);
char* get_str(Shm_char shm,int ind,int str_size);