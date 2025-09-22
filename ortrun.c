#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include "da.h"


#define ENV_PATH_SIZE 1024
#define CMD_NAME_MAX 128
#define ENV_MAX_CHAR (1024*1024)
#define CMD_MAX_CHAR (1024*1024)
#define COMMANDS_FILE "cache-ortrun"


typedef struct Command{
    char name[CMD_NAME_MAX];
    char path[ENV_PATH_SIZE];
    char cmd[CMD_MAX_CHAR];
} Command;

typedef struct Commands {
    Command* items;
    int capacity;
    int count;
} Commands;

Commands commands;


void print_usage(){
    printf("ortrun, run command with env file.\n");
    printf("USAGE: ortrun [options] <command>\n");
    printf("    -f: Env file path. Default is '.env'. Ex.: -f ../.env.prod\n");
    printf("    -v: Verbose, print env's\n");
    printf("    -a: Add command with name. Ex.: -a mycommand\n");
    printf("    -l: List all saved commands\n");
    printf("    -r: run saved command. Ex.: -r api\n");
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

char* get_cmd_from_opt(int argc, char** argv){
    char* cmd = calloc(sizeof(char) * CMD_MAX_CHAR, sizeof(char));
    for(; optind < argc; optind++){
        sprintf(cmd, "%s %s", cmd, argv[optind]);
    }

    if(strlen(cmd) == 0){
        failure("Empty command");
    }

    return cmd;
}

char* create_command(char* env, int argc, char** argv){
    char* cmd=NULL;
    if(argc == -1){
        cmd = argv;
    } else {
        cmd = get_cmd_from_opt(argc, argv);
    }
    char* c = malloc(sizeof(char)*(strlen(cmd)+strlen(env)));
    sprintf(c, "%s %s", env, cmd);
    return c;
}

void load_commands(){
    FILE* f = fopen(COMMANDS_FILE, "rb");
    if(!f){
        DA_INIT(commands, 1);
        return;
    }

    if(fread(&commands.count, sizeof(int), 1, f) != 1){
        fclose(f);
        DA_INIT(commands, 1);
        return;
    }

    commands.capacity = commands.count > 0 ? commands.count : 1;
    commands.items = malloc(sizeof(Command) * commands.capacity);

    if(commands.count > 0){
        if(fread(commands.items, sizeof(Command), commands.count, f) != commands.count){
            printf("Error reading commands from file\n");
            free(commands.items);
            DA_INIT(commands, 1);
            fclose(f);
            return;
        }
    }

    fclose(f);
}

void save_commands(){
    FILE* f = fopen(COMMANDS_FILE, "w+b");
    if(!f){
        perror("SAVE COMMANDS");
        exit(EXIT_FAILURE);
    }

    if(fwrite(&commands.count, sizeof(int), 1, f) != 1){
        perror("Error writing commands count");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    if(commands.count > 0){
        if(fwrite(commands.items, sizeof(Command), commands.count, f) != commands.count){
            perror("Error writing commands");
            fclose(f);
            exit(EXIT_FAILURE);
        }
    }
    fclose(f);
}

void run(char* env_path, int argc, char** argv, bool verbose){
    char* env_file = read_env(env_path);
    char* env = parse_env(env_file);
    char* command = create_command(env, argc, argv);
    if(verbose){
        printf("[ORTRUN INFO]: Env path: '%s'\n", env_path);
        printf("[ORTRUN INFO]: Command: '%s'\n", command);
    }
    system(command);
}

int main(int argc, char** argv){
    if(argc<=1){
        print_usage();
        exit(EXIT_FAILURE);
    }

    bool verbose = false, add_command = false, run_command = false,
     list_commands = false;
    char env_path[ENV_PATH_SIZE] = ".env";
    char cmd_name[CMD_NAME_MAX] = {0};
    int opt;
    while ((opt = getopt(argc, argv, ":f:hva:l")) != -1) {
        switch (opt) {
            case 'f':
            snprintf(env_path, ENV_PATH_SIZE, "%s", optarg);
            break;
            case 'v':
            verbose = true;
            break;
            case 'a':
            add_command = true;
            sprintf(cmd_name, "%s", optarg);
            break;
            case 'r':
            run_command = true;
            sprintf(cmd_name, "%s", optarg);
            break;
            case 'l':
            list_commands = true;
            break;
            case 'h':
            default:
            print_usage();
            return 0;
        }
    }

    if(list_commands){
        load_commands();
        printf("[ORTRUN] Saved commands (%d):\n", commands.count);
        for(int i = 0; i < commands.count; i++){
            printf("  %d. Name: '%s'\n", i+1, commands.items[i].name);
            printf("     Env:  '%s'\n", commands.items[i].path);
            printf("     Cmd:  '%s'\n", commands.items[i].cmd);
            printf("\n");
        }
        DA_FREE(commands);
        return 0;
    }

    if(add_command){
        load_commands();

        Command c = {0};
        strcpy(c.name, cmd_name);
        strcpy(c.path, env_path);
        strcpy(c.cmd, get_cmd_from_opt(argc, argv));

        DA_PUSH(commands, c);
        save_commands();
        DA_FREE(commands);

        printf("[ORTRUN] '%s' command added.\n", cmd_name);
        printf("[ORTRUN] Run 'ortrun -r %s'\n", cmd_name);
        return 0;
    }

    if(run_command){
        load_commands();
        Command* c=NULL;
        for(int i=0; i< commands.count; i++){
            if(strcmp(cmd_name, commands.items[i].name) == 0){
                c = &commands.items[i];
            }
        }
        if(!c){
            char msg[CMD_NAME_MAX*2];
            sprintf(msg, "'%s' command not found", msg);
            failure(msg);
        }
        run(c->path, -1, c->cmd, verbose);
    }

    run(env_path, argc, argv, verbose);

    return 0;
}
