#include <hellomake.h>
#include <stdio.h>

int main() {
    unsigned char charType = 0;

    // call a function in another file
    myPrintHelloMake();

    // test what happens when a datatype rolls over
    for (int i = 0; i< 300; i++)
    {
        charType++;
        if (i > 250 || i < 5)
            printf ("charType: %d\n", charType);
    }

    return(0);
}
