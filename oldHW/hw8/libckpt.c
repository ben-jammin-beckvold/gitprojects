#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

void ckpt_fnc()
{
    void* buf[10000];
    
    ucontext_t thisContext;
    int fin = open("/proc/self/maps", O_RDONLY);
    int chkptImage = open("ckpt.img", O_WRONLY);

    int numRead = read(fin, buf, 10000);
    close(fin);

    getContext(&thisContext);

    write(chkptImage, &thisContext, sizeof(thisContext));
    write(chkptImage, buf, numRead);    

//    printf(buf);
    close(chkptImage); 
    
}

__attribute__((constructor))
    void myConstructor()
{
    signal(SIGUSR2, ckpt_fnc);
}
