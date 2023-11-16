#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>

using namespace std;

void sem_wait(int);
void sem_signal(int);

int main(){
    int mem_size;
    cout<<"Enter desired shared memory size (in bytes): ";
    cin>>mem_size;

    // in order to create a mutex I will use a...
    // semaphore - maintains a counter for number of allowed accesses to a resource.
    //      * In this scenario I am using a binaty semaphore but counting semaphores exist

    key_t semKey = ftok("semfile", 66); //create unique key
    int semID = semget(semKey, 1, 0666 | IPC_CREAT); // system call to create indentifier for semaphore
    semctl(semID, 0, SETVAL, 0); //sets semaphore 'count' to 0

    key_t shmKey = ftok("smhfile", 65); // unique key for shared mem
    int shmid = shmget(shmKey, mem_size, 0666 | IPC_CREAT); //create shared mem segment
    char *str = (char*)shmat(shmid, nullptr, 0); // attach string to shared mem

    pid_t pid = fork();

    if(pid < 0){
        cout<<"Error forking process"<<endl;
    } else if(pid == 0){
        sem_wait(semID); //wait till signal is recieved
        std::cout << "Child process reading from shared memory: " << str << std::endl;
        shmdt(str);
    } else {
        cout<<"Parent process writing to shared memory: ";
        cin.ignore();
        cin.getline(str, mem_size);

        sem_signal(semID); //signal child parent is done
        wait(nullptr);
        shmdt(str);
        shmctl(shmid, IPC_RMID, nullptr);
    }
    semctl(semID, 0, IPC_RMID); //delete semaphore
    shmctl(shmid, IPC_RMID, NULL); // destroy the shared memory
}

void sem_wait(int semId) {
    struct sembuf p_op = {0, -1, 0}; //defines operation to preform on semaphore (decrement to wait)
    semop(semId, &p_op, 1); // applies it to semaphore
}

void sem_signal(int semId) {
    struct sembuf v_op = {0, 1, 0};
    semop(semId, &v_op, 1);
}
