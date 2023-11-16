#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main() {
    key_t key = ftok("shmfile", 65); // create a unique key
    int shmid = shmget(key, 1024, 0666|IPC_CREAT); // create a shared memory segment
    char *str = (char*) shmat(shmid, (void*)0, 0); // attach to shared memory

    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("Child process reading from shared memory: %s\n", str);
        shmdt(str); // detach from shared memory
    } else {
        // Parent process
        printf("Parent process writing to shared memory\n");
        fgets(str, 1024, stdin); // read up to 1024 characters from stdin
        printf("Parent process wrote: %s\n", str);
        wait(NULL); // wait for child to finish
        shmdt(str); // detach from shared memory
        shmctl(shmid, IPC_RMID, NULL); // destroy the shared memory
    }

    return 0;
}
