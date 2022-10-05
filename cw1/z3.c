#include <stdio.h>
#include <stdbool.h>

char bajty[50];

bool drukowalne_arr(const void* buf, int len)
{
    const char* arr = buf;
    int i;
    for(i = 0; i < len; i++)
    {
        char c = arr[i];
        if(c < 32 || c > 126)
            return false;
    }
    return true;
}

bool drukowalne_ptr(const void* buf, int len)
{
    const char* ptr = buf;
    int i;
    for(i = 0; i < len; i++)
    {
        if(*ptr < 32 || *ptr > 126)
            return false;
        ptr += 1;
    }
    return true;
}

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
        bajty[i] = tmp;
        i++;
    }

    if(drukowalne_arr(bajty, i))
        printf("TAK\n");
    if(drukowalne_ptr(bajty, i))
        printf("TAK\n");

    return 0;
}
