#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <setjmp.h>

//variables for the pattern
int pattern_fd;
int pattern_size;
struct stat pattern_info;
char *pattern = "";

int p_option = -1;          //used to check if need to munmap()
int context_bytes = -1;     //store -c argument
int retVal = 1;             //stores return value for main


sigjmp_buf int_jb;             //for setjmp and longjmp
void sig_handler(int sig){
    siglongjmp(int_jb, 1);
}


//function to handle each input file provided
int search_input(char *inputfile, char *pattern){
    //variables for the input files
    int input_fd;
    int input_size;
    struct stat input_info;
    char *input = "";

    signal(SIGBUS, sig_handler);

    // if inputfile == "", use standard input, otherwise try to open the file
    if (strcmp(inputfile, "") == 0){
        inputfile = "standard input";
        input_fd = 0;
    }
    else if ((input_fd = open(inputfile, O_RDONLY)) < 0){
        fprintf(stderr, "Can't open %s for reading: %s\n", inputfile, strerror(errno));
        return -1;
    }

    //handle SIGBUS
    if (sigsetjmp(int_jb, 1) != 0){
        fprintf(stderr, "Error: recieved SIGBUS signal\n");
        if (munmap(input, input_size) < 0)
            fprintf(stderr, "could not munmap input: %s\n", strerror(errno));
        if (input_fd != 0 && close(input_fd) < 0)
            fprintf(stderr, "could not close input file %s: %s\n", inputfile, strerror(errno));
        return -1;
    }
    
    //mmap the input file into variable 'input'
    if (fstat(input_fd, &input_info) != 0){
        fprintf(stderr, "fstat error for %s: %s\n", inputfile, strerror(errno));
        return -1;
    }
    input_size = input_info.st_size;
    if ((input = mmap(0, input_size, PROT_READ, MAP_PRIVATE, input_fd, 0)) == MAP_FAILED){
        fprintf(stderr, "could not mmap %s: %s\n", inputfile, strerror(errno));
        return -1;
    }

    //search through 'input' using 'pattern'
    for (int i = 0; i < input_size; i++){
        for (int j = 0; j < pattern_size; j++){ 
            //go through for loop until find a full match
            if (input[i+j] != pattern[j])
                break;
            if (j == pattern_size - 1){
                // if there were no errors before this, retVal = 0 for 'found a match'
                if (retVal != -1)
                    retVal = 0;
                
                fprintf(stdout, "%s:%d  ", inputfile, i);
                //if there was no -c option, stop here
                if (context_bytes == -1){
                    fprintf(stdout, "\n");
                    break;
                }

                //print things for -c option, human-readable and hex output of binary context
                for (int k = i - context_bytes; k < i + pattern_size + context_bytes; k++){
                    if (k >= 0 && k < input_size){
                        if (input[k] > 31 && input[k] < 127)
                            fprintf(stdout, " %c", input[k]);
                        else 
                            fprintf(stdout, " ?");
                    }
                }
                fprintf(stdout, "   ");
                for (int k = i - context_bytes; k < i + pattern_size + context_bytes; k++){
                    if (k >= 0 && k < input_size){
                        fprintf(stdout, " %.2X", input[k]);
                    }
                }
                fprintf(stdout, "\n");
            }
        }
    }


    if (munmap(input, input_size) < 0){
        fprintf(stderr, "could not munmap input: %s\n", strerror(errno));
        retVal = -1;
    }
    if (input_fd != 0){
        if (close(input_fd) < 0){
            fprintf(stderr, "could not close input file %s: %s\n", inputfile, strerror(errno));
            retVal = -1;
        }
    }
    return retVal;
}


int main(int argc, char **argv){
    int c;
    while((c = getopt(argc, argv, "p:c:")) != -1){
        switch (c) {
            case 'p':
                p_option = 1;
                if ((pattern_fd = open(optarg, O_RDONLY)) < 0){
                    fprintf(stderr, "Can't open %s for reading: %s\n", optarg, strerror(errno));
                    return -1;
                }
                if (fstat(pattern_fd, &pattern_info) != 0){
                    fprintf(stderr, "fstat error for %s: %s\n", optarg, strerror(errno));
                    return -1;
                }
                pattern_size = pattern_info.st_size;
                if ((pattern = mmap(0, pattern_size, PROT_READ, MAP_PRIVATE, pattern_fd, 0)) == MAP_FAILED){
                    fprintf(stderr, "could not mmap %s: %s\n", optarg, strerror(errno));
                    return -1;
                }

                //close the pattern file and check if it fails
                if (close(pattern_fd) < 0){
                    fprintf(stderr, "could not close pattern file %s: %s\n", optarg, strerror(errno));
                    return -1;
                }
                break;

            case 'c':
                context_bytes = atoi(optarg);
                break;

            case '?': /* missing option argument */
                if (optopt == 'p'||  optopt == 'c'){
                    fprintf(stderr, "ERROR: option '-%c' requires an argument\n", optopt);
                    return -1;
                }
                break;
        }
    }

    //set pattern to be the first argument after the option flags, if there is no -p option flag
    if (strcmp(pattern, "") == 0){
        if (optind == argc){
            fprintf(stderr, "Error: a pattern needs to be provided\n");
            return -1;
        }
        pattern = argv[optind];
        pattern_size = strlen(pattern);
        optind++;       //increment optind so we dont treat the pattern as an input file
    }

    //if there are no input files, use standard input
    if (optind == argc){
        retVal = search_input("", pattern);
    }
    else {
        //loop through all the input files
        for (int i = optind; i < argc; i++){
            retVal = search_input(argv[i], pattern);
        }
    }

    if (p_option == 1 && munmap(pattern, pattern_size) < 0){
        fprintf(stderr, "could not munmap pattern: %s\n", strerror(errno));
        return -1;
    }

    return retVal;
}