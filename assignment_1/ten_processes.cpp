#include <stdio.h>
#include <unistd.h>

int main() {
    int num_forks = 9;  // Number of additional processes (children) to create
    int process_num = 0; 

    for (int i = 0; i < num_forks; i++) {
        // printf("i : %d\n", i);
        pid_t child_pid = fork();  // Fork the current process
        
        if (child_pid == -1) {
            // Forking failed
            perror("Fork failed");
            return 1;
        } else if (child_pid == 0) {
            // child code
            printf("Child process: PID=%d\n", getpid());
            break;  // Break out of the loop to prevent child from forking
        } else {
            process_num++;
            printf("Parent process: PID=%d, Created Child PID=%d\n", getpid(), child_pid);
        }

        printf("Number of Children Counter: %d\n", process_num);
    }
    return 0;
}
