#include <stdio.h>
#include <stdlib.h>

int* liczby;

int main(int argc, char* argv[])
{
    liczby = malloc(sizeof(int) * 50);
    int* ptr = liczby;

    int tmp;
    int i = 0;
    while(scanf("%d", &tmp))
    {
        if(tmp == 0)
            break;
        if(i == 50)
            break;
        *ptr = tmp;
        ptr += 1;
        i++;
    }

    ptr = liczby;
    int j;
    for(j = 0; j <= i; j++)
    {
        int l = *ptr;
        if(l > 10 && l < 100)
            printf("%d\n", l);
        ptr += 1;
    }

    return 0;
}
