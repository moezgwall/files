/* operations on files */
/* using c language */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)

char *ReadAfile(const char *path);
bool AppendToFile(const char *path, const char *line);
bool CreateFile(const char *path);
bool DeleteFile(const char *path);
bool RenameFile(const char *oldname, const char *newname);
bool ReadByLineFile(const char *path);
bool IsFound(const char *path);
long FileSize(const char *path);
bool WriteToFile(const char *path, const char *data);
const char *GetFileExtension(const char *path);
/* reading a file*/
char *ReadAfile(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        LOG_ERROR("cannot open file: [%s]", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    rewind(file);
    char *buffer = (char *)malloc(len + 1);
    if (!buffer)
    {
        LOG_ERROR("allocating memory failed");
        fclose(file);
        return NULL;
    }

    size_t readSize = fread(buffer, 1, len, file);
    if (readSize != len)
    {
        LOG_ERROR("error reading file: [%s]", path);
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[len] = '\0';
    fclose(file);
    return buffer;
}

/* creating a file*/
bool CreateFile(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        LOG_ERROR("failed to create a file : [%s]", filename);
        return false; // on fail
    }

    fclose(file);

    return true;
}
/* appending to file */
bool AppendToFile(const char *path, const char *line)
{
    FILE *file = fopen(path, "a");
    if (!file)
    {
        LOG_ERROR("appending to : [%s] failed!", path);
        return false;
    }
    fputs(line, file);
    fclose(file);

    return true;
}
/* removing a file*/
bool DeleteFile(const char *path)
{
    return remove(path) == 0;
}
/* renaming a file*/
bool RenameFile(const char *oldname, const char *newname)
{
    return rename(oldname, newname) == 0;
}

/* reading line by line */
bool ReadByLineFile(const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        LOG_ERROR("cannot read file : [%s]", path);
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

long FileSize(const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        LOG_ERROR("failed to open file : [%s]", path);
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size;
}
<<<<<<< HEAD
=======
/* get a file extension*/
const char *GetFileExtension(const char *path)
{
    const char *ext = strrchr(path, '.');
    if (!ext || ext == path)
        return "";
    return ext + 1;
}

/* overwrite a file */
bool WriteToFile(const char *path, const char *data)
{

    FILE *file = fopen(path, "w");
    if (!file)
    {
        LOG_ERROR("Failed to write to file : [%s]", path);
        return false;
    }
    fputs(data, file);
    fclose(file);
    return true;
}
>>>>>>> 8a481b741317ba9204cf37d23b1aea6a7613bc46
