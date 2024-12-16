#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/wait.h>
#include "int_shm.h"
#include "char_shm.h"
#include "semaphore.h"
// we define macros instead of the struct shmbuf thing !! (much cleaner)
#define Pcpplain(sem) P(sem, 0);
#define Vcpplain(sem) V(sem, 0);

#define Pcpvide(sem) P(sem, 1);
#define Vcpvide(sem) V(sem, 1);

#define Pexplain(sem) P(sem, 2);
#define Vexplain(sem) V(sem, 2);

#define Pexvide(sem) P(sem, 3);
#define Vexvide(sem) V(sem, 3);

#define Pmutex(sem) P(sem, 4);
#define Vmutex(sem) V(sem, 4);

#define Pfinexo(sem) P(sem, 5);
#define Vfinexo(sem) V(sem, 5);
#define N 5
#define STR_SIZE 1
void coriger(int exo_id, int etudiant_id)
{
    printf("Exo corigé %d du etudiant %d\n", exo_id + 1, etudiant_id + 1);
    sleep(1); // Simuler la production
}
void enseignant(SemSet sem, Shm_char bcp, Shm_char bex, Shm_int cpt, Shm_int cpin)
{
    printf("ensegnant executed");
    int k = 0;
    int i = 0;

    for (int j = 0; j < 5; j++)
    {
        Pexvide(sem); // Attente sur le sémaphore
        bex.value[0] = '\0';
        Vexplain(sem);
        Pcpplain(sem);
        for (k = 0; k < N; k++)
        {
            coriger(j, k);
        }
        Vcpvide(sem);
    }
}

void faire_exo(int exo_id, int etudiant_id)
{
    printf("Etudiant %d a fait Exo : %d\n", etudiant_id + 1, exo_id + 1);
    sleep(1); // Simuler la production
}

void etudiant(SemSet sem, Shm_char bcp, Shm_char bex, Shm_int cpt, Shm_int cpin, int etudiant_id)
{
    printf("etudiant executed");
    int k = 0;
    int i = 0;
    //char *exo;
    for (int j = 0; j < 5; j++)
    {
        Pmutex(sem);
        if (cpt.value[0] == 0)
            Pexplain(sem);

        //exo = bex.value;
        cpt.value[0] = (cpt.value[0] + 1) % N;
        if (cpt.value[0] == 0)
            Vexvide(sem);
        Vmutex(sem);
        faire_exo(j, etudiant_id);
        Pcpvide(sem);
        char *copie_ptr = get_str(bcp, cpin.value[0], STR_SIZE);
        strcpy(copie_ptr, "solution");
        cpin.value[0] = (cpin.value[0] + 1) % N;
        if (cpin.value[0] == 0)
        {
            Vcpplain(sem);
            Vfinexo(sem);
            Pfinexo(sem);
        }
        else
        {
            Vcpvide(sem);
            Pfinexo(sem);
            Vfinexo(sem);
        }
    }
    exit(0);
}

int main()
{
    // Créer le semaphore
    SemSet sem = sem_create("/tmp", 'A', 6, 0, 1, 0, 1, 1, 0);
    // DECLARATIN DE T : LA ZONE MEMOIRE PARTAGEE TAMPON
    Shm_int cpt = attach_shm_int("/tmp", 1,'B' );
    Shm_int cpin = attach_shm_int("/tmp", 1, 'C');
    Shm_char bex = attach_shm_char("/tmp", STR_SIZE, 'D');
    Shm_char bcp = attach_shm_char("/tmp", STR_SIZE * N, 'E');


    
    if (fork() == 0)
    { // Processus fils (consommateur)
        printf("\nsemaphore fully created\n");

        etudiant(sem, bcp, bex, cpt, cpin, 0);
        printf("\nsemaphore fully created\n");
        printf("\nsemaphore fully created\n");

        exit(EXIT_SUCCESS); // Exit after finishing the work
    }
    else
    { // Processus parent

        for (int i = 0; i < N-1; i++)
        {
            pid_t pid = fork(); // Create a subprocess
            if (pid < 0)
            {
                // Error handling for fork
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                etudiant(sem, bcp, bex, cpt, cpin, i+1);
                printf("hello");
                exit(EXIT_SUCCESS); // Exit after finishing the work
            }
            // Parent process continues to create the next subprocess
        }
        enseignant(sem, bcp, bex, cpt, cpin);


        while (wait(NULL) != -1);

        detach_shm_int(cpt);  // detach
        detach_shm_int(cpin); // detach
        detach_shm_char(bcp); // detach
        detach_shm_char(bex); // detach

        free_shm_int(cpt);  // free
        free_shm_int(cpin); // free
        free_shm_char(bcp); // free
        free_shm_char(bex); // free

        // Supprimer le sémaphore
        sem_destroy(sem);
        printf("Sémaphore supprimé\n");
    }

    return 0;
}
