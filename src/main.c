#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

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
    contents[bytes_read] = '\0';    
    
    return contents;
}

void print_usage(char** argv) {
    printf("USAGE: %s <path_to_file>\n", argv[0]);
}

typedef struct Error {
    enum ErrorType {
        ERROR_NONE = 0,
        ERROR_ARGUMENTS,
        ERROR_TYPE,
        ERROR_GENERIC,
        ERROR_SYNTAX,
        ERROR_TODO,
    } type;
    char* msg;
} Error;

Error err_ok = { ERROR_NONE, NULL };
#define ERROR_CREATE(n, t, msg) Error (n) = { (t), (msg) }
#define ERROR_PREP(n, t, message) \
    (n).type = (t); \
    (n).msg = (message)

void print_error(Error err) {
    if(err.type == ERROR_NONE) return;
    printf("ERROR - ");
    switch(err.type) {
    default:
        printf("Unknown error type...");
        break;
    case ERROR_TODO:
        printf("TODO (not implemented!)");
        break;
    case ERROR_SYNTAX:
        printf("Invalid syntax");
        break;
    case ERROR_ARGUMENTS:
        printf("Invalid arguments");
        break;
    case ERROR_TYPE:
        printf("Mismatched types");
        break;
    case ERROR_NONE:
    case ERROR_GENERIC:
        break;
    }
    putchar('\n');
    if(err.msg) {
        printf(" : %s\n", err.msg);
    }
}

const char* whitespace = " \r\n";
const char* delimiters = " \r\n,():";

Error lex(char* source, char** beg, char** end) {
    Error err = err_ok;
    if(!source || !beg || !end) {
        ERROR_PREP(err, ERROR_ARGUMENTS, "Cannot lex empty source.");
        return err;
    }

    *beg = source;
    *beg += strspn(*beg, whitespace);
    *end = *beg;
    if(**end == '\0') { return err; }
    *end += strcspn(*beg, delimiters);
    if(*end == *beg) {
        *end += 1;
    }

    return err;
}

Error parse_expr(char* source) {
    char* beg = source;
    char* end = source;
    Error err = err_ok;
    while((err = lex(end, &beg, &end)).type == ERROR_NONE) {
        if(end - beg == 0) { break; }
        printf("lexed: %.*s\n", end - beg, beg);
    }
    return err;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        print_usage(argv);
        exit(0);
    }
    
    char* path = argv[1];
    char* contents = file_contents(path);

    Error err = parse_expr(contents);
    print_error(err);
    
    free(contents);
    return 1;
}