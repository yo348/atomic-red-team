#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __APPLE__
#define PX_TRACEME PT_TRACE_ME
#else /* LINUX */
#include <sys/reg.h>
#define PX_TRACEME PTRACE_TRACEME
#endif


int main()
{
    pid_t child = 0;
    long res = 0;
    child = fork();
    if(child == 0) {
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, 1);
        dup2(fd, 2);

        ptrace(PX_TRACEME, 0, 0, 0);
        execl("/bin/ls", "ls", NULL);
    }
    else {
        wait(NULL);
#ifdef __APPLE__
        // Ptrace cannot read other processes data on MacOS
        res = ptrace(PT_KILL, child, 0, 0);
#else
        res = ptrace(PTRACE_PEEKUSER, child, (char*)(sizeof(void*) * ORIG_RAX), 0);
        ptrace(PTRACE_CONT, child, 0, 0);
#endif
    }
	printf("T1055-008-ptrace.c child trace has executed %ld\n", res);
    return 0;
}
