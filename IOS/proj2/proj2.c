#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// Structure for storing user's input
typedef struct
{
    int NO;
    int NH;
    int TI;
    int TB;
} params;

// Semaphores
sem_t *mutex = NULL;
sem_t *mutex2 = NULL;
sem_t *oxyQueue = NULL;
sem_t *hydroQueue = NULL;
sem_t *fileWrite = NULL;
sem_t *turnstile = NULL;
sem_t *turnstile2 = NULL;

// Shared memory
int *oxygen = NULL;
int *hydrogen = NULL;
int *molecules = NULL;
int *lineCount = NULL;
int *count = NULL;
int *done = NULL;

FILE *file = NULL;

// Checks for parameter validity
params parseParams(int argc, char **argv)
{
    int err = 0;
    params p = {0};
    if (argc != 5)
    {
        fprintf(stderr, "Program called with bad parameters.\n");
        exit(1);
    }

    char *ptr;

    p.NO = strtol(argv[1], &ptr, 10);
    if (*ptr != '\0' || ptr == argv[1] || p.NO < 1)
        err = 1;

    p.NH = strtol(argv[2], &ptr, 10);
    if (*ptr != '\0' || ptr == argv[2] || p.NH < 1)
        err = 1;

    p.TI = strtol(argv[3], &ptr, 10);
    if (*ptr != '\0' || ptr == argv[3] || p.TI < 0 || p.TI > 1000)
        err = 1;

    p.TB = strtol(argv[4], &ptr, 10);
    if (*ptr != '\0' || ptr == argv[4] || p.TB < 0 || p.TB > 1000)
        err = 1;

    if (err == 1)
    {
        fprintf(stderr, "Parameters weren't in correct format.\n");
        exit(1);
    }

    return p;
}

// Closes and unlinks all semaphores and then unmaps shared memory
void clean()
{
    sem_close(mutex);
    sem_unlink("/xbarto0g_h2o_mutex");

    sem_close(mutex2);
    sem_unlink("/xbarto0g_h2o_mutex2");

    sem_close(oxyQueue);
    sem_unlink("/xbarto0g_h2o_oxyQueue");

    sem_close(hydroQueue);
    sem_unlink("/xbarto0g_h2o_hydroQueue");

    sem_close(turnstile);
    sem_unlink("/xbarto0g_h2o_turnstile");

    sem_close(turnstile2);
    sem_unlink("/xbarto0g_h2o_turnstile2");

    sem_close(fileWrite);
    sem_unlink("/xbarto0g_h2o_fileWrite");

    munmap(oxygen, sizeof *oxygen);
    munmap(hydrogen, sizeof *hydrogen);
    munmap(molecules, sizeof *molecules);
    munmap(lineCount, sizeof *lineCount);
    munmap(count, sizeof *count);
    munmap(done, sizeof *done);
}

// Prints error after unsuccessful attempt to map memory or initialize semaphore
void init_failure(int message)
{
    if (message == 1)
        fprintf(stderr, "Couldn't allocate shared memory.\n");
    if (message == 0)
        fprintf(stderr, "Couldn't open semaphore.\n");
    clean();
    if (file != NULL)
        fclose(file);
    exit(1);
}

// Tries to map memory, initialize semaphores, and open file
void init()
{
    lineCount = mmap(NULL, sizeof *lineCount, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (lineCount == MAP_FAILED)
        init_failure(1);

    *lineCount = 0;

    hydrogen = mmap(NULL, sizeof *hydrogen, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (hydrogen == MAP_FAILED)
        init_failure(1);

    *hydrogen = 0;

    oxygen = mmap(NULL, sizeof *oxygen, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (oxygen == MAP_FAILED)
        init_failure(1);

    *oxygen = 0;

    molecules = mmap(NULL, sizeof *molecules, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (molecules == MAP_FAILED)
        init_failure(1);

    *molecules = 1;

    count = mmap(NULL, sizeof *count, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (count == MAP_FAILED)
        init_failure(1);

    *count = 0;

    done = mmap(NULL, sizeof *done, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (done == MAP_FAILED)
        init_failure(1);

    *done = 0;

    mutex = sem_open("/xbarto0g_h2o_mutex", O_CREAT | O_EXCL, 0666, 1);
    if (mutex == SEM_FAILED)
        init_failure(0);

    mutex2 = sem_open("/xbarto0g_h2o_mutex2", O_CREAT | O_EXCL, 0666, 1);
    if (mutex2 == SEM_FAILED)
        init_failure(0);

    oxyQueue = sem_open("/xbarto0g_h2o_oxyQueue", O_CREAT | O_EXCL, 0666, 0);
    if (oxyQueue == SEM_FAILED)
        init_failure(0);

    hydroQueue = sem_open("/xbarto0g_h2o_hydroQueue", O_CREAT | O_EXCL, 0666, 0);
    if (hydroQueue == SEM_FAILED)
        init_failure(0);

    fileWrite = sem_open("/xbarto0g_h2o_fileWrite", O_CREAT | O_EXCL, 0666, 1);
    if (fileWrite == SEM_FAILED)
        init_failure(0);

    turnstile = sem_open("/xbarto0g_h2o_turnstile", O_CREAT | O_EXCL, 0666, 0);
    if (turnstile == SEM_FAILED)
        init_failure(0);

    turnstile2 = sem_open("/xbarto0g_h2o_turnstile2", O_CREAT | O_EXCL, 0666, 1);
    if (turnstile2 == SEM_FAILED)
        init_failure(0);

    file = fopen("proj2.out", "w");
    if (file == NULL)
        init_failure(0);

    setbuf(file, NULL);
}

// Simulates creation of molecule/atom going to queue
void rand_sleep(unsigned wait_time)
{
    srand(time(NULL) * getpid());
    usleep((rand() % (wait_time + 1)) * 1000);
}

// Signals queue semaphores so redundant children can be killed
void process_cleanup(int mol_max, params p)
{
    // Free remaining oxygens
    for (int i = mol_max; i < p.NO; i++)
    {
        sem_post(oxyQueue);
    }

    // Free remaining hydrogens
    for (int i = mol_max * 2; i < p.NH; i++)
    {
        sem_post(hydroQueue);
    }
}

// Cleanup after failed fork
void forkError(params p)
{
    *done = 2;
    process_cleanup(0, p);
    while (wait(NULL) > -1)
        ;
    clean();
    fclose(file);
}

void oxygen_f(int id_o, unsigned wait_time, unsigned create_time, int mol_max, params p)
{
    sem_wait(fileWrite);
    fprintf(file, "%d: O %d: started\n", ++(*lineCount), id_o);
    sem_post(fileWrite);

    rand_sleep(wait_time);

    sem_wait(fileWrite);
    fprintf(file, "%d: O %d: going to queue\n", ++(*lineCount), id_o);
    sem_post(fileWrite);

    sem_wait(mutex);

    ++(*oxygen);

    // If there are enough atoms for making a molecule, signal both queues and update count
    if (*hydrogen >= 2)
    {
        sem_post(hydroQueue);
        sem_post(hydroQueue);
        *hydrogen -= 2;
        sem_post(oxyQueue);
        *oxygen -= 1;
    }
    else
    {
        sem_post(mutex);
    }

    // Atom waits until molecule can be created
    sem_wait(oxyQueue);

    // Atom can't be made into a molecule
    if (*done > 0)
    {
        // If done is set to 1, program ends peacefully; 2 is for error handling
        if (*done == 1)
        {
            sem_wait(fileWrite);
            fprintf(file, "%d: O %d: not enough H\n", ++(*lineCount), id_o);
            sem_post(fileWrite);
        }
        sem_post(mutex);
        fclose(file);
        exit(0);
    }

    sem_wait(fileWrite);
    fprintf(file, "%d: O %d: creating molecule %d\n", ++(*lineCount), id_o, *molecules);
    sem_post(fileWrite);

    rand_sleep(create_time);

    // Reusable barrier for molecule creation
    sem_wait(mutex2);
    ++(*count);
    if (*count == 3)
    {
        sem_wait(turnstile2);
        sem_post(turnstile);
    }
    sem_post(mutex2);

    sem_wait(turnstile);
    sem_post(turnstile);

    sem_wait(fileWrite);
    fprintf(file, "%d: O %d: molecule %d created\n", ++(*lineCount), id_o, *molecules);
    sem_post(fileWrite);

    sem_wait(mutex2);
    --(*count);
    if (*count == 0)
    {
        sem_wait(turnstile);
        sem_post(turnstile2);
    }
    sem_post(mutex2);

    sem_wait(turnstile2);
    sem_post(turnstile2);
    ++(*molecules);

    // All molecules were created
    if (*molecules == mol_max + 1)
    {
        *done = 1;
        process_cleanup(mol_max, p);
    }

    sem_post(mutex);
    fclose(file);
    exit(0);
}

void hydrogen_f(int id_o, unsigned wait_time)
{
    sem_wait(fileWrite);
    fprintf(file, "%d: H %d: started\n", ++(*lineCount), id_o);
    sem_post(fileWrite);

    rand_sleep(wait_time);

    sem_wait(fileWrite);
    fprintf(file, "%d: H %d: going to queue\n", ++(*lineCount), id_o);
    sem_post(fileWrite);

    sem_wait(mutex);

    ++(*hydrogen);

    // If there are enough atoms for making a molecule, signal both queues and update count
    if (*hydrogen >= 2 && *oxygen >= 1)
    {
        sem_post(hydroQueue);
        sem_post(hydroQueue);
        *hydrogen -= 2;
        sem_post(oxyQueue);
        *oxygen -= 1;
    }
    else
    {
        sem_post(mutex);
    }

    sem_wait(hydroQueue);

    if (*done > 0)
    {
        // If done is set to 1, program ends peacefully; 2 is for error handling
        if (*done == 1)
        {
            sem_wait(fileWrite);
            fprintf(file, "%d: H %d: not enough O or H\n", ++(*lineCount), id_o);
            sem_post(fileWrite);
        }
        sem_post(mutex);
        fclose(file);
        exit(0);
    }

    sem_wait(fileWrite);
    fprintf(file, "%d: H %d: creating molecule %d\n", ++(*lineCount), id_o, *molecules);
    sem_post(fileWrite);

    // Reusable barrier for molecule creation
    sem_wait(mutex2);
    ++(*count);
    if (*count == 3)
    {
        sem_wait(turnstile2);
        sem_post(turnstile);
    }
    sem_post(mutex2);

    sem_wait(turnstile);
    sem_post(turnstile);

    sem_wait(fileWrite);
    fprintf(file, "%d: H %d: molecule %d created\n", ++(*lineCount), id_o, *molecules);
    sem_post(fileWrite);

    sem_wait(mutex2);
    --(*count);
    if (*count == 0)
    {
        sem_wait(turnstile);
        sem_post(turnstile2);
    }
    sem_post(mutex2);

    sem_wait(turnstile2);
    sem_post(turnstile2);

    fclose(file);
    exit(0);
}

int main(int argc, char **argv)
{
    params p = parseParams(argc, argv);

    init();

    // Calculate number of atoms, that can be created
    int maxMolAmount = (((p.NH / 2) < (p.NO)) ? (p.NH / 2) : (p.NO));

    // If no molecules can be created, atoms can be killed right after they enter queue
    if (maxMolAmount == 0)
    {
        *done = 1;
        process_cleanup(0, p);
    }

    // Create oxygen process NO number of times
    for (int id = 1; id <= p.NO; ++id)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Hydro fork %d failed.\n", getpid());
            forkError(p);
            exit(1);
        }
        if (pid == 0)
        {
            oxygen_f(id, p.TI, p.TB, maxMolAmount, p);
        }
    }

    // Create hydrogen process NH number of times
    for (int id = 1; id <= p.NH; ++id)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Oxy fork %d failed.\n", getpid());
            forkError(p);
            exit(1);
        }
        if (pid == 0)
        {
            hydrogen_f(id, p.TI);
        }
    }

    // Wait for all children to die
    while (wait(NULL) > 0)
        ;
    clean();
    fclose(file);
    return 0;
}
