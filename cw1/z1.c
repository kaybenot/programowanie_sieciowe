#include <stdio.h>

int liczby[50];

int main(int argc, char* argv[])
{
    int tmp;
    int i = 0;
    while(scanf("%d", &tmp))
    {
        if(tmp == 0)
            break;
        if(i == 50)
            break;
        liczby[i] = tmp;
        i++;
    }

    int j;
    for(j = 0; j <= i; j++)
    {
        int l = liczby[j];
        if(l > 10 && l < 100)
            printf("%d\n", l);
    }

    return 0;
}
