#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

char *username = "";        //houses -u arg if string
int user_id = 0;            //houses -u arg if it does not contain an alpha, 0 otherwise
int mtime = 0;              //houses -m arg for date comparison later
int optx = 0;               //houses 1 if -x flag is set, 0 otherwise
int optl = 0;               //houses 1 if -l flag is set, 0 otherwise
char *targetlink = "";      //houses -l arg for target comparison
int init_vol_num = -5;      //houses the initial volume number given by st_dev


//display the directory given the path name and stat information
void display_directory(char *path_name, struct stat info){
    char symlink[1025];
    char uxbit = '-';
    char gxbit = '-';
    char oxbit = '-';

    struct passwd *pswd;
    struct group *group;
    if ((pswd = getpwuid(info.st_uid)) == NULL){
        fprintf(stderr, "getpwuid() error for %s\n", path_name);
        exit(1);
    }
    if ((group = getgrgid(info.st_gid)) == NULL){
        fprintf(stderr, "getgrgid() error for %s\n", path_name);
        exit(1);
    }

    //check if username was 'user' or 'uid' and then address the flag
    if (user_id == 0){
        if (strcmp(username, "") != 0  && strcmp(username, pswd->pw_name) != 0)
            return;
    }
    else {
        if (user_id != info.st_uid)
            return;
    }

    //check how to treat mtime and then address the flag
    time_t now = time(0);
    if (mtime >= 0){
        if (now - info.st_mtime <= mtime)
            return;
    }
    else {
        if (now - info.st_mtime >= mtime * -1)
            return;
    }

    //address the -l flag if provided
    if (optl == 1 && !S_ISLNK(info.st_mode)){
        return;
    }

    
    if (S_ISLNK(info.st_mode)){
        int r;
        if ((r = readlink(path_name, symlink, 1025)) < 0){
            fprintf(stderr, "readlink error for %s\n", path_name);
            exit(1);
        }
        
        symlink[r] = '\0';

        if (optl == 1) {
            struct stat syminfo;
            struct stat targinfo;
            if (stat(symlink, &syminfo) >= 0 && stat(targetlink, &targinfo) >= 0){
                if (syminfo.st_ino != targinfo.st_ino)
                    return;
            }
            else
                return;
        }
    }


    //set the executable permissions for everyone
    if ((info.st_mode & S_IXUSR) && (info.st_mode & S_ISUID))
        uxbit = 's';
    else if (info.st_mode & S_ISUID)
        uxbit = 'S';
    else if (info.st_mode & S_IXUSR)
        uxbit = 'x';

    if ((info.st_mode & S_IXGRP) && (info.st_mode & S_ISGID))
        gxbit = 's';
    else if (info.st_mode & S_ISGID)
        gxbit = 'S';
    else if (info.st_mode & S_IXGRP)
        gxbit = 'x';

    if ((info.st_mode & S_IXOTH) && (info.st_mode & S_ISVTX))
        oxbit = 't';
    else if (info.st_mode & S_ISVTX)
        oxbit = 'T';
    else if (info.st_mode & S_IXOTH)
        oxbit = 'x';


    //handle all printing
    printf("%x/",       info.st_dev);
    printf("%-8llu ",   info.st_ino);
    
    if (S_ISBLK(info.st_mode))
        printf("b");
    else if (S_ISCHR(info.st_mode))
        printf("c");
    else if (S_ISLNK(info.st_mode))
        printf("l");
    else if (S_ISFIFO(info.st_mode))
        printf("p");
    else if (S_ISSOCK(info.st_mode))
        printf("s");
    else
        printf((S_ISDIR(info.st_mode))  ? "d" : "-");

    printf( (info.st_mode & S_IRUSR) ? "r" : "-");
    printf( (info.st_mode & S_IWUSR) ? "w" : "-");
    printf("%c",        uxbit );
    printf( (info.st_mode & S_IRGRP) ? "r" : "-");
    printf( (info.st_mode & S_IWGRP) ? "w" : "-");
    printf("%c",        gxbit );
    printf( (info.st_mode & S_IROTH) ? "r" : "-");
    printf( (info.st_mode & S_IWOTH) ? "w" : "-");
    printf("%c",        oxbit );
    printf(" %4d",      info.st_nlink);
    printf(" %-10s",    pswd->pw_name);
    printf(" %-8s",    group->gr_name);
    

    if (S_ISBLK(info.st_mode) || S_ISCHR(info.st_mode))
        printf(" %10x",     info.st_rdev);
    else
        printf(" %10llu",   info.st_size);


    //display time
    char date[20];
    strftime(date, 20, "%Y-%m-%d %H:%M", localtime(&(info.st_mtime)));
    printf(" %s",       date);
    printf(" %s",       path_name);

    if (S_ISLNK(info.st_mode))
        printf(" -> %s", symlink);

    printf("\n");
}




//walk through the directory, last four arguments of the function are used to hold the optargs
void walk_through(char *fn){
    DIR* directory;
    if ((directory = opendir(fn)) == NULL){
        fprintf(stderr, "opendir failed for %s\n", fn);
        exit(1);
    }

    char path[1025];
    char symlink[1025];
    struct dirent* entry;
    struct stat info;

    //print information on fn
    if (stat(fn, &info) != 0){
        fprintf(stderr, "stat error for %s: %s\n", fn, strerror(errno));
        exit(1);
    }

    if (init_vol_num == -5){            //using -5 in case info.st_dev returns -1 
        init_vol_num = info.st_dev;
    }

    display_directory(fn, info);
    

    //process other files
    while (directory){
        errno = 0;
        if ((entry = readdir(directory)) == NULL){
            if (errno != 0)
                fprintf(stderr, "readdir error for %s: %s\n", path, strerror(errno));
            break;
        }

        strcpy(path, fn);
        strcat(path, "/");
        strcat(path, entry->d_name);
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            
            if (lstat(path, &info) != 0){
                fprintf(stderr, "lstat error for %s: %s\n", path, strerror(errno));
                return;
            }

            if (S_ISDIR(info.st_mode)){
                if (optx == 1 && init_vol_num != info.st_dev)
                    fprintf(stderr, "note: not crossing mount point at %s\n", path);
                else
                    walk_through(path);
            }

            else
                display_directory(path, info);
        }
    }

    if (closedir(directory) < 0){
        fprintf(stderr, "closedir error for %s: %s\n", path, strerror(errno));
        exit(1);
    }
}




int main(int argc, char **argv){

    opterr = 0;
    int c;
    int length;

    while ((c = getopt(argc, argv, "u:m:xl:")) != -1){
        switch (c) {
            case 'u':
                username = optarg;
                length = strlen(username);
                for(int i = 0; i < length; i++){
                    if (username[i] < '0' || username[i] > '9')
                        break;
                    if (i == length - 1)
                        user_id = atoi(username);
                }
                break;

            case 'm':
                if (optarg == 0){
                    fprintf(stderr, "Error: option '-m' requires a positive or negative integer\n");
                    return -1;
                }
                length = strlen(optarg);

                if (optarg[0] == '-' || optarg[0] >= '0' || optarg[0] <= '9'){
                    for(int i = 1; i < length; i++){
                        if (optarg[i] < '0' || optarg[i] > '9'){
                            fprintf(stderr, "Error: option '-m' requires a number\n");
                            return -1;
                        }
                    }
                }
                else{
                    fprintf(stderr, "Error: option '-m' requires a number\n");
                    return -1;
                }

                mtime = atoi(optarg);
                break;

            case 'x':
                optx = 1;
                break;

            case 'l':
                optl = 1;
                targetlink = optarg;
                break;

            case '?': /* missing option argument */
                if (optopt == 'u'|| optopt == 'm' || optopt == 'l')
                    fprintf(stderr, "Error: option '-%c' requires an argument\n", optopt);
                else
                    fprintf(stderr, "Error: illegal option -- %c\n", optopt);
                return -1;
        }
    }
    walk_through(argv[optind]);
    return 0;
}