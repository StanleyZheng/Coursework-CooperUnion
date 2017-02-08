#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

int main(int argc, char **argv){
    char line[1024];     //command line
    char* args[100];        //array of all strings in command line
    int argcount;           //argument count
    int redirect_index;     //index to keep track of where redirects begin
    struct timeval t1, t2;  //used for getting real time consumed
    int retVal = 0;

    //process any script that follows mysh; only allows one, if two print error
    if (argc == 2){
        int fd;
        if ((fd = open(argv[1], O_RDONLY)) < 0){
            fprintf(stderr, "Error: %s - open error for %s\n", strerror(errno), argv[1]);
            return -1;
        }
        dup2(fd, 0);
        close(fd);
    }
    else if (argc > 2){
        fprintf(stderr, "Error: there are too many arguments for mysh\n");
        return -1;
    }

    //whileloop to parse each line from stdin or provided script file
    while(1){
        if(!fgets(line, 1024, stdin)){
            fprintf(stderr, "end of file\n");
            break;
        }
        if(*line == '\n' || *line == '#')
            continue;

        //split command into separate strings
        argcount = 0;
        char *token;
        token = strtok(line," ");
        if (*token == '\n')
            continue;
        while(token != NULL && argcount < 512){
            args[argcount] = token;
            token = strtok(NULL," ");
            argcount++;
        }
        args[argcount] = NULL;


        //Delete newline and insert NULL character
        if(args[argcount - 1][strlen(args[argcount - 1]) - 1] == '\n')
            args[argcount - 1][strlen(args[argcount - 1]) - 1] = '\0';


        //Set redirect_index for processing redirection arguments later AND 
        //Put argline together to print out later when executing program
        char argline[1024] = "";
        redirect_index = -1;
        for(int i = 0; i < argcount; i++){
            if (args[i][0] == '-'){
                strcat(argline, args[i]);
                strcat(argline, " ");
            }
            if (args[i][0] == '<' || args[i][0] == '>' || (args[i][0] == '2' && args[i][1] == '>')){
                redirect_index = i;
                break;
            }
        }
        for (int i = 0; i < strlen(argline); i++){
            if(argline[i] == '\n'){
                argline[i] = '\0';
                break;
            }
        }


        //fork and exec command
        gettimeofday(&t1, NULL);
        pid_t child_pid = fork();

        if (child_pid == -1) {
            fprintf(stderr, "Error: %s - fork failed\n", strerror(errno));
        }

        if (child_pid == 0){
            if (strlen(argline) != 0)
            fprintf(stderr, "Executing command %s with arguments \"%s\"\n", args[0], argline);

            //Parse arguments to get redirection locations
            char redir_in[256] = "";
            char redir_out[256] = "";
            char redir_err[256] = "";
            int shld_continue = 0;

            //forloop to take in all redirects and continues if file descriptor is redirected multiple times
            for(int i = redirect_index; i < argcount && i > 0; i++){
                if (args[i][0] == '<'){
                    if (strcmp(redir_in, "") != 0){
                        fprintf(stderr, "Error: %s - stdin is redirected more than once in command\n", strerror(errno));
                        exit(-1);
                    }
                    strcpy(redir_in, &args[i][1]);
                }

                else if (args[i][0] == '>'){
                    if (strcmp(redir_out, "") != 0){
                        fprintf(stderr, "Error: %s - stdout is redirected more than once in command\n", strerror(errno));
                        exit(-1);
                    }
                    strcpy(redir_out, &args[i][1]);
                }

                else if (args[i][0] == '2' && args[i][1] == '>'){
                    if (strcmp(redir_err, "") != 0){
                        fprintf(stderr, "Error: %s - stderr is redirected more than once in command\n", strerror(errno));
                        exit(-1);
                    }
                    strcpy(redir_err, &args[i][2]);
                }
            }// end - process redirection arguments

            //if there were any redirects provided handle them here
            if (redirect_index != -1){
                //redir_in
                if (strcmp(redir_in, "") != 0){
                    int fd_infile;
                    if ((fd_infile = open(redir_in, O_RDONLY)) < 0){
                        fprintf(stderr, "Error: %s - open error for %s for redirecting stdin\n", strerror(errno), redir_in);
                        exit(-1);
                    }
                    dup2(fd_infile, 0);
                    close(fd_infile);
                }

                //redir_out
                if (strcmp(redir_out, "") != 0){
                    int fd_outfile;
                    if (redir_out[0] == '>'){
                        if ((fd_outfile = open(&redir_out[1], O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0){
                            fprintf(stderr, "Error: %s - open error for %s for redirecting stdout\n", strerror(errno), redir_out);
                            exit(-1);
                        }
                    }
                    else {
                        if ((fd_outfile = open(redir_out, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0){
                            fprintf(stderr, "Error: %s - open error for %s for redirecting stdout\n", strerror(errno), redir_out);
                            exit(-1);
                        }
                    }
                    dup2(fd_outfile, 1);
                    close(fd_outfile);
                }

                //redir_err
                if (strcmp(redir_err, "") != 0){
                    int fd_errfile;
                    if (redir_err[0] == '>'){
                        if ((fd_errfile = open(&redir_err[1], O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0){
                            fprintf(stderr, "Error: %s - open error for %s for redirecting stderr\n", strerror(errno), &redir_err[1]);
                            exit(-1);
                        }
                    }
                    else {
                        if ((fd_errfile = open(redir_err, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0){
                            fprintf(stderr, "Error: %s - open error for %s for redirecting stderr\n", strerror(errno), redir_err);
                            exit(-1);
                        }
                    }
                    dup2(fd_errfile, 2);
                    close(fd_errfile);
                }
            }
            //end Parse arguments to get redirection locations

            //shorten argument list to not include redirections
            args[redirect_index] = NULL;

            execvp(args[0], args);
            fprintf(stderr, "Error: %s - execvp failed for %s\n", strerror(errno), args[0]);
            exit(-1);
        }

        else{
            struct rusage ru;
            int childStatus;

            // Wait for child process to finish
            if (wait3(&childStatus, 0, &ru) == -1){
                fprintf(stderr, "Error: %s - wait() for child failed", strerror(errno));
                retVal = -1;
            }
            else{
                gettimeofday(&t2, NULL);
                if(WIFEXITED(childStatus))
                    fprintf(stderr, "Command returned with return code %d\n", WEXITSTATUS(childStatus));
                else{
                    fprintf(stderr, "Child did not terminate with exit\n");
                    retVal = -1;
                }

                fprintf(stderr,"consuming %.3f real seconds, %.3f user, %.3f system\n", 
                    (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec) / 1000000.0),
                    ru.ru_utime.tv_sec + (ru.ru_utime.tv_usec / 1000000.0),
                    ru.ru_stime.tv_sec + (ru.ru_stime.tv_usec / 1000000.0));
            }
        }

    }// close while loop

    return retVal;
}