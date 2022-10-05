#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int desc = open(argv[1], 0);
    if(desc == -1)
    {
        perror("Could not open input file");
        return -1;
    }
    int desc_out = open(argv[2], O_TRUNC | O_CREAT | O_RDWR, 0777);
    if(desc_out == -1)
    {
        perror("Could not open/create output file");
        return -1;
    }

    int seek_ret = lseek(desc, 0, SEEK_END);
    if(seek_ret == -1)
    {
        perror("Error seeking file");
        return -1;
    }
    int size = lseek(desc, 0, SEEK_CUR);
    seek_ret = lseek(desc, 0, SEEK_SET);
    if(seek_ret == -1)
    {
        perror("Error seeking file");
        return -1;
    }

    char* bytes = malloc(size * sizeof(char));
    int bytes_read = read(desc, bytes, size);
    if(bytes_read != size)
    {
        perror("Read less bytes than input file contains");
        return -1;
    }
    if(bytes_read == -1)
    {
        perror("Error reading input file");
        return -1;
    }

    int new_size = 0;
    int line = 1;
    int i;
    for(i = 0; i < size; i++)
    {
        char c = bytes[i];
        if(line % 2 != 0)
            new_size++;
        if(c == '\n')
            line++;
    }
    char* new_bytes = malloc(new_size * sizeof(char));
    line = 1;
    new_size = 0;
    for(i = 0; i < size; i++)
    {
        char c = bytes[i];
        if(line % 2 != 0)
        {
            new_bytes[new_size] = c;
            new_size++;
        }
        if(c == '\n')
            line++;
    }

    int written = write(desc_out, new_bytes, new_size);
    if(written == -1)
    {
        perror("Error writing to out file");
        return -1;
    }

    if(close(desc) == -1 || close(desc_out) == -1)
    {
        perror("Error closing files");
        return -1;
    }

    return 0;
}