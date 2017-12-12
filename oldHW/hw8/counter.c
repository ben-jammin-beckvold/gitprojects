#include <stdio.h>
#include <unistd.h>

void main()
{
    int count = 0;
    while(1 == 1)
    {
        printf("%d\t", count);
       
        count = count + 1;
        
        //getchar();
        sleep(1);
    }
}
