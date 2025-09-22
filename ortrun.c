#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
// #include "da.h"


#define ENV_PATH_SIZE 1024
#define ENV_MAX_CHAR (1024*1024)
#define CMD_MAX_CHAR (1024*1024)


void print_usage(){
    printf("ortrun, run command with env file.\n");
    printf("USAGE: ortrun [options] <command>\n");
    printf("    -f: Env file path. Default is '.env'. Ex.: -f ../.env.prod\n");
    printf("    -v: Verbose, print env's\n");
    printf("    -h: Help\n");
}

void failure(char* m){
    if(m != NULL){
        printf("ERROR: %s\n", m);
    }
    print_usage();
    exit(EXIT_FAILURE);
}

char* read_env(char* path){
    FILE* f = fopen(path, "r");
    if(!f){
        char* msg = malloc(ENV_PATH_SIZE);
        snprintf(msg, ENV_PATH_SIZE, "Env file not found: %s", path);
        failure(msg);
    }

    struct stat sb;
    if(fstat(fileno(f), &sb) < 0){
        perror("fstat env read error");
        exit(EXIT_FAILURE);
    }

    if(sb.st_size > ENV_MAX_CHAR){
        failure("ERROR: env is too big. Change env buffersize with '-fs' option\n");
    }

    if(sb.st_size == 0){
        printf("[ORTRUN INFO]: '%s' file is empty.\n", path);
    }

    char *env = malloc(sizeof(char) * sb.st_size + 1);
    if(fread(env, 1, sb.st_size, f) < sb.st_size){
        failure("Env file couldnt read");
    }
    env[sb.st_size] = '\0';

    fclose(f);
    return env;
}

char* parse_env(const char* env_file){
        int file_len = strlen(env_file);
    char* env = calloc(sizeof(char)*file_len, sizeof(char));
    memset(env, '\0', sizeof(char)*file_len);

    char* c=env_file;
    int i=0;
    while(*c != '\0'){
        if(*c == '#'){
            // goto end of line
            while(*c != '\n') c++;
        }

        if(*c == '\n'){
            c++;
            env[i++] = ' ';
            continue;
        }

        env[i++] = *(c++);
    }
    return env;
}

char* create_command(char* env, int argc, char** argv){
    char* cmd = calloc(sizeof(char) * CMD_MAX_CHAR, sizeof(char));
    for(; optind < argc; optind++){
        sprintf(cmd, "%s %s", cmd, argv[optind]);
    }
    char* c = malloc(sizeof(char)*(strlen(cmd)+strlen(env)));
    sprintf(c, "%s %s", env, cmd);
    return c;
}

int main(int argc, char** argv){
    if(argc<=1){
        print_usage();
        exit(EXIT_FAILURE);
    }

    bool verbose = false;
    char env_path[ENV_PATH_SIZE] = ".env";
    int opt;
    while ((opt = getopt(argc, argv, ":f:hv")) != -1) {
        switch (opt) {
        case 'f':
            snprintf(env_path, ENV_PATH_SIZE, "%s", optarg);
            break;
        case 'v': 
            verbose = true;
            break;
        case 'h':
        default:
            print_usage();
            return 0;
        }
    }

    char* env_file = read_env(env_path);
    char* env = parse_env(env_file);
    char* command = create_command(env, argc, argv);
    if(verbose){
        printf("[ORTRUN INFO]: Env path: '%s'\n", env_path);
        printf("[ORTRUN INFO]: Command: '%s'\n", command);
    }
    system(command);
    return 0;
}
