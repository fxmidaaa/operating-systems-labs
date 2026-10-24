#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static void close_pipes(int p[2][2])
{
    close(p[0][0]);
    close(p[0][1]);
    close(p[1][0]);
    close(p[1][1]);
}

int main(void)
{
    int p[2][2];
    pid_t children[3];
    if (pipe(p[0]) < 0) { perror("pipe 1"); return 1; }
    if (pipe(p[1]) < 0) {
        perror("pipe 2");
        close(p[0][0]); close(p[0][1]);
        return 1;
    }
    for (int i = 0; i < 3; i++) {
        children[i] = fork();
        if (children[i] < 0) {
            perror("fork");
            close_pipes(p);
            for (int j = 0; j < i; j++) waitpid(children[j], NULL, 0);
            return 1;
        }
        if (children[i] == 0) {
            if (i > 0 && dup2(p[i - 1][0], STDIN_FILENO) < 0) {
                perror("dup2 input"); _exit(1);
            }
            if (i < 2 && dup2(p[i][1], STDOUT_FILENO) < 0) {
                perror("dup2 output"); _exit(1);
            }
            close_pipes(p);
            if (i == 0)
                execlp("ls", "ls", "/usr/bin", (char *)NULL);
            else if (i == 1)
                execlp("grep", "grep", "x", (char *)NULL);
            else
                execlp("wc", "wc", "-l", (char *)NULL);
            perror("exec");
            _exit(127);
        }
    }
    close_pipes(p);
    int failed = 0;
    for (int i = 0; i < 3; i++) {
        int status;
        pid_t result;
        do { result = waitpid(children[i], &status, 0); }
        while (result < 0 && errno == EINTR);
        if (result < 0) { perror("waitpid"); failed = 1; }
        else if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) failed = 1;
    }
    return failed;
}
