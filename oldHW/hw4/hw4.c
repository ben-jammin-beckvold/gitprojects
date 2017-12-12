// Ben Beckvold
// HW 4 - shell w/ functions


/* See Chapter 5 of Advanced UNIX Programming:  http://www.basepath.com/aup/
 *   for further related examples of systems programming.  (That home page
 *   has pointers to download this chapter free.
 *
 * Copyright (c) Gene Cooperman, 2006; May be freely copied as long as this
 *   copyright notice remains.  There is no warranty.
 */

/* To know which "includes" to ask for, do 'man' on each system call used.
 * For example, "man fork" (or "man 2 fork" or man -s 2 fork") requires:
 *   <sys/types.h> and <unistd.h>
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define MAXLINE 200  /* This is how we declare constants in C */
#define MAXARGS 20

/* In C, "static" means not visible outside of file.  This is different
 * from the usage of "static" in Java.
 * Note that end_ptr is an output parameter.
 */

//1 = >, 2 = <, 3 = |, 4 = &
int redirect = 0;

static char * getword(char * begin, char **end_ptr) {
    char * end = begin;

    while ( *begin == ' ' )
        begin++;  /* Get rid of leading spaces. */
    end = begin;
    while ( *end != '\0' && *end != '\n' && *end != ' ' && *end != '#')
        end++;  /* Keep going. */
    if ( end == begin )
        return NULL;  /* if no more words, return NULL */
    *end = '\0';  /* else put string terminator at end of this word. */
    *end_ptr = end;
    if (begin[0] == '$') { /* if this is a variable to be expanded */
        begin = getenv(begin+1); /* begin+1, to skip past '$' */
	if (begin == NULL) {
	    perror("getenv");
	    begin = "UNDEFINED";
        }
    }
    return begin; /* This word is now a null-terminated string.  return it. */
}

/* In C, "int" is used instead of "bool", and "0" means false, any
 * non-zero number (traditionally "1") means true.
 */
/* argc is _count_ of args (*argcp == argc); argv is array of arg _values_*/
static void getargs(char cmd[], int *argcp, char *argv[])
{
    char *cmdp = cmd;
    char *end;
    int i = 0;
    redirect = 0;

    /* fgets creates null-terminated string. stdin is pre-defined C constant
     *   for standard intput.  feof(stdin) tests for file:end-of-file.
     */
    if (fgets(cmd, MAXLINE, stdin) == NULL && feof(stdin)) {
        printf("Couldn't read from standard input. End of file? Exiting ...\n");
        exit(1);  /* any non-zero value for exit means failure. */
    }
    while ( (cmdp = getword(cmdp, &end)) != NULL ) { /* end is output param */
        /* getword converts word into null-terminated string */ 
        //1 = >, 2 = <, 3 = |, 4 = &
        switch (*cmdp)
        {
            case '>':
	        redirect = 1; 
                cmdp = end + 1;
                break;
            case '<':
                redirect = 2;
                cmdp = end + 1;
                break;
            case '|':
                redirect = 3;
                cmdp = end + 1;
                break;
            case '&':
                redirect = 4;
                cmdp = end + 1;
                break;
            default:
                argv[i++] = cmdp;
                cmdp = end + 1;
        }
    }
    argv[i] = NULL; /* Create additional null word at end for safety. */
    *argcp = i;
}

//execute pipe function specifically
int pipefd[2];
static void pipeThis(int argc, char *argv[])
{
    pid_t child1, child2;
    int i;

    if (-1 == pipe(pipefd)) {
         perror("pipe");
    }

    child1 = fork();
    if (child1 == -1) {
        perror("fork"); 
        printf("  (failed to execute command)\n") ;
    }
    /* child1 > 0 implies parent */
    if (child1 > 0) {
        child2 = fork();
    }
    if (child1 == 0) {
        if (-1 == close(STDOUT_FILENO)) {
            perror("close");
        }
        if (-1 == dup2 (pipefd[1], STDOUT_FILENO)) {
            perror("dup2");
        }
        
        close(pipefd[0]);
        close(pipefd[1]);
        
        if (-1 == execvp(argv[0], argv + 1)) {
            perror("execvp");
        }
        
        exit(1);
    }
    if (child2 == -1) {
        perror("fork");
        printf("  (failed to execute command)\n") ;
    }

    if (child2 == 0) {
        if (-1 == close(STDIN_FILENO)) {
            perror("close");
        }
        if (-1 == dup2(pipefd[0],STDIN_FILENO)) {
            perror("dup2");
        }
		
        close(pipefd[0]);
        close(pipefd[1]);

        if (-1 == execvp(argv[1], argv + 1)) {
            perror("execvp");
        }
        
        exit(1);
    }
    else {
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(child1, NULL, 0);
        waitpid(child2, NULL, 0);
    }
    return;
}//end Pipe

static void execute(int argc, char *argv[])
{
    pid_t childpid; /* child process ID */
    
    if (redirect == 3)
    {
        pipeThis(argc, argv);
        return;
    }

    childpid = fork();
    if (childpid == -1) {
        perror("fork");
        printf("  (failed to execute command)\n");
    }
    if (childpid == 0) {
        //1 = >, 2 = <, 3 = |, 4 = &
        //printf("argv[1]: %s \n", argv[1]);
        //printf("argv[0]: %s \n", argv[0]);
        //printf("redirect: %d \n", redirect);
        
        switch (redirect)
	{
            case 1: // '>'
	        close(1);
                open(argv[1], O_CREAT | O_RDWR,  00666);
                argv[1] = NULL;
                break;
            case 2: // '<'
                close(0);
                open(argv[1], O_RDONLY);
                break;
            case 3: // '|'
            case 4: // '&'
            default:
                ;
        }
	if (-1 == execvp(argv[0], argv)) {
          perror("execvp");
          printf("  (couldn't find command)\n");
        }
        exit(1);
    } else if (redirect != 4)
        waitpid(childpid, NULL, 0);
    return;
}//end execute

void interrupt_handler(int signum)
{
    printf(" # [control-C]\n"); 
}

int main(int argc, char *argv[])
{
    char cmd[MAXLINE];
    char *childargv[MAXARGS];
    int childargc;
    FILE *fd;

    if (argc > 1)
    {
	fd = freopen(argv[1], "r", stdin);
    }

    signal(SIGINT, interrupt_handler);

    while (1) {
        printf("%% ");
        fflush(stdout);
	getargs(cmd, &childargc, childargv);
        if ( childargc > 0 && strcmp(childargv[0], "exit") == 0 )
            exit(0);
	else if ( childargc > 0 && strcmp(childargv[0], "logout") == 0 )
            exit(0);
        else
	    execute(childargc, childargv);
    }
    /* NOT REACHED */
}
