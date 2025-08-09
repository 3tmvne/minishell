#include <unistd.h>
#include <stdio.h>

int main() {
    int fd[2];
    pipe(fd); // create pipe

    if (fork() == 0) {
        // Child process
        close(fd[1]); // close write end
        dup2(fd[0], STDIN_FILENO); // redirect stdin to read end of pipe
        close(fd[0]); // close read end
        char buffer[100];
        read(0, buffer, 100);
        printf("Child read: %s\n", buffer);
    } 
    else {
        // Parent process
        close(fd[0]); // close read end
        dup2(fd[1], STDOUT_FILENO); // redirect stdout to write end of pipe
        close(fd[1]);
        write(1, "Hello from parent", 18);
    }

    return 0;
}
