#include <stdio.h>
#include <stdbool.h>

char bajty[50];

bool drukowalne_arr(const char* buf)
{
    const char* arr = buf;
    int i = 0;
    while(true)
    {
        char c = arr[i];
        if(c == 0)
            return true;
        if(c < 32 || c > 126)
            return false;
        i++;
    }
    return false;
}

bool drukowalne_ptr(const void* buf)
{
    const char* ptr = buf;
    while(true)
    {
        if(*ptr == 0)
            return true;
        if(*ptr < 32 || *ptr > 126)
            return false;
        ptr += 1;
    }
    return false;
}

int main(int argc, char* argv[])
{
    int tmp;
    int i = 0;
    while(scanf("%d", &tmp))
    {
        if(i == 50)
            break;
        bajty[i] = tmp;
        i++;
        if(tmp == 0)
            break;
    }

    if(drukowalne_arr(bajty))
        printf("TAK\n");
    if(drukowalne_ptr(bajty))
        printf("TAK\n");

    return 0;
}
