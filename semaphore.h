#include <sys/types.h>
#define IPC_RESULT_ERROR (-1)
union semun
{
    int val;               // Utilisé pour SETVAL
    struct semid_ds *buf;  // Utilisé pour IPC_STAT et IPC_SET
    unsigned short *array; // Utilisé pour GETALL et SETALL
};
typedef struct
{
    key_t sem_id;
    int key_id;
    int sem_count;
} SemSet;

void P(SemSet sem, int sem_num);
void V(SemSet sem, int sem_num);
SemSet sem_create_all(const char *path, int id, int sem_count,int initial_value);
SemSet sem_create(const char *path, int id, int sem_count, ... /* initial values */);
void sem_destroy(SemSet sem);
void print_sem(SemSet sem);