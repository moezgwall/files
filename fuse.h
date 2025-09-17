/* operations on files */
/* using c language */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
char *Read_File(const char *path);
bool Append_File(const char *path, char *line);
bool Create_File(const char *path);
bool Read_BylineFile(const char *path);
bool IsFound(const char *path);
long File_Size(const char *path);
/*readfile*/

char *Read_File(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        fprintf(stderr, "cannot open file: %s \n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    rewind(file);
    char *buffer = (char *)malloc(len + 1);
    if (!buffer)
    {
        fprintf(stderr, "malloc failed \n");
        fclose(file);
        return NULL;
    }

    size_t readSize = fread(buffer, 1, len, file);
    if (readSize != len)
    {
        fprintf(stderr, "error reading file: %s\n", path);
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[len] = '\0';
    fclose(file);
    return buffer;
}

/* create a file*/
bool Create_File(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "failed to create a file : %s \n", filename);
        return false; // on fail
    }

    fclose(file);

    return true;
}
/* appending to file */
bool Append_File(const char *path, char *line)
{
    FILE *file = fopen(path, "a");
    if (!file)
    {
        fprintf(stderr, "appending to : %s failed! ..\n", path);
        return false;
    }
    fputs(line, file);
    fclose(file);

    return true;
}

/* reading line by line */
bool Read_BylineFile(const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        fprintf(stderr, "cannot read file : %s", path);
        return false; // fail
    }
    char buffer[1024]; // 1kb
    while (fgets(buffer, sizeof(buffer), file))
    {
        printf("%s", buffer);
    }

    fclose(file);

    return true; // sucesss
}
/* does file exist ?*/
bool IsFound(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file)
    {
        fclose(file);
        return true;
    }

    return false;
}

/* get the file size in bytes*/

long File_Size(const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        fprintf(stderr, "failed to open the file : %s \n", path);
        return -1;
    }
    fseek(file, 1, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size;
}