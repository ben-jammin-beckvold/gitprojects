#include <stdio.h>
#define IN   1  /* inside a word */
#define OUT  0  /* outside a word */

/* count lines, words, and characters in input */
/* to run, try:    gcc THIS_FILE.c ; cat ANY_FILE | ./a.out */

void main()
{
    char x1[100]="The quick brown fox jumped over the lazy dog.\n";
    int c, nw, nc, state, i;
    state = OUT;
    nw = nc = i = 0;
    char *x1ptr = x1;
    while ((c = *(x1ptr + i)) != '\n') {
        ++nc;
        if (c == ' ' || c == '\n' || c == '\t')
            state = OUT;
        else if (state == OUT) {
            state = IN;
            ++nw;
        }
	i++;
    }
    printf("%d %d \n", nw, nc);
}
