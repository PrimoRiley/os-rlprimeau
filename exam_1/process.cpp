#include <stdio.h>
#include <unistd.h>



int main() {
    // Fork the current process
    int test = 3; 
    pid_t child_pid = fork();
    
    printf("PID: PID=%d\n", child_pid);

    if (child_pid == -1) {
        // Forking failed
        perror("Fork failed");
        return 1;
    } else if (child_pid == 0) {
        // This code is executed by the child process
        printf("Child process: PID=%d\n", getpid());
        test = test + 8;
    } else {
        // This code is executed by the parent process
        printf("Parent process: PID=%d, Child PID=%d\n", getpid(), child_pid);
        test = test + 2;
    }

    printf("Test: %d\n", test);

    return 0;
}
