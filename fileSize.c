
/* 
A SIMPLE C PROGRAM THAT DETECT BOTH FOLDER & ALL FILES 
SIZES INSIDE OF IT 
RETURN THEM IN HUMAN READABLE FORMAT 
*/



#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#define ull unsigned long long 
#define SEP '/'

void formatterSize(ull size,char* out_buffer,size_t outbuff_size){
    const char* sz_units[] = {"Byte","KByte","Mbyte","Gbyte"};
    int currentIdx =0;
    double readSize = (double) size;

    while(readSize >= 1024 && currentIdx < 3){
        readSize /= 1024.0;
        currentIdx++; // 0:->Byte....3:->Gbyte
    }

    snprintf(out_buffer,outbuff_size,"%.3f %s",readSize,sz_units[currentIdx]);
}


ull getSize(const char* fpath){
    struct stat st;
    ull totalSize = 0;

    if( stat(fpath,&st)!=0){
        fprintf(stderr,"cannot stat file: %s",fpath);
        return 0;
    }

    if (!S_ISDIR(st.st_mode)){
        char buffer[64];
        formatterSize((ull)st.st_size,buffer,sizeof(buffer));
        printf("%-60s %12s.\n",fpath,buffer);
        return (ull) st.st_size;
    }
    DIR* directory = opendir(fpath);
    if(!directory){
        fprintf(stderr,"cannot open dir: %s",fpath);
        return 0;
    }
    struct dirent* start;
    char buffer[4096];
    while((start = readdir(directory)) != NULL){
        // skip . & ..
        if (strcmp(start->d_name , ".") == 0 ||strcmp(start->d_name , "..") ==0 ){
            // skip next iterations
            continue;
        }

        snprintf(buffer,sizeof(buffer),"%s%c%s",fpath,SEP,start->d_name);
        totalSize += getSize(buffer);
    }
    closedir(directory);
    return totalSize;

}


int main(int argc , char* argv[]){
    if (argc < 2 ){
        printf("feed the file path via cmd");
        return 1;
    }
    char* progName = argv[0];

    clock_t start = clock();
    ull size = getSize(argv[1]);
    clock_t end = clock();
    double totalTime = (double) (end-start) / CLOCKS_PER_SEC;
    double totalTimeMnt = totalTime / 60.0;
    double totalTimeHrs = totalTimeMnt / 60.0;

    char buffer[64];
    formatterSize(size,buffer,sizeof(buffer));
    printf("\nTotal size of '%s' : %s\n", argv[1], buffer);
    printf("Time taken: %.6f minutes\n", totalTimeMnt);


    return 0;
}