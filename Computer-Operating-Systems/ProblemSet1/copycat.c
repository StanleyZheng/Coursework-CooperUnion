#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){

    int c;
    char *x;
    char *o_opt_arg;
    int buff_size = 512;
    opterr = 0;

    while ((c = getopt(argc, argv, "b:o:")) != -1){
        switch (c) {
            case 'b':
                buff_size = atoi(optarg);
                if (buff_size <= 0 || buff_size >= 1000000){
                    fprintf(stderr, "ERROR: option '-%c' requires an argument in the range (0,1000000)\n", optopt);
                    return -1;
                }
                break;
            case 'o':
                o_opt_arg = optarg;
                break;
            case '?': /* missing option argument */
                if (optopt == 'b'||  optopt == 'o'){
                    fprintf(stderr, "ERROR: option '-%c' requires an argument\n", optopt);
                    return -1;
                }
                break;
        }
    }

    char buf[buff_size];
    char *error_stringin, *error_stringout; //for displaying diff errors
    int index = optind - 1; //setting index to the last argument with a '-' or argument
    int outfile, infile, readbuf, writefile, pwcounter = 0;

    error_stringout = o_opt_arg;

    if (o_opt_arg == NULL){
        outfile = 1;
        error_stringout = "standard output";
    }
    else if ((outfile = open(o_opt_arg, O_TRUNC | O_CREAT | O_APPEND | O_WRONLY, 0666)) < 0){
        perror(o_opt_arg);
        return -1;
    }

    //adjust index to include '-' files (-a, -c, -d, -anything)
    while(index > 0){
        char tempstr[4]; //used to differentiate '-o ' and '-ooutput.txt'
        strncpy(tempstr, argv[index], 3);
        //while loop to find last '-b' or '-o'
        if ((strcmp(tempstr, "-b") == 0) || (strcmp(tempstr, "-o") == 0)){
            //check if current index is the last argument and if current arg was '-o' or '-ooutput'
            if (index != argc - 1 && tempstr[2] == '\0')
                index++;
            index++;
            break;
        }
        index--;
    }

    /*----- Process Input files -----*/
    //used to differentiate between './a.out' and './a.out -'
    if (index == 0 && argc > 1)
        index++;

    while (1){
        error_stringin = argv[index];

        //handle missing input files and '-'s
        if (index == argc || strcmp(argv[index],"-") == 0 || index == 0){
            infile = 0;
            error_stringin = "standard input";
        }
        else if ((infile = open(argv[index], O_RDONLY)) < 0){
            perror("open");
            return -1;
        }

        while (1){
            if ((readbuf = read(infile, buf, buff_size)) < 0){
                perror(error_stringin);
                return -1;
            }
            writefile = write(outfile, buf, readbuf);
            x = buf;
            if (writefile < 0){
                perror(error_stringout);
                return -1;
            }
            while (writefile < readbuf){
                pwcounter++;
                x += writefile;
                readbuf -= writefile;
                writefile = write(outfile, x, readbuf);
                if (pwcounter > 1000000){
                    fprintf(stderr, "ERROR: partial write attempts exceeded the limit");
                    return -1;
                }
            }

            if (readbuf == 0)
                break;
        }

        //close input files if they are not standard input
        if (infile != 0){
            if (close(infile) < 0){
                perror(error_stringin);
                return -1;
            }
        }

        index++;
        if (index >= argc)
            break;
    }

    if (outfile != 1){
        if (close(outfile) < 0){
            perror(error_stringout);
            return -1;
        }
    }
    
    return 0;
}