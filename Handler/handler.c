#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/signal.h>
#include <ucontext.h>
 
void sighandler(int signum, siginfo_t *info, void *context) { 
    FILE *file = fopen("dump.out", "w");
    fprintf(file, "SIGSEGV is handled\naddress: %p, si_code: ", info->si_addr); 
    if (info->si_code == SEGV_MAPERR) {
        fprintf(file, "SEGV_MAPERR\n"); 
    } else {
        fprintf(file, "SIGV_ACCERR\n"); 
    }
    fprintf(file, "gregs:\n");
    ucontext_t* ucontext = (ucontext_t *) context;
    for (int i = 0; i < 23; ++i) {
        fprintf(file, "0x%08x\n", ucontext->uc_mcontext.gregs[i]);
    }
    fprintf(file, "nearest addresses %p %p\n", info->si_addr - 1, info->si_addr + 1);
    exit(1); 
}
 
int main() {     
    struct sigaction action;  
    memset(&action, 0, sizeof(action)); 
    action.sa_sigaction = sighandler; 
    action.sa_flags = SA_SIGINFO; 
    sigaction(SIGSEGV, &action, NULL); 

    int *ptr = (int *) 0x64;
    *ptr = -1;
    return 0; 
}