#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

long file_size(FILE* file) {
    if(!file) { return 0; }
    fpos_t original;
    if(fgetpos(file, &original) != 0) {
        printf("fgetpos() failed: %i\n", errno);
        return 0;
    }
    fseek(file, 0, SEEK_END);
    long out = ftell(file);
    if(fsetpos(file, &original) != 0) {
        printf("fsetpos() failed: %i\n", errno);
    }
    return out;
}

char* file_contents(char* path) {
    FILE* file = fopen(path, "r");
    if(!file) {
        printf("Could not open file at %s\n");
        return NULL;
    }
    long size = file_size(file);
    char* contents = malloc(size);

    size_t bytes_read = fread(contents, 1, size, file);
    if(bytes_read != size) {
        free(contents);
        return NULL;
    }
    contents[size + 1] = '\0';    
    
    return contents;
}

void print_usage(char** argv) {
    printf("USAGE: %s <path_to_file>\n", argv[0]);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        print_usage(argv);
        exit(0);
    }
    
    char* path = argv[1];
    char* contents = file_contents(path);

    printf("%s\n", contents);

    free(contents);
    return 1;
}